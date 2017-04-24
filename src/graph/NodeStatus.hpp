#pragma once

#include <list>
#include <vector>
#include <map>
#include "../wx.hpp"

namespace GraphStructure {
class Node;
class Edge;

class NodeStatus {
  protected:
    std::list<Node> nodes;
    std::list<Edge> edges;
    std::list<Node *> selectedNodes;

  public:
          std::vector<Edge *> selectedEdges;

    void init();
    typedef std::map<int, std::list<std::pair<int ,int>>> AdjacencyList;

    const std::list<Node> &getNodes() const;
    const std::list<Edge> &getEdges() const;
    const std::list<Node *> &getSelectedNodes() const;
    const std::vector<Edge *> &getSelectedEdges() const;

    virtual void selectNode(Node &node);
    virtual void selectEdge(Edge &edge);
    virtual void deselectNodes();
    virtual void deselectEdges();

    virtual void addNode(Node node, int id = -1);
    virtual void removeNode(const Node *node);
    virtual void removeEdge(const Edge *edge);
    virtual void removeAllNodes();
    virtual void removeAllEdges();

    virtual void connect(Node &from, Node &to, int weight = 1);
    AdjacencyList getAdjacencyList();
    std::list<std::pair<int ,int>> getAdjacentNodes(const Node& node);

    Node *getNodeAt(wxPoint pos);
    Edge *getEdgeAt(wxPoint pos);
    bool isNodeSelected(const Node *node) const;
    bool isEdgeSelected(const Edge *edge) const;
    bool hasNodeSelected() const;
    bool hasEdgeSelected() const;
    Node* getNodeById(int id);

    virtual bool isModified() const = 0;
};
}
