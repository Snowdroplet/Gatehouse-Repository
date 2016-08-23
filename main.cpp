#include <cstdio>
#include <iostream>
#include <cstring>
#include <ctime>
#include <cmath>
#include <vector>

#include <boost/filesystem.hpp>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_physfs.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <physfs.h>

#include "allegrocustom.h"

#include "gamesystem.h"
#include "control.h"
#include "resource.h"

#include "extfile.h"

#include "terminal.h"

#include "node.h"
#include "graph.h"
#include "area.h"

#include "item.h"
#include "being.h"
#include "player.h"
#include "npc.h"

b2World *physics;
Player *player;
Area *area;

std::vector<Being*>beings; // All beings currently in play.
std::vector<Being*>actionQueue; // All beings queued to move.
bool CompareAPDescending(Being *lhs, Being *rhs)
{
    return lhs->actionPoints > rhs->actionPoints;
}

std::vector<Item*>items; // All items currently in play.
std::vector<Item*>floorItems; // All items currently on the floor.

std::vector<int>baseIDsKnown; // A list of all previously identified itemIDs. These will auto-identify when encountered again.


void GameLogic();
void LoadingLogic();
void TitleLogic();


void GameDrawing();
void LoadingDrawing();
void TitleDrawing();


void DrawGUI();
void DrawTiles();
void DrawDebugOverlay();

void UpdateVectors(); //Update elements and delete elements that have been deactivated
void InterpretControl(); // Merge with control module once it is possible to put "player" in gamesystem

/// MAIN
int main(int argc, char *argv[])
{
    srand(time(NULL));

    //Replace fprintf with native dialogue later
    if(!al_init())
    {
        fprintf(stderr, "failed to initialize allegro!\n");
        return -1;
    }
    if(!al_install_keyboard())
    {
        fprintf(stderr, "failed to install keyboard!\n");
        return -1;
    }
    if(!al_install_mouse())
    {
        fprintf(stderr, "failed to install mouse!\n");
        return -1;
    }
    if(!al_init_image_addon())
        return -1;
    if(!al_init_primitives_addon())
        return -1;

    al_init_font_addon();

    if(!al_init_ttf_addon())
        return -1;

    if(!PHYSFS_init(argv[0]))
        return -1;

    if(!PHYSFS_mount("./gamedata.zip", "/", 1))
    {
        fprintf(stderr, "gamedata not found!\n");
        return -1;
    }

    al_set_physfs_file_interface();

    AllegroCustomInit();

    FPStimer = al_create_timer(1.0/FPS);
    if(!FPStimer)
    {
        fprintf(stderr, "failed to create fpstimer!\n");
        return -1;
    }

    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display)
    {
        fprintf(stderr, "failed to create display!\n");
        al_destroy_timer(FPStimer);
        return -1;
    }

    eventQueue = al_create_event_queue();
    if(!eventQueue)
    {
        fprintf(stderr, "failed to create event queue!\n");
        al_destroy_display(display);
        al_destroy_timer(FPStimer);
        return -1;
    }

    al_register_event_source(eventQueue, al_get_display_event_source(display));
    al_register_event_source(eventQueue, al_get_timer_event_source(FPStimer));
    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_mouse_event_source());

    LoadResources();

    physics = new b2World(physicsGravity);
    physics->SetAllowSleeping(true);

#ifdef D_CREATE_TESTING_AREA
    area = new Area(); // Create random test area
#endif

#ifndef D_CREATE_TESTING_AREA
    area = new Area(true); // Initialization of basic area properties unecessary because they will be deserialized from .areafile
    if(!LoadAreaState("test area", area, false)) // Name of file: "test area" // target = area // false for area base
        gameExit = true;
    area->InitByArchive(); // The InitByArchive functions are meant to initialize the properties of objects that are not included in serialization.

    /* Debug for use if areafile serialization error suspected
        std::cout << "Area name: " << area->name << std::endl;
        std::cout << "Area floor: " << area->floor << std::endl;
        std::cout << "Area dLevel: " << area->dLevel << std::endl;
        if(area->wallmap.empty())
            std::cout << "Error: Wallmap vector is empty" << std::endl;
        else
            std::cout << "Wallmap vector is not empty" << std::endl;
        if(area->tilemap.empty())
            std::cout << "Error: Tilemap vector is empty" << std::endl;
        else
            std::cout << "Tilemap vector is not empty" << std::endl;
        if(area->occupied.empty())
            std::cout << "Error: Occupied vector is empty" << std::endl;
        else
            std::cout << "Occupied vector is not empty" << std::endl;
    */


#endif

    //player = new Player(5,5);
    player = new Player(true); //true to initalize saved player - Remove in end for atrium map
    LoadPlayerState("player",player);
    player->InitByArchive();

    //End testing

    beings.push_back(player);

    beings.push_back(new NPC(SLIME,6,4));
    beings.push_back(new NPC(QUICKLING,8,6));

    al_start_timer(FPStimer);

    while(!gameExit)
    {
        al_wait_for_event(eventQueue, &ev);
        Control();

        if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            if(mainPhase == MAIN_PHASE_GAME)
            {
                GameLogic();
                GameDrawing();
            }
            else if(mainPhase == MAIN_PHASE_LOADING)
            {
                LoadingLogic();
                LoadingDrawing();
            }
            else if(mainPhase == MAIN_PHASE_TITLE)
            {
                TitleLogic();
                TitleDrawing();
            }

            redraw = true;
        }
    }

    //Testing Savedata
    SaveAreaState(area);
    SavePlayerState(player);
    //End

    delete physics;

    delete player;
    delete area;

    UnloadResources();
    al_destroy_timer(FPStimer);
    al_destroy_display(display);
    al_destroy_event_queue(eventQueue);
    PHYSFS_deinit();

    return 0;
}

/// ########## ########## BEGIN MAIN LOGIC FUNCTIONS ########## ##########

void GameLogic()
{
    static bool actionOpen = true; // Whether the system is open to new actions, whether by player or AI. When an action is taken, actionOpen becomes false.
    static bool checkAnimationComplete = true;

    static std::vector<Being*>::iterator aqit; // Static action queue iterator meant to be the front element of actionQueue.

    if(ev.type == ALLEGRO_EVENT_TIMER)
    {
        if(awaitingPlayerCommand)
        {
            if(player->actionBlocked)
            {
                player->actionCost = 100;
                player->Move(NO_DIRECTION);
                submittedPlayerCommand = true;
                player->actionBlocked = false;
            }
            else if(keyInput[KEY_PAD_8] || keyInput[KEY_UP])
            {
                player->actionCost = 100;
                player->Move(NORTH);
                submittedPlayerCommand = true;
            }
            else if(keyInput[KEY_PAD_9] || (keyInput[KEY_UP] && keyInput[KEY_RIGHT]))
            {
                player->actionCost = 100;
                player->Move(NORTHEAST);
                submittedPlayerCommand = true;
            }
            else if(keyInput[KEY_PAD_6] || keyInput[KEY_RIGHT])
            {
                player->actionCost = 100;
                player->Move(EAST);
                submittedPlayerCommand = true;
            }
            else if(keyInput[KEY_PAD_3] || (keyInput[KEY_DOWN] && keyInput[KEY_RIGHT]))
            {
                player->actionCost = 100;
                player->Move(SOUTHEAST);
                submittedPlayerCommand = true;
            }
            else if(keyInput[KEY_PAD_2] || keyInput[KEY_DOWN])
            {
                player->actionCost = 100;
                player->Move(SOUTH);
                submittedPlayerCommand = true;
            }
            else if(keyInput[KEY_PAD_1] || (keyInput[KEY_DOWN] && keyInput[KEY_LEFT]))
            {
                player->actionCost = 100;
                player->Move(SOUTHWEST);
                submittedPlayerCommand = true;
            }
            else if(keyInput[KEY_PAD_4] || keyInput[KEY_LEFT])
            {
                player->actionCost = 100;
                player->Move(WEST);
                submittedPlayerCommand = true;
            }
            else if(keyInput[KEY_PAD_7] || (keyInput[KEY_UP] && keyInput[KEY_LEFT]))
            {
                player->actionCost = 100;
                player->Move(NORTHWEST);
                submittedPlayerCommand = true;
            }
            else if(keyInput[KEY_PAD_5])
            {
                player->actionCost = 100;
                player->Move(NO_DIRECTION);
                submittedPlayerCommand = true;
            }
#ifdef D_TEST_PATHFINDING
            static int debugPathRequestDelay = 0;

            if(keyInput[KEY_T] && keyInput[KEY_Y])
            {
                if(debugPathRequestDelay <= 0)
                {
                    player->ResetPath();

                    int testDestX = rand()%areaCellWidth;
                    int testDestY = rand()%areaCellHeight;

                    std::cout << std::endl;
                    std::cout << "####DEBUG PATH REQUEST####" << std::endl;
                    std::cout << "Start X: " << player->xCell << " | Start Y: " << player->yCell << std::endl;
                    std::cout << "Dest X: " << testDestX << " | Dest Y: " << testDestY << std::endl;

                    player->currentPath = player->graph->RequestPath(player->xCell,player->yCell,testDestX,testDestY); ///ERROR!

                    std::cout << "Path retrieved." << std::endl;

                    for(std::vector<Node*>::iterator it = player->currentPath.begin(); it != player->currentPath.end(); ++it)
                    {
                        std::cout << (*it)->id << " ";
                    }
                    std::cout << std::endl;
                }
                else
                    std::cout << "Debug Path Request Delay..." << std::endl;

                debugPathRequestDelay = 300;
            }
            if(debugPathRequestDelay > 0)
                debugPathRequestDelay --;
#endif
        }

    }

    if(turnLogicPhase == GRANT_ACTION_POINTS)
    {
        turn ++;
#ifdef D_TURN_LOGIC
        std::cout << "### TURN " << turn << " ###" << std::endl;
        std::cout << "Granting AP:" << std::endl;
#endif

        for(std::vector<Being*>::iterator it = beings.begin(); it != beings.end(); ++it)
        {
            (*it)->actionPoints += (*it)->effectiveSpeed;
#ifdef D_TURN_LOGIC
            std::cout << (*it)->name << " has " << (*it)->actionPoints << "/" << (*it)->effectiveSpeed << " AP." << std::endl;
#endif

            if((*it)->actionPoints >= 100)
            {
#ifdef D_TURN_LOGIC
                std::cout << "(Added to action queue)" << std::endl;
#endif
                actionQueue.push_back(beings[it-beings.begin()]);
            }
        }

        turnLogicPhase = SORT_ACTION_QUEUE;
    }

    if(turnLogicPhase == SORT_ACTION_QUEUE)
    {
        std::sort(actionQueue.begin(), actionQueue.end(), CompareAPDescending);
        aqit = actionQueue.begin();
        turnLogicPhase = SELECT_ACTION;
    }
    if(turnLogicPhase == SELECT_ACTION)
    {
        //ActionOpen should be true if the being in queue to move has not yet chosen a player action or AI option yet.
        if(!actionQueue.empty())
        {
            if(actionOpen && !awaitingPlayerCommand)
            {

                if((*aqit)->derivedType == BEING_TYPE_NPC)
                {
                    //It is possible for a being to have its AP reduced below the minimum to move even while queued to move.
                    if((*aqit)->actionPoints < 100)
                        (*aqit)->actionBlocked = true; // And if blocked, the being in question may only choose to wait.

#ifdef D_TURN_LOGIC
                    std::cout << (*aqit)->name << " executes its AI: ";
#endif

                    ((NPC*)(*aqit))->AI();

#ifdef D_TURN_LOGIC
                    std::cout << (*aqit)->actionName << std::endl;
#endif
                    (*aqit)->actionPoints -= (*aqit)->actionCost;

#ifdef D_TURN_LOGIC
                    std::cout << (*aqit)->name << " now has " << (*aqit)->actionPoints << "/" << (*aqit)->effectiveSpeed << "AP" << std::endl;
#endif
                    actionOpen = false;
                }
                else if((*aqit)->derivedType == BEING_TYPE_PLAYER)
                {
                    //It is possible that the player's AP is reduced below the minimum to move even while queued to move.
                    if((*aqit)->actionPoints < 100)
                        (*aqit)->actionBlocked = true; // If blocked, may only wait.

                    awaitingPlayerCommand = true;
                }
            }

            if(awaitingPlayerCommand && submittedPlayerCommand)
            {
#ifdef D_TURN_LOGIC
                std::cout << player->name << " executes your command" << std::endl;
#endif
                player->actionPoints -= player->actionCost;
#ifdef D_TURN_LOGIC
                std::cout << player->name << " now has " << player->actionPoints << "/" << player->effectiveSpeed << "AP" << std::endl;
#endif
                awaitingPlayerCommand = false;
                submittedPlayerCommand = false;
                actionOpen = false;

                ShiftTerminal(); // This might have to be moved to be triggered by different conditions later on.
            }
            //Remove front element from action queue if it does not have enough AP (100).
            //It is possible for a being to have its AP reduced while in the actionQueue and thus become ineligible to move.
            if((*aqit)->actionPoints < 100)
            {
#ifdef D_TURN_LOGIC
                std::cout << "Erased " << (*aqit)->name << " from movequeue" << std::endl;
#endif
                actionQueue.erase(aqit);
            }
        }
        else
        {
            turnLogicPhase = GRANT_ACTION_POINTS;
        }

        //If a player action/AI option has been confirmed, no further actions may be processed until its animation is complete.
        if(!actionOpen)
            turnLogicPhase = ANIMATION;
    }

    if(turnLogicPhase == ANIMATION)
    {
        for(std::vector<Being*>::iterator it = beings.begin(); it != beings.end(); ++it)
        {
            //Progress the animation of all beings by one step
            (*it)->ProgressAnimation();

            //Flag the animation phase as unfinished if any being's animation is incomplete
            //e.g. draw position does not match x/y position corresponding to current cell in a geometric translation...
            if(!(*it)->animationComplete)
                checkAnimationComplete = false;

        }

        //Move on to calculation if the phase is complete, otherwise reset the flag
        if(checkAnimationComplete)
            turnLogicPhase = CALCULATION;
        else
            checkAnimationComplete = true;
    }

    if(turnLogicPhase == CALCULATION)
    {
        // Something here.

        //Calculations having been applied, the flags are reset to await new actions.
        actionOpen = true;
        turnLogicPhase = SELECT_ACTION;
    }

    UpdateVectors();

}

void LoadingLogic()
{
    if(ev.type == ALLEGRO_EVENT_TIMER)
    {
        if(keyInput[KEY_PAD_6])
            loadingCamX ++;
        if(keyInput[KEY_PAD_2])
            loadingCamY ++;
        if(keyInput[KEY_PAD_4])
            loadingCamX --;
        if(keyInput[KEY_PAD_8])
            loadingCamY --;
#ifdef D_TERMINATE_LOADING_SIGNAL
        if(keyInput[KEY_Z])
            D_TERMINATELOADINGPHASESIGNAL = true;  // Obviously only for developmental purposes since Area::Generate is incomplete.
#endif // D_TERMINATE_LOADING_SIGNAL

#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
        if(keyInput[KEY_Q] && D_PROGRESSPAUSEDVISUALIZATIONTIMER == 0)
        {
            area->D_UNPAUSE_VISUALIZATION();
            D_PROGRESSPAUSEDVISUALIZATIONTIMER = 100;
        }

#endif // D_GEN_VISUALIZATION_PHASE_PAUSE

    }

#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
    if(D_PROGRESSPAUSEDVISUALIZATIONTIMER > 0)
        D_PROGRESSPAUSEDVISUALIZATIONTIMER --;
#endif // D_GEN_VISUALIZATION_PHASE_PAUSE

    if(needGeneration
#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
            && !area->D_GET_GENERATOR_VISUALIZATION_PAUSE()
#endif // D_GEN_VISUALIZATION_PHASE_PAUSE
      )
    {
        area->Generate();
        if(area->GetGenerationComplete())
            needGeneration = false;
    }

#ifdef D_TERMINATE_LOADING_SIGNAL
    if(D_TERMINATELOADINGPHASESIGNAL)
    {
        mainPhase = MAIN_PHASE_GAME;
    }
#endif // D_TERMINATE_LOADING_SIGNAL
}

void TitleLogic()
{
    mainPhase = MAIN_PHASE_LOADING;
}


/// ########## ########## END MAIN LOGIC FUNCTIONS ########## ##########

/// ########## ########## BEGIN MAIN DRAWING FUNCTIONS ########## ##########
void GameDrawing()
{
    if(redraw && al_is_event_queue_empty(eventQueue))
    {
        redraw = false;
        al_clear_to_color(al_map_rgb(0,0,0));

        DrawTiles();

        al_draw_bitmap(gfxPlayer,SCREEN_W/2, PLAY_H/2,0);

        for(std::vector<Being*>::iterator it = beings.begin(); it != beings.end(); ++it)
        {
            if((*it)->derivedType == BEING_TYPE_NPC)//As opposed to player, which is drawn seperately
            {
                if((*it)->animationState == PASSIVE)
                    al_draw_bitmap(gfxNPCPassive[(*it)->spriteID + (*it)->animationFrame],
                                   (*it)->xPosition + SCREEN_W/2 - player->xPosition,
                                   (*it)->yPosition + PLAY_H/2 - player->yPosition,
                                   0);
            }
        }

#ifdef D_DRAW_DEBUG_OVERLAY
        DrawDebugOverlay();
#endif // D_DRAW_DEBUG_OVERLAY

        DrawGUI();

        al_flip_display();
    }
}

void LoadingDrawing()
{
    if(redraw && al_is_event_queue_empty(eventQueue))
    {
        redraw = false;
        al_clear_to_color(al_map_rgb(0,0,0));

#ifdef D_SHOW_LOADING_VISUALIZATION
        if(D_SHOWLOADINGVISUALIZATION)
        {
#endif // D_SHOW_LOADING_VISUALIZATION
            ///Draw the grid
            for(int i = 0; i <= miniAreaWidth; i += MINI_TILESIZE) //Columns
            {
                int lineThickness = 1;
                ALLEGRO_COLOR lineColor = DIM_NEUTRAL_GRAY;
                if(i%5 == 0)
                {
                    lineThickness = 1.5;
                    lineColor = DIM_NEUTRAL_WHITE;
                }
                al_draw_line(i          -loadingCamX,
                             0          -loadingCamY,
                             i          -loadingCamX,
                             miniAreaHeight -loadingCamY,
                             lineColor,lineThickness);


            }
            for(int i = 0; i <= miniAreaHeight; i += MINI_TILESIZE) //Rows
            {
                int lineThickness = 1;
                ALLEGRO_COLOR lineColor = DIM_NEUTRAL_GRAY;
                if(i%5 == 0)
                {
                    lineThickness = 1.5;
                    lineColor = DIM_NEUTRAL_WHITE;
                }

                al_draw_line(0         -loadingCamX,
                             i         -loadingCamY,
                             miniAreaWidth -loadingCamX,
                             i         -loadingCamY,
                             lineColor,lineThickness);
            }

            /// Draw room layout as coloured rectangles
            if(area->GetGenerationPhase() >= GEN_LAYOUT_SKELETON) // Draw from cell layout phase onwards
            {
                for(int y = 0; y < areaCellHeight; y++)
                {
                    for(int x = 0; x < areaCellWidth; x++)
                    {
                        switch(area->genLayout[y*areaCellWidth+x])
                        {
                        case GEN_CELL_MAIN_ROOM:
                            al_draw_filled_rectangle(x*MINI_TILESIZE                 - loadingCamX,
                                                     y*MINI_TILESIZE                 - loadingCamY,
                                                     x*MINI_TILESIZE + MINI_TILESIZE - loadingCamX,
                                                     y*MINI_TILESIZE + MINI_TILESIZE - loadingCamY,
                                                     POISON_GREEN);
                            break;

                        case GEN_CELL_HALLWAY_SKELETON:
                            al_draw_filled_rectangle(x*MINI_TILESIZE                 - loadingCamX,
                                                     y*MINI_TILESIZE                 - loadingCamY,
                                                     x*MINI_TILESIZE + MINI_TILESIZE - loadingCamX,
                                                     y*MINI_TILESIZE + MINI_TILESIZE - loadingCamY,
                                                     FIRE_ORANGE);
                            break;

                        case GEN_CELL_HALLWAY:
                            al_draw_filled_rectangle(x*MINI_TILESIZE                 - loadingCamX,
                                                     y*MINI_TILESIZE                 - loadingCamY,
                                                     x*MINI_TILESIZE + MINI_TILESIZE - loadingCamX,
                                                     y*MINI_TILESIZE + MINI_TILESIZE - loadingCamY,
                                                     FIRE_ORANGE);
                            break;

                        case GEN_CELL_HALLWAY_EXTENSION:
                            al_draw_filled_rectangle(x*MINI_TILESIZE                 - loadingCamX,
                                                     y*MINI_TILESIZE                 - loadingCamY,
                                                     x*MINI_TILESIZE + MINI_TILESIZE - loadingCamX,
                                                     y*MINI_TILESIZE + MINI_TILESIZE - loadingCamY,
                                                     BLOOD_RED);
                            break;

                        case GEN_CELL_HALL_ROOM:
                            al_draw_filled_rectangle(x*MINI_TILESIZE                 - loadingCamX,
                                                     y*MINI_TILESIZE                 - loadingCamY,
                                                     x*MINI_TILESIZE + MINI_TILESIZE - loadingCamX,
                                                     y*MINI_TILESIZE + MINI_TILESIZE - loadingCamY,
                                                     DARK_YELLOW);
                            break;
                        }
                    }
                }
            }

            ///Draw room generation boxes
            if(area->GetGenerationPhase() >= GEN_PHYSICAL_DISTRIBUTION) // Draw from physics simulation onwards
            {
                for(std::vector<RoomGenBox*>::iterator it = area->roomGenBoxes.begin(); it != area->roomGenBoxes.end(); ++it)
                {

                    // Concerning generationPhase == GEN_PHYSICAL_DISTRIBUTION, but always active.
                    // Draws outlines of room objects. Orange rooms denote that the corresponding physics body is awake. Blue rooms are asleep.

                    if((*it)->correspondingBodyAwake)
                    {
                        al_draw_rectangle((*it)->x1 - loadingCamX,       // Awake
                                          (*it)->y1 - loadingCamY,
                                          (*it)->x2 - loadingCamX,
                                          (*it)->y2 - loadingCamY,
                                          FIRE_ORANGE, 1);
                    }
                    else
                    {
                        if((*it)->designatedMainRoom)
                            al_draw_rectangle((*it)->x1 - loadingCamX,       // Asleep, designated main room
                                              (*it)->y1 - loadingCamY,
                                              (*it)->x2 - loadingCamX,
                                              (*it)->y2 - loadingCamY,
                                              BRIGHT_GREEN, 2);
                        else
                            al_draw_rectangle((*it)->x1 - loadingCamX,       // Asleep, not main room
                                              (*it)->y1 - loadingCamY,
                                              (*it)->x2 - loadingCamX,
                                              (*it)->y2 - loadingCamY,
                                              COLD_BLUE, 1);
                    }


                    s_al_draw_text(terminalFont, NEUTRAL_WHITE,
                                   (*it)->x1-loadingCamX+4,
                                   (*it)->y1-loadingCamY+4,
                                   ALLEGRO_ALIGN_LEFT,
                                   std::to_string(it-area->roomGenBoxes.begin()));

                }
            }


            /// Draw the room connection graph
            if(area->GetGenerationPhase() == GEN_TRIANGULATION || area->GetGenerationPhase() == GEN_MST) //Draw up to MST
            {
                // Draw the graph created by delaunay triangulation
                for(std::vector<TriEdge>::iterator it = area->triEdges.begin(); it != area->triEdges.end(); ++it)
                {
                    al_draw_line((*it).p1.x        -loadingCamX,
                                 (*it).p1.y        -loadingCamY,
                                 (*it).p2.x        -loadingCamX,
                                 (*it).p2.y        -loadingCamY,
                                 DARK_VIOLET,2);
                }

            }
            else if(area->GetGenerationPhase() >= GEN_MST)
            {
                // Draw the graph created the the minimum spanning tree/ re-addition of edges.
                for(std::vector<TriEdge>::iterator it = area->demoEdges.begin(); it != area->demoEdges.end(); ++it)
                {
                    al_draw_line((*it).p1.x        -loadingCamX,
                                 (*it).p1.y        -loadingCamY,
                                 (*it).p2.x        -loadingCamX,
                                 (*it).p2.y        -loadingCamY,
                                 HOLY_INDIGO,2);
                }

            }
#ifdef D_SHOW_LOADING_VISUALIZATION
        }
        else //if ! D_SHOWLOADINGVISUALIZATION
        {
            /** Show loading screen here */
        }
#endif // D_SHOW_LOADING_VISUALIZATION





#ifdef D_DRAW_DEBUG_OVERLAY
        DrawDebugOverlay();
#endif // D_DRAW_DEBUG_OVERLAY

        al_flip_display();
    }
}

void TitleDrawing()
{

}

void DrawGUI()
{
    ALLEGRO_COLOR colorToDraw = NEUTRAL_WHITE;

    al_draw_bitmap(gfxTerminal,0,SCREEN_H - TERMINAL_H,0);

    for(int i = 0; i < EXCERPT_NUM_LINES; ++i)
    {
        s_al_draw_text(terminalFont,colorToDraw,
                       TERMINAL_TEXT_OPEN_X,
                       TERMINAL_TEXT_OPEN_Y-(20*i),
                       ALLEGRO_ALIGN_LEFT,
                       terminalExcerpt[i]);
    }

    std::string statText;
    statText = "turn" + std::to_string(turn);

    s_al_draw_text(terminalFont,NEUTRAL_WHITE,STATS_BAR_OPEN_X,STATS_BAR_OPEN_Y,ALLEGRO_ALIGN_LEFT,statText);

}

void DrawTiles()
{
    //Assumes 800x600 window and 120 height terminal

    int startCellX = player->xCell-14; // 13, being just about higher than 25/2 (800/32)
    if(startCellX < 0)
        startCellX = 0;

    int startCellY = player->yCell-9; // 8, being just about higher than 15/2 ((600-120)/32)
    if(startCellY < 0)
        startCellY = 0;

    int endCellX = player->xCell+14;
    if(endCellX > areaCellWidth)
        endCellX = areaCellHeight;

    int endCellY = player->yCell+9;
    if(endCellY > areaCellHeight)
        endCellY = areaCellHeight;

    for(int y = startCellY; y < endCellY; y++)
    {
        for(int x = startCellX; x < endCellX; x++)
        {
            al_draw_bitmap(gfxTiles
                           [
                               area->tilemap[y * areaCellWidth + x]
                           ],
                           x*32 + SCREEN_W/2 - player->xPosition,
                           y*32 + PLAY_H/2 - player->yPosition,
                           0);
        }
    }
}

void DrawDebugOverlay()
{
    if(mainPhase == MAIN_PHASE_GAME)
    {
        //Draw player's cell coordinates
        std::string posStr = "(" + std::to_string(player->xCell) + ", " + std::to_string(player->yCell) + ")";
        s_al_draw_text(terminalFont,al_map_rgb(255,255,255),0,0,ALLEGRO_ALIGN_LEFT,posStr);
    }
    if(mainPhase == MAIN_PHASE_LOADING)
    {
        //Draw crosshair on the screen and coordinate of crosshair
        std::string posStr = "(" + std::to_string(loadingCamX+SCREEN_W/2) + ", " + std::to_string(loadingCamY+SCREEN_H/2) + ")";
        s_al_draw_text(terminalFont,HOLY_INDIGO,0,0,ALLEGRO_ALIGN_LEFT,posStr);

        al_draw_line(SCREEN_W/2,0,SCREEN_W/2,SCREEN_H,HOLY_INDIGO,1);
        al_draw_line(0,SCREEN_H/2,SCREEN_W,SCREEN_H/2,HOLY_INDIGO,1);


    }
}
/// ########## ########## END MAIN DRAWING FUNCTIONS ########## ##########


/// ########## ########## BEGIN CLASS AND MEMBER OBJECT MANAGEMENT FUNCTIONS ########## ##########

void UpdateVectors()
{
    for(std::vector<Being*>::iterator it = beings.begin(); it != beings.end();)
    {
        //Check the derived type and call the logic function specific to its class.
        //GameLogic() just handles data that needs to be constantly updated.

        (*it)->BaseLogic();
        if((*it)->derivedType == BEING_TYPE_PLAYER)
            ((Player*)(*it))->Logic();
        else if((*it)->derivedType == BEING_TYPE_NPC)
            ((NPC*)(*it))->Logic();

        if(gameExit)
            (*it)->active = false;

        if(!(*it)->active)
        {
            //Save deletion of player for procedures in main, all others deleted
            if(!(*it)->derivedType == BEING_TYPE_PLAYER)
            {
                delete *it;
                beings.erase(it);
            }
            else
                ++it;
        }
        else
        {
            ++it;
        }
    }
}

/// ########## ########## END CLASS AND MEMBER OBJECT MANAGMEMENT FUNCTIONS ########## ##########
