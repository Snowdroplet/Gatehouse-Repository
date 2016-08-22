#include "graph.h"


Graph::Graph(int graphW, int graphH)
{
    graphWidth = graphW;
    graphHeight = graphH;

    for(int i = 0; i < graphWidth*graphHeight; i++)
    {
        nodeGraph.push_back(new Node(i,graphWidth));
    }
}

Graph::~Graph()
{
    DestroyGraph();
}

void Graph::OpenNode(int id, int whichParent)
{
    nodeGraph[id]->state = OPEN;
    nodeGraph[id]->parentNodeID = whichParent;
#ifdef D_TEST_NODE_LISTS
    std::cout << "node " << id << " (" << nodeGraph[id]->x << ", " << nodeGraph[id]->y << ")" << " is added to open list. Parent: " << whichParent << std::endl;
#endif //D_TEST_NODE_LISTS
    openNodes.push_back(nodeGraph[id]);
}

void Graph::CloseNode(int id)
{
    nodeGraph[id]->state = CLOSED;
#ifdef D_TEST_NODE_LISTS
    std::cout << "node " << id << " (" << nodeGraph[id]->x << ", " << nodeGraph[id]->y << ")" << " is added to closed list..." << std::endl;
#endif //D_TEST_NODE_LISTS
    closedNodes.push_back(nodeGraph[id]);

    //Remove node id from open list
    openNodes.erase
    (
        std::remove_if(openNodes.begin(),openNodes.end(),CheckClosedNode),
        openNodes.end()
    );
}


void Graph::ResetGraph()
{
    openNodes.clear();
    closedNodes.clear();

    for(int i = 0; i < graphWidth*graphHeight; i++)
        *nodeGraph[i] = Node(i,graphHeight); //The node being pointed to is reset via Node constructor

}

void Graph::DestroyGraph()
{
    //std::cout << "Cleaning up graph memory..." << std::endl;
    openNodes.clear();
    closedNodes.clear();

    for(std::vector<Node*>::iterator it = nodeGraph.begin(); it != nodeGraph.end();)
    {
        delete *it;
        nodeGraph.erase(it);
    }
}

std::vector<Node*> Graph::RequestPath(int startX, int startY, int destX, int destY)
{
    bool graphExhausted = false;
    bool goalNodeFound = false;
    int startNodeID = startY*graphWidth+startX;
    int goalNodeID = destY*graphWidth+destX;

    int testAdjacentID;
    int testParentID;
    int testLowestFID;

    std::vector<Node*>path; //Copy node pointers to this vector for output.
    int outputPathID;

    int pathMoveCost = 0;

    //No path if no movement is necessary, for whatever reason.
    if(startNodeID == goalNodeID)
        return path;


    //ResetGraph(); occurs at the end. RequestPath() assumes that graph is already in pristine condition.

    //Compute F G H values (based on Being's movement preferences profile -- Not yet implemented)
    for(std::vector<Node*>::iterator it = nodeGraph.begin(); it != nodeGraph.end(); ++it) //Optimize later: May not be necessary to precompute heuristic for the entire graph.
    {
        //H (Heuristic)
        (*it)->heuristic = std::abs(destX-(*it)->x) + std::abs(destY-(*it)->y);

        //G (Move cost)
        switch(area->wallmap[(*it)->id])
        {
        case WALL_EMPTY:
            (*it)->moveCost += 10;
            break;
        case WALL_IMPASSABLE:
            (*it)->moveCost += 100;
            //Consider closing the node at this point.
            break;
        case WALL_BREAKABLE:
            //More nuanced formula based on being's ability to break walls needed
            (*it)->moveCost += 100;
            break;
        case WALL_HOLE:
            (*it)->moveCost += 100;
            //Close node at this point if being can't fly.
            break;
        case WALL_MOAT:
            (*it)->moveCost += 100;
            //Close node at this point if being can't swim.
            break;
        case WALL_DOOR:
            (*it)->moveCost += 20;
            //Close node at this point if being can't open doors.
            break;
        }

        //F = (G+H)
        (*it)->fValue = (*it)->moveCost + (*it)->heuristic;
    }

    // Add starting node to closed list.
    CloseNode(startNodeID);

    // Populate open list with nodes adjacent to start node.
    // Parent these nodes to start node.
    for(int i = EAST; i <= NORTHEAST; i++)
    {
        testAdjacentID = nodeGraph[startNodeID]->adjacentNodeIDs[i];
        if(testAdjacentID != -1)
            OpenNode(testAdjacentID,startNodeID);
    }

    while(!goalNodeFound && !graphExhausted)
    {
        std::sort(openNodes.begin(),openNodes.end(),CompareFValueAscending);  ///Potential source of error?
        testLowestFID = openNodes[0]->id;
        testParentID = openNodes[0]->parentNodeID;

#ifdef D_TEST_PARENT_OPTIMIZATION
        std::cout << std::endl;
        std::cout << "Lowest F value is node " << testLowestFID << " ("
                  << nodeGraph[testLowestFID]->fValue << " = "
                  << nodeGraph[testLowestFID]->moveCost << " + "
                  << nodeGraph[testLowestFID]->heuristic << ")" << std::endl;
#endif // D_TEST_PARENT_OPTIMIZATION

        CloseNode(testLowestFID);

        /*
        If nodes adjacent to the LOWEST F VALUE NODE
        1) Exist (adjacent not -1)
        2) Share the same parent node

        Check if the move cost of these two nodes combined is LESS than
        the cost of moving from the parent node to the adjacent node directly.

        If it is, then reparent ADJACENT NODE to LOWEST F VALUE NODE.

        */


        for(int i = EAST; i <= NORTHEAST; i++)
        {
            testAdjacentID = nodeGraph[testLowestFID]->adjacentNodeIDs[i]; //Check open nodes for adjacent nodes.
            //std::cout << "Node " << openNodes[0]->id << " is adjacent to: " << openNodes[0]->adjacentNodeIDs[i] << " " << std::endl;


            if(testAdjacentID != -1)                             //An adjacent node exists
            {

                if(nodeGraph[testAdjacentID]->state == UNLISTED) //Adjacent node should neither be on closed list (not necessary to check) or open list (next line adds them to open list).
                    OpenNode(testAdjacentID,testLowestFID);      //Add this node to open list, parent to node with lowest F value.


                if(nodeGraph[testAdjacentID]->parentNodeID == nodeGraph[testLowestFID]->parentNodeID /// SUSPECTED SOURCE OF ERROR
                        && nodeGraph[testLowestFID]->moveCost + nodeGraph[testAdjacentID]->moveCost
                        < nodeGraph[testParentID]->moveCost + nodeGraph[testAdjacentID]->moveCost)
                {
#ifdef D_TEST_PARENT_OPTIMIZATION
                    std::cout << "****Node " << testAdjacentID << " reparented to " << testLowestFID << "****" << std::endl;
#endif // D_TEST_PARENT_OPTIMIZATION
                    nodeGraph[testAdjacentID]->parentNodeID = testLowestFID;
                }
            }

        }

        if(testLowestFID == goalNodeID)
        {
#ifdef D_TEST_OUTPUT_PATH_REQUEST
            std::cout << std::endl;
            std::cout << "Path completed... ";
#endif // D_TEST_OUTPUT_PATH_REQUEST
            goalNodeFound = true;
        }


        if(openNodes.empty())
        {
#ifdef D_TEST_OUTPUT_PATH_REQUEST
            std::cout << "Graph exhausted. No path found." << std::endl;
#endif // D_TEST_OUTPUT_PATH_REQUEST
            graphExhausted = true;
        }

    }


    /* Debug: Verify correctness of adjacent nodes
    std::cout << "##Test of adjacent nodes: Adjacent to node 26:" << std::endl;
    for(int i = EAST; i <= NORTHEAST; i++)
    {
        std::cout << nodeGraph[26]->adjacentNodeIDs[i] << " ";
    }
    std::cout << std::endl;
    */ //end debug

    //If a path exists, output optimal path as a vector of nodes.
    if(goalNodeFound)
    {
#ifdef D_TEST_OUTPUT_PATH_REQUEST
        std::cout << "Copying path to output vector:" << std::endl;
#endif // D_TEST_OUTPUT_PATH_REQUEST

        outputPathID = goalNodeID;

        while(outputPathID != startNodeID)
        {
            path.push_back(nodeGraph[outputPathID]);

            pathMoveCost += nodeGraph[outputPathID]->moveCost;
            outputPathID = nodeGraph[outputPathID]->parentNodeID;
        }

#ifdef D_TEST_OUTPUT_PATH_REQUEST
        std::cout << "Output vector sucessfully populated." << std::endl;
#endif // D_TEST_OUTPUT_PATH_REQUEST

        ResetGraph();
        return path;
    }
    else
    {
#ifdef D_TEST_OUTPUT_PATH_REQUEST
        std::cout << "No path available." << std::endl;
#endif // D_TEST_OUTPUT_PATH_REQUEST
        ResetGraph();
        path.clear();
        return path;
    }

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
void NodeToXYCoords(int nodeID, int *x, int *y)
{
    *y = nodeID / areaCellWidth;
    *x = nodeID - *y * areaCellWidth;
}

int XYToNode(int x, int y)
{
    return y*areaCellWidth + x;
}
*/

bool CompareFValueAscending(Node *lhsNode, Node *rhsNode)
{
    return lhsNode->fValue < rhsNode->fValue;
}

bool CheckClosedNode(Node *tocheck)
{
    if(tocheck->state == OPEN)
        return false;
    else
    {
        return true;
    }
}
