#include "PrimAlgorithm.h"
#include "Edge.hpp"
#include "Node.hpp"

#include <queue>
#include <vector>

void PrimAlgorithm::operator()(GraphStructure::NodeStatus &nodeStatus)
{
    if (nodeStatus.getNodes().size() == 0)
        return;

    using IPair = std::pair<unsigned int, int>;
    const int INF = std::numeric_limits<int>::max();

    GraphStructure::NodeStatus::AdjacencyList adjList = nodeStatus.getAdjacencyList();
    int nodesNumber = nodeStatus.getNodes().size();

    std::priority_queue< IPair, std::vector <IPair> , std::greater<IPair> > queue;
    std::vector<int> keys(nodesNumber, INF);
    std::vector<int> parent(nodesNumber, -1);
    std::vector<bool> included(nodesNumber, false);

    int startNode = 0;
    queue.push(std::make_pair(0, startNode));
    keys[startNode] = 0;

    while (!queue.empty())
    {
        int minKeyNode = queue.top().second;
        queue.pop();

        if(!included[minKeyNode])
            nodeStatus.selectNode(*nodeStatus.getNodeById(minKeyNode));

        included[minKeyNode] = true;
        if (parent[minKeyNode] != -1)
        {
            for(auto&& edge: nodeStatus.getEdges()){
                if((edge.from.id == minKeyNode && edge.to.id == parent[minKeyNode]) ||
                   (edge.to.id == minKeyNode && edge.from.id == parent[minKeyNode])){
                    GraphStructure::Edge *tempEdge = const_cast<Edge*>(&edge);
                    nodeStatus.selectEdge(*tempEdge);
                }
            }
        }

        for(auto adjacent: adjList[minKeyNode])
        {
            int v = adjacent.second;
            int weight = adjacent.first;

            if (!included[v] && (keys[v] > weight))
            {
                keys[v] = weight;
                queue.push(std::make_pair(keys[v], v));
                parent[v] = minKeyNode;
            }
        }
    }
}
