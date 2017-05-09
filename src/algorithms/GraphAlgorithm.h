#pragma once
#include <vector>
#include "Graph.hpp"

using namespace GraphStructure;

class GraphAlgorithm {
  public:
    GraphAlgorithm() {};

    virtual std::vector<int> execute (GraphStructure::Graph &graph) = 0;
    virtual ~GraphAlgorithm() {}
};
