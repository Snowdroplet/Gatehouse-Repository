/***
Error:
    NPC sprite drawing position updates too slow.
    Const char al_draw_text flickering/corruption in inventory and pstat GUIs
    ! Fix sporadic infinite looping in distributing generation bodies

To optimize later:
    ! Restructure so that UpdateObjects' non-animation functions
        (esp. iterating through all vectors) is only called upon when "stuff" happens
        checking every logic cycle seems a waste of CPU

    ! Being movement is disjointed; not smooth and continuous. Find out why and fix

*/

/***
How to operate during generator debug visualization:

    Q = Progress generation
    Numpad = move camera

How to operate debug:
    TY = Output test path from player to random cell
    UI = Move player to last cell on path

    numpad = move
    12345 = open/close/switch inventories
    c = open character stat window
    Shift+c = exit character stat window
    l = toggle lock-on context
    z = weapon stance context
    shift + z = close weapon stance context
*/


#define __USE_MINGW_ANSI_STDIO 0

#include <cstdio>
#include <iostream>
#include <cstring>
#include <ctime>
#include <cmath>
#include <vector>

//#include <boost/filesystem.hpp>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_physfs.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <physfs.h>

#include "allegrocustom.h"

#include "gamesystem.h"

#include "guisystem.h"

#include "control.h"
#include "resource.h"

//#include "extfile.h"

#include "terminal.h"

#include "node.h"
#include "graph.h"

#include "generator.h"

#include "area.h"

#include "spell.h"

#include "item.h"
#include "equip.h"
#include "tool.h"
#include "magic.h"
#include "material.h"
#include "key.h"
#include "misc.h"

#include "being.h"
#include "player.h"
#include "npc.h"

/// Single-object classes ###########
Generator *generator = nullptr;
Area *area = nullptr;
Player *player = nullptr;

/// Item containers and functions ##################
std::vector<Item*>areaItems; // All items currently in play in the current area.
Item*viewedItem = nullptr; // Item being viewed in inventory selection.
int viewedItemPosition;

void PopulateItems(); // Fill items vector with all items currently held by all beings and in play.

void DevAddTestItemsToPlayer(); // In development...

/// Spell containers and functions #################
std::vector<Spell*>activeSpells;

/// Being containers and functions #################
std::vector<Being*>beings; // All beings currently in play.
std::vector<Being*>actionQueue; // All beings queued to act.
std::vector<Being*>walkAnimQueue; // All beings queued to animate walk.

Being*spotlightedBeing = nullptr; // If an action other than walking/idling is being animated, only one such action can be animated at a time.

std::vector<Being*>targetableBeings; // All beings which can be locked on to during targeting context.
Being*hardTargetedBeing = nullptr;  // Which being is currently under explicit lock-on.
Being*autoTargetedBeing = nullptr; // Which being is currently under auto-lock on. Will match hardTargetedBeing unless it is nullptr.

bool CompareAPDescending(Being *lhs, Being *rhs)
{
    return lhs->actionPoints > rhs->actionPoints;
}

bool CompareTargetableDistanceAscending(Being *lhs, Being *rhs)
{
    return sqrt(pow(lhs->yCell - playerYCell, 2) + pow(lhs->xCell - playerXCell, 2)) <  sqrt(pow(rhs->yCell - playerYCell, 2) + pow(rhs->xCell - playerXCell, 2));
}

void PopulateTargetableBeings();
Being* AcquireAutoTarget();

/// Main loop functions ##############################
void GameLogic();
void LoadingLogic();
void TitleLogic();

void GameDrawing();
void LoadingDrawing();
void TitleDrawing();

void DrawGUI();
void DrawTiles();
void DrawDebugOverlay();

void UpdateObjects(); //Update elements and delete elements that have been deactivated
void ProcessInput(int whatContext); // Merge with control module once it is possible to put "player" in gamesystem
void UpdateGamesystem(); // Merge with gamesystem once it is possible to put "player" in gamesystem


/// MAIN() ############################################
int main(int argc, char *argv[])
{
    srand(time(nullptr));

    //Replace fprintf with native dialogue later
    if(!al_init())
    {
        fprintf(stderr, "failed to initialize allegro!\n");
        return false;
    }
    if(!al_install_keyboard())
    {
        fprintf(stderr, "failed to install keyboard!\n");
        return false;
    }
    if(!al_install_mouse())
    {
        fprintf(stderr, "failed to install mouse!\n");
        return false;
    }
    if(!al_init_image_addon())
        return false;
    if(!al_init_primitives_addon())
        return false;

    al_init_font_addon();

    if(!al_init_ttf_addon())
        return false;

    if(!PHYSFS_init(argv[0]))
        return false;

    if(!PHYSFS_mount("./gamedata.zip", "/", 1))
    {
        fprintf(stderr, "gamedata not found!\n");
        return false;
    }

    al_set_physfs_file_interface();

    AllegroCustomColours();

    FPStimer = al_create_timer(1.0/FPS);
    if(!FPStimer)
    {
        fprintf(stderr, "failed to create fpstimer!\n");
        return false;
    }

    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display)
    {
        fprintf(stderr, "failed to create display!\n");
        al_destroy_timer(FPStimer);
        return false;
    }

    eventQueue = al_create_event_queue();
    if(!eventQueue)
    {
        fprintf(stderr, "failed to create event queue!\n");
        al_destroy_display(display);
        al_destroy_timer(FPStimer);
        return false;
    }

    al_register_event_source(eventQueue, al_get_display_event_source(display));
    al_register_event_source(eventQueue, al_get_timer_event_source(FPStimer));
    al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_mouse_event_source());

    LoadResources();

    GuiInit();

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

        if(ev.type == ALLEGRO_EVENT_TIMER || gameExit == true)
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

    /*
    if(area != nullptr)
        SaveAreaState(area);

    if(player != nullptr)
        SavePlayerState(player);

    */

    /// Cleanup
    if(player != nullptr)
        delete player;

    delete area;
    delete generator;

    GuiDeinit();

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
    static bool walkAnimQueueReleased = false;
    static bool animationPhaseComplete = false;

    static std::vector<Being*>::iterator actionQueueFront; // Doesn't actually iterate. Merely indexes the front element of actionQueue.

    /** ### 0.1: Update objects that constantly need updates ####
        -Such as elements of the GUI.
    **/

    UpdateGamesystem();

    GuiUpdateElements();

/// ### 0.2: Receive input, interpret and process according to context ####


    if(ev.type == ALLEGRO_EVENT_TIMER)
    {

        if(awaitingPlayerActionCommand)
        {
            ProcessInput(controlContext);

#ifdef D_TEST_TRACEPATH

            if(keyInput[KEY_U] && keyInput[KEY_I])
            {
                if(!player->currentPath.empty())
                {
                    player->actionCost = 100;
                    player->currentAction = ACTION_WALK;
                    player->TracePath();
                    submittedPlayerActionCommand = true;
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
                    std::cout << "Start X: " << playerXCell << " | Start Y: " << playerYCell << std::endl;
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



    /** ### 1: GRANT ACTION POINTS #####
        -Each being receives AP according to its effective speed.
        -Beings that have accumulated at least 100 AP are permitted to move. These beings are added to actionQueue.
    **/

    if(turnLogicPhase == LOGIC_PHASE_GRANT_ACTION_POINTS)
    {
        turn ++; // Increment (currently cosmetic) global turn counter
#ifdef D_TURN_LOGIC
        std::cout << "### TURN " << turn << " ###" << std::endl;
        std::cout << "Granting AP:" << std::endl;
#endif

        for(std::vector<Being*>::iterator it = beings.begin(); it != beings.end(); ++it)
        {
            (*it)->actionPoints += (*it)->secondary[STAT_WALK_SPEED][BEING_STAT_EFFECTIVE]; // Each being receives AP according to its effective speed.
#ifdef D_TURN_LOGIC
            std::cout << (*it)->name << " has " << (*it)->actionPoints << "/" << (*it)->secondary[STAT_WALK_SPEED][BEING_STAT_EFFECTIVE]; << " AP." << std::endl;
#endif

            if((*it)->actionPoints >= 100) // If a being has at least 100 AP, add it to actionQueue.
            {
#ifdef D_TURN_LOGIC
                std::cout << "(Added to action queue)" << std::endl;
#endif
                actionQueue.push_back(beings[it-beings.begin()]);
            }
        }

        turnLogicPhase = LOGIC_PHASE_SORT_ACTION_QUEUE;
    }

    /** ### 2: SORT ACTION QUEUE ###
        -All Beings on the action queue (having, from the previous phase, at least 100 AP) are sorted by their AP from highest to lowest AP (descending order).
    **/

    if(turnLogicPhase == LOGIC_PHASE_SORT_ACTION_QUEUE)
    {
        std::sort(actionQueue.begin(), actionQueue.end(), CompareAPDescending); // Sort by descending order of AP.
        actionQueueFront = actionQueue.begin();
        turnLogicPhase = LOGIC_PHASE_SELECT_ACTION;
    }

    /** ### 3: SELECT ACTION ###

        actionQueue: A vector populated during phase 1 with pointers to Beings eligible to take an action (having at least 100 AP).
        gameLogicActionOpen: Whether the system is ready to receive actions, independent of whether actionQueue is populated.

        walkAnimQueue: A vector populated during this phase with pointers to beings who have chosen the WALK action.
                       In order to minimize real time spent waiting for each individual Being's walk from one cell to the next to be animated,
                       this queue allows for all WALK animations to be displayed simultaneously.
        walkAnimQueueReleased: A boolean. When any unit performs an action other than walking OR when actionQueue is emptied of Beings,
                               all walking animations queued to this point are animated (to the user, simultaneously).

        -If the action queue is NOT empty (currently containing Beings who have least 100 AP):
            -If gameLogicActionOpen is true (system is ready to receive actions)
                - NPC beings choose an action according to their AI. A certain amount of AP is spent corresponding to the action. (note that the "idle" action costs AP equivalent to moving)
                - PLAYER Beings await input from the player - When input is detected corresponding to an action, a certain amount of AP is spent corresponding to the action.
                - In either case, gameLogicActionOpen becomes false (system may not receive actions) until the action has been processed during the PROCESSING phase.


                - Any Being that has selected the WALK action will be added to the walk animation queue (walkAnimQueue).
                - Any Being that selects an option other than WALK will release the global walk animation queue.
                - Any Being who becomes ineligible to move (having less than 100 AP) will be removed from the actionQueue.


        IF  A) an action is taken which releases walkAnimQueue
            B) actionQueue is emptied (which releases walkAnimQueue)
            C) gameLogicActionOpen = false. (Note that gameLogicActionOpen refers to both player and NPC actions).

        Then go to ANIMATION phase. Actions must not be allowed (gameLogicActionOpen = false) until any and all necessary animations have been completed.

        -If the action queue is empty, the cycle of phases is completed and returns to GRANT AP.
    **/

    if(turnLogicPhase == LOGIC_PHASE_SELECT_ACTION)
    {
        if(!actionQueue.empty()) // There are Beings (with >= 100 AP) queued to move.
        {
            if(gameLogicActionOpen && !awaitingPlayerActionCommand) //gameLogicActionOpen should be true if the player/NPC in queue to move has not yet chosen a player action or AI option yet. (AI should be processed instantaneously though).
            {
/// ** Two versions of the similar logic below: One for NPC and one for player. ///////////////////////
                if((*actionQueueFront)->derivedType == BEING_TYPE_NPC) // If being is an NPC (non-player), use the NPC version of the logic.
                {
                    //It is possible for a being to have its AP reduced below the minimum to move even while queued to move, e.g. through spells with certain crowd control effects.
                    if((*actionQueueFront)->actionPoints < 100)
                        (*actionQueueFront)->actionBlocked = true; // And if blocked, the being in question may only choose to wait.

#ifdef D_TURN_LOGIC
                    std::cout << (*actionQueueFront)->name << " executes its AI: ";
#endif

                    ((NPC*)(*actionQueueFront))->AI();

#ifdef D_TURN_LOGIC
                    std::cout << (*actionQueueFront)->actionName << std::endl;
#endif
                    (*actionQueueFront)->actionPoints -= (*actionQueueFront)->actionCost; // Spend AP cost of action. It is very possible for moves to put beings into negative AP (e.g. power-attacks)

#ifdef D_TURN_LOGIC
                    std::cout << (*actionQueueFront)->name << " now has " << (*actionQueueFront)->actionPoints << "/" << (*actionQueueFront)->secondary[STAT_WALK_SPEED][BEING_STAT_EFFECTIVE]; << "AP" << std::endl;
#endif

                    if((*actionQueueFront)->currentAction == ACTION_WALK) // If the selected action is walk, add to the walk animation queue.
                        walkAnimQueue.push_back(*actionQueueFront);
                    else if((*actionQueueFront)->currentAction != ACTION_IDLE) // If the selected action is OTHER THAN walk, but NOT idle, release animation queue.
                    {
                        walkAnimQueueReleased = true;
                        spotlightedBeing = *actionQueueFront;
                        gameLogicActionOpen = false; // System becomes closed to action because it must first process the consequences of action.
                    }
                }
                else if((*actionQueueFront)->derivedType == BEING_TYPE_PLAYER) // If being is a player (not NPC), use the player version of this logic.
                {
                    //It is possible that the player's AP is reduced below the minimum to move even while queued to move, e.g. through spells with crowd control effects.
                    if((*actionQueueFront)->actionPoints < 100)
                        (*actionQueueFront)->actionBlocked = true; // If blocked in such a way, the player may only wait.

                    awaitingPlayerActionCommand = true;
                }
/// ** ///////////////////////////////////////////////////////////////////////////////////////////
            }

/// **  The following code concerns dealing with submitted user commands ///////////////////
            if(awaitingPlayerActionCommand)
            {
                walkAnimQueueReleased = true;

                if(submittedPlayerActionCommand)
                {
#ifdef D_TURN_LOGIC
                    std::cout << player->name << " executes your command" << std::endl;
#endif
                    player->actionPoints -= player->actionCost;
#ifdef D_TURN_LOGIC
                    std::cout << player->name << " now has " << player->actionPoints << "/" << player->secondary[STAT_WALK_SPEED][BEING_STAT_EFFECTIVE]; << "AP" << std::endl;
#endif

                    // Reset flags to default, false.
                    awaitingPlayerActionCommand = false;
                    submittedPlayerActionCommand = false;
                    gameLogicActionOpen = false;

                    if(player->currentAction == ACTION_WALK) // If the selected action is walk, add to the walk animation queue.
                    {
                        player->Move(player->intendedWalkDirection);
                        walkAnimQueue.push_back(player);
                    }
                    else if(player->currentAction != ACTION_IDLE) // If the selected action is OTHER THAN WALK, but NOT idle...
                    {
                        spotlightedBeing = player;
                        walkAnimQueueReleased = true;
                    }


                    ShiftTerminal(); // This might have to be moved to be triggered by different conditions later on.
                }
            }
/// ** ////////////////////////////////////////////////////////////////////////////////////

            //Remove front element from action queue if it does not have enough AP (100) to continue moving.
            if((*actionQueueFront)->actionPoints < 100)
            {
#ifdef D_TURN_LOGIC
                std::cout << "Erased " << (*actionQueueFront)->name << " from movequeue" << std::endl;
#endif
                actionQueue.erase(actionQueueFront);
            }
        }
        else // if actionQueue.empty() returned true
        {
            //std::cout << "Error if repeats endlessly" << std::endl;
            turnLogicPhase = LOGIC_PHASE_GRANT_ACTION_POINTS;
        }

        if(actionQueue.empty())
        {
            //std::cout << "Walk anim queue is empty" << std::endl;
            walkAnimQueueReleased = true; // If there are no beings to queue actions for, might as well release walk animation queue.
        }

        //If a player action/AI option has been confirmed, no further actions may be processed until its animation is complete.
        if(!gameLogicActionOpen)
        {
            turnLogicPhase = LOGIC_PHASE_ANIMATION;

        }

    }

    /** ### 4: ANIMATION  ###
        -Progress the idle animation of all beings.
        -Update the drawing position of all beings that performed the walk action.

        -If the walkAnimQueue is NOT EMPTY, progress relevant beings' walk animations.
        -If the walkAnimQueue IS EMPTY...
            1) spotlightedBeing is animated (not necessarily the being at the front of the actionQueue, e.g. free auto-retaliate skill).
            2) If there is no currently spotlighted being performing a non-walk action, phase completes.
    **/
    if(turnLogicPhase == LOGIC_PHASE_ANIMATION)
    {
        animationPhaseComplete = false; // Reset flag.

        //Progress the IDLE animation of all beings by one step.
        //Note that all beings will go through this (possibly hidden to the player) progression in idle animation (even) when they are not queued to act or animate.
        for(std::vector<Being*>::iterator it = beings.begin(); it != beings.end(); ++it)
        {
            (*it)->ProgressIdleAnimation();
        }
        if(spotlightedBeing != nullptr) // There IS a being being spotlighted.
        {
//std::cout << "Being spotlighted." << std::endl;
            if(spotlightedBeing->animationComplete) // The spotlighted being has completed its animation
            {
                animationPhaseComplete = true; // Move on to Processing phase.
                spotlightedBeing = nullptr; // Reset pointer.
            }
            else // Animation of spotlightedBeing is incomplete.
            {
                /// **Progress the animation of being in question here**
            }
        }
        else // if(spotlightedBeing == nullptr) // There IS NOT a being being spotlighted.
        {
//std::cout << "No being spotlighted." << std::endl;
            if(walkAnimQueueReleased)
            {
                for(std::vector<Being*>::iterator it = walkAnimQueue.begin(); it != walkAnimQueue.end();)
                {
                    // Being is NOT offscreen and is visible to player.
                    if(abs((*it)->xCell - playerXCell) <= drawingXCellCutoff
                            && abs((*it)->yCell - playerYCell) <= drawingYCellCutoff
                            && (*it)->visibleToPlayer)
                    {
                        // Progress walk animation by one step. If this progression completes its animation, erase it from walkAnimQueue.
                        (*it)->ProgressWalkAnimation();
                        if((*it)->animationComplete)
                        {
                            walkAnimQueue.erase(it);
                        }
                        else // Walk animation not complete
                            ++it; // Being stays in queue, iterate to next being in queue.
                    }
                    else // Animation of this being is auto-completed if offscreen or invisible to player.
                    {
                        (*it)->InstantCompleteWalkAnimation();
                        walkAnimQueue.erase(it);
                    }
                }

                if(walkAnimQueue.empty())
                {
                    animationPhaseComplete = true; // Flag animation phase is complete.
                    walkAnimQueueReleased = false; // Reset flag.
                }
            }

        }

        //Move on to processing phase if the animation phase is complete.
        if(animationPhaseComplete)
        {
            turnLogicPhase = LOGIC_PHASE_PROCESSING;
        }
    }


    /** ### 5: PROCESSING ###

    **/

    if(turnLogicPhase == LOGIC_PHASE_PROCESSING)
    {
        // Something here.

        //Processing having been applied, turn logic flags are reset to await new actions.
        gameLogicActionOpen = true;
        turnLogicPhase = LOGIC_PHASE_SELECT_ACTION;
    }

    UpdateObjects();
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
            D_PROGRESSPAUSEDVISUALIZATIONTIMER = 20;
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

        /// ********Set Being initial testing positions here **********
        // Perhaps by creating and copying from a vector of initial beings?
        // Such as generator->beingSpawnpositions[blah];

        player = new Player(area->upstairsXCell,area->upstairsYCell);
        //player = new Player(true); //true to initalize saved player - Remove in end for atrium map
        //LoadPlayerState("player",player);
        //player->InitByArchive();

        //End testing

        beings.push_back(player);

        DevAddTestItemsToPlayer();

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

        // The player Being is drawn in the center.
        al_draw_bitmap_region(gfxPlayer,
                              TILESIZE*player->animationFrame, 0,
                              TILESIZE, TILESIZE,
                              SCREEN_W/2, SCREEN_H/2, 0);

        // The drawing positions of NPCs are relative to that of the player Being.
        for(std::vector<Being*>::iterator it = beings.begin(); it != beings.end(); ++it)
        {
            if((*it)->derivedType == BEING_TYPE_NPC)//As opposed to player, which is drawn seperately
            {
                if((*it)->animationState == ANIM_IDLE)
                    al_draw_bitmap_region(gfxNPCPassive[(*it)->spriteID],
                                          TILESIZE*(*it)->animationFrame, 0,
                                          TILESIZE, TILESIZE,
                                          (*it)->xPosition + SCREEN_W/2 - playerXPosition,
                                          (*it)->yPosition + SCREEN_H/2 - playerYPosition,
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


                    s_al_draw_text(robotoSlabFont, NEUTRAL_WHITE,
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

/// BEGIN INVENTORY CONTEXT /////////////////////////////////////////////////////////

    if(controlContext > INVENTORY_CONTEXT_MARKER_BEGIN &&
            controlContext < INVENTORY_CONTEXT_MARKER_END)
    {
        // Background of all inventory windows
        al_draw_bitmap(gfxItemUI, guiItemUIX, guiItemUIY, 0);

        // Nameplate of current active inventory window
        al_draw_bitmap(gfxUINameplate, guiItemNameplateX, guiItemNameplateY, 0);

        // Inactive inventory windows' mini tabs
        if(guiDrawInventoryIconTab[0]) // if == true
        {
            al_draw_bitmap(gfxEquipUIIconSmall,guiItemInactiveTabX,guiItemInactiveTabY, 0);
            al_draw_text(robotoSlabFont, BLOOD_RED, guiItemInactiveTabX + 55, guiItemInactiveTabY, 0, "1");
        }
        if(guiDrawInventoryIconTab[1])
        {
            al_draw_bitmap(gfxToolUIIconSmall,guiItemInactiveTabX,guiItemInactiveTabY+guiItemInactiveTabSpacing, 0);
            al_draw_text(robotoSlabFont, BLOOD_RED, guiItemInactiveTabX + 55, guiItemInactiveTabY+guiItemInactiveTabSpacing, 0, "2");
        }
        if(guiDrawInventoryIconTab[2])
        {
            al_draw_bitmap(gfxMagicUIIconSmall,guiItemInactiveTabX,guiItemInactiveTabY+guiItemInactiveTabSpacing*2, 0);
            al_draw_text(robotoSlabFont, BLOOD_RED, guiItemInactiveTabX + 55, guiItemInactiveTabY+guiItemInactiveTabSpacing*2, 0, "3");
        }
        if(guiDrawInventoryIconTab[3])
        {
            al_draw_bitmap(gfxMaterialUIIconSmall,guiItemInactiveTabX,guiItemInactiveTabY+guiItemInactiveTabSpacing*3, 0);
            al_draw_text(robotoSlabFont, BLOOD_RED, guiItemInactiveTabX + 55, guiItemInactiveTabY+guiItemInactiveTabSpacing*3, 0, "4");
        }
        if(guiDrawInventoryIconTab[4])
        {
            al_draw_bitmap(gfxKeyUIIconSmall,guiItemInactiveTabX,guiItemInactiveTabY+guiItemInactiveTabSpacing*4, 0);
            al_draw_text(robotoSlabFont, BLOOD_RED, guiItemInactiveTabX + 55, guiItemInactiveTabY+guiItemInactiveTabSpacing*4, 0, "5");
        }
        if(guiDrawInventoryIconTab[5])
        {
            al_draw_bitmap(gfxMiscUIIconSmall,guiItemInactiveTabX,guiItemInactiveTabY+guiItemInactiveTabSpacing*5, 0);
            al_draw_text(robotoSlabFont, BLOOD_RED, guiItemInactiveTabX + 55, guiItemInactiveTabY+guiItemInactiveTabSpacing*5, 0, "6");
        }

        // Active inventory window's big tab
        if(controlContext == EQUIP_INVENTORY_CONTEXT)
            al_draw_bitmap(gfxEquipUIIcon,
                           guiItemActiveTabX,
                           guiItemActiveTabY,
                           0);
        else if(controlContext == TOOL_INVENTORY_CONTEXT)
            al_draw_bitmap(gfxToolUIIcon,
                           guiItemActiveTabX,
                           guiItemActiveTabY,
                           0);
        else if(controlContext == MAGIC_INVENTORY_CONTEXT)
            al_draw_bitmap(gfxMagicUIIcon,
                           guiItemActiveTabX,
                           guiItemActiveTabY,
                           0);
        else if(controlContext == MATERIAL_INVENTORY_CONTEXT)
            al_draw_bitmap(gfxMaterialUIIcon,
                           guiItemActiveTabX,
                           guiItemActiveTabY,
                           0);
        else if(controlContext == KEY_INVENTORY_CONTEXT)
            al_draw_bitmap(gfxKeyUIIcon,
                           guiItemActiveTabX,
                           guiItemActiveTabY,
                           0);
        else if(controlContext == MISC_INVENTORY_CONTEXT)
            al_draw_bitmap(gfxMiscUIIcon,
                           guiItemActiveTabX,
                           guiItemActiveTabY,
                           0);


        // Viewed item image and text
        if(viewedItem != nullptr)
        {
            if(viewedItem->derivedType == ITEM_TYPE_EQUIP)
                al_draw_bitmap_region(gfxEquipSheet,
                                      viewedItem->spriteID * ITEM_ICONSIZE, 0,
                                      ITEM_ICONSIZE, ITEM_ICONSIZE,
                                      guiViewedItemX,
                                      guiViewedItemY,
                                      0);
            else if(viewedItem->derivedType == ITEM_TYPE_TOOL)
                al_draw_bitmap_region(gfxToolSheet,
                                      viewedItem->spriteID * ITEM_ICONSIZE, 0,
                                      ITEM_ICONSIZE, ITEM_ICONSIZE,
                                      guiViewedItemX,
                                      guiViewedItemY,
                                      0);
            else if(viewedItem->derivedType == ITEM_TYPE_MAGIC)
                al_draw_bitmap_region(gfxMagicSheet,
                                      viewedItem->spriteID * ITEM_ICONSIZE, 0,
                                      ITEM_ICONSIZE, ITEM_ICONSIZE,
                                      guiViewedItemX,
                                      guiViewedItemY,
                                      0);
            else if(viewedItem->derivedType == ITEM_TYPE_MATERIAL)
                al_draw_bitmap_region(gfxMaterialSheet,
                                      viewedItem->spriteID * ITEM_ICONSIZE, 0,
                                      ITEM_ICONSIZE, ITEM_ICONSIZE,
                                      guiViewedItemX,
                                      guiViewedItemY,
                                      0);
            else if(viewedItem->derivedType == ITEM_TYPE_KEY)
                al_draw_bitmap_region(gfxKeySheet,
                                      viewedItem->spriteID * ITEM_ICONSIZE, 0,
                                      ITEM_ICONSIZE, ITEM_ICONSIZE,
                                      guiViewedItemX,
                                      guiViewedItemY,
                                      0);
            else if(viewedItem->derivedType == ITEM_TYPE_MISC)
                al_draw_bitmap_region(gfxMiscSheet,
                                      viewedItem->spriteID * ITEM_ICONSIZE, 0,
                                      ITEM_ICONSIZE, ITEM_ICONSIZE,
                                      guiViewedItemX,
                                      guiViewedItemY,
                                      0);

            // Viewed item's name
            s_al_draw_text(penFontLarge, PEN_INK,
                           guiItemNameX, guiItemNameY,
                           ALLEGRO_ALIGN_CENTER, viewedItem->baseName);

            // Viewed item's text
            for(int i = 0; i < NUM_ITEM_DESCRIPTION_LINES; i++)
            {
                s_al_draw_text(penFont, PEN_INK,
                               guiItemDescriptionOriginX, guiItemDescriptionOriginY + (i*guiItemDescriptionLineSpacing),
                               0, viewedItem->description[i]);

            }
        }
    }

    if(controlContext == EQUIP_INVENTORY_CONTEXT)
    {
        // Label the window "Equipment"
        al_draw_text(penFont, PEN_INK,
                     guiItemNameplateTextX,
                     guiItemNameplateTextY,
                     ALLEGRO_ALIGN_CENTER, "Equipment");

        // Draw icons of items in inventory
        for(std::vector<Equip*>::iterator it = player->equipInventory.begin(); it != player->equipInventory.end(); ++it)
        {
            int elementPosition = std::distance(player->equipInventory.begin(), it);

            al_draw_bitmap_region(gfxEquipSheet,
                                  (*it)->spriteID * ITEM_ICONSIZE, 0,
                                  ITEM_ICONSIZE, ITEM_ICONSIZE,
                                  guiItemUIOriginX + ITEM_UI_SLOT_WIDTH * (elementPosition % ITEM_UI_ROW_WIDTH),
                                  guiItemUIOriginY + ITEM_UI_SLOT_WIDTH * (elementPosition / ITEM_UI_ROW_WIDTH),
                                  0);
        }
    }
    else if(controlContext == TOOL_INVENTORY_CONTEXT)
    {
        al_draw_text(penFont, PEN_INK,
                     guiItemNameplateTextX,
                     guiItemNameplateTextY,
                     ALLEGRO_ALIGN_CENTER, "Tools");

        for(std::vector<Tool*>::iterator it = player->toolInventory.begin(); it != player->toolInventory.end(); ++it)
        {
            int elementPosition = std::distance(player->toolInventory.begin(), it);

            al_draw_bitmap_region(gfxToolSheet,
                                  (*it)->spriteID * ITEM_ICONSIZE, 0,
                                  ITEM_ICONSIZE, ITEM_ICONSIZE,
                                  guiItemUIOriginX + ITEM_UI_SLOT_WIDTH * (elementPosition % ITEM_UI_ROW_WIDTH),
                                  guiItemUIOriginY + ITEM_UI_SLOT_WIDTH * (elementPosition / ITEM_UI_ROW_WIDTH),
                                  0);
        }
    }
    else if(controlContext == MAGIC_INVENTORY_CONTEXT)
    {
        al_draw_text(penFont, PEN_INK,
                     guiItemNameplateTextX,
                     guiItemNameplateTextY,
                     ALLEGRO_ALIGN_CENTER, "Magic");

        for(std::vector<Magic*>::iterator it = player->magicInventory.begin(); it != player->magicInventory.end(); ++it)
        {
            int elementPosition = std::distance(player->magicInventory.begin(), it);

            al_draw_bitmap_region(gfxMagicSheet,
                                  (*it)->spriteID * ITEM_ICONSIZE, 0,
                                  ITEM_ICONSIZE, ITEM_ICONSIZE,
                                  guiItemUIOriginX + ITEM_UI_SLOT_WIDTH * (elementPosition % ITEM_UI_ROW_WIDTH),
                                  guiItemUIOriginY + ITEM_UI_SLOT_WIDTH * (elementPosition / ITEM_UI_ROW_WIDTH),
                                  0);
        }
    }
    else if(controlContext == MATERIAL_INVENTORY_CONTEXT)
    {

        al_draw_text(penFont, PEN_INK,
                     guiItemNameplateTextX,
                     guiItemNameplateTextY,
                     ALLEGRO_ALIGN_CENTER, "Materials");

        for(std::vector<Material*>::iterator it = player->materialInventory.begin(); it != player->materialInventory.end(); ++it)
        {
            int elementPosition = std::distance(player->materialInventory.begin(), it);

            al_draw_bitmap_region(gfxMaterialSheet,
                                  (*it)->spriteID * ITEM_ICONSIZE, 0,
                                  ITEM_ICONSIZE, ITEM_ICONSIZE,
                                  guiItemUIOriginX + ITEM_UI_SLOT_WIDTH * (elementPosition % ITEM_UI_ROW_WIDTH),
                                  guiItemUIOriginY + ITEM_UI_SLOT_WIDTH * (elementPosition / ITEM_UI_ROW_WIDTH),
                                  0);
        }
    }
    else if(controlContext == KEY_INVENTORY_CONTEXT)
    {
        al_draw_text(penFont, PEN_INK,
                     guiItemNameplateTextX,
                     guiItemNameplateTextY,
                     ALLEGRO_ALIGN_CENTER, "Key Items");

        for(std::vector<Key*>::iterator it = player->keyInventory.begin(); it != player->keyInventory.end(); ++it)
        {
            int elementPosition = std::distance(player->keyInventory.begin(), it);

            al_draw_bitmap_region(gfxKeySheet,
                                  (*it)->spriteID * ITEM_ICONSIZE, 0,
                                  ITEM_ICONSIZE, ITEM_ICONSIZE,
                                  guiItemUIOriginX + ITEM_UI_SLOT_WIDTH * (elementPosition % ITEM_UI_ROW_WIDTH),
                                  guiItemUIOriginY + ITEM_UI_SLOT_WIDTH * (elementPosition / ITEM_UI_ROW_WIDTH),
                                  0);
        }
    }
    else if(controlContext == MISC_INVENTORY_CONTEXT)
    {
        al_draw_text(penFont, PEN_INK,
                     guiItemNameplateTextX,
                     guiItemNameplateTextY,
                     ALLEGRO_ALIGN_CENTER, "Misc. Items");

        for(std::vector<Misc*>::iterator it = player->miscInventory.begin(); it != player->miscInventory.end(); ++it)
        {
            int elementPosition = std::distance(player->miscInventory.begin(), it);

            al_draw_bitmap_region(gfxMiscSheet,
                                  (*it)->spriteID * ITEM_ICONSIZE, 0,
                                  ITEM_ICONSIZE, ITEM_ICONSIZE,
                                  guiItemUIOriginX + ITEM_UI_SLOT_WIDTH * (elementPosition % ITEM_UI_ROW_WIDTH),
                                  guiItemUIOriginY + ITEM_UI_SLOT_WIDTH * (elementPosition / ITEM_UI_ROW_WIDTH),
                                  0);
        }
    }

    if(controlContext > INVENTORY_CONTEXT_MARKER_BEGIN && controlContext < INVENTORY_CONTEXT_MARKER_END)
    {
        for(int i = 0; i < 24; i++)
        {
            //std::string s = std::to_string('a' + i);
            //char const *pchar = s.c_str();

            char c = 'a' + i;
            const char *cc = &c;

            al_draw_text(robotoSlabFont, BLOOD_RED,
                         guiItemUIOriginX + ITEM_UI_SLOT_WIDTH * (i % ITEM_UI_ROW_WIDTH),
                         guiItemUIOriginY + ITEM_UI_SLOT_WIDTH * (i / ITEM_UI_ROW_WIDTH),
                         0,cc);
        }
    }
/// END INVENTORY CONTEXT /////////////////////////////////////////////////////////
/// BEGIN PLAYER STAT CONTEXT /////////////////////////////////////////////////////

    if(controlContext == PLAYER_STAT_CONTEXT)
    {
        // Background of player stat window
        al_draw_bitmap(gfxPstatUI, guiPstatUIX,guiPstatUIY,0);

        // Nameplate
        al_draw_bitmap(gfxUINameplate, guiPstatNameplateX, guiPstatNameplateY, 0);
        al_draw_text(penFont, PEN_INK,
                     guiPstatNameplateTextX,
                     guiPstatNameplateTextY,
                     ALLEGRO_ALIGN_CENTER, "Status");

        // Player's portrait and spells

        // Draw icons of items equipped by player
        for(std::vector<Equip*>::iterator it = player->wornEquipment.begin(); it != player->wornEquipment.end(); ++it)
        {
            if(*it != nullptr)
            {
                int elementPosition = std::distance(player->wornEquipment.begin(), it);

                al_draw_bitmap_region(gfxEquipSheet,
                                      (*it)->spriteID * ITEM_ICONSIZE, 0,
                                      ITEM_ICONSIZE, ITEM_ICONSIZE,
                                      guiPstatEquipOriginX + PSTAT_UI_EQUIP_SLOT_WIDTH * (elementPosition % PSTAT_UI_EQUIP_ROW_WIDTH),
                                      guiPstatEquipOriginY + PSTAT_UI_EQUIP_SLOT_WIDTH * (elementPosition / PSTAT_UI_EQUIP_ROW_WIDTH),
                                      0);
            }
        }

        for(int i = 0; i < 8; i++)
        {
            //std::string s = std::to_string('a' + i);
            //char const *pchar = s.c_str();

            char c = 'a' + i;
            const char *cc = &c;

            al_draw_text(robotoSlabFont, BLOOD_RED,
                         guiPstatEquipOriginX + PSTAT_UI_EQUIP_SLOT_WIDTH * (i % PSTAT_UI_EQUIP_ROW_WIDTH),
                         guiPstatEquipOriginY + PSTAT_UI_EQUIP_SLOT_WIDTH * (i / PSTAT_UI_EQUIP_ROW_WIDTH),
                         0,cc);
        }

        // Draw player primary stats
        for(int i = 0; i < STAT_PRIMARY_TOTAL; i++)
        {
            s_al_draw_text(sourceCodeFont, PEN_INK,
                           guiPstatPrimaryOriginX, guiPstatPrimaryOriginY+guiPstatPrimaryYSpacing*i,
                           ALLEGRO_ALIGN_LEFT, player->primaryString[i]);
        }

        // Draw player secondary stats
        for(int i = 0+2; i < STAT_SECONDARY_TOTAL; i++) // Skip LIFE/ANIMA
        {
            s_al_draw_text(sourceCodeFont, PEN_INK,
                           guiPstatSecondaryOriginX + guiPstatSecondaryXSpacing * (i % guiPstatSecondaryNumCols),
                           guiPstatSecondaryOriginY + guiPstatSecondaryYSpacing * (i / guiPstatSecondaryNumCols),
                           ALLEGRO_ALIGN_LEFT, player->secondaryString[i]);
        }


    }


/// END PLAYER STAT CONTEXT ///////////////////////////////////////////////////////
/// BEGIN TARGETTING CONTEXTS /////////////////////////////////////////////////////

    if(controlContext == TARGETING_CONTEXT)
    {
        al_draw_bitmap(gfxGuiTarget,
                       targetScanXCell*TILESIZE + SCREEN_W/2 - playerXPosition,
                       targetScanYCell*TILESIZE + SCREEN_H/2 - playerYPosition,
                       0);

        if(!targetableBeings.empty())
            for(unsigned int i = 0; i < targetableBeings.size(); i++)
            {
                c_al_draw_centered_bitmap(gfxGuiTargetableListTag,
                                          targetableBeings[i]->xPosition + TILESIZE + SCREEN_W/2 - playerXPosition,
                                          targetableBeings[i]->yPosition + SCREEN_H/2 - playerYPosition,
                                          0);

                std::string intAlpha;
                intAlpha = (char)97+i;

                s_al_draw_centered_text(robotoSlabFont, NEUTRAL_WHITE,
                                        targetableBeings[i]->xPosition + TILESIZE + SCREEN_W/2 - playerXPosition,
                                        targetableBeings[i]->yPosition + SCREEN_H/2 - playerYPosition,
                                        ALLEGRO_ALIGN_CENTER,
                                        intAlpha);
            }
    }

    if(targetLockLevel > TARGET_LOCK_MARKER_HARD)
    {
        al_draw_bitmap(gfxGuiTargetHard,
                       targetLockXCell*TILESIZE + SCREEN_W/2 - playerXPosition,
                       targetLockYCell*TILESIZE + SCREEN_H/2 - playerYPosition,
                       0);
    }

/// END TARGETTING CONTEXTS /////////////////////////////////////////////////////

    ALLEGRO_COLOR colorToDraw = NEUTRAL_WHITE;

    // Draw the (now unused)
    //al_draw_bitmap(gfxTerminal,0,SCREEN_H - TERMINAL_H,0);

    // Draw the text that would go in the terminal (make a better terminal later)
    for(int i = 0; i < EXCERPT_NUM_LINES; ++i)
    {
        s_al_draw_text(robotoSlabFont,colorToDraw,
                       TERMINAL_TEXT_OPEN_X,
                       TERMINAL_TEXT_OPEN_Y-(20*i),
                       ALLEGRO_ALIGN_LEFT,
                       terminalExcerpt[i]);
    }

    // Draw turn counter (put it in an appropriate place later)
    std::string statText;
    statText = "turn" + std::to_string(turn);
    s_al_draw_text(robotoSlabFont,NEUTRAL_WHITE,STATS_BAR_OPEN_X,STATS_BAR_OPEN_Y,ALLEGRO_ALIGN_LEFT,statText);

}

void DrawTiles()
{
    //Assumes 800x600 window
    int startCellX = playerXCell-drawingXCellCutoff;
    if(startCellX < 0)
        startCellX = 0;

    int startCellY = playerYCell-drawingYCellCutoff;
    if(startCellY < 0)
        startCellY = 0;

    int endCellX = playerXCell+drawingXCellCutoff;
    if(endCellX > areaCellWidth)
        endCellX = areaCellHeight;

    int endCellY = playerYCell+drawingYCellCutoff;
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
                                      x*TILESIZE + SCREEN_W/2 - playerXPosition,
                                      y*TILESIZE + SCREEN_H/2 - playerYPosition,
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
                                      x*TILESIZE + SCREEN_W/2 - playerXPosition,
                                      y*TILESIZE + SCREEN_H/2 - playerYPosition,
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
                                      x*TILESIZE + SCREEN_W/2 - playerXPosition,
                                      y*TILESIZE + SCREEN_H/2 - playerYPosition,
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
        std::string posStr = "(" + std::to_string(playerXCell) + ", " + std::to_string(playerYCell) + ")";
        s_al_draw_text(robotoSlabFont,al_map_rgb(255,255,255),0,0,ALLEGRO_ALIGN_LEFT,posStr);
    }
    if(mainPhase == MAIN_PHASE_LOADING)
    {
        //Draw crosshair on the screen and coordinate of crosshair
        int crosshairX = loadingCamX+SCREEN_W/2;
        int crosshairY = loadingCamY+SCREEN_H/2;
        int crosshairXCell = crosshairX/MINI_TILESIZE;
        int crosshairYCell = crosshairY/MINI_TILESIZE;
        std::string posStr = "(" + std::to_string(crosshairX) + ", " + std::to_string(crosshairY) + ") : (" + std::to_string(crosshairXCell) + ", " + std::to_string(crosshairYCell) + ")";
        s_al_draw_text(robotoSlabFont,HOLY_INDIGO,0,0,ALLEGRO_ALIGN_LEFT,posStr);

        al_draw_line(SCREEN_W/2,0,SCREEN_W/2,SCREEN_H,HOLY_INDIGO,1);
        al_draw_line(0,SCREEN_H/2,SCREEN_W,SCREEN_H/2,HOLY_INDIGO,1);


    }
}


/// ########## ########## END MAIN DRAWING FUNCTIONS ########## ##########


/// ########## ########## BEGIN CLASS AND MEMBER OBJECT MANAGEMENT FUNCTIONS ########## ##########

void UpdateObjects()
{
    for(std::vector<Being*>::iterator it = beings.begin(); it != beings.end();)
    {
        area->beingmap[(*it)->yCell * areaCellWidth + (*it)->xCell] = nullptr; // "Uproot" being from map to fiddle with it

        //Check the derived type and call the logic function specific to its class.
        //GameLogic() just handles data that needs to be constantly updated.
        (*it)->BaseLogic();
        if((*it)->derivedType == BEING_TYPE_PLAYER)
            ((Player*)(*it))->Logic();
        else if((*it)->derivedType == BEING_TYPE_NPC)
            ((NPC*)(*it))->Logic();


        // If the Being has cast a spell, add the spell to activeSpells vector
        if((*it)->castSpell != nullptr)
        {
            activeSpells.push_back((*it)->castSpell)
            (*it)->castSpell = nullptr;
        }


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
            area->beingmap[(*it)->yCell * areaCellWidth + (*it)->xCell] = (*it); // "Plant" being back into map
            ++it;
        }
    }

    for(std::vector<Spell*>::iterator it = activeSpells.begin(); it != activeSpells.end();)
    {
        // Check cells covered by the spell for beings/objects.
        for(std::vector<int>::iterator ccit = (*it)->cellsCovered.begin(); ccit != (*it)->cellsCovered.end(); ++ccit)
        {
            Being*bPointer;

            if(area->beingmap[*ccit] != nullptr) // There is a Being present on the cell being iterated over.
            {
                if((*it)->canAffectSelf)
                {

                }
                if((*it)->canAffectAlly)
                {

                }
                if((*it)->canAffectEnemy)
                {

                }
                if((*it)->canAffectNeutral)
                {

                }
                if((*it)->canAffectEnvironment)
                {

                }
            }


            /*
            if(area->beingmap[*ccit] != nullptr)
            {
                bpointer = area->beingmap[*ccit];
                bpointer->effects.insert(bpointer->effects.end(),(*it)->effects.begin(), (*it)->effects.end());
            }
            */
        }

        (*it)->Logic();


        if(gameExit)
            (*it)->active = false;

        if(!(*it)->active)
        {
            delete *it;
            activeSpells.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for(std::vector<Item*>::iterator it = areaItems.begin(); it != areaItems.end();)
    {
        if(gameExit)
            (*it)->active = false;

        if(!(*it)->active)
        {
            delete *it;
            areaItems.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

/// ########## ########## END CLASS AND MEMBER OBJECT MANAGMEMENT FUNCTIONS ########## ##########

void PopulateItems()
{

    areaItems.clear();

    /*

    for(std::vector<Being*>::iterator it = beings.begin(); it != beings.end(); ++it)
    {

    }
    */
}

void PopulateTargetableBeings()
{
    //Generate list of target-able NPCs and tag them a-z

    for(std::vector<Being*>::iterator it = beings.begin(); it != beings.end(); ++it)
    {
        // if visible within a certain distance...

        if((*it)->derivedType == BEING_TYPE_NPC)
        {
            if(sqrt(pow((*it)->yCell - playerYCell, 2) + pow((*it)->xCell - playerXCell, 2)) <= 10)
            {
                targetableBeings.push_back(*it);
            }

        }
    }

}

Being* AcquireAutoTarget()
{
    /* Function to target closest NPC by default. If no such NPC, initalize targeting at player cell by default (no lock-on)*/

    targetableBeings.clear();
    PopulateTargetableBeings();

    if(!targetableBeings.empty())
    {
        // sort by distance to player
        std::sort(targetableBeings.begin(), targetableBeings.end(), CompareTargetableDistanceAscending);

        targetLockXCell = targetScanXCell = targetableBeings[0]->xCell;
        targetLockYCell = targetScanYCell = targetableBeings[0]->yCell;
        targetLockLevel = TARGET_LOCK_AUTO;

        return targetableBeings[0];
    }
    else
    {
        targetLockXCell = targetScanXCell = playerXCell;
        targetLockYCell = targetScanYCell = playerYCell;
        targetLockLevel = TARGET_LOCK_NONE;

        return nullptr;
    }
}

void ProcessInput(int whatContext)
{
    int keypadDirection = 0; // ULRD 0000

    switch(whatContext)
    {
    case NORMAL_CONTEXT:

        if(player->actionBlocked)
        {
            player->actionCost = 100;
            player->Move(NO_DIRECTION);
            player->actionBlocked = false;

            submittedPlayerActionCommand = true;
        }
        else // If !actionBlocked
        {
/// Normal control context (interpret alphanumeric [non-keypad])////////////////////////////////////////////////////////////////////////

            if(keyInput[KEY_1] && controlContextChangeDelay == 0)
            {
                ChangeControlContext(NORMAL_CONTEXT,EQUIP_INVENTORY_CONTEXT);
                guiDrawInventoryIconTab[0] = false;
            }

            else if((keyInput[KEY_2] && controlContextChangeDelay == 0) ||
                    (keyInput[KEY_I] && controlContextChangeDelay == 0))
            {
                ChangeControlContext(NORMAL_CONTEXT,TOOL_INVENTORY_CONTEXT);
                guiDrawInventoryIconTab[1] = false;
            }
            else if(keyInput[KEY_3] && controlContextChangeDelay == 0)
            {
                ChangeControlContext(NORMAL_CONTEXT,MAGIC_INVENTORY_CONTEXT);
                guiDrawInventoryIconTab[2] = false;
            }

            else if(keyInput[KEY_4] && controlContextChangeDelay == 0)
            {
                ChangeControlContext(NORMAL_CONTEXT,MATERIAL_INVENTORY_CONTEXT);
                guiDrawInventoryIconTab[3] = false;
            }
            else if(keyInput[KEY_5] && controlContextChangeDelay == 0)

            {
                ChangeControlContext(NORMAL_CONTEXT,KEY_INVENTORY_CONTEXT);
                guiDrawInventoryIconTab[4] = false;
            }
            else if(keyInput[KEY_6] && controlContextChangeDelay == 0)
            {
                ChangeControlContext(NORMAL_CONTEXT,MISC_INVENTORY_CONTEXT);
                guiDrawInventoryIconTab[5] = false;
            }

            else if(keyInput[KEY_C] && controlContextChangeDelay == 0)
            {
                ChangeControlContext(NORMAL_CONTEXT,PLAYER_STAT_CONTEXT);
            }

            else if(keyInput[KEY_F])
            {
                if(targetLockLevel == TARGET_LOCK_NONE)
                {
                    targetableBeings.clear();
                    autoTargetedBeing = AcquireAutoTarget();
                    if(autoTargetedBeing == nullptr)
                    {
                        // Nothing happens. To do: terminal outputs: "No target selected, with msg spam delay"
                    }
                    else
                    {
                        player->ReleaseCurrentSpell();
                    }
                }

                if(targetLockLevel == TARGET_LOCK_AUTO)
                {
                    player->ReleaseCurrentSpell();
                    //player->actionCost = 100;
                    //submittedPlayerActionCommand = true;
                }
                else if(targetLockLevel == TARGET_LOCK_CELL)
                {
                    player->ReleaseCurrentSpell();
                    //player->actionCost = 100;
                    //submittedPlayerActionCommand = true;
                }
                else if(targetLockLevel == TARGET_LOCK_BEING)
                {
                    player->ReleaseCurrentSpell();
                    //player->actionCost = 100;
                    //submittedPlayerActionCommand = true;
                }

            }

            else if(keyInput[KEY_G])
            {
                // List pickup-able items.
            }

            else if(keyInput[KEY_L] && controlContextChangeDelay == 0)
            {
                ChangeControlContext(NORMAL_CONTEXT,TARGETING_CONTEXT);

                if(targetLockLevel < TARGET_LOCK_MARKER_HARD)
                    autoTargetedBeing = AcquireAutoTarget();
                else
                {
                    targetLockXCell = targetScanXCell = playerXCell;
                    targetLockYCell = targetScanYCell = playerYCell;
                    targetLockLevel = TARGET_LOCK_NONE;
                }

            }

            else if(keyInput[KEY_Z] && controlContextChangeDelay == 0)
            {
                ChangeControlContext(NORMAL_CONTEXT, WEAPON_SPELL_CONTEXT);
            }

/// Normal control context (interpret keypad) ////////////////////////////////////////////////////////////////////////

            if(gameLogicActionOpen)
            {
                if(keyInput[KEY_PAD_8] || keyInput[KEY_UP])    keypadDirection += 1000;
                if(keyInput[KEY_PAD_4] || keyInput[KEY_LEFT])  keypadDirection +=  100;
                if(keyInput[KEY_PAD_6] || keyInput[KEY_RIGHT]) keypadDirection +=   10;
                if(keyInput[KEY_PAD_2] || keyInput[KEY_DOWN])  keypadDirection +=    1;

                if(keypadDirection == 0)
                {
                    if(keyInput[KEY_PAD_7]) keypadDirection = 1100;
                    if(keyInput[KEY_PAD_9]) keypadDirection = 1010;
                    if(keyInput[KEY_PAD_1]) keypadDirection =  101;
                    if(keyInput[KEY_PAD_3]) keypadDirection =   11;
                    if(keyInput[KEY_PAD_5]) keypadDirection = 1111;
                }

                if(keypadDirection > 0)
                {
                    player->actionCost = 100;
                    player->currentAction = ACTION_WALK;
                    // ********* check emptiness of destination here **************************
                    player->intendedWalkDirection = keypadDirection;

// std::cout << playerXPosition << ", " << playerYPosition << " | " << playerXCell << ", " << playerYCell << std::endl;

                    submittedPlayerActionCommand = true;
                }
            }
        }
        break;
/// Targeting control context ///////////////////////////////////////////////////////////////////////////////

    case TARGETING_CONTEXT:
        /*
        if no targets are available
        */

        if(!targetableBeings.empty())
        {
            for(unsigned int i = KEY_A; i < KEY_Z; i++)
            {
                if(keyInput[i] && i < targetableBeings.size())
                {
                    hardTargetedBeing = targetableBeings[i];
                    targetScanXCell = targetLockXCell = hardTargetedBeing->xCell;
                    targetScanYCell = targetLockYCell = hardTargetedBeing->yCell;

                    targetLockLevel = TARGET_LOCK_BEING;
                    ChangeControlContext(TARGETING_CONTEXT,NORMAL_CONTEXT);

                    break;
                }
            }
        }

        // Pressing L: Leave targeting context, return to normal context
        if(keyInput[KEY_L] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(TARGETING_CONTEXT, NORMAL_CONTEXT);
        }
        else if(keyInput[KEY_PAD_5] || keyInput[KEY_ENTER])
        {
            // Target cell. To target a being, use alphabetical control instead.

            targetLockLevel = TARGET_LOCK_CELL;

            targetLockXCell = targetScanXCell;
            targetLockYCell = targetScanYCell;

            ChangeControlContext(TARGETING_CONTEXT, NORMAL_CONTEXT);
        }
        else if(keyInput[KEY_PAD_7])
            MoveTargetScanCell(-1,-1);
        else if(keyInput[KEY_PAD_8])
            MoveTargetScanCell( 0,-1);
        else if(keyInput[KEY_PAD_9])
            MoveTargetScanCell( 1,-1);
        else if(keyInput[KEY_PAD_4])
            MoveTargetScanCell(-1,0);
        else if(keyInput[KEY_PAD_6])
            MoveTargetScanCell( 1,0);
        else if(keyInput[KEY_PAD_1])
            MoveTargetScanCell(-1, 1);
        else if(keyInput[KEY_PAD_2])
            MoveTargetScanCell( 0, 1);
        else if(keyInput[KEY_PAD_3])
            MoveTargetScanCell( 1, 1);

        break;

/// Inventory context //////////////////////////////////////////////////////////////////////////////////////

    case EQUIP_INVENTORY_CONTEXT:
        if( (keyInput[KEY_1] && controlContextChangeDelay == 0) ||
                (keyInput[KEY_I] && controlContextChangeDelay == 0)) // Should later add in ESC
        {
            ChangeControlContext(EQUIP_INVENTORY_CONTEXT, NORMAL_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[0] = true;
        }
        else if((keyInput[KEY_2] && controlContextChangeDelay == 0) ||
                (keyInput[KEY_I] && controlContextChangeDelay == 0))
        {
            ChangeControlContext(EQUIP_INVENTORY_CONTEXT,TOOL_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[0] = true;
            guiDrawInventoryIconTab[1] = false;
        }
        else if(keyInput[KEY_3] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(EQUIP_INVENTORY_CONTEXT,MAGIC_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[0] = true;
            guiDrawInventoryIconTab[2] = false;
        }
        else if(keyInput[KEY_4] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(EQUIP_INVENTORY_CONTEXT,MATERIAL_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[0] = true;
            guiDrawInventoryIconTab[3] = false;
        }
        else if(keyInput[KEY_5] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(EQUIP_INVENTORY_CONTEXT,KEY_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[0] = true;
            guiDrawInventoryIconTab[4] = false;
        }
        else if(keyInput[KEY_6] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(EQUIP_INVENTORY_CONTEXT,MISC_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[0] = true;
            guiDrawInventoryIconTab[5] = false;
        }

        else if(keyInput[KEY_Z] && viewedItem != nullptr)
        {
            Equip*viewedEquip = (Equip*)viewedItem; // Type cast
            if(viewedEquip->equipType == EQUIP_TYPE_MAIN_HAND)
            {
                if(player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] == nullptr) // Player's main hand is empty.
                {
                    player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition); // ** Erases element but not not delete object! **
                    viewedItem = nullptr;  // Forces user to select a new item from equipInventory, with the side effect of acquiring new viewedItemPosition.

                }
                else if(player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND]->equipType == EQUIP_TYPE_MAIN_HAND) //Player's main hand holds a one-handed weapon.
                {
                    if(player->wornEquipment[PLAYER_EQUIP_SLOT_OFF_HAND] == nullptr) // Offhand slot is empty.
                    {
                        player->wornEquipment[PLAYER_EQUIP_SLOT_OFF_HAND] = viewedEquip;
                        player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                        viewedItem = nullptr;

                    }
                    else // Offhand slot is filled.
                    {
                        // Swap viewedEquip with mainhand slot.
                        // Exchange one vector's pointer for the other:
                        player->equipInventory.push_back(player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND]); // Copy "unequipped" item to equipment inventory.
                        player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] = viewedEquip;
                        player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                        viewedItem = nullptr;


                    }
                }
                else if(player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND]->equipType == EQUIP_TYPE_TWO_HAND) // If a two-handed weapon is current wielded, unequip it for the new weapon.
                {
                    // Exchange one vector's pointer for the other:
                    player->equipInventory.push_back(player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND]); // Copy "unequipped" item to equipment inventory.
                    player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;

                }

            }

            else if(viewedEquip->equipType == EQUIP_TYPE_OFF_HAND)
            {
                if(player->wornEquipment[PLAYER_EQUIP_SLOT_OFF_HAND] == nullptr) // Player's off hand is empty.
                {
                    player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition); // ** Erases element but not not delete object! **

                    if(player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND]->equipType == EQUIP_TYPE_TWO_HAND) // A two-handed weapon is currently wielded (and must be un-equipped)
                    {
                        player->equipInventory.push_back(player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND]); // Copy "unequipped" two-hander to equipment inventory.
                        player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] = nullptr; // The two-hander's pointer in wornEquipment must be nullified.
                    }

                    viewedItem = nullptr;  // Forces user to select a new item from equipInventory, with the side effect of acquiring new viewedItemPosition.

                }
                else if(player->wornEquipment[PLAYER_EQUIP_SLOT_OFF_HAND] != nullptr) // Player's off hand is NOT empty (which implies a two-hander is not equipped anyway, so two-hander check is not necessary).
                {

                    // Exchange one vector's pointer for the other:
                    player->equipInventory.push_back(player->wornEquipment[PLAYER_EQUIP_SLOT_OFF_HAND]); // Copy "unequipped" item to equipment inventory.
                    player->wornEquipment[PLAYER_EQUIP_SLOT_OFF_HAND] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;

                }
            }

            else if(viewedEquip->equipType == EQUIP_TYPE_TWO_HAND)
            {
                if(player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] == nullptr
                        && player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] == nullptr) // Items are currently equipped in NEITHER main and offhand.
                {
                    player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;

                }
                else if(player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] != nullptr
                        && player->wornEquipment[PLAYER_EQUIP_SLOT_OFF_HAND] != nullptr) // An item is already equipped in BOTH main and offhand.
                {
                    if(player->equipInventory.size() < 24-1) // Player equipInventory has room for *two* items.
                    {
                        player->equipInventory.push_back(player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND]); // Copy "unequipped" item to equipment inventory.
                        player->equipInventory.push_back(player->wornEquipment[PLAYER_EQUIP_SLOT_OFF_HAND]); // Copy "unequipped" item to equipment inventory.
                        player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] = viewedEquip;
                        player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                        viewedItem = nullptr;

                    }
                    else
                    {
                        // Flash error message (no room in equip inventory).
                    }
                }
                else if(player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] != nullptr
                        && player->wornEquipment[PLAYER_EQUIP_SLOT_OFF_HAND] == nullptr) // An item is equipped in ONLY main hand; NOT offhand.
                {
                    player->equipInventory.push_back(player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND]);
                    player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;

                }
                else if(player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] == nullptr
                        && player->wornEquipment[PLAYER_EQUIP_SLOT_OFF_HAND] != nullptr) // An item is equipped in ONLY offhand; NOT main hand.
                {
                    player->equipInventory.push_back(player->wornEquipment[PLAYER_EQUIP_SLOT_OFF_HAND]);
                    player->wornEquipment[PLAYER_EQUIP_SLOT_OFF_HAND] = nullptr; // Unlike previous case, offhand slot must be nullified.
                    player->wornEquipment[PLAYER_EQUIP_SLOT_MAIN_HAND] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;

                }
            }

            else if(viewedEquip->equipType == EQUIP_TYPE_RELIC)
            {
                if(player->wornEquipment[PLAYER_EQUIP_SLOT_RELIC_1] == nullptr) // First relic slot is open
                {
                    player->wornEquipment[PLAYER_EQUIP_SLOT_RELIC_1] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;

                }
                else if(player->wornEquipment[PLAYER_EQUIP_SLOT_RELIC_2] == nullptr) // (First relic slot is NOT open) but second relic slot is open
                {
                    player->wornEquipment[PLAYER_EQUIP_SLOT_RELIC_2] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;

                }
                else // (Both relic slots are occupied)
                {
                    // Just swap out relic 1 for now.
                    player->equipInventory.push_back(player->wornEquipment[PLAYER_EQUIP_SLOT_RELIC_1]);
                    player->wornEquipment[PLAYER_EQUIP_SLOT_RELIC_1] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;

                }
            }

            else if(viewedEquip->equipType == EQUIP_TYPE_HEAD)
            {
                if(player->wornEquipment[PLAYER_EQUIP_SLOT_HEAD] == nullptr)
                {
                    player->wornEquipment[PLAYER_EQUIP_SLOT_HEAD] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;

                }
                else
                {
                    player->equipInventory.push_back(player->wornEquipment[PLAYER_EQUIP_SLOT_HEAD]);
                    player->wornEquipment[PLAYER_EQUIP_SLOT_HEAD] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;

                }
            }

            else if(viewedEquip->equipType == EQUIP_TYPE_BODY)
            {
                if(player->wornEquipment[PLAYER_EQUIP_SLOT_BODY] == nullptr)
                {
                    player->wornEquipment[PLAYER_EQUIP_SLOT_BODY] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;

                }
                else
                {
                    player->equipInventory.push_back(player->wornEquipment[PLAYER_EQUIP_SLOT_BODY]);
                    player->wornEquipment[PLAYER_EQUIP_SLOT_BODY] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;


                }
            }

            else if(viewedEquip->equipType == EQUIP_TYPE_ARMS)
            {
                if(player->wornEquipment[PLAYER_EQUIP_SLOT_ARMS] == nullptr)
                {
                    player->wornEquipment[PLAYER_EQUIP_SLOT_ARMS] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;


                }
                else
                {
                    player->equipInventory.push_back(player->wornEquipment[PLAYER_EQUIP_SLOT_ARMS]);
                    player->wornEquipment[PLAYER_EQUIP_SLOT_ARMS] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;


                }
            }

            else if(viewedEquip->equipType == EQUIP_TYPE_LEGS)
            {
                if(player->wornEquipment[PLAYER_EQUIP_SLOT_LEGS] == nullptr)
                {
                    player->wornEquipment[PLAYER_EQUIP_SLOT_LEGS] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;


                }
                else
                {
                    player->equipInventory.push_back(player->wornEquipment[PLAYER_EQUIP_SLOT_LEGS]);
                    player->wornEquipment[PLAYER_EQUIP_SLOT_LEGS] = viewedEquip;
                    player->equipInventory.erase(player->equipInventory.begin()+viewedItemPosition);
                    viewedItem = nullptr;

                }
            }

            player->RecalculateStats();

            player->RecalculateSpells();


        }

        for(unsigned int i = KEY_A; i < KEY_A+23; i++) // 0 - 25
        {
            if(keyInput[i] && player->equipInventory.size() >= i-KEY_A+1)
            {
                viewedItem = player->equipInventory[i-KEY_A];
                viewedItemPosition = 0+ i-KEY_A;
            }
        }

        break;


    case TOOL_INVENTORY_CONTEXT:
        if( (keyInput[KEY_2] && controlContextChangeDelay == 0) ||
                (keyInput[KEY_I] && controlContextChangeDelay == 0)) // Should later add in ESC
        {
            ChangeControlContext(TOOL_INVENTORY_CONTEXT, NORMAL_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[1] = true;
        }
        else if(keyInput[KEY_1] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(TOOL_INVENTORY_CONTEXT,EQUIP_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[1] = true;
            guiDrawInventoryIconTab[0] = false;
        }
        else if(keyInput[KEY_3] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(TOOL_INVENTORY_CONTEXT,MAGIC_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[1] = true;
            guiDrawInventoryIconTab[2] = false;
        }
        else if(keyInput[KEY_4] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(TOOL_INVENTORY_CONTEXT,MATERIAL_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[1] = true;
            guiDrawInventoryIconTab[3] = false;
        }
        else if(keyInput[KEY_5] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(TOOL_INVENTORY_CONTEXT,KEY_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[1] = true;
            guiDrawInventoryIconTab[4] = false;
        }
        else if(keyInput[KEY_6] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(TOOL_INVENTORY_CONTEXT,MISC_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[1] = true;
            guiDrawInventoryIconTab[5] = false;
        }

        for(unsigned int i = KEY_A; i < KEY_A+23; i++)
        {
            if(keyInput[i] && player->toolInventory.size() >= i-KEY_A+1)
                viewedItem = player->toolInventory[i-KEY_A];
        }
        break;

    case MAGIC_INVENTORY_CONTEXT:
        if( (keyInput[KEY_3] && controlContextChangeDelay == 0) ||
                (keyInput[KEY_I] && controlContextChangeDelay == 0)) // Should later add in ESC
        {
            ChangeControlContext(MAGIC_INVENTORY_CONTEXT, NORMAL_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[2] = true;
        }
        else if(keyInput[KEY_1] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(MAGIC_INVENTORY_CONTEXT,EQUIP_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[2] = true;
            guiDrawInventoryIconTab[0] = false;
        }
        else if(keyInput[KEY_2] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(MAGIC_INVENTORY_CONTEXT,TOOL_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[2] = true;
            guiDrawInventoryIconTab[1] = false;
        }
        else if(keyInput[KEY_4] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(MAGIC_INVENTORY_CONTEXT,MATERIAL_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[2] = true;
            guiDrawInventoryIconTab[3] = false;
        }
        else if(keyInput[KEY_5] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(MAGIC_INVENTORY_CONTEXT,KEY_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[2] = true;
            guiDrawInventoryIconTab[4] = false;
        }
        else if(keyInput[KEY_6] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(MAGIC_INVENTORY_CONTEXT,MISC_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[2] = true;
            guiDrawInventoryIconTab[5] = false;
        }

        for(unsigned int i = KEY_A; i < KEY_A+23; i++)
        {
            if(keyInput[i] && player->magicInventory.size() >= i-KEY_A+1)
                viewedItem = player->magicInventory[i-KEY_A];
        }
        break;

    case MATERIAL_INVENTORY_CONTEXT:
        if( (keyInput[KEY_4] && controlContextChangeDelay == 0) ||
                (keyInput[KEY_I] && controlContextChangeDelay == 0)) // Should later add in ESC
        {
            ChangeControlContext(MATERIAL_INVENTORY_CONTEXT, NORMAL_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[3] = true;
        }
        else if(keyInput[KEY_1] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(MATERIAL_INVENTORY_CONTEXT,EQUIP_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[3] = true;
            guiDrawInventoryIconTab[0] = false;
        }
        else if((keyInput[KEY_2] && controlContextChangeDelay == 0) ||
                (keyInput[KEY_I] && controlContextChangeDelay == 0))
        {
            ChangeControlContext(MATERIAL_INVENTORY_CONTEXT,TOOL_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[3] = true;
            guiDrawInventoryIconTab[1] = false;
        }
        else if(keyInput[KEY_3] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(MATERIAL_INVENTORY_CONTEXT,MAGIC_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[3] = true;
            guiDrawInventoryIconTab[2] = false;
        }
        else if(keyInput[KEY_5] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(MATERIAL_INVENTORY_CONTEXT,KEY_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[3] = true;
            guiDrawInventoryIconTab[4] = false;
        }
        else if(keyInput[KEY_6] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(MATERIAL_INVENTORY_CONTEXT,MISC_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[3] = true;
            guiDrawInventoryIconTab[5] = false;
        }

        for(unsigned int i = KEY_A; i < KEY_A+23; i++)
        {
            if(keyInput[i] && player->materialInventory.size() >= i-KEY_A+1)
                viewedItem = player->materialInventory[i-KEY_A];
        }
        break;

    case KEY_INVENTORY_CONTEXT:
        if(keyInput[KEY_5] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(KEY_INVENTORY_CONTEXT,NORMAL_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[4] = true;
        }
        else if( (keyInput[KEY_1] && controlContextChangeDelay == 0) ||
                 (keyInput[KEY_I] && controlContextChangeDelay == 0)) // Should later add in ESC
        {
            ChangeControlContext(KEY_INVENTORY_CONTEXT, EQUIP_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[4] = true;
            guiDrawInventoryIconTab[0] = false;
        }
        else if((keyInput[KEY_2] && controlContextChangeDelay == 0) ||
                (keyInput[KEY_I] && controlContextChangeDelay == 0))
        {
            ChangeControlContext(KEY_INVENTORY_CONTEXT,TOOL_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[4] = true;
            guiDrawInventoryIconTab[1] = false;
        }
        else if(keyInput[KEY_3] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(KEY_INVENTORY_CONTEXT,MAGIC_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[4] = true;
            guiDrawInventoryIconTab[2] = false;
        }
        else if(keyInput[KEY_4] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(KEY_INVENTORY_CONTEXT,MATERIAL_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[4] = true;
            guiDrawInventoryIconTab[3] = false;
        }
        else if(keyInput[KEY_6] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(KEY_INVENTORY_CONTEXT,MISC_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[4] = true;
            guiDrawInventoryIconTab[5] = false;
        }

        for(unsigned int i = KEY_A; i < KEY_A+23; i++)
        {
            if(keyInput[i] && player->keyInventory.size() >= i-KEY_A+1)
            {
                viewedItem = player->keyInventory[i-KEY_A];
            }
        }
        break;

    case MISC_INVENTORY_CONTEXT:
        if(keyInput[KEY_6] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(MISC_INVENTORY_CONTEXT,NORMAL_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[5] = true;
        }
        else if( (keyInput[KEY_1] && controlContextChangeDelay == 0) ||
                 (keyInput[KEY_I] && controlContextChangeDelay == 0)) // Should later add in ESC
        {
            ChangeControlContext(MISC_INVENTORY_CONTEXT, EQUIP_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[5] = true;
            guiDrawInventoryIconTab[0] = false;
        }
        else if((keyInput[KEY_2] && controlContextChangeDelay == 0) ||
                (keyInput[KEY_I] && controlContextChangeDelay == 0))
        {
            ChangeControlContext(MISC_INVENTORY_CONTEXT,TOOL_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[5] = true;
            guiDrawInventoryIconTab[1] = false;
        }
        else if(keyInput[KEY_3] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(MISC_INVENTORY_CONTEXT, MAGIC_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[5] = true;
            guiDrawInventoryIconTab[2] = false;
        }
        else if(keyInput[KEY_4] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(MISC_INVENTORY_CONTEXT, MATERIAL_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[5] = true;
            guiDrawInventoryIconTab[3] = false;
        }
        else if(keyInput[KEY_5] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(MISC_INVENTORY_CONTEXT, KEY_INVENTORY_CONTEXT);
            viewedItem = nullptr;
            guiDrawInventoryIconTab[5] = true;
            guiDrawInventoryIconTab[4] = false;
        }

        for(unsigned int i = KEY_A; i < KEY_A+23; i++)
        {
            if(keyInput[i] && player->miscInventory.size() >= i-KEY_A+1)
            {
                viewedItem = player->miscInventory[i-KEY_A];
            }
        }
        break;

/// Player stats context //////////////////////////////////////////////////////////////////////////////////

    case PLAYER_STAT_CONTEXT:
        if(keyInput[KEY_SHIFT] && keyInput[KEY_C] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(PLAYER_STAT_CONTEXT, NORMAL_CONTEXT);
        }

        for(unsigned int i = KEY_A; i < KEY_A+7; i++)
        {
            if(keyInput[i] && player->wornEquipment.size() >= i-KEY_A+1)
            {
                if(player->wornEquipment[i-KEY_A] != nullptr) // Item is equipped here.
                {
                    if(player->equipInventory.size() < 24) // Player equipInventory has room to un-equip.
                    {
                        // "Unequip"
                        player->equipInventory.push_back(player->wornEquipment[i-KEY_A]); // 1. Add a copy of item pointer to equipinventory vector.
                        player->wornEquipment[i-KEY_A] = nullptr; //2. Nullify item's slot in wornequip vector.

                        player->RecalculateStats();
                        player->RecalculateSpells();
                    }
                    else // Player equipInventory has no room.
                    {
                        // error message flash + terminal log.
                    }
                }

                // //viewedItem = player->wornEquipment[i-KEY_A];
            }
        }
        break;

/// Weapon spell select context ////////////////////////////////////////////////////////////////////////////

    case WEAPON_SPELL_CONTEXT:
        if(keyInput[KEY_SHIFT] && keyInput[KEY_Z] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(WEAPON_SPELL_CONTEXT, NORMAL_CONTEXT);
        }


        break;

    }
}

void UpdateGamesystem()
{
    if(controlContextChangeDelay > 0)
        controlContextChangeDelay --;

    playerXCell = player->xCell;
    playerYCell = player->yCell;
    playerXPosition = player->xPosition;
    playerYPosition = player->yPosition;

    if(targetScanMoveDelay > 0)
        targetScanMoveDelay --;

}

/// Development Functions ///////////////////////////

void DevAddTestItemsToPlayer()
{

    player->equipInventory.push_back(new Equip(EQUIP_TEMPLATE_XIPHOS));

    std::cout << "debug- Xiphos' primary stat modifiers:";
    for(int i = 0; i < STAT_PRIMARY_TOTAL; i++)
    {
        std::cout << player->equipInventory[0]->primaryMod[i] << " | ";
    }
    std::cout << std::endl;

    player->equipInventory.push_back(new Equip(EQUIP_TEMPLATE_SCYTHE));
    player->equipInventory.push_back(new Equip(EQUIP_TEMPLATE_KRIS));
    player->equipInventory.push_back(new Equip(EQUIP_TEMPLATE_KATANA));

    player->toolInventory.push_back(new Tool(TOOL_TEMPLATE_POTION));
    player->toolInventory.push_back(new Tool(TOOL_TEMPLATE_BOMB));
    player->toolInventory.push_back(new Tool(TOOL_TEMPLATE_POTION));
    player->toolInventory.push_back(new Tool(TOOL_TEMPLATE_SPEAR));
    player->toolInventory.push_back(new Tool(TOOL_TEMPLATE_TRICK_KNIFE));

    player->materialInventory.push_back(new Material(MATERIAL_TEMPLATE_MATERIA));
    player->materialInventory.push_back(new Material(MATERIAL_TEMPLATE_WOOD));
    player->materialInventory.push_back(new Material(MATERIAL_TEMPLATE_MAPLE_LEAF));

    player->RecalculateStats();
    player->RecalculateSpells();

}


