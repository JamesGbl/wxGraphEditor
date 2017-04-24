#pragma once

#include "../wx.hpp"

namespace GraphStructure {
class Node;
class Edge;

class NodeProperties : public wxDataViewListCtrl {
  public:
    NodeProperties(wxFrame *parent);

    void showNode(const Node &node);
    void showEdge(const Edge &edge);
};
}
