#pragma once
#include <algorithm>
#include "../wx.hpp"

namespace GraphStructure {
class Node;

class Edge {
  protected:

  public:
    Node &from; ///куда ведёт
    Node &to; ///куда ведёт
    int weight;
    int id;
    Edge(Node &from, Node &to, int weight = 1, int id = -1);
    Edge();
    void render(bool selected = false) const;
};
}
