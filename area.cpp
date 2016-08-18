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
#define D_GEN_PHYS_DIST_POSITIONS

RoomGenBox::RoomGenBox(b2Body *cb2b, int w, int h)
{
    correspondingB2Body = cb2b;
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

    x3 = updatedPosition.x;
    y3 = updatedPosition.y;

    x1 = x3 - width/2;
    x2 = x3 + width/2;
    y1 = y3 - height/2;
    y2 = y3 + height/2;
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
}

/// Reset / (re)initialize everything relevant to generating an area.
void Area::ResetGeneratorState()
{
    mtRng.seed(static_cast<unsigned int>(std::time(0)));

    generationPhase = GEN_INACTIVE;
    generationComplete = true;
    generationPhaseComplete = false;

    bodiesGenerated = false;
    bodiesDistributed = false;

    roomGenBody.type = b2_dynamicBody;
    roomGenBody.fixedRotation = true;
    roomGenBody.linearDamping = 0.3f;
    roomGenBody.allowSleep = true;

    roomGenFixture.density = 1.0f;
    roomGenFixture.restitution = 6.0f;
    roomGenFixture.friction = 0.0f;
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
#ifdef D_GEN_PHYS_DIST_POSITIONS
        static bool dGenDistPosNeeded = false;
        static bool dGenDistPosGiven = false;
#endif //D_GEN_PHYS_DIST_POSTIONS

        /**                               ### PHYSICAL DISTRIBUTION PHASE: ###
            1) Create many overlapping PHYSICS BODIES within a circle of random size according to the normal distribution.
            2) Create ROOM OBJECTS corresponding to physics bodies in shape and position.
            3) Distribute PHYSICS BODIES into field via Box2D physics simulation so that none overlap.
            4) Constantly update positions of ROOM OBJECTS to match PHYSICS BODIES.
            5) Align ROOM OBJECTS to grid.
        */

        // Room dimensions randomized. Probability represented by normal distribution.
        boost::random::normal_distribution<float> randomRoomWidth (averageRoomWidth,averageRoomWidth*0.15);   // (Average, 15% standard deviation)
        boost::random::normal_distribution<float> randomRoomHeight(averageRoomHeight,averageRoomHeight*0.15);

        // Random point in circle selected using random radius and angle. Probability is uniform.
        boost::random::uniform_real_distribution<double> randomGenRadius(0.0,TILESIZE*5);    // (Range min, range max)
        boost::random::uniform_real_distribution<double> randomGenTheta(0.0,2*PI);

        static int roomsToGenerate = 20;

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

#ifdef D_GEN_PHYS_DIST_RANDOM
                std::cout << "Room width before rounding: " << roomWidth << std::endl;
                std::cout << "Room height before rounding: " << roomHeight << std::endl;
#endif // D_GEN_PHYS_DIST_RANDOM

                // Round room dimensions to the nearest multiple of TILESIZE.
                if(roomWidth%TILESIZE > TILESIZE/2)             // If rounding up to nearest TILESIZE:
                    roomWidth += TILESIZE - roomWidth%TILESIZE; // Increase dimensions by to meet nearest multiple of TILESIZE.
                else                                            // If rounding down to nearest TILESIZE:
                    roomWidth -= roomWidth%TILESIZE;            // Decrease dimensions by excess of nearest multiple of TILESIZE.

                if(roomHeight%TILESIZE > TILESIZE/2)
                    roomHeight += TILESIZE - roomHeight%TILESIZE;
                else
                    roomHeight -= roomHeight%TILESIZE;

#ifdef D_GEN_PHYS_DIST_RANDOM
                std::cout << "Room width after rounding: " << roomWidth << std::endl;
                std::cout << "Room height after rounding: " << roomHeight << std::endl;
#endif // D_GEN_PHYS_DIST_RANDOM

                // Take a random point in the circle
                double circleCenterX = areaWidth/2;  // Circle center position in the world may vary at a later point in development.
                double circleCenterY = areaHeight/2;

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
                b2Body *body = physics->CreateBody(&roomGenBody);  // Create body according to roomGenBody's description. Physics is the name of the simulation's world object.
                roomGenBody.position.Set(circleCenterX+genX,circleCenterY+genY); // Set position of body in the world to chosen random point in circle.


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
            physics->Step(timeStep, velocityIterations, positionIterations);

            /*
            b2Vec2 snapPosition;

            for(b2Body*b = physics->GetBodyList(); b; b = b->GetNext())
            {
                if(!b->IsAwake())
                {
                    snapPosition = b->GetPosition();
                    int x = snapPosition.x;
                    int y = snapPosition.y;

                    if(x%TILESIZE > TILESIZE/2)
                        x += TILESIZE - x%TILESIZE;
                    else
                        x -= x%TILESIZE;

                    if(y%TILESIZE > TILESIZE/2)
                        y += TILESIZE - y%TILESIZE;
                    else
                        y -= y%TILESIZE;


                    b->SetTransform(b2Vec2(x,y),0);

                }
            }

            */

            for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end(); ++it)
            {
                (*it)->UpdatePosition();
            }

            physics->ClearForces();

#ifdef D_GEN_PHYS_DIST_POSITIONS
            dGenDistPosNeeded = true;
            for(b2Body*b = physics->GetBodyList(); b; b = b->GetNext())
            {
                if(b->IsAwake())
                    dGenDistPosNeeded = false;
            }
#endif //D_GEN_PHYS_DIST_POSITIONS
        }

#ifdef D_GEN_PHYS_DIST_POSITIONS
        if(dGenDistPosNeeded && !dGenDistPosGiven)
        {
            dGenDistPosGiven = true;
            std::cout << std::endl;
            for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end(); ++it)
            {
                std::cout << "Coordinates for #" << it-roomGenBoxes.begin() << ":" << std::endl;
                std::cout << "Gen Center: (" << (*it)->x3 << ", " << (*it)->y3 << ")" << std::endl;
                std::cout << "Gen Open: (" << (*it)->x1 << ", " << (*it)->y1 << ")" << std::endl;
                std::cout << "Gen Close: (" << (*it)->x2 << ", " << (*it)->y2 << ")" << std::endl;

                b2Vec2 position = (*it)->correspondingB2Body->GetPosition();

                std::cout << "Physics body Center: (" << position.x << ", " << position.y << ")" <<std::endl;
                //std::cout << "Extents x: " << extents.x << std::endl;
                //std::cout << "Extents y: " << extents.y << std::endl;
                //std::cout << "Physics body Open: (" << std::endl;
                //std::cout << "Physics body Close: (" << std::endl;
                std::cout << std::endl;
            }
        }
#endif //D_GEN_PHYS_DIST_POSITIONS

        if(generationPhaseComplete)
        {
            for(b2Body* b = physics->GetBodyList(); b; b = b->GetNext())
            {
                physics->DestroyBody(b);
            }

            for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end(); ++it)
            {
                delete *it;
                roomGenBoxes.erase(it);
            }
        }
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


