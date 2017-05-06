#pragma once

#include <list>
#include <vector>
#include <map>
#include "../wx.hpp"

namespace GraphStructure {
class Node;
class Edge;

class Graph {
  protected:
    std::list<Node> nodes;
    std::list<Edge> edges;

  public:
    typedef std::map<int, std::list<std::pair<int, int>>> AdjacencyList;

    std::list<Node> &getNodes();
    std::list<Edge> &getEdges();

    virtual void addNode(Node node, int id = -1);
    virtual void removeNode(const Node *node);
    virtual void removeEdge(const Edge *edge);
    void removeAllNodes();
    void removeAllEdges();

    virtual void connect(Node &from, Node &to, int weight = 1, int id = std::numeric_limits<int>::min());
    AdjacencyList getAdjacencyList();
    std::list<std::pair<int, int>> getAdjacentNodes(const Node &node);

    Node *getNodeById(int id);
    Edge *getEdgeById(int id);
};
}
