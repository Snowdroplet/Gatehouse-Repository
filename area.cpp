
#include "area.h"

//#define D_GEN_PHYS_DIST_RANDOM
//#define D_GEN_TRIANGULATION
//#define D_VECTOR_UNIQUE_NODE_ID
//#define D_TRI_MST_COMBINATION
#define D_CELL_LAYOUT

#define D_GEN_VISUALIZATION_PHASE_PAUSE

RoomGenBox::RoomGenBox(int bn, b2Body *cb2b, int w, int h)
{
    boxNumber = bn;

    correspondingB2Body = cb2b;
    correspondingBodyAwake = correspondingB2Body->IsAwake();
    designatedMainRoom = false;

    width = w;
    height = h;
    cellWidth = width/MINI_TILESIZE;
    cellHeight = height/MINI_TILESIZE;

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
    int x = x1; // Conversion to integer type for the purpose of modulo operation
    int y = y1;

    x1 -= x%MINI_TILESIZE;
    y1 -= y%MINI_TILESIZE;
    x2 -= x%MINI_TILESIZE;
    y2 -= y%MINI_TILESIZE;
    x3 =  x1+width/2;
    y3 =  y1+height/2;

}

bool RoomGenBox::BoundaryDeletionCheck(int strictness)
{
    if(x1 < 0+MINI_TILESIZE*strictness || y1 < 0+MINI_TILESIZE*strictness)
        return true;
    if(x2 > miniAreaWidth-MINI_TILESIZE*strictness || y2 > miniAreaHeight-MINI_TILESIZE*strictness)
        return true;
    else return false;
}

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
    for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end();)
    {
        delete *it;
        roomGenBoxes.erase(it);
    }


}

bool Area::GetGenerationComplete()
{
    return generationComplete;
}

int Area::GetGenerationPhase()
{
    return generationPhase;
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

    SetRemovedEdgeReturnPercentage( (float)(rand()%20) / 100 ); // 0-20%

    for(int i = 0; i < areaCellWidth*areaCellHeight; i++)
    {
        genLayout.push_back(GEN_CELL_EMPTY);
    }

    D_GENERATORDEBUGSTASIS = true;
    D_GENERATORVISUALIZATIONPAUSE = false;
}

void Area::Generate()
{
    /// This generation function is based on the procedure described in this article:
    /// http://www.gamasutra.com/blogs/AAdonaac/20150903/252889/Procedural_Dungeon_Generation_Algorithm.php

    if(generationPhase == GEN_INACTIVE)
    {
        generationComplete = false;
        generationPhaseComplete = true;
    }

    else if(generationPhase == GEN_PHYSICAL_DISTRIBUTION)
    {

        /**                      ### ROOM OBJECTS DISTRIBUTED VIA PHYSICS SIMULATION ###
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
                if(roomWidth < MINI_TILESIZE*2)
                    roomWidth = MINI_TILESIZE*2;
                if(roomHeight < MINI_TILESIZE*2)
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
                roomGenBoxes.push_back(new RoomGenBox(i, body, roomWidth, roomHeight)); // (Corresponding PHYSICS BODY, width of ROOM OBJECT, height of ROOM OBJECT).

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

            for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end();)
            {
                (*it)->correspondingB2Body = NULL; // Nullify all pointers to corresponding PHYSICS BODIES, as the relationship is no longer necessary
                (*it)->SnapToGrid(); // Snap ROOM OBJECTS to grid by flooring them to the nearest MINI_TILESIZE.
                if((*it)->BoundaryDeletionCheck(1))//Remove rooms which are within 1 cell of the edge of the area, or even outside.
                {
                    delete *it;
                    roomGenBoxes.erase(it);
                }
                else
                    ++it;

            }

            // Destroy all PHYSICS BODIES
            for(b2Body* b = physics->GetBodyList(); b; b = b->GetNext())
            {
                physics->DestroyBody(b);
            }

            generationPhaseComplete = true;
        }
    }

    else if(generationPhase == GEN_MAIN_ROOM_SELECTION)
    {
        // Room boxes above a set width/height threshold are designated main rooms.
        for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end(); ++it)
        {
            if((*it)->width >= mainRoomWidthThreshold && (*it)->height >= mainRoomHeightThreshold)
            {
                mainRooms.push_back(*it);
                (*it)->designatedMainRoom = true;
            }
        }

        generationPhaseComplete = true;
    }


    else if(generationPhase == GEN_GRAPH_CREATION)
    {

        /**                ### DELAUNAY TRIANGULATION AND MINIMUM SPANNING TREE  ###

        Note: There are two graphs. The FIRST for triangulation, the SECOND for the MST.

        1)      Using Bl4ckb0ne's delaunay triangulation code (details can be found in delaunay.h/cpp),
            generate the FIRST graph - The center coordinates of ROOM OBJECTS designated as main rooms make up
            the nodes.

        2)      Generate the SECOND graph as a vector of node ID pairs (edges) from the triangulation.
            Each node IDs is the index of the cell corresponding to a main room's center coordinates.

        3)      Using the Kruskal algorithm (details be be found in mintree.h/cpp),
            generate and output a minimum spanning tree from the SECOND graph.

        4)      Shuffle and add a random percentage of the edges culled by the MST back to give the area a bit more connectivity.


        */

        std::vector<Vec2f>mainRoomCenterCoords;

        for(std::vector<RoomGenBox*>::iterator it = mainRooms.begin(); it != mainRooms.end(); ++it)
        {
            // The main-room-center coordinate to record
            Vec2f centerCoord( ((*it)->x3) , ((*it)->y3) );

            // Record the main-room-center coordinate in a vector -- The vector will be passed to the triangulation function.
            mainRoomCenterCoords.push_back(centerCoord);
        }


        Delaunay triangulation;
        triangles = triangulation.triangulate(mainRoomCenterCoords); // Pass vector of main-room-center coordinates to triangulation function.
        triEdges = triangulation.getTriEdges();

#ifdef D_GEN_TRIANGULATION
        std::cout << std::endl;
        std::cout << "### Triangulation results: ###" << std::endl;

        std::cout << "\nPoints : " << mainRoomCenterCoords.size() << std::endl;
        for(auto &p : mainRoomCenterCoords)
            std::cout << p << std::endl;

        std::cout << "\nTriangles : " << triangles.size() << std::endl;
        for(auto &t : triangles)
            std::cout << t << std::endl;

        std::cout << "\nEdges : " << triEdges.size() << std::endl;
        for(auto &e : triEdges)
            std::cout << e << std::endl;
#endif // D_GEN_TRIANGULATION


        for(std::vector<TriEdge>::iterator it = triEdges.begin(); it != triEdges.end(); ++it)
        {
            int nodeA = GetCenterCellID( (*it).p1.x , (*it).p1.y);
            int nodeB = GetCenterCellID( (*it).p2.x , (*it).p2.y);

            Vec2f coordsA = (*it).p1;
            Vec2f coordsB = (*it).p2;

            float coordDistance = std::sqrt(((coordsB.x-coordsA.x) * (coordsB.x-coordsA.x))  // Just the regular old pythagorean formula.
                                            +
                                            ((coordsB.y-coordsA.y) * (coordsB.y-coordsA.y)));

            // Store the edges found by the triangulation as the indexes of the cells corresponding to the coordinates.
            minTreeInput.push_back(MinTreeEdge(nodeA, nodeB));

            // Populate the MST graph with edges.
            mtg.minTreeEdges.push_back(MinTreeEdge(nodeA,nodeB,coordDistance));


            // Populate the MST graph with vertices (node IDs).
            mtg.nodeIDs.push_back(nodeA);
            //mtg.nodeIDs.push_back(nodeB);


        }

        // Keep only unique node IDs in the MST graph.
        std::vector<int>::iterator unid_it;
        unid_it = std::unique (mtg.nodeIDs.begin(), mtg.nodeIDs.end());
        mtg.nodeIDs.resize(std::distance(mtg.nodeIDs.begin(),unid_it));

#ifdef D_VECTOR_UNIQUE_NODE_ID
        std::cout << std::endl;
        std::cout << "Graph's nodeID vector contains:";
        for (uit=mtg.nodeIDs.begin(); uit!=mtg.nodeIDs.end(); ++uit)
            std::cout << ' ' << *uit;
        std::cout << '\n';
#endif //D_VECTOR UNIQUE_NODE_ID

        // Create the MST and store in a vector.
        minTreeOutput = Kruskal(mtg);

        //Compare minTreeOutput with minTreeInput and add back a certain percentage of the edges removed in the MST.
        int numberOfEdgesToReturn = std::round((triEdges.size()-minTreeOutput.size()) * removedEdgeReturnPercentage);  //The number of edges that have been removed is the triangulation's edges minus the MST's edges.

        //Shuffle mintreeinput (Containing the edges between nodeIDs after the triangulation)
        std::random_shuffle(minTreeInput.begin(),minTreeInput.end());

#ifdef D_TRI_MST_COMBINATION
        std::cout << std::endl;
        std::cout << "Number of edges in the triangulation:" << triEdges.size() << std::endl;
        std::cout << "Number of edges in the MST:" << minTreeOutput.size() << std::endl;
        std::cout << "Number of edges removed from the triangulation:" << triEdges.size() - minTreeOutput.size() << std::endl;
        std::cout << "Percentage of edges to re-add: " << removedEdgeReturnPercentage * 100 << "%" <<std::endl;
        std::cout << "Number of Edges to re-add: " << numberOfEdgesToReturn << std::endl;
        std::cout << "Before re-adding edges, the number of edges is " << minTreeOutput.size() << ": " << std::endl;
        std::cout << "Re-adding edges......." << std::endl;

#endif // D_TRI_MST_COMBINATION

        // Add elements of previously shuffled mintreeinput to mintreeoutput until its size increases by the number of edges we are returning.
        // Prevent duplicates using unique_copy.

        int currentSize = minTreeOutput.size();
        int targetSize = currentSize + numberOfEdgesToReturn;
        int sizeDistance = targetSize - currentSize;
        while(sizeDistance != 0)
        {
            //Probably really inefficient

            for(std::vector<MinTreeEdge>::iterator it = minTreeInput.begin(); it != minTreeInput.begin()+sizeDistance; ++it)
                minTreeOutput.push_back(*it);

            std::vector<MinTreeEdge>::iterator mto_it;
            mto_it = std::unique (minTreeOutput.begin(), minTreeOutput.end());

            currentSize = minTreeOutput.size();
            sizeDistance = targetSize - currentSize;
        }

#ifdef D_TRI_MST_COMBINATION
        std::cout << std::endl;
        std::cout << "Size of output vector: " << minTreeOutput.size() << std::endl;
        std::cout << "After re-adding edges, the number of edges is " << minTreeOutput.size() << ": " << std::endl;
        for(std::vector<MinTreeEdge>::iterator it = minTreeOutput.begin(); it!= minTreeOutput.end(); ++it)
        {
            std::cout << (*it).node1ID << " is connected to " << (*it).node2ID << " - Distance: " << (*it).weight << std::endl;
        }

#endif // D_TRI_MST_COMBINATION

        generationPhaseComplete = true;
    }

    else if(generationPhase == GEN_LAYOUT)
    {
        /**              ### CELL LAYOUT ###
        1)  All cells are initiallized EMPTY. (Already done in generator reset)

        2)  Any cells within the boundaries of a designated main room
            are set as a MAIN ROOM cell.

        3)  Create a path between two center cells linked by an edge
            and set all cells on the path as HALLWAY
            except MAIN ROOM cells.

        4)  Any cells in a non-main room
            that possesses a hallway
            are set as HALLROOM,
            including HALLWAY cells.
        */


        // Cells within the boundaries of a main room are set as a main room cell.
        for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end(); ++it)
        {
            if((*it)->designatedMainRoom)
            {
                int xBegin = ((*it)->x1) / MINI_TILESIZE;
                int xEnd   = ((*it)->x2) / MINI_TILESIZE;

                int yBegin = ((*it)->y1) / MINI_TILESIZE;
                int yEnd   = ((*it)->y2) / MINI_TILESIZE;

                for(int y = yBegin; y < yEnd; y++)
                {
                    for(int x = xBegin; x < xEnd; x++)
                    {

                        genLayout[y *areaCellWidth + x] = GEN_CELL_MAIN_ROOM;
                    }
                }
            }
        }


        // Create a path between two center cells linked by an edge and set all cells on the path as HALLWAY except MAIN ROOM cells.
        for(std::vector<MinTreeEdge>::iterator it = minTreeOutput.begin(); it != minTreeOutput.end(); ++it)
        {
            int startCellID = (*it).node1ID;
            int pathCellX  = startCellID%areaCellWidth;
            int pathCellY  = startCellID/areaCellWidth;

            int endCellID = (*it).node2ID;
            int endCellX  = endCellID%areaCellWidth;
            int endCellY  = endCellID/areaCellWidth;

            bool hallwayPathAxis = rand()%PATH_Y_AXIS; // Random whether we start by closing the X difference or the Y difference first.

            /// Add later:
            ///        -vary distance closed per step
            ///        -a chance to switch the pathing axis midway
            ///        -a chance to mis-steer
            ///        the end X/Ycell.

            while(pathCellX != endCellX || pathCellY != endCellY) //While the path has not yet reached the end X/Y cell
            {
                if(hallwayPathAxis == PATH_X_AXIS) // Close the X distance
                {
                    if(pathCellX < endCellX) // Target X cell is to the right
                    {
                        // Cells right of the current cell are converted to hallway, up to target X
                        for(int i = 0; i < std::abs(endCellX-pathCellX);)
                        {
                            if(genLayout[pathCellX+i] != GEN_CELL_MAIN_ROOM)
                                genLayout[pathCellX+i] = GEN_CELL_HALLWAY;
                        }
                    }
                    else if(pathCellX > endCellX) // Target X cell is to the left
                    {
                        // Cells left of the current cell are converted to hallway, up to target X
                        for(int i = 0; i < std::abs(endCellX-pathCellX);)
                        {
                            if(genLayout[pathCellX+i] != GEN_CELL_MAIN_ROOM)
                                genLayout[pathCellX+i] = GEN_CELL_HALLWAY;
                        }
                    }
                    pathCellX = endCellX; // Update current X cell to target X cell
                    hallwayPathAxis = PATH_Y_AXIS; // Switch the path making to the Y axis
                }
                else if(hallwayPathAxis == PATH_Y_AXIS) // Close the Y distance
                {
                    if(pathCellY < endCellY) // Target Y cell is below
                    {
                        // Cells below the current cell are converted to hallway, up to target Y
                        for(int i = 0; i < std::abs(endCellY-pathCellY);)
                        {
                            if(genLayout[(pathCellY-i)*pathCellY] != GEN_CELL_MAIN_ROOM)
                                genLayout[(pathCellY-i)*pathCellY] = GEN_CELL_HALLWAY;
                        }
                    }
                    else if(pathCellX > endCellY) // Target Y cell is above
                    {
                        // Cells above the current cell are converted to hallway, down to target Y
                        for(int i = 0; i < std::abs(endCellY-pathCellY);)
                        {
                            if(genLayout[(pathCellY+i)*pathCellY] != GEN_CELL_MAIN_ROOM)
                                genLayout[(pathCellY+i)*pathCellY] = GEN_CELL_HALLWAY;
                        }
                    }
                    pathCellY = endCellY; // Update current Y cell to target Y cell
                    hallwayPathAxis = PATH_X_AXIS; // Switch the path making to the X axis
                }
            }
        }

        generationPhaseComplete = true;
    }

    else if(generationPhase == GEN_COMPLETE)
    {

    }



    /// Generator progress

    if(generationPhaseComplete)
    {
        generationPhase ++;
        generationPhaseComplete = false;

#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
        D_GENERATORVISUALIZATIONPAUSE = true;
#endif // D_GEN_VISUALIZATION_PHASE_PAUSE
    }
    if(generationPhase == GEN_COMPLETE && !D_GENERATORDEBUGSTASIS)
    {
        ResetGeneratorState();
        needGeneration = false;
    }

}

void Area::SetRemovedEdgeReturnPercentage(float rerp)
{
    removedEdgeReturnPercentage = rerp;

    if(removedEdgeReturnPercentage < 0)
        removedEdgeReturnPercentage = 0;
    if(removedEdgeReturnPercentage > 1.0)
        removedEdgeReturnPercentage = 1.0;
}

int GetCenterCellID(int xCoord, int yCoord)
{
    int centerCellID; // The index of the cell (in a 1D array) that happens to host the room object's center coordinates.

    // Convert the center coordinates in pixels to center coordinates in cells, rounded down.
    int cellX = (int)xCoord/MINI_TILESIZE;
    int cellY = (int)yCoord/MINI_TILESIZE;

    // Calculate index of the corresponding cell.

    centerCellID = cellY*areaCellWidth + cellX;

    // Due to the nature of integer division, the right-most and bottom-most cell will be favored in the case of rooms with even-numbered cell dimensions.
    // To ensure fairness, give a 50% such rooms a chance to choose the left/upper cell.

    if(xCoord%MINI_TILESIZE == 0) // The room box in question has an even-numbered cell width because the center X-coordinate falls on a multiple of MINI_TILESIZE.
    {
        if(rand()%1)
            centerCellID --;
    }
    if(yCoord%MINI_TILESIZE == 0) // The room box in question has an even-numbered cell width because the center Y-coordinate falls on a multiple of MINI_TILESIZE.
    {
        if(rand()%1)
            centerCellID -= areaCellWidth;
    }

    return centerCellID;
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

/// Debug and demo stuff
#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
bool Area::D_GET_GENERATOR_VISUALIZATION_PAUSE()
{
    return D_GENERATORVISUALIZATIONPAUSE;
}

void Area::D_UNPAUSE_VISUALIZATION()
{
    D_GENERATORVISUALIZATIONPAUSE = false;
}
#endif // D_GEN_VISUALIZATION_PHASE_PAUSE


