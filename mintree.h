#ifndef MINTREE_H_INCLUDED
#define MINTREE_H_INCLUDED

/// Kruskal algorithm to create a minimum spanning tree.
/// Adapted from Bo Qian's video tutorial found at Reference: https://www.youtube.com/watch?v=5xosHRdxqHA

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iostream>

extern std::unordered_map<int, int> PARENT;  // Records parental relationship between nodes (vertices)
extern std::unordered_map<int, int> RANK;    // Records depth of the tree



struct MinTreeEdge
{
    /*
    bool operator<(MinTreeEdge const &rhs) const noexcept
    {
            return lhs < rhs.lhs;
    }
    int lhs;
    */

    bool operator==(MinTreeEdge const &rhs) const noexcept
    {
        return lhs == rhs.lhs;
    }
    int lhs;


    int node1ID;
    int node2ID;
    int weight;

    MinTreeEdge(int n1, int n2       ):node1ID(n1), node2ID(n2), weight(0) {} // Double-dipping to use MinTreeEdge as storage.
    MinTreeEdge(int n1, int n2, int w):node1ID(n1), node2ID(n2), weight(w) {} // Assign proper endpoint nodes and weights between them for use by MinTreeGraph.
};

struct MinTreeGraph
{
    std::vector<int>nodeIDs;
    std::vector<MinTreeEdge>minTreeEdges;
};

std::vector<MinTreeEdge> Kruskal(MinTreeGraph &g);

int Find(int nodeID);
void Union (int root1, int root2);

void MakeSet(int nodeID);

bool SameMinTreeEdge(MinTreeEdge a, MinTreeEdge b);

#endif // MINTREE_H_INCLUDED
