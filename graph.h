#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include <iostream>
#include "area.h"
#include "node.h"


struct Graph
{
    int graphWidth;
    int graphHeight;

    std::vector<Node*>nodeGraph;
    std::vector<Node*>openNodes;
    std::vector<Node*>closedNodes;

    Graph(int graphW, int graphH);
    ~Graph();

    void OpenNode(int id, int whichParent);
    void CloseNode(int id);

    void ResetGraph();
    void DestroyGraph();
    std::vector<Node*> RequestPath(int startX, int startY, int destX, int destY);

};

//void NodeToXYCoords(int nodeID, int *x, int *y);
//int XYToNode(int x, int y);

bool CheckClosedNode(Node *tocheck);
bool CompareFValueAscending(Node *lhsNode, Node *rhsNode);


#endif // GRAPH_H_INCLUDED
