/***
Error:
    NPC sprite drawing position updates too slow.

To do:
    !! Fire spell
        if(keyInput[key_f]) and target is acquired
            Player calls Being::ReleaseSpell();

        Check UpdateObjects function for spell coverage functionality

    !! Equipment system must be implemented before fire spell.
        Change current equipment with 'w'. Add context: equipment.

    !! Inventory system must be implemented before equipment system.
        Add context: inventory.

    ! Select active spell with 'z'.

To optimize later:
    ! Restructure so that UpdateObjects' non-animation functions
        (esp. iterating through all vectors) is only called upon when "stuff" happens
        checking every logic cycle seems a waste of CPU

    ! Fix sporadic infinite looping in distributing generation bodies

    ! Movement is not smooth, find out why and fix

*/

/***
How to operate during generator debug visualization:

    Q = Progress generation
    Numpad = move camera

How to operate debug:
    TY = Output test path from player to random cell
    UI = Move player to last cell on path


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
#include "material.h"

#include "being.h"
#include "player.h"
#include "npc.h"

/// Single-object classes ###########
Generator *generator = nullptr;
Area *area = nullptr;
Player *player = nullptr;

/// Item containers and functions ##################
std::vector<Item*>items; // All items currently in play in the current area.
std::vector<int>baseIDsKnown; // A list of all previously identified itemIDs. These will auto-identify when encountered again.

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
            (*it)->actionPoints += (*it)->effectiveSpeed; // Each being receives AP according to its effective speed.
#ifdef D_TURN_LOGIC
            std::cout << (*it)->name << " has " << (*it)->actionPoints << "/" << (*it)->effectiveSpeed << " AP." << std::endl;
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
                    std::cout << (*actionQueueFront)->name << " now has " << (*actionQueueFront)->actionPoints << "/" << (*actionQueueFront)->effectiveSpeed << "AP" << std::endl;
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
                    std::cout << player->name << " now has " << player->actionPoints << "/" << player->effectiveSpeed << "AP" << std::endl;
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

        /// ********Set Being initial positions here **********
        // Perhaps by creating and copying from a vector of initial beings?
        // Such as generator->beingSpawnpositions[blah];

        player = new Player(area->upstairsXCell,area->upstairsYCell);
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

                s_al_draw_centered_text(terminalFont, NEUTRAL_WHITE,
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

    GuiDrawFrame();

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

void UpdateObjects()
{
    for(std::vector<Item*>::iterator it = items.begin(); it != items.end();)
    {
        if(gameExit)
            (*it)->active = false;

        if(!(*it)->active)
        {
            delete *it;
            items.erase(it);
        }
        else
        {
            ++it;
        }
    }


    for(std::vector<Spell*>::iterator it = activeSpells.begin(); it != activeSpells.end();)
    {
        /*
        for(std::vector<int>::iterator ccit = (*it)->cellsCovered.begin(); ccit != (*it)->cellsCovered.end(); ++ccit)
        {
            if(area->beingmap[*ccit] != nullptr)
            {
                // Combine target being's active effects vector with spell's effects vector.
                Being*bpointer = area->beingmap[*ccit];
                bpointer->effects.insert(bpointer->effects.end(),(*it)->effects.begin(), (*it)->effects.end());
            }
        }
        */

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
}

/// ########## ########## END CLASS AND MEMBER OBJECT MANAGMEMENT FUNCTIONS ########## ##########

void PopulateItems()
{

    items.clear();

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

            if(keyInput[KEY_F])
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

            else if(keyInput[KEY_I] && controlContextChangeDelay == 0)
            {
                ChangeControlContext(NORMAL_CONTEXT,INVENTORY_CONTEXT);
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

    case INVENTORY_CONTEXT:
        if(keyInput[KEY_I]) // Should later change this to ESC
            ChangeControlContext(INVENTORY_CONTEXT, NORMAL_CONTEXT);

        for(unsigned int i = KEY_A; i < KEY_Z; i++)
        {
            if(keyInput[i])
            {

            }
        }

        break;

/// Weapon spell context ///////////////////////////////////////////////////////////////////////////////////

    case WEAPON_SPELL_CONTEXT:
        if(keyInput[KEY_Z] && controlContextChangeDelay == 0)
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
    player->equipInventory.push_back(new Equip(EQUIP_TEMPLATE_DAGGER));
    player->toolInventory.push_back(new Tool(TOOL_TEMPLATE_POTION));
    player->materialInventory.push_back(new Material(MATERIAL_TEMPLATE_WOOD));


}


