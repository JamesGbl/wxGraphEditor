#pragma once
#include "GraphAlgorithm.h"

class PrimAlgorithm : public GraphAlgorithm {
  public:
    PrimAlgorithm() {};
    ~PrimAlgorithm() {};

    void operator() (GraphStructure::Graph &graph);
};
