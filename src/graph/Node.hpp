#pragma once

#include "../wx.hpp"
#include "Edge.hpp"
#include <list>

namespace GraphStructure {
class Edge;

class Node {
  public:
    wxPoint pos;
    wxString label;

    int id = -1;

    std::list<Edge> edgeConnections;
    unsigned int radius;

    Node(wxPoint pos, const wxString &label);
    void render(bool selected = false) const;
};
}
