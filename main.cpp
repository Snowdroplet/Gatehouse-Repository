/***

Known issues:

1)If 0 main rooms are created (i.e. while debugging with a low number of generation regions), program crashes trying to determine triangulation graph
To fix: Discard generation when < threshold main rooms are created

Current work to do:

3) Pathfinding test under actual play conditions

4) Begin implementation of combat system

10) Velocity of roomgenbox repulsion must be at least 1, plus or minus some random chaotic jiggling of velocity to prevent infinite loops.

*/


#define __USE_MINGW_ANSI_STDIO 0

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

#include "guisystem.h"
#include "terminal.h"

#include "node.h"
#include "graph.h"

#include "generator.h"

#include "area.h"

#include "item.h"
#include "being.h"
#include "player.h"
#include "npc.h"

GuiSystem *guiSystem = nullptr;
Generator *generator = nullptr;
Area *area = nullptr;
Player *player = nullptr;

std::vector<Being*>beings; // All beings currently in play.
std::vector<Being*>actionQueue; // All beings queued to act.
std::vector<Being*>walkAnimQueue; // All beings queued to animate walk.
Being*currentAnimatedBeing = nullptr; // Which being is being animated, if an action other than walking is being animated.

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
    srand(time(nullptr));

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

    guiSystem = new GuiSystem();

    generator = new Generator(); // Create a generator object


#ifdef D_CREATE_TESTING_AREA
    area = new Area(); // Create random test area
#endif

#ifndef D_CREATE_TESTING_AREA


    area = new Area(true); // Initialization of basic area properties normally unecessary because they will be deserialized from .areafile
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

    /// Save data
    if(area != nullptr)
        SaveAreaState(area);

    if(player != nullptr)
        SavePlayerState(player);


    if(player != nullptr)
        delete player;

    delete area;
    delete generator;

    delete guiSystem;

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

    static bool walkAnimQueueReleased = false;
    static bool checkAnimationPhaseComplete = true;

    static std::vector<Being*>::iterator aqit; // Static action queue iterator meant to be the front element of actionQueue.

    if(ev.type == ALLEGRO_EVENT_TIMER)
    {
        if(awaitingPlayerCommand)
        {

            player->ProcessInput();

#ifdef D_TEST_TRACEPATH

            if(keyInput[KEY_U] && keyInput[KEY_I])
            {
                if(!player->currentPath.empty())
                {
                    player->actionCost = 100;
                    player->currentAction = ACTION_WALK;
                    player->TracePath();
                    submittedPlayerCommand = true;
                }
            }
#endif // D_TEST_TRACEPATH

#ifdef D_TEST_PATHFINDING
            static int debugPathRequestDelay = 0;

            if(keyInput[KEY_T] && keyInput[KEY_Y])
            {
                if(debugPathRequestDelay <= 0)
                {
                    player->ResetPath();

                    int testDestX = area->upstairsXCell;
                    int testDestY = area->upstairsYCell;

                    std::cout << std::endl;
                    std::cout << "####DEBUG PATH REQUEST####" << std::endl;
                    std::cout << "Start X: " << player->xCell << " | Start Y: " << player->yCell << std::endl;
                    std::cout << "Dest X: " << testDestX << " | Dest Y: " << testDestY << std::endl;

                    player->SetPath(testDestX,testDestY);

                    std::cout << "Path retrieved:" << std::endl;

                    for(std::vector<Node*>::reverse_iterator rit = player->currentPath.rbegin(); rit != player->currentPath.rend(); ++rit)
                    {
                        std::cout << "(" << (*rit)->id%areaCellWidth << ", " << (*rit)->id/areaCellHeight << ") ";
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

/** ### 0: Update objects that constantly need updates
    -Such as elements of the GUI.
**/
    guiSystem->ProgressElements();



/** ### 1: GRANT ACTION POINTS #####
    -Each being receives AP according to its effective speed.
    -Beings that have accumulated at least 100 AP are permitted to move. These beings are added to actionQueue.
**/

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

/** ### 2: SORT ACTION QUEUE ###
    -All Beings on the action queue (having, from the previous phase, at least 100 AP) are sorted by their AP from highest to lowest AP (descending order).
**/

    if(turnLogicPhase == SORT_ACTION_QUEUE)
    {
        std::sort(actionQueue.begin(), actionQueue.end(), CompareAPDescending);
        aqit = actionQueue.begin();
        turnLogicPhase = SELECT_ACTION;
    }

/** ### 3: SELECT ACTION ###

    actionQueue: A vector populated during phase 1 with pointers to Beings eligible to take an action (having at least 100 AP).
    actionOpen: Whether the system is ready to receive actions, independent of whether actionQueue is populated.

    walkAnimQueue: A vector populated during this phase with pointers to beings who have chosen the WALK action.
                   In order to minimize real time spent waiting for each individual Being's walk from one cell to the next to be animated at a certain speed,
                   this queue allows for all movement animations to be displayed simultaneously.
    walkAnimQueueReleased: A boolean. When any unit performs an action other than walking OR when actionQueue is emptied of Beings,
                           all walking animations queued to this point are animated.

    -If the action queue is NOT empty (currently containing Beings who have least 100 AP):
        -If actionOpen is true (system is ready to receive actions)
            * note: the "idle" action is equivalent to moving in AP usage.
            - NPCs choose an action according to their AI. A certain amount of AP is spent corresponding to the action.
            - Player Beings await input from the player - When input is detected corresponding to an action, a certain amount of AP is spent corresponding to the action.
            - In either case, actionOpen becomes false (system may not receive actions) until the action has been processed during the CALCULATION phase.


            - Any Being that has selected the WALK action will be added to the walk animation queue (walkAnimQueue).
            - Any Being that selects an option other than WALK will release the global walk animation queue.

            - Any Being who becomes ineligible to move (having less than 100 AP) will be removed from the actionQueue.

            WHEN ANY ACTION HAS BEEN CONFIRMED (!actionOpen) whether by NPC or Player, go to ANIMATION phase. No further actions may be taken until any and all necessary animations have been processed.


    -If the action queue is empty, the cycle of phases is completed and returns to GRANT AP.
**/

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

                    if((*aqit)->currentAction == ACTION_WALK)
                        walkAnimQueue.push_back(*aqit);
                    else if((*aqit)->currentAction != ACTION_IDLE)
                    {
                        walkAnimQueueReleased = true;
                        currentAnimatedBeing = *aqit;
                        actionOpen = false;
                    }
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

                if(player->currentAction == ACTION_WALK)
                {
                    walkAnimQueue.push_back(player);
                    //walkAnimQueueReleased = true;
                    /// Shouldn't walkanimqueue be released whenever the player moves,
                    /// in case a player with AP greatly outnumbering the next highest being fails to see an npc's updated position?
                }
                else if(player->currentAction != ACTION_IDLE)
                {
                    walkAnimQueueReleased = true;
                    currentAnimatedBeing = player;
                }

                ShiftTerminal(); // This might have to be moved to be triggered by different conditions later on.
            }

            //It is possible for a being to have its AP reduced (i.e. crowd control slow) while in the actionQueue and thus become ineligible to move.
            //Remove front element from action queue if it does not have enough AP (100).
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

        if(actionQueue.empty())
        {
            walkAnimQueueReleased = true;
        }

        //If a player action/AI option has been confirmed, no further actions may be processed until its animation is complete.
        if(!actionOpen)
            turnLogicPhase = ANIMATION;
    }

    if(turnLogicPhase == ANIMATION)
    {
        checkAnimationPhaseComplete = false;

        //Progress the IDLE animation of all beings by one step.
        //Note that all beings will go through this (possibly hidden to the player) progression in idle animation (even) when they are not queued to act or animate.
        for(std::vector<Being*>::iterator it = beings.begin(); it != beings.end(); ++it)
            (*it)->ProgressIdleAnimation();


        if(currentAnimatedBeing != nullptr)
        {
            if(currentAnimatedBeing->animationComplete)
            {
                checkAnimationPhaseComplete = true;
                currentAnimatedBeing = nullptr;
            }
            else
            {
                // **Progress the animation of being in question here**
            }
        }
        else if(walkAnimQueueReleased)
        {
            for(std::vector<Being*>::iterator it = walkAnimQueue.begin(); it != walkAnimQueue.end();)
            {
                if(abs((*it)->xCell - player->xCell) <= drawingXCellCutoff &&
                   abs((*it)->yCell - player->yCell) <= drawingYCellCutoff &&
                   (*it)->visibleToPlayer) // No point in animating something outside the screen boundaries or is invisible/obscured/imperceptible/off-screen.
                   {
                       (*it)->ProgressWalkAnimation();
                        if((*it)->animationComplete)
                        {
                            walkAnimQueue.erase(it);
                        }
                        else
                            ++it;
                   }
                else // Animation of this being is auto-completed if invisible/obscured/imperceptible/off-screen.
                {
                    (*it)->CompleteWalkAnimation();
                    walkAnimQueue.erase(it);
                }
            }

            if(walkAnimQueue.empty())
            {
                checkAnimationPhaseComplete = true;
                walkAnimQueueReleased = false;
            }
        }

        //Move on to calculation if the phase is complete, otherwise reset the flag
        if(checkAnimationPhaseComplete)
        {
            turnLogicPhase = CALCULATION;
        }
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
            loadingCamX += 4;
        if(keyInput[KEY_PAD_2])
            loadingCamY += 4;
        if(keyInput[KEY_PAD_4])
            loadingCamX -= 4;
        if(keyInput[KEY_PAD_8])
            loadingCamY -= 4;
#ifdef D_TERMINATE_LOADING_SIGNAL
        if(keyInput[KEY_Z])
            D_TERMINATELOADINGPHASESIGNAL = true;  // Obviously only for developmental purposes since Area::Generate is incomplete.
#endif // D_TERMINATE_LOADING_SIGNAL

#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
        if(keyInput[KEY_Q] && D_PROGRESSPAUSEDVISUALIZATIONTIMER == 0)
        {
            generator->D_UNPAUSE_VISUALIZATION();
            D_PROGRESSPAUSEDVISUALIZATIONTIMER = 50;
        }

#endif // D_GEN_VISUALIZATION_PHASE_PAUSE

    }

#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
    if(D_PROGRESSPAUSEDVISUALIZATIONTIMER > 0)
        D_PROGRESSPAUSEDVISUALIZATIONTIMER --;
#endif // D_GEN_VISUALIZATION_PHASE_PAUSE


    /// ** There is a design bug here: 1) needGeneration is a global in gamesystem and can be botched if generators are running in the background.
    ///    Should make this more nuanced. Maybe a vector of needGenerations for each level.
    if(needGeneration)
    {
#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
        if(!generator->D_GET_GENERATOR_VISUALIZATION_PAUSE())
#endif // D_GEN_VISUALIZATION_PHASE_PAUSE
        {
            generator->Generate();
            if(generator->GetGenerationComplete())
                needGeneration = false;
        }
    }
    else // !needGeneration
    {
        // What is produced by the generator becomes the area.

        area->occupied = generator->occupied;
        area->floormap = generator->floormap;
        area->wallmap  = generator->wallmap;
        area->featuremap = generator->featuremap;
        area->floormapImageCategory   = generator->floormapImageCategory;
        area->wallmapImageCategory    = generator->wallmapImageCategory;
        area->featuremapImageCategory = generator->featuremapImageCategory;
        area->floormapImageIndex      = generator->floormapImageIndex;
        area->wallmapImageIndex       = generator->wallmapImageIndex;
        area->featuremapImageIndex    = generator->featuremapImageIndex;

        area->downstairsXCell         = generator->downstairsXCell;
        area->downstairsYCell         = generator->downstairsYCell;
        area->upstairsXCell           = generator->upstairsXCell;
        area->upstairsYCell           = generator->upstairsYCell;

        /// ********Set Being initial positions here **********
        // Perhaps by creating and copying from a vector of initial beings?
        // Such as generator->beingSpawnpositions[blah];

        player = new Player(area->downstairsXCell,area->downstairsYCell);
        //player = new Player(true); //true to initalize saved player - Remove in end for atrium map
        //LoadPlayerState("player",player);
        //player->InitByArchive();

        //End testing

        beings.push_back(player);

        beings.push_back(new NPC(SLIME,area->upstairsXCell,area->upstairsYCell));
        beings.push_back(new NPC(QUICKLING,area->upstairsXCell,area->upstairsYCell));

        // Release memory
        generator->ReleaseOutputContainers();


        // Loading complete! Go to main game.
        mainPhase = MAIN_PHASE_GAME;
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

        al_draw_bitmap_region(gfxPlayer,
                              TILESIZE*player->animationFrame, 0,
                              TILESIZE, TILESIZE,
                              SCREEN_W/2, SCREEN_H/2, 0);

        for(std::vector<Being*>::iterator it = beings.begin(); it != beings.end(); ++it)
        {
            if((*it)->derivedType == BEING_TYPE_NPC)//As opposed to player, which is drawn seperately
            {
                if((*it)->animationState == ANIM_IDLE)
                    al_draw_bitmap(gfxNPCPassive[(*it)->spriteID + (*it)->animationFrame],
                                   (*it)->xPosition + SCREEN_W/2 - player->xPosition,
                                   (*it)->yPosition + SCREEN_H/2 - player->yPosition,
                                   0);
            }
        }

        DrawGUI();

#ifdef D_DRAW_DEBUG_OVERLAY
        DrawDebugOverlay();
#endif // D_DRAW_DEBUG_OVERLAY

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
            if(generator->GetGenerationPhase() >= GEN_LAYOUT_FLOOR_SKELETON) // Draw from cell layout phase onwards
            {
                for(int y = 0; y < areaCellHeight; y++)
                {
                    for(int x = 0; x < areaCellWidth; x++)
                    {
                        switch(generator->genLayout[y*areaCellWidth+x])
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
                        case GEN_CELL_WALL_SKELETON:
                            al_draw_filled_rectangle(x*MINI_TILESIZE                 - loadingCamX,
                                                     y*MINI_TILESIZE                 - loadingCamY,
                                                     x*MINI_TILESIZE + MINI_TILESIZE - loadingCamX,
                                                     y*MINI_TILESIZE + MINI_TILESIZE - loadingCamY,
                                                     DARK_VIOLET);
                            break;
                        }
                    }
                }
            }

            ///Draw room generation boxes
            if(generator->GetGenerationPhase() >= GEN_SEPARATION) // Draw from physics simulation onwards
            {
                for(std::vector<RoomGenBox*>::iterator it = generator->roomGenBoxes.begin(); it != generator->roomGenBoxes.end(); ++it)
                {

                    // Concerning generationPhase == GEN_SEPARATION, but always active.
                    // Draws outlines of room objects. Orange denotes that the room has overlaps. Blue denotes that the room does not have overlaps.


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


                    s_al_draw_text(terminalFont, NEUTRAL_WHITE,
                                   (*it)->x1-loadingCamX+4,
                                   (*it)->y1-loadingCamY+4,
                                   ALLEGRO_ALIGN_LEFT,
                                   std::to_string(it-generator->roomGenBoxes.begin()));

                }
            }


            /// Draw the room connection graph
            if(generator->GetGenerationPhase() == GEN_TRIANGULATION || generator->GetGenerationPhase() == GEN_MST) //Draw up to MST
            {
                // Draw the graph created by delaunay triangulation
                for(std::vector<TriEdge>::iterator it = generator->triEdges.begin(); it != generator->triEdges.end(); ++it)
                {
                    al_draw_line((*it).p1.x        -loadingCamX,
                                 (*it).p1.y        -loadingCamY,
                                 (*it).p2.x        -loadingCamX,
                                 (*it).p2.y        -loadingCamY,
                                 DARK_VIOLET,2);
                }

            }
            else if(generator->GetGenerationPhase() >= GEN_MST && generator->GetGenerationPhase() <= GEN_LAYOUT_FLOOR_SKELETON)
            {
                // Draw the graph created the the minimum spanning tree/ re-addition of edges.
                for(std::vector<TriEdge>::iterator it = generator->demoEdges.begin(); it != generator->demoEdges.end(); ++it)
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
    guiSystem->DrawFrame();

    ALLEGRO_COLOR colorToDraw = NEUTRAL_WHITE;

    // Draw the (now unused)
    //al_draw_bitmap(gfxTerminal,0,SCREEN_H - TERMINAL_H,0);

    // Draw the text that would go in the terminal (make a better terminal later)
    for(int i = 0; i < EXCERPT_NUM_LINES; ++i)
    {
        s_al_draw_text(terminalFont,colorToDraw,
                       TERMINAL_TEXT_OPEN_X,
                       TERMINAL_TEXT_OPEN_Y-(20*i),
                       ALLEGRO_ALIGN_LEFT,
                       terminalExcerpt[i]);
    }

    // Draw turn counter (put it in an appropriate place later)
    std::string statText;
    statText = "turn" + std::to_string(turn);
    s_al_draw_text(terminalFont,NEUTRAL_WHITE,STATS_BAR_OPEN_X,STATS_BAR_OPEN_Y,ALLEGRO_ALIGN_LEFT,statText);

}

void DrawTiles()
{
    //Assumes 800x600 window and 120 height terminal

    int startCellX = player->xCell-drawingXCellCutoff;
    if(startCellX < 0)
        startCellX = 0;

    int startCellY = player->yCell-drawingYCellCutoff;
    if(startCellY < 0)
        startCellY = 0;

    int endCellX = player->xCell+drawingXCellCutoff;
    if(endCellX > areaCellWidth)
        endCellX = areaCellHeight;

    int endCellY = player->yCell+drawingYCellCutoff;
    if(endCellY > areaCellHeight)
        endCellY = areaCellHeight;

    for(int y = startCellY; y < endCellY; y++)
    {
        for(int x = startCellX; x < endCellX; x++)
        {

            int cellIndex = y*areaCellWidth+x;

            /// Draw the floor
            if(area->floormap[cellIndex] != FT_FLOOR_EMPTY) // Floor exists on this cell
            {
                int floorRegionDrawX = area->floormapImageIndex[cellIndex]%FLOOR_TILE_SHEET_CELLWIDTH*TILESIZE;
                int floorRegionDrawY = FLOOR_TILE_SHEET_CELLHEIGHT_PER_CATEGORY * area->floormapImageCategory[cellIndex] * TILESIZE
                                       +
                                       area->floormapImageIndex[cellIndex]/FLOOR_TILE_SHEET_CELLWIDTH*TILESIZE;


                al_draw_bitmap_region(gfxFloorTiles,
                                      floorRegionDrawX,
                                      floorRegionDrawY,
                                      TILESIZE,
                                      TILESIZE,
                                      x*TILESIZE + SCREEN_W/2 - player->xPosition,
                                      y*TILESIZE + SCREEN_H/2 - player->yPosition,
                                      0);
            }

            /// Draw the walls
            if(area->wallmap[cellIndex] != WT_WALL_EMPTY) // Wall exists on this cell
            {
                int wallRegionDrawX = area->wallmapImageIndex[cellIndex]%WALL_TILE_SHEET_CELLWIDTH*TILESIZE;
                int wallRegionDrawY = WALL_TILE_SHEET_CELLHEIGHT_PER_CATEGORY * area->wallmapImageCategory[cellIndex] * TILESIZE
                                      +
                                      area->wallmapImageIndex[cellIndex]/WALL_TILE_SHEET_CELLWIDTH*TILESIZE;

                al_draw_bitmap_region(gfxWallTiles,
                                      wallRegionDrawX,
                                      wallRegionDrawY,
                                      TILESIZE,
                                      TILESIZE,
                                      x*TILESIZE + SCREEN_W/2 - player->xPosition,
                                      y*TILESIZE + SCREEN_H/2 - player->yPosition,
                                      0);
            }

            /// Draw level features and furniture
            if(area->featuremap[cellIndex] != FEATURE_EMPTY) // Feature exists on this cell
            {
                //std::cout << "cellIndex: " << cellIndex << ", feature: " << area->featuremap[cellIndex] << std::endl;

                int featureRegionDrawX = area->featuremapImageIndex[cellIndex]%FEATURE_TILE_SHEET_CELLWIDTH*TILESIZE;
                int featureRegionDrawY = FEATURE_TILE_SHEET_CELLHEIGHT_PER_CATEGORY * area->featuremapImageCategory[cellIndex] * TILESIZE
                                         +
                                         area->featuremapImageIndex[cellIndex]/FEATURE_TILE_SHEET_CELLWIDTH*TILESIZE;

                al_draw_bitmap_region(gfxFeatureTiles,
                                      featureRegionDrawX,
                                      featureRegionDrawY,
                                      TILESIZE,
                                      TILESIZE,
                                      x*TILESIZE + SCREEN_W/2 - player->xPosition,
                                      y*TILESIZE + SCREEN_H/2 - player->yPosition,
                                      0);



            }
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
        int crosshairX = loadingCamX+SCREEN_W/2;
        int crosshairY = loadingCamY+SCREEN_H/2;
        int crosshairXCell = crosshairX/MINI_TILESIZE;
        int crosshairYCell = crosshairY/MINI_TILESIZE;
        std::string posStr = "(" + std::to_string(crosshairX) + ", " + std::to_string(crosshairY) + ") : (" + std::to_string(crosshairXCell) + ", " + std::to_string(crosshairYCell) + ")";
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
