#pragma once
#include <vector>
#include "GraphAlgorithm.h"

class PrimAlgorithm : public GraphAlgorithm {
  public:
    PrimAlgorithm() {};
    ~PrimAlgorithm() {};

    std::vector<int> execute (GraphStructure::Graph &graph);
};
