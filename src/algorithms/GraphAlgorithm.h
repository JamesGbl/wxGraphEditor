#pragma once
#include "Graph.hpp"

using namespace GraphStructure;

class GraphAlgorithm
{
    public:
        GraphAlgorithm(){};

        virtual void operator() (GraphStructure::Graph &graph) = 0;
};
