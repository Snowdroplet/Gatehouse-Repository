#include "mintree.h"

#define D_PRINTOUT_MST

std::unordered_map<int, int> PARENT;  // Records parental relationship between nodes (vertices)
std::unordered_map<int, int> RANK;    // Records depth of the tree

int Find(int nodeID)
{
    if(PARENT[nodeID] == nodeID)
        return PARENT[nodeID];
    else
        return Find(PARENT[nodeID]);
}

void Union(int root1, int root2)
{
    if(RANK[root1] > RANK[root2])
        PARENT[root2] = root1;
    else if(RANK[root2] > RANK[root1])
        PARENT[root1] = root2;
    else
    {
        PARENT[root1] = root2;
        RANK[root2]++;
    }
}

void MakeSet(int nodeID)
{
    PARENT[nodeID] = nodeID;
    RANK[nodeID] = 0;
}

std::vector<MinTreeEdge> Kruskal(MinTreeGraph &g)
{
#ifdef D_PRINTOUT_MST
    std::cout << "Kruskalize me, captain." << std::endl;
#endif //D_PRINTOUT_MST

    //Take all the edges of the graph and make it a single "disjointed set"
    std::vector<MinTreeEdge> A;
    for(auto c : g.nodeIDs)      // Note to self, read up on auto.
    {
        MakeSet(c);
    }

    //Sort all the edges according to their weight.
    std::sort(g.minTreeEdges.begin(), g.minTreeEdges.end(), [](MinTreeEdge x, MinTreeEdge y){return x.weight < y.weight;}); // Note to self, learn how the lambda functions properly work. This is so much more convenient than writing a function elsewhere.

    //If root of nodeID 1 and nodeID 2 are not the same, merge disjointed sets of nodeID 1 and nodeID 2 into one disjointed set.
    for(MinTreeEdge e : g.minTreeEdges)
    {
        int root1 = Find(e.node1ID);
        int root2 = Find(e.node2ID);
        if(root1 != root2)
        {
            A.push_back(e);
            Union(root1, root2);
        }
    }

#ifdef D_PRINTOUT_MST
    std::cout << std::endl;
    for(MinTreeEdge e : A)
    {
        std::cout << e.node1ID << " -- " << e.node2ID << " " << e.weight << std::endl;
    }
#endif // D_PRINTOUT_MST

    return A;
}

