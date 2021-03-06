#include "generator.h"

Generator::Generator()
{
    InitialState();
}

Generator::~Generator()
{
    ReleaseOutputContainers();

    for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end();)
    {
        delete *it;
        roomGenBoxes.erase(it);
    }
}

void Generator::GenerateRoomBoxes()
{
    /**                  ### ROOM BOXES DISTRIBUTED ###
        1) Create many overlapping ROOM BOXES within a circle of random size according to a normal distribution.
    */

    // Room dimensions randomized. Probability represented by normal distribution.
    boost::random::normal_distribution<float> randomRoomWidth (averageRoomWidth,stdWidthDeviation);   // (Average, 25% standard deviation)
    boost::random::normal_distribution<float> randomRoomHeight(averageRoomHeight,stdHeightDeviation);

    // Random point in circle selected using random radius and angle. Probability is uniform.
    boost::random::uniform_real_distribution<double> randomGenRadius(0.0,miniAreaWidth*0.20);    // (Range min, range max)
    boost::random::uniform_real_distribution<double> randomGenTheta(0.0,2*PI);

    for(int i = 0; i < roomBoxesToGenerate; i++)
    {
        int roomWidth = randomRoomWidth(mtRng);  // Implicit conversion to int for the purposes of the modulo operation below.
        int roomHeight = randomRoomHeight(mtRng);

        // Prevent the creation of rooms less than two tiles wide or high
        if(roomWidth < MINI_TILESIZE*2)
            roomWidth = MINI_TILESIZE*2;
        if(roomHeight < MINI_TILESIZE*2)
            roomHeight = MINI_TILESIZE*2;


        // Round room dimensions to the nearest multiple of MINI_TILESIZE.
        if(roomWidth%MINI_TILESIZE > MINI_TILESIZE/2)             // If rounding up to nearest MINI_TILESIZE:
            roomWidth += MINI_TILESIZE - roomWidth%MINI_TILESIZE; // Increase dimensions by to meet nearest multiple of MINI_TILESIZE.
        else                                                      // If rounding down to nearest MINI_TILESIZE:
            roomWidth -= roomWidth%MINI_TILESIZE;                 // Decrease dimensions by excess of nearest multiple of MINI_TILESIZE.

        if(roomHeight%MINI_TILESIZE > MINI_TILESIZE/2)
            roomHeight += MINI_TILESIZE - roomHeight%MINI_TILESIZE;
        else
            roomHeight -= roomHeight%MINI_TILESIZE;


        // Take a random point in the circle
        double circleCenterX = miniAreaWidth/2;  // Circle center position in the world may vary at a later point in development.
        double circleCenterY = miniAreaHeight/2;

        double genRadius = randomGenRadius(mtRng);
        double genTheta = randomGenTheta(mtRng);

        double genX = genRadius*cos(genTheta);
        double genY = genRadius*sin(genTheta);

        roomGenBoxes.push_back(new RoomGenBox(i,circleCenterX+genX,circleCenterY+genY, roomWidth, roomHeight));

    }

    generationPhaseComplete = true;
#ifdef D_GEN_PHASE_CHECK
    std::cout << "Room generation boxes created..." << std::endl;
#endif // D_GEN_PHASE_CHECK
}

void Generator::Separation()
{
    /**                  ### ROOM OBJECTS SEPERATED ###
        1) Separate ROOM BOXES until none are overlapping
    */


    static bool overlapCheckPassed = false;

    if(!overlapCheckPassed)
    {
        overlapCheckPassed = true;

        // Compare every room object with every other room object for intersections
        // Implement spatial partitioning of some sort later in development to reduce time complexity

        for(std::vector<RoomGenBox*>::iterator a = roomGenBoxes.begin(); a != roomGenBoxes.end(); ++a)
        {
            for(std::vector<RoomGenBox*>::iterator b = roomGenBoxes.begin(); b != roomGenBoxes.end(); ++b)
            {
                if((*a)->CheckOverlap(*b))
                {
                    overlapCheckPassed = false;
                    (*a)->AddOverlap(*b);
                }
                else
                {
                    (*a)->ClearVelocity();
                }
            }
        }

        for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end(); ++it)
        {
            (*it)->RepulseOverlaps();
            (*it)->ClearOverlaps();
            (*it)->Move();
        }

    }
    else if(overlapCheckPassed)
    {
        // Snap resting room objects to grid
        for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end();)
        {
            (*it)->SnapToGrid();

            if((*it)->BoundaryDeletionCheck(1))
            {
                delete *it;
                roomGenBoxes.erase(it);
            }
            else
            {
                ++it;
            }
        }

        generationPhaseComplete = true;
#ifdef D_GEN_PHASE_CHECK
        std::cout << "Room generation boxes separated..." << std::endl;
#endif // D_GEN_PHASE_CHECK
    }
}

void Generator::MainRoomSelection()
{
    /**                ### MAIN ROOM SELECTION ###

    1) Room generation boxes above a certain height/width threshold are designated main rooms
    2) A random room is chosen as the starting room - Contains down stairs
    3) A random room is chosen as the ending room - Contains up stairs

    */
    for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end(); ++it)
    {
        if((*it)->width >= mainRoomWidthThreshold && (*it)->height >= mainRoomHeightThreshold)
        {
            mainRooms.push_back(*it);
            (*it)->designatedMainRoom = true;
        }
    }

    generationPhaseComplete = true;
#ifdef D_GEN_PHASE_CHECK
    std::cout << "Main rooms selected..." << std::endl;
#endif // D_GEN_PHASE_CHECK
}

void Generator::Triangulation()
{
    /**                ### DELAUNAY TRIANGULATION AND MINIMUM SPANNING TREE  ###

    Note: There are two graphs. The FIRST for triangulation, the SECOND for the MST.

    1)      Using Bl4ckb0ne's delaunay triangulation code (details can be found in delaunay.h/cpp),
        generate the FIRST graph - The center coordinates of ROOM OBJECTS designated as main rooms make up
        the nodes.

    2)      Generate the SECOND graph as a vector of node ID pairs (edges) from the triangulation.
        Each node IDs is the index of the cell corresponding to a main room's center coordinates.

    */

    std::vector<Vec2f>mainRoomCenterCoords;

    for(std::vector<RoomGenBox*>::iterator it = mainRooms.begin(); it != mainRooms.end(); ++it)
    {
        // The main-room-center coordinate to record
        Vec2f centerCoord( ((*it)->x3), ((*it)->y3) );

        // Record the main-room-center coordinate in a vector -- The vector will be passed to the triangulation function.
        mainRoomCenterCoords.push_back(centerCoord);
    }


    Delaunay triangulation;
    triangles = triangulation.triangulate(mainRoomCenterCoords); // Pass vector of main-room-center coordinates to triangulation function.
    triEdges = triangulation.getTriEdges();

    generationPhaseComplete = true;
#ifdef D_GEN_PHASE_CHECK
    std::cout << "Triangulation completed..." << std::endl;
#endif // D_GEN_PHASE_CHECK
}

void Generator::MinimumSpanningTree()
{
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

    /**         ### (JURY-RIGGED) MINIMUM SPANNING TREE ###
    1)      Using the Kruskal algorithm (details be be found in mintree.h/cpp),
        generate and output a minimum spanning tree from the SECOND graph.

    2)      Shuffle and add a random percentage of the edges culled by the MST back to give the area a bit more connectivity.
    */

    for(std::vector<TriEdge>::iterator it = triEdges.begin(); it != triEdges.end(); ++it)
    {
        int nodeA = GetCenterCellID( (*it).p1.x, (*it).p1.y);
        int nodeB = GetCenterCellID( (*it).p2.x, (*it).p2.y);

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


    /// ** The following code is purposeless if the visualization is not going to be shown.

    // Convert center cell node IDs back to coordinates for drawing
    if(D_SHOWLOADINGVISUALIZATION)
    {
        for(std::vector<MinTreeEdge>::iterator it = minTreeOutput.begin(); it != minTreeOutput.end(); ++it)
        {
            Vec2f node1Coords((*it).node1ID%areaCellWidth*MINI_TILESIZE,(*it).node1ID/areaCellWidth*MINI_TILESIZE);
            Vec2f node2Coords((*it).node2ID%areaCellWidth*MINI_TILESIZE,(*it).node2ID/areaCellWidth*MINI_TILESIZE );

            demoEdges.push_back(TriEdge(node1Coords,node2Coords));
        }
#ifdef D_TRI_MST_COMBINATION
        std::endl;
        for(unsigned int i = 0; i < demoEdges.size(); i++)
        {
            std::cout << "Contained in demoedges:" << std::endl;
            std::cout << demoEdges[i].p1.x << ", " << demoEdges[i].p1.y << " -- " << demoEdges[i].p2.x << ", " << demoEdges[i].p2.y << std::endl;
        }
#endif //D_TRI_MST_COMBINATION
    }

    generationPhaseComplete = true;
#ifdef D_GEN_PHASE_CHECK
    std::cout << "Minimum spanning tree generated..." << std::endl;
#endif // D_GEN_PHASE_CHECK
}

void Generator::LayoutFloorSkeleton()
{
    /**              ### CELL LAYOUT ###
        1)  All cells are initiallized EMPTY. (Already done in generator reset)

        2)  Any cells within the boundaries of a designated main room
            are set as a MAIN ROOM cell.

        3)  Create a path between two center cells linked by an edge
            and set all cells on the path as HALLWAY SKELETON
            except MAIN ROOM cells.
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

        bool hallwayPathAxis;         // The axis on which distance to the target cell is to be closed, first.
        bool hallwayComplete = false; // Two center nodes are considered linked once the current cell (pathCell) and the end cell match.

        // bool allowDiagonalPath = false; // Don't do it yet.

        switch(preferedHallwayLayout) //This currently doesn't actually work
        {
        case PREFER_CONVEX:
            // The initial hallway pathing axis prefers the direction that is a longer distance from from the center. This produces somewhat rounder layouts since
            // a) due to the triangulation's generally convex shape, only rooms that are on a the triangulation's edge have a chance of reaching corner areas
            // b) less hallway overlap in the areas closer to the center
            if(std::abs(pathCellX-areaCellWidth/2) < std::abs(pathCellY-areaCellHeight/2))
                hallwayPathAxis = PATH_X_AXIS;
            else
                hallwayPathAxis = PATH_Y_AXIS;
            break;

        case PREFER_CONCAVE:
            // The opposite idea of PREFER_CONVEX. Attempt to produce a more star shaped layout.
            if(std::abs(pathCellX-areaCellWidth/2) > std::abs(pathCellY-areaCellHeight/2))
                hallwayPathAxis = PATH_X_AXIS;
            else
                hallwayPathAxis = PATH_Y_AXIS;
            break;

        case NO_LAYOUT_PREFERENCE:
            hallwayPathAxis = rand()%PATH_Y_AXIS; // Random whether we start by closing the X difference or the Y difference first.
            break;
        }

        while(!hallwayComplete)
        {
            if(hallwayPathAxis == PATH_X_AXIS) // Close the X distance
            {
                if(pathCellX < endCellX) // Target X cell is to the right
                {
                    // Cells right of the current cell are converted to hallway, up to target X
                    for(int i = 0; i < std::abs(endCellX-pathCellX); i++)
                    {
                        if(genLayout[pathCellY*areaCellWidth + pathCellX+i] != GEN_CELL_MAIN_ROOM)
                            genLayout[pathCellY* areaCellWidth + pathCellX+i] = GEN_CELL_HALLWAY_SKELETON;
                    }
                }
                else if(pathCellX >= endCellX) // Target X cell is to the left or on the same row
                {
                    // Cells left of the current cell are converted to hallway, up to target X
                    for(int i = 0; i < std::abs(endCellX-pathCellX); i++)
                    {
                        if(genLayout[pathCellY*areaCellWidth + pathCellX-i] != GEN_CELL_MAIN_ROOM)
                            genLayout[pathCellY*areaCellWidth + pathCellX-i] = GEN_CELL_HALLWAY_SKELETON;
                    }
                }
                pathCellX = endCellX; // Update current X cell to target X cell
            }


            else if(hallwayPathAxis == PATH_Y_AXIS) // Close the Y distance
            {
                if(pathCellY < endCellY) // Target Y cell is below
                {
                    // Cells below the current cell are converted to hallway, up to target Y
                    for(int i = 0; i < std::abs(endCellY-pathCellY); i++)
                    {
                        if(genLayout[(pathCellY+i)*areaCellWidth + pathCellX] != GEN_CELL_MAIN_ROOM)
                            genLayout[(pathCellY+i)*areaCellWidth + pathCellX] = GEN_CELL_HALLWAY_SKELETON;
                    }
                }
                else if(pathCellY >= endCellY) // Target Y cell is above or on the same row
                {
                    // Cells above the current cell are converted to hallway, down to target Y
                    for(int i = 0; i < std::abs(endCellY-pathCellY); i++)
                    {
                        if(genLayout[(pathCellY-i)*areaCellWidth + pathCellX] != GEN_CELL_MAIN_ROOM)
                            genLayout[(pathCellY-i)*areaCellWidth + pathCellX] = GEN_CELL_HALLWAY_SKELETON;
                    }
                }
                pathCellY = endCellY; // Update current Y cell to target Y cell
            }


            if(pathCellX == endCellX && pathCellY == endCellY) //Nothing more needs to be done if we've reached the endpoint.
                hallwayComplete = true;
            else // Switch axis
            {
                if(hallwayPathAxis == PATH_X_AXIS)
                    hallwayPathAxis = PATH_Y_AXIS;
                else if(hallwayPathAxis == PATH_Y_AXIS)
                    hallwayPathAxis = PATH_X_AXIS;
            }

        }
    }

    generationPhaseComplete = true;
#ifdef D_GEN_PHASE_CHECK
    std::cout << "Floor layout skeleton completed" << std::endl;
#endif // D_GEN_PHASE_CHECK
}

void Generator::LayoutFloorFill()
{
    /**  ### FILLING OUT THE LAYOUT ###

        Note: Hallway skeletons turn into hallways or hallway extensions.
        The difference is that proper hallways convert rooms (see #2)
        while hallway extensions do not.

        1)   Empty cells adjacent to hallways skeletons
             have a chance to become a hallway type cell.   (somewhat even probability)
             If chosen, they will:
             a) turn into hallway extension cells           (likely)
             b) turn into hallways cells proper             (fairly unlikely)

        2)   In any non non-main room
             where any of its cells are hallway cells,
             all cells in the room are set as HALLROOM cells
             including HALLWAY cells.
        */

    // Widen hallways on each side with hallway extension cells.
    // Hallway extension cells are the same as hallways, EXCEPT they do not convert cells rooms into HALLROOMS.

    // Maybe I should diffrentiate between main hallways, side hallways, and make-shift tunnels... And make wideness/cleanliness of the path adjustable?
    boost::random::bernoulli_distribution<float> adopt(hallwayAdoptionRate);  // The chance that a cell next to a hallway will be chosen to become a hallway type cell.
    boost::random::bernoulli_distribution<float> hEx(hallwayExtensionRate);   // The chance that the adopted cell will become a hallway extension cell.
    boost::random::bernoulli_distribution<float> hCon(hallwayConversionRate); // The chance that the adopted cell will hallway will become a proper hallway cell.


    for(int i = 0; i < areaCellWidth*areaCellHeight; i++)
    {
        if(genLayout[i] == GEN_CELL_HALLWAY_SKELETON)
        {
            genLayout[i] = GEN_CELL_HALLWAY;


            /// Currently disabled code that mades hallways 3 squares wide
            /*

            if(adopt(mtRng)
                    && genLayout[i-areaCellWidth] == GEN_CELL_EMPTY) // check above
            {
                if(hCon(mtRng))
                    genLayout[i-areaCellWidth] = GEN_CELL_HALLWAY;
                else if(hEx(mtRng))
                    genLayout[i-areaCellWidth] = GEN_CELL_HALLWAY_EXTENSION;

            }
            if(adopt(mtRng)
                    && genLayout[i+areaCellWidth] == GEN_CELL_EMPTY) // check below
            {
                if(hCon(mtRng))
                    genLayout[i+areaCellWidth] = GEN_CELL_HALLWAY;
                else if(hEx(mtRng))
                    genLayout[i+areaCellWidth] = GEN_CELL_HALLWAY_EXTENSION;
            }
            if(adopt(mtRng)
                    && genLayout[i+1] == GEN_CELL_EMPTY) // check right
            {
                if(hCon(mtRng))
                    genLayout[i+1] = GEN_CELL_HALLWAY;
                else if(hEx(mtRng))
                    genLayout[i+1] = GEN_CELL_HALLWAY_EXTENSION;
            }
            if(adopt(mtRng)
                    && genLayout[i-1] == GEN_CELL_EMPTY) // check left
            {
                if(hCon(mtRng))
                    genLayout[i-1] = GEN_CELL_HALLWAY;
                else if(hEx(mtRng))
                    genLayout[i-1] = GEN_CELL_HALLWAY_EXTENSION;
            }

            */
        }

    }


    for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end(); ++it)
    {
        if(!(*it)->designatedMainRoom)
        {
            int xBegin = ((*it)->x1) / MINI_TILESIZE;
            int xEnd   = ((*it)->x2) / MINI_TILESIZE;

            int yBegin = ((*it)->y1) / MINI_TILESIZE;
            int yEnd   = ((*it)->y2) / MINI_TILESIZE;

            [&]  // Double loop break
            {
                for(int y = yBegin; y < yEnd; y++)
                {
                    for(int x = xBegin; x < xEnd; x++)
                    {
                        if(genLayout[y *areaCellWidth + x] == GEN_CELL_HALLWAY)
                        {
                            (*it)->designatedHallRoom = true;
                            return;
                        }
                    }
                }
            }();
        }
    }

    for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end(); ++it)
    {
        int xBegin = ((*it)->x1) / MINI_TILESIZE;
        int xEnd   = ((*it)->x2) / MINI_TILESIZE;

        int yBegin = ((*it)->y1) / MINI_TILESIZE;
        int yEnd   = ((*it)->y2) / MINI_TILESIZE;

        if((*it)->designatedHallRoom)
        {
            for(int y = yBegin; y < yEnd; y++)
            {
                for(int x = xBegin; x < xEnd; x++)
                {
                    genLayout[y *areaCellWidth + x] = GEN_CELL_HALL_ROOM;
                }
            }
        }
    }

    // Transform wall skeletons into wall of appropriate type

    generationPhaseComplete = true;
#ifdef D_GEN_PHASE_CHECK
    std::cout << "Floor layout filled out..." << std::endl;
#endif // D_GEN_PHASE_CHECK
}

void Generator::LayoutWallSkeleton()
{
    /// Encircle everything with a 1-cellwidth thickness wall skeleton:
    /// If a cell is empty
    /// and is next to a floor cell
    /// it becomes a wall.
    for(int i = 0; i < areaCellWidth*areaCellHeight; i++)
    {
        if(genLayout[i] > GEN_CELL_FLOOR_MARKER_BEGIN && genLayout[i] < GEN_CELL_FLOOR_MARKER_END) // Check if this cell is floor
        {

            if(i >= areaCellWidth) // Not on top edge
            {
                if(genLayout[i-areaCellWidth] == GEN_CELL_EMPTY) // Check above for empty space
                    genLayout[i-areaCellWidth] = GEN_CELL_WALL_SKELETON;
            }
            if(i%areaCellWidth > 0) // Not on left edge
            {
                if(genLayout[i-1] == GEN_CELL_EMPTY)  // Check left for any type of floor
                    genLayout[i-1] = GEN_CELL_WALL_SKELETON;
            }
            if(i%areaCellWidth < areaCellWidth-1) // Not on right edge
            {
                if(genLayout[i+1] == GEN_CELL_EMPTY)  // Check right for any type of floor
                    genLayout[i+1] = GEN_CELL_WALL_SKELETON;
            }
            if(i/areaCellWidth < areaCellHeight-1) // Not on bottom edge
            {
                if(genLayout[i+areaCellWidth] == GEN_CELL_EMPTY) // Check below for any type of floor
                    genLayout[i+areaCellWidth] = GEN_CELL_WALL_SKELETON;
            }

            if(i >= areaCellWidth && i%areaCellWidth > 0) // Not on top edge or left edge
            {
                if(genLayout[i-areaCellWidth-1] == GEN_CELL_EMPTY)
                    genLayout[i-areaCellWidth-1] = GEN_CELL_WALL_SKELETON;
            }
            if(i >= areaCellWidth && i%areaCellWidth < areaCellWidth-1) // Not on top edge or right edge
            {
                if(genLayout[i-areaCellWidth+1] == GEN_CELL_EMPTY)
                    genLayout[i-areaCellWidth+1] = GEN_CELL_WALL_SKELETON;
            }
            if(i/areaCellWidth < areaCellHeight-1 && i%areaCellWidth > 0) // Not on the bottom edge or the left edge
            {
                if(genLayout[i+areaCellWidth-1] == GEN_CELL_EMPTY)
                    genLayout[i+areaCellWidth-1] = GEN_CELL_WALL_SKELETON;
            }
            if(i/areaCellWidth < areaCellHeight-1 && i%areaCellWidth < areaCellWidth-1)
            {
                if(genLayout[i+areaCellWidth+1] == GEN_CELL_EMPTY)
                    genLayout[i+areaCellWidth+1] = GEN_CELL_WALL_SKELETON;
            }
        }
    }

    generationPhaseComplete = true;
#ifdef D_GEN_PHASE_CHECK
    std::cout << "Wall layout skeleton completed..." << std::endl;
#endif // D_GEN_PHASE_CHECK
}

void Generator::LayoutWallFill()
{

    generationPhaseComplete = true;
#ifdef D_GEN_PHASE_CHECK
    std::cout << "Wall layout filled out..." << std::endl;
#endif // D_GEN_PHASE_CHECK
}

void Generator::CommitShape()
{
    /// ### Detail and finalize the layout for output                ###
    /// ### See InitialOutputContainers() for default, empty states. ###

    // The theme of dungeon wall/ floor to draw
    int whichFloormapCategory = FC_STONE_DUNGEON_FLOOR;
    int whichWallmapCategory = WC_LIGHT_DUNGEON_WALL;


    for(int i = 0; i < areaCellWidth*areaCellHeight; i++)
    {

        floormapImageCategory[i] = whichFloormapCategory; // ***Will be more nuanced later.***
        wallmapImageCategory[i] = whichWallmapCategory;   // ***Will be more nuanced later.***

        int whatFloormapImageIndex = 0; /* Whether adjacent cells are floor tiles ,
                                                      * represented in the form 0 0 0 0.  (1 = true)
                                                      *                         U L R D           */
        int whatWallmapImageIndex = 0;  /* Whether adjacent cells are wall tiles ,
                                                      * represented in the form 0 0 0 0.  (1 = true)
                                                      *                         U L R D           */

        ///                  ########### DETAIL THE FLOOR ##########

        if(genLayout[i] > GEN_CELL_FLOOR_MARKER_BEGIN && genLayout[i] < GEN_CELL_FLOOR_MARKER_END) //Cell is a type of floor // ***Will be more nuanced later.***
        {
            floormap[i] = FT_FLOOR_REGULAR;

            if(i >= areaCellWidth) // Not on the top row
            {
                if(genLayout[i-areaCellWidth] > GEN_CELL_FLOOR_MARKER_BEGIN && genLayout[i-areaCellWidth] < GEN_CELL_FLOOR_MARKER_END) // Check row above for floor    ***Beware, we are no longer checking for the same category of flooring***
                {
                    whatFloormapImageIndex += 1000; // U = 1
                }
            }
            if(i%areaCellWidth > 0) // Not the left-edge column
            {
                if(genLayout[i-1] > GEN_CELL_FLOOR_MARKER_BEGIN && genLayout[i-1] < GEN_CELL_FLOOR_MARKER_END) // Check row to the left for floor
                {
                    whatFloormapImageIndex += 100; // L = 1
                }
            }
            if(i%areaCellWidth < areaCellWidth-1) // Not the right-edge column
            {
                if(genLayout[i+1] > GEN_CELL_FLOOR_MARKER_BEGIN && genLayout[i+1] < GEN_CELL_FLOOR_MARKER_END) // Check row to the right for floor
                {
                    whatFloormapImageIndex += 10; // R = 1
                }
            }
            if(i/areaCellWidth < areaCellHeight-1) // Not on the bottom row
            {
                if(genLayout[i+areaCellWidth] > GEN_CELL_FLOOR_MARKER_BEGIN && genLayout[i+areaCellWidth] < GEN_CELL_FLOOR_MARKER_END) // Check row below for floor
                {
                    whatFloormapImageIndex += 1; // D = 1
                }
            }

            switch(whatFloormapImageIndex)
            {
            case    0:
                floormapImageIndex[i] = SI_XXXX_FLOOR;
                break;
            case    1:
                floormapImageIndex[i] = SI_XXXD_FLOOR;
                break;
            case   10:
                floormapImageIndex[i] = SI_XXRX_FLOOR;
                break;
            case   11:
                floormapImageIndex[i] = SI_XXRD_FLOOR;
                break;
            case  100:
                floormapImageIndex[i] = SI_XLXX_FLOOR;
                break;
            case  101:
                floormapImageIndex[i] = SI_XLXD_FLOOR;
                break;
            case  110:
                floormapImageIndex[i] = SI_XLRX_FLOOR;
                break;
            case  111:
                floormapImageIndex[i] = SI_XLRD_FLOOR;
                break;
            case 1000:
                floormapImageIndex[i] = SI_UXXX_FLOOR;
                break;
            case 1001:
                floormapImageIndex[i] = SI_UXXD_FLOOR;
                break;
            case 1010:
                floormapImageIndex[i] = SI_UXRX_FLOOR;
                break;
            case 1011:
                floormapImageIndex[i] = SI_UXRD_FLOOR;
                break;
            case 1100:
                floormapImageIndex[i] = SI_ULXX_FLOOR;
                break;
            case 1101:
                floormapImageIndex[i] = SI_ULXD_FLOOR;
                break;
            case 1110:
                floormapImageIndex[i] = SI_ULRX_FLOOR;
                break;
            case 1111:
                floormapImageIndex[i] = SI_ULRD_FLOOR;
                break;
            }
        }

        ///                     ########### DETAIL THE WALLS ##########

        if(genLayout[i] > GEN_CELL_MARKER_WALLS_BEGIN && genLayout[i] < GEN_CELL_MARKER_WALLS_END) // Cell is a type of wall // ***Will be more nuanced later.***
        {

            wallmap[i] = WT_WALL_IMPASSABLE;
            if(i >= areaCellWidth)// Not on the top row
            {
                if(genLayout[i-areaCellWidth] > GEN_CELL_MARKER_WALLS_BEGIN && genLayout[i-areaCellWidth] < GEN_CELL_MARKER_WALLS_END) // Check above row for wall's existence
                {
                    whatWallmapImageIndex += 1000; // U = 1
                }
            }

            if(i%areaCellWidth > 0) // Not the left-edge column
            {
                if(genLayout[i-1] > GEN_CELL_MARKER_WALLS_BEGIN && genLayout[i-1] < GEN_CELL_MARKER_WALLS_END) // Check to the left for wall's existence
                {
                    whatWallmapImageIndex += 100; // L = 1
                }
            }

            if(i%areaCellWidth < areaCellWidth-1) // Not the right-edge column
            {
                if(genLayout[i+1] > GEN_CELL_MARKER_WALLS_BEGIN && genLayout[i+1] < GEN_CELL_MARKER_WALLS_END) // Check to the right for wall's existence
                {
                    whatWallmapImageIndex += 10; // R = 1
                }
            }

            if(i/areaCellWidth < areaCellHeight-1) // Not on the bottom row
            {
                if(genLayout[i+areaCellWidth] > GEN_CELL_MARKER_WALLS_BEGIN && genLayout[i+areaCellWidth] < GEN_CELL_MARKER_WALLS_END) // Check below for wall's existence
                {
                    whatWallmapImageIndex += 1; // D = 1
                }
            }

            switch(whatWallmapImageIndex)
            {
            case 0:
                wallmapImageIndex[i] = SI_XXXX_WALL;
                break; // index correct
            case 1:
                wallmapImageIndex[i] = SI_XXXD_WALL;
                break; // index correct
            case 10:
                wallmapImageIndex[i] = SI_XXRX_WALL;
                break; // index correct
            case 11:
                wallmapImageIndex[i] = SI_XXRD_WALL;
                break; // index = 0 - (But XXRD is supposed to be 0 anyway)
            case 100:
                wallmapImageIndex[i] = SI_XLXX_WALL;
                break; // index correct
            case 101:
                wallmapImageIndex[i] = SI_XLXD_WALL;
                break; // index correct
            case 110:
                wallmapImageIndex[i] = SI_XLRX_WALL;
                break; // index correct
            case 111:
                wallmapImageIndex[i] = SI_XLRD_WALL;
                break; // index correct
            case 1000:
                wallmapImageIndex[i] = SI_UXXX_WALL;
                break; // index correct
            case 1001:
                wallmapImageIndex[i] = SI_UXXD_WALL;
                break; // index correct
            case 1010:
                wallmapImageIndex[i] = SI_UXRX_WALL;
                break; // Error - index = 0
            case 1011:
                wallmapImageIndex[i] = SI_UXRD_WALL;
                break; // Error - index = 0
            case 1100:
                wallmapImageIndex[i] = SI_ULXX_WALL;
                break; // Error - index = 0
            case 1101:
                wallmapImageIndex[i] = SI_ULXD_WALL;
                break; // Error - index = 0
            case 1110:
                wallmapImageIndex[i] = SI_ULRX_WALL;
                break; // Error - index = 0
            case 1111:
                wallmapImageIndex[i] = SI_ULRD_WALL;
                break; // Possible error, but it's very rare to see a 1111 cell
            }
        }

    }

    generationPhaseComplete = true;
#ifdef D_GEN_PHASE_CHECK
    std::cout << "Layout finalized - Ready to read." << std::endl;
#endif // D_GEN_PHASE_CHECK
}

void Generator::Furnish()
{
    // Naively choose the first and last room (by element) to be start and end rooms.
    // Later, record the two furthest rooms ids apart (by MST node distance before re-addition of edges).
    mainRooms[0]->designatedStartRoom = true;
    int rsx = rand()%(mainRooms[0]->cellWidth);
    int rsy = rand()%(mainRooms[0]->cellHeight);
    downstairsXCell = (mainRooms[0]->x1)/MINI_TILESIZE +rsx;
    downstairsYCell = (mainRooms[0]->y1)/MINI_TILESIZE +rsy;

    mainRooms[mainRooms.size()-1]->designatedEndRoom = true;
    int rex = rand()%(mainRooms[mainRooms.size()-1]->cellWidth);
    int rey = rand()%(mainRooms[mainRooms.size()-1]->cellHeight);
    upstairsXCell = (mainRooms[mainRooms.size()-1]->x1)/MINI_TILESIZE + rex;
    upstairsYCell = (mainRooms[mainRooms.size()-1]->y1)/MINI_TILESIZE + rey;

    featuremap[downstairsYCell*areaCellWidth+downstairsXCell] = FEATURE_DOWNSTAIRS;
    featuremap[upstairsYCell*areaCellWidth+upstairsXCell] = FEATURE_UPSTAIRS;

    std::cout << "Room ID " << mainRooms[0]->boxNumber << " designated start room." << std::endl;
    std::cout << "Downstairs located at " << downstairsXCell << ", " << downstairsYCell << std::endl;
    std::cout << "Room ID " << mainRooms[mainRooms.size()-1]->boxNumber << " designated end room." << std::endl;
    std::cout << "Downstairs located at " << upstairsXCell << ", " << upstairsYCell << std::endl;

    for(int i = 0; i < areaCellWidth*areaCellHeight; i++)
    {
        switch(featuremap[i])
        {
        case FEATURE_DOWNSTAIRS:
            featuremapImageIndex[i] = FI_DOWNSTAIRS;
            break;
        case FEATURE_UPSTAIRS:
            featuremapImageIndex[i] = FI_UPSTAIRS;
            break;
        }
    }

    generationPhaseComplete = true;
#ifdef D_GEN_PHASE_CHECK
    std::cout << "Furnishing floor..." << std::endl;
#endif // D_GEN_PHASE_CHECK
}

void Generator::Generate()
{
    switch(generationPhase)
    {
    case GEN_INACTIVE:                // Wasteful phase that exists just so I can hit Q to start the simulation.
        std::cout << "Generator test begins..." << std::endl;
        InitialOutputContainers();
        generationComplete = false;
        generationPhaseComplete = true;
        break;

    case GEN_ROOM_BOXES:
        GenerateRoomBoxes();
        break;

    case GEN_SEPARATION:
        Separation();
        break;

    case GEN_MAIN_ROOM_SELECTION:
        MainRoomSelection();
        break;

    case GEN_TRIANGULATION:
        Triangulation();
        break;

    case GEN_MST:
        MinimumSpanningTree();
        break;

    case GEN_LAYOUT_FLOOR_SKELETON:
        LayoutFloorSkeleton();
        break;

    case GEN_LAYOUT_FLOOR_FILL:
        LayoutFloorFill();
        break;

    case GEN_LAYOUT_WALL_SKELETON:
        LayoutWallSkeleton();
        break;

    case GEN_LAYOUT_WALL_FILL:
        LayoutWallFill();
        break;

    case GEN_COMMIT_SHAPE:
        CommitShape();
        break;

    case GEN_FURNISH:
        Furnish();
        break;
    }

    if(generationPhase == GEN_COMPLETE)
    {
#ifdef D_GEN_PHASE_CHECK
        std::cout << "Generation complete!" << std::endl;
#endif // D_GEN_PHASE_CHECK
        InitialState();
#ifdef D_GEN_PHASE_CHECK
        std::cout << "Generator flags reset for reuse." << std::endl;
#endif // D_GEN_PHASE_CHECK
        generationComplete = true;
    }
    else if(generationPhaseComplete)
    {
        generationPhase ++;
        generationPhaseComplete = false;

#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
        D_GENERATORVISUALIZATIONPAUSE = true;
#endif // D_GEN_VISUALIZATION_PHASE_PAUSE
    }
}

void Generator::InitialState()
{
    /// Random
    mtRng.seed(static_cast<unsigned int>(std::time(0)));

    /// State flags
    D_GENERATORDEBUGSTASIS = true;
    D_GENERATORVISUALIZATIONPAUSE = false;

    generationPhase = GEN_INACTIVE;
    generationComplete = false;
    generationPhaseComplete = false;

    /// Knobs

    roomBoxesToGenerate = 250;

    averageRoomWidth = MINI_TILESIZE*3;
    averageRoomHeight = MINI_TILESIZE*3;
    stdWidthDeviation = averageRoomWidth*0.30;
    stdHeightDeviation = averageRoomHeight*0.30;

    mainRoomWidthThreshold = MINI_TILESIZE*4;
    mainRoomHeightThreshold = MINI_TILESIZE*4;

    SetRemovedEdgeReturnPercentage( (float)(rand()%75) / 100 ); // 0-60%

    preferedHallwayLayout = PREFER_CONVEX;

    hallwayAdoptionRate = 1.0;
    hallwayExtensionRate = 0.85;
    hallwayConversionRate = 0.1;

    /// Vector memory reset
    for(std::vector<RoomGenBox*>::iterator it = roomGenBoxes.begin(); it != roomGenBoxes.end();)
    {
        delete *it;
        roomGenBoxes.erase(it);
    }
    std::vector<RoomGenBox*>().swap(roomGenBoxes);
    std::vector<RoomGenBox*>().swap(mainRooms);
    std::vector<Triangle>().swap(triangles);
    std::vector<TriEdge>().swap(triEdges);
    std::vector<MinTreeEdge>().swap(minTreeInput);
    std::vector<MinTreeEdge>().swap(minTreeOutput);
    std::vector<int>().swap(genLayout);
}

void Generator::InitialOutputContainers()
{
    genLayout = std::vector<int>(areaCellWidth*areaCellHeight, GEN_CELL_EMPTY); // Generation layout is empty by default
    floormap = std::vector<int>(areaCellWidth*areaCellHeight, FT_FLOOR_EMPTY); // All floor spaces are nonexistent by default
    wallmap = std::vector<int>(areaCellWidth*areaCellHeight, WT_WALL_EMPTY); // All wall spaces are nonexistent by default
    featuremap = std::vector<int>(areaCellWidth*areaCellHeight, FEATURE_EMPTY);
    floormapImageCategory = std::vector<int>(areaCellWidth*areaCellHeight, FC_COLD_DUNGEON_FLOOR); // The dungeon style is cold by default
    floormapImageIndex = std::vector<int>(areaCellWidth*areaCellHeight, 0); // All indexes 0000 by default
    wallmapImageCategory = std::vector<int>(areaCellWidth*areaCellHeight, WC_LIGHT_DUNGEON_WALL); // The dungeon wall style is light by default
    wallmapImageIndex = std::vector<int>(areaCellWidth*areaCellHeight, 0); // All indexes 0000 by default
    featuremapImageCategory = std::vector<int>(areaCellWidth*areaCellHeight, FEATC_MISC); // The currently all-encompassing "misc" style of furniture
    featuremapImageIndex = std::vector<int>(areaCellWidth*areaCellHeight, FI_EMPTY); // All cells appear unfurnished by default
}

void Generator::ReleaseOutputContainers()
{
    std::vector<int>().swap(floormap);
    std::vector<int>().swap(wallmap);
    std::vector<int>().swap(featuremap);

    std::vector<int>().swap(floormapImageCategory);
    std::vector<int>().swap(floormapImageIndex);
    std::vector<int>().swap(wallmapImageCategory);
    std::vector<int>().swap(wallmapImageIndex);
    std::vector<int>().swap(featuremapImageCategory);
    std::vector<int>().swap(featuremapImageIndex);
}


void Generator::SetRemovedEdgeReturnPercentage(float rerp)
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


bool Generator::GetGenerationComplete()
{
    return generationComplete;
}

int Generator::GetGenerationPhase()
{
    return generationPhase;
}

/// Debug and demo stuff
#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
bool Generator::D_GET_GENERATOR_VISUALIZATION_PAUSE()
{
    return D_GENERATORVISUALIZATIONPAUSE;
}

void Generator::D_UNPAUSE_VISUALIZATION()
{
    D_GENERATORVISUALIZATIONPAUSE = false;
}
#endif // D_GEN_VISUALIZATION_PHASE_PAUSE


