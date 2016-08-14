#include "area.h"

#define D_GEN_PHYS_DIST_RANDOM

RoomGenBox::RoomGenBox(b2Body *cb2b, int w, int h)
{
    correspondingB2Body = cb2b;
    width = w;
    height = h;

    UpdatePosition();
}

RoomGenBox::~RoomGenBox()
{
    correspondingB2Body = NULL;
}

void RoomGenBox::UpdatePosition()
{
    b2Vec2 position = correspondingB2Body->GetPosition();

    x3 = position.x;
    y3 = position.y;

    x1 = x3-width/2;
    x2 = x3+width/2;
    y1 = y3-height/2;
    y2 = y3+height/2;
}

Area::Area() // Default creates test area
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

Area::Area(bool savedArea) // Nothing needs to be done because all relevant variables are found in the serialized areafile.
{
    ResetGeneratorState();
}

void Area::InitByArchive()
{
}

Area::~Area()
{
}

void Area::ResetGeneratorState()
{
    generationPhase = GEN_INACTIVE;
    generationComplete = true;
    generationPhaseComplete = false;

    bodiesGenerated = false;
    bodiesDistributed = false;

    roomGenBody.type = b2_dynamicBody;
    roomGenBody.fixedRotation = true;
    roomGenBody.linearDamping = 0.3f;
    roomGenBody.allowSleep = true;

    roomGenFixture.shape = &roomGenBox;
    roomGenFixture.density = 1.0f;
    roomGenFixture.friction = 0.3f;
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
        /*
            1) Create many (almost-certainly) overlapping AABBs within a circle of random size according to the normal distribution.
            2) Expand AABBs into field by pushing intersecting AABBs apart using pseudo-physics functions.
            3) Align to TILESIZE grid while removing overlaps.
        */

        boost::random::normal_distribution<float> randomRoomWidth (averageRoomWidth,averageRoomWidth*0.15);
        boost::random::normal_distribution<float> randomRoomHeight(averageRoomHeight,averageRoomHeight*0.15);

        boost::random::uniform_real_distribution<double> randomGenRadius(0.0,TILESIZE*5);
        boost::random::uniform_real_distribution<double> randomGenTheta(0.0,2*PI);

        static int roomsToGenerate = 2;

        if(!bodiesGenerated)
        {
            for(int i = 0; i < roomsToGenerate; i++)
            {
#ifdef D_GEN_PHYS_DIST_RANDOM
                std::cout << std::endl;
                std::cout << "#Generating room #" << i << std::endl;
#endif // D_GEN_PHYS_DIST_RANDOM

                // Conversion to int for the purpose of modulo operation.
                int roomWidth = randomRoomWidth(mtRng);
                int roomHeight = randomRoomHeight(mtRng);

#ifdef D_GEN_PHYS_DIST_RANDOM
                std::cout << "Room width before rounding: " << roomWidth << std::endl;
                std::cout << "Room height before rounding: " << roomHeight << std::endl;
#endif // D_GEN_PHYS_DIST_RANDOM

                // Round AABB dimensions in pixels to the nearest multiple of TILESIZE.
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

                //Take a random point in circle


                double circleCenterX = areaWidth/2; // This may vary at a later point.
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

                //Generate body centered at chosen point.

                roomGenBox.SetAsBox(roomWidth/2,roomHeight/2);
                roomGenBody.position.Set(circleCenterX+genX,circleCenterY+genY);

                b2Body *body = physics->CreateBody(&roomGenBody);
                body->CreateFixture(&roomGenFixture);

                roomGenBoxes.push_back(new RoomGenBox(body,roomHeight,roomWidth));

            }
            bodiesGenerated = true;
        }

        if(!bodiesDistributed)
        {
            physics->Step(timeStep, velocityIterations, positionIterations);

            for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end(); ++it)
            {
                (*it)->UpdatePosition();
            }

            physics->ClearForces();
        }

        if(generationPhaseComplete)
        {
            for (b2Body* b = physics->GetBodyList(); b; b = b->GetNext())
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


