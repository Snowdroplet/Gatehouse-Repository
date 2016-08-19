/**
*
* ERROR: MISMATCH BETWEEN BODY DIMENSIONS AND ROOMGENBOX DIMENSIONS
* CENTER COORDINATE (X3, Y3) MATCHES
* OPENING AND CLOSING COORDINATES OF BODY DO NOT MATCH ROOM GEN BOX OBJECT DO NOT MATCH (X1, Y1) / (X2, Y2)
*
*
**/



#include "area.h"

//#define D_GEN_PHYS_DIST_RANDOM
#define D_GEN_PHYS_DIST_SNAP

RoomGenBox::RoomGenBox(b2Body *cb2b, int w, int h)
{
    correspondingB2Body = cb2b;
    correspondingBodyAwake = correspondingB2Body->IsAwake();
    width = w;
    height = h;

    UpdatePosition();
}

RoomGenBox::~RoomGenBox()
{
}

void RoomGenBox::UpdatePosition()
{
    b2Vec2 updatedPosition = correspondingB2Body->GetPosition();
    correspondingBodyAwake = correspondingB2Body->IsAwake();

    x3 = updatedPosition.x;
    y3 = updatedPosition.y;

    x1 = x3 - width/2;
    x2 = x3 + width/2;
    y1 = y3 - height/2;
    y2 = y3 + height/2;
}

void RoomGenBox::SnapToGrid()
{
    // Snap to grid - All coordinates rounded down to next lowest multiple of MINI_TILESIZE.
    int x = x1; // Integer type for the purpose of modulo operation
    int y = y1;

    x1 -= x%MINI_TILESIZE;
    y1 -= y%MINI_TILESIZE;
    x2 -= x%MINI_TILESIZE;
    y2 -= y%MINI_TILESIZE;
    x3 =  x1+width;
    y3 =  y1+height;

}

bool RoomGenBox::BoundaryDeletionCheck(int strictness)
{
    if(x1 < 0+MINI_TILESIZE*strictness || y1 < 0+MINI_TILESIZE*strictness)
        return true;
    if(x2 > miniAreaWidth-MINI_TILESIZE*strictness || y2 > miniAreaHeight-MINI_TILESIZE*strictness)
        return true;
    else return false;
}

/// Default constructor which creates random square test area
/// Data normally found in the serialized "areafile" is initialized here.
Area::Area()
{
    name = "test area";
    floor = "test floor";
    dLevel = 0;

    for(int i = 0; i < areaCellWidth*areaCellHeight; i++)
    {
        occupied.push_back(false);
        wallmap.push_back(WALL_EMPTY);
        tilemap.push_back(rand()%3);
    }

    ResetGeneratorState();
}

/// Savefile constructor: All necessary variables are initialized from the serialized "areafile."
Area::Area(bool savedArea)
{
    ResetGeneratorState();
}

void Area::InitByArchive()
{
}

Area::~Area()
{
    // Although all room gen boxes should have been deleted by now, anyway, this covers us during development/debug.
    for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end(); ++it)
    {
        delete *it;
        roomGenBoxes.erase(it);
    }
}

/// Reset / (re)initialize everything relevant to generating an area.
void Area::ResetGeneratorState()
{
    mtRng.seed(static_cast<unsigned int>(std::time(0)));

    generationPhase = GEN_INACTIVE;
    generationComplete = true;
    generationPhaseComplete = false;

    averageRoomWidth = MINI_TILESIZE*5; // 5 Cells wide
    averageRoomHeight = MINI_TILESIZE*5;

    mainRoomWidthThreshold = MINI_TILESIZE*6;
    mainRoomHeightThreshold = MINI_TILESIZE*6;

    bodiesGenerated = false;
    bodiesDistributed = false;

    roomGenBody.type = b2_dynamicBody;
    roomGenBody.fixedRotation = true;
    roomGenBody.linearDamping = 0.0f;
    roomGenBody.allowSleep = true;

    GENERATORDEBUGSTASIS = true;
}

void Area::Generate()
{
    if(generationPhase == GEN_INACTIVE)
    {
        generationComplete = false;
        generationPhaseComplete = true;
    }

    else if(generationPhase == GEN_PHYSICAL_DISTRIBUTION)
    {

        /**                               ### PHYSICAL DISTRIBUTION PHASE: ###
            1) Create many overlapping PHYSICS BODIES within a circle of random size according to the normal distribution.
            2) Create ROOM OBJECTS corresponding to physics bodies in shape and position.
            3) Distribute PHYSICS BODIES into field via Box2D physics simulation so that none overlap.
            4) Constantly update positions of ROOM OBJECTS to match PHYSICS BODIES.
            5) When all bodies are at rest, align ROOM OBJECTS to grid.
        */

        // Room dimensions randomized. Probability represented by normal distribution.
        boost::random::normal_distribution<float> randomRoomWidth (averageRoomWidth,averageRoomWidth*0.25);   // (Average, 25% standard deviation)
        boost::random::normal_distribution<float> randomRoomHeight(averageRoomHeight,averageRoomHeight*0.25);

        // Random point in circle selected using random radius and angle. Probability is uniform.
        boost::random::uniform_real_distribution<double> randomGenRadius(0.0,miniAreaWidth*0.25);    // (Range min, range max)
        boost::random::uniform_real_distribution<double> randomGenTheta(0.0,2*PI);

        static int roomsToGenerate = 150;

        if(!bodiesGenerated)
        {
            for(int i = 0; i < roomsToGenerate; i++)
            {
#ifdef D_GEN_PHYS_DIST_RANDOM
                std::cout << std::endl;
                std::cout << "#Generating room #" << i << std::endl;
#endif // D_GEN_PHYS_DIST_RANDOM

                int roomWidth = randomRoomWidth(mtRng);  // Implicit conversion to int for the purposes of the modulo operation below.
                int roomHeight = randomRoomHeight(mtRng);

                // Prevent the creation of rooms less than two tiles wide or high
                if(roomWidth < MINI_TILESIZE*1.49)
                    roomWidth = MINI_TILESIZE*2;
                if(roomHeight < MINI_TILESIZE*1.49)
                    roomHeight = MINI_TILESIZE*2;

#ifdef D_GEN_PHYS_DIST_RANDOM
                std::cout << "Room width before rounding: " << roomWidth << std::endl;
                std::cout << "Room height before rounding: " << roomHeight << std::endl;
#endif // D_GEN_PHYS_DIST_RANDOM

                // Round room dimensions to the nearest multiple of MINI_TILESIZE.
                if(roomWidth%MINI_TILESIZE > MINI_TILESIZE/2)             // If rounding up to nearest MINI_TILESIZE:
                    roomWidth += MINI_TILESIZE - roomWidth%MINI_TILESIZE; // Increase dimensions by to meet nearest multiple of MINI_TILESIZE.
                else                                            // If rounding down to nearest MINI_TILESIZE:
                    roomWidth -= roomWidth%MINI_TILESIZE;            // Decrease dimensions by excess of nearest multiple of MINI_TILESIZE.

                if(roomHeight%MINI_TILESIZE > MINI_TILESIZE/2)
                    roomHeight += MINI_TILESIZE - roomHeight%MINI_TILESIZE;
                else
                    roomHeight -= roomHeight%MINI_TILESIZE;

#ifdef D_GEN_PHYS_DIST_RANDOM
                std::cout << "Room width after rounding: " << roomWidth << std::endl;
                std::cout << "Room height after rounding: " << roomHeight << std::endl;
#endif // D_GEN_PHYS_DIST_RANDOM

                // Take a random point in the circle
                double circleCenterX = miniAreaWidth/2;  // Circle center position in the world may vary at a later point in development.
                double circleCenterY = miniAreaHeight/2;

                double genRadius = randomGenRadius(mtRng);
                double genTheta = randomGenTheta(mtRng);

                double genX = genRadius*cos(genTheta);
                double genY = genRadius*sin(genTheta);

#ifdef D_GEN_PHYS_DIST_RANDOM
                std::cout << "Random generation circle --" << std::endl;
                std::cout << "x: " << circleCenterX << ", y: " << circleCenterY << std::endl;
                std::cout << "r: " << genRadius << ", Theta in radians: " << genTheta << std::endl;
                std::cout << "r cos(t) = " << genX << ", r sin(t) = " << genY << std::endl;
#endif // D_GEN_PHYS_DIST_RANDOM

                //Generate PHYSICS BODY centered at chosen random point in circle.
                roomGenBody.position.Set(circleCenterX+genX,circleCenterY+genY); // Set position of body in the world to chosen random point in circle.
                b2Body *body = physics->CreateBody(&roomGenBody);  // Create body according to roomGenBody's description. Physics is the name of the simulation's world object.


                // Describe the shape of the fixture that will be created on the PHYSICS BODY.
                b2Vec2 genPolyVertices[4];                          // Vertices are used to create a rectangle. Coordinates are listed counterclockwise.
                genPolyVertices[0].Set(-roomWidth/2, -roomHeight/2); // Top Left.
                genPolyVertices[1].Set(-roomWidth/2,  roomHeight/2); // Bottom Left.
                genPolyVertices[2].Set( roomWidth/2,  roomHeight/2); // Bottom Right.
                genPolyVertices[3].Set( roomWidth/2, -roomHeight/2); // Top Right.

                // Set the fixture to the shape described above.
                roomGenPolygonShape.Set(genPolyVertices,4);         // (Receives vertice array, number of elements in vertice array)
                roomGenFixture.shape = &roomGenPolygonShape;        //  Fixture takes on shape described above.

                // Attach fixture to PHYSICS BODY.
                body->CreateFixture(&roomGenFixture);              // Attach fixture to aformentioned body, according to roomGenFixture's description.

                // Create ROOM OBJECT corresponding to the PHYSICS BODY in both shape and position.
                roomGenBoxes.push_back(new RoomGenBox(body,roomWidth,roomHeight)); // (Corresponding PHYSICS BODY, width of ROOM OBJECT, height of ROOM OBJECT).

            }


            bodiesGenerated = true;
        }

        if(!bodiesDistributed)
        {
            physics->Step(1/30.0f, 1, 10); // (Time step, number of velocity iterations, number of position iterations)

            bodiesDistributed = true;
            for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end(); ++it)
            {
                (*it)->UpdatePosition();

                if((*it)->correspondingB2Body->IsAwake())
                    bodiesDistributed = false;
            }

            physics->ClearForces();
        }
        else if(bodiesDistributed)
        {

            for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end(); ++it)
            {
                (*it)->SnapToGrid(); // Snap ROOM OBJECTS to grid by flooring them to the nearest 32.
                if((*it)->BoundaryDeletionCheck(1))//Remove rooms which are within 1 cell of the edge of the area, or even outside.
                {
                    delete *it;
                    roomGenBoxes.erase(it);
                }
            }

            // Destroy physics bodies
            for(b2Body* b = physics->GetBodyList(); b; b = b->GetNext())
            {
                physics->DestroyBody(b);
            }

            generationPhaseComplete = true;
        }
    }

    if(generationPhase == GEN_NODE_DISTRIBUTION)
    {
        /** #### NODE DISTRIBUTION PHASE ####
        1) MAIN ROOMS vector populated with ROOM OBJECTS
        */

            for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end(); ++it)
            {
                if((*it)->width >= mainRoomWidthThreshold && (*it)->height >= mainRoomHeightThreshold)
                {
                    mainRooms.push_back(*it);
                }
            }

            // *possible thingy here to pause simulation if debug mode is on?*
            generationPhaseComplete = true;
    }

    if(generationPhase == GEN_DELAUNAY_TRIANGULATION)
    {

    }



    /// Generator progress

    if(generationPhaseComplete)
    {
        generationPhase ++;
        generationPhaseComplete = false;
    }
    if(generationPhase == GEN_COMPLETE)
    {
        ResetGeneratorState();
        needGeneration = false;
    }

}


bool SaveAreaState(Area *target)
{
    std::string fileName = (target->name) + ".areafile";
    std::ofstream areafile(fileName);

    if(!areafile.is_open())
    {
        s_al_show_native_message_box(display, "SAVE FAILURE", "SAVE FAILURE", fileName + "could not be created/opened.",
                                     NULL, ALLEGRO_MESSAGEBOX_ERROR);

        return false;
    }

    boost::archive::text_oarchive outArchive(areafile);
    outArchive & *target;

    return true;
}

bool LoadAreaState(std::string areaName, Area *target, bool baseState)
{
    std::cout << "Debug: entered area loading function..." << std::endl;

    std::string fileName;
    if(baseState)
        fileName = areaName + ".areabase";
    else
        fileName = areaName + ".areafile";


    std::ifstream areafile(fileName);

    if(!areafile.is_open())
    {
        s_al_show_native_message_box(display, "LOAD FAILURE", "LOAD FAILURE", fileName + "could not be found/opened.",
                                     NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return false;
    }

    std::cout << "Debug: file opened sucessfully..." << std::endl;

    boost::archive::text_iarchive inArchive(areafile);
    inArchive >> *target;

    std::cout << "Debug: target Area object restored..." << std::endl;

    return true;
}


