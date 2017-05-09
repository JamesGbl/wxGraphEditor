#include "PrimAlgorithm.h"
#include "Edge.hpp"
#include "Node.hpp"

#include <queue>
#include <vector>

std::vector<int> PrimAlgorithm::execute(GraphStructure::Graph &graph) {
    std::vector<int> actionList;
    if (graph.getNodes().size() == 0)
        return actionList;

    using IPair = std::pair<unsigned int, int>;
    const int INF = std::numeric_limits<int>::max();

    GraphStructure::Graph::AdjacencyList adjList = graph.getAdjacencyList();

    std::priority_queue< IPair, std::vector <IPair>, std::greater<IPair> > queue;
    std::map<int, int> keys;
    for(auto node: graph.getNodes()) {
        keys[node.id] = INF;
    }
    std::map<int, int> parent;
    for(auto node: graph.getNodes()) {
        parent[node.id] = -1;
    }
    std::map<int, bool> included;
    for(auto node: graph.getNodes()) {
        included[node.id] = false;
    }

    int startNode = graph.getNodes().front().id;
    queue.push(std::make_pair(0, startNode));
    keys[startNode] = 0;

    while (!queue.empty()) {
        int minKeyNode = queue.top().second;
        queue.pop();

        if(!included[minKeyNode]) {
            actionList.push_back(minKeyNode);
        }

        included[minKeyNode] = true;
        if (parent[minKeyNode] != -1) {
            for(auto&& edge: graph.getEdges()) {
                if((edge.from.id == minKeyNode && edge.to.id == parent[minKeyNode]) ||
                        (edge.to.id == minKeyNode && edge.from.id == parent[minKeyNode])) {
                    actionList.push_back(edge.id);
                }
            }
        }

        for(auto adjacent: adjList[minKeyNode]) {
            int v = adjacent.second;
            int weight = adjacent.first;

            if (!included[v] && (keys[v] > weight)) {
                keys[v] = weight;
                queue.push(std::make_pair(keys[v], v));
                parent[v] = minKeyNode;
            }
        }
    }
    return actionList;
}
