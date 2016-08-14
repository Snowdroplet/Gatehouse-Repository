#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include "gamesystem.h"

enum enumNodeState
{
    CLOSED = -1,
    UNLISTED = 0,
    OPEN = 1
};

struct Node
{
    int id;
    int x;
    int y;
    int adjacentNodeIDs[8]; // Starting east, then moving counterclockwise, the IDs of the nodes adjacent. -1 denotes no adjacent node (the edge of a map).
    int heuristic;
    int moveCost;
    int fValue;
    int parentNodeID;
    int state;

    Node(int iid, int w); // ID determined by iterator in InitGraph()'s for loop. W is equal to areaCellWidth.
    ~Node();
};



#endif // NODE_H_INCLUDED
