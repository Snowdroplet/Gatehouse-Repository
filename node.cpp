#include "node.h"

Node::Node(int iid, int w)
{
    id = iid;
    y = id/w;
    x = id - y*w;

    adjacentNodeIDs[EAST]      = (id+1);
    adjacentNodeIDs[SOUTHEAST] = (id+w+1);
    adjacentNodeIDs[SOUTH]     = (id+w);
    adjacentNodeIDs[SOUTHWEST] = (id+w-1);
    adjacentNodeIDs[WEST]      = (id-1);
    adjacentNodeIDs[NORTHWEST] = (id-w-1);
    adjacentNodeIDs[NORTH]     = (id-w);
    adjacentNodeIDs[NORTHEAST] = (id-w+1);

    if(x == 0) // Test for left edge of graph
    {
        adjacentNodeIDs[WEST]      = -1;
        adjacentNodeIDs[NORTHWEST] = -1;
        adjacentNodeIDs[SOUTHWEST] = -1;
    }

    if(x == w-1) // Test for right edge of graph
    {
        adjacentNodeIDs[EAST]      = -1;
        adjacentNodeIDs[NORTHEAST] = -1;
        adjacentNodeIDs[SOUTHEAST] = -1;
    }
    if(y == 0) //Test for upper edge of the graph
    {
        adjacentNodeIDs[NORTH]     = -1;
        adjacentNodeIDs[NORTHWEST] = -1;
        adjacentNodeIDs[NORTHEAST] = -1;
    }
    if(y == w-1) // Assuming square map (to change later?), test for the bottom edge of the graph
    {
        adjacentNodeIDs[SOUTH]     = -1;
        adjacentNodeIDs[SOUTHWEST] = -1;
        adjacentNodeIDs[SOUTHEAST] = -1;
    }

    heuristic = 0;
    moveCost = 0;
    fValue = 0;
    parentNodeID = -1;
    state = UNLISTED;
}

Node::~Node()
{

}
