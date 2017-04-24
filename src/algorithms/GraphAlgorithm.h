#pragma once
#include "NodeStatus.hpp"

using namespace GraphStructure;

class GraphAlgorithm
{
    public:
        GraphAlgorithm(){};

        virtual void operator() (GraphStructure::NodeStatus &nodeStatus) = 0;
};
