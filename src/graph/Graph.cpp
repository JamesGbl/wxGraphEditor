#include "Graph.hpp"

#include <algorithm>
//#include <cmath>
#include "Node.hpp"
#include "Edge.hpp"

namespace GraphStructure {
std::list<Node> &Graph::getNodes() {
    return nodes;
}

std::list<Edge> &Graph::getEdges() {
    return edges;
}

Node *Graph::getNodeById(int id) {
    for(auto node = nodes.begin(); node != nodes.end(); ++node) {
        if( id == node->id ) {
            return &*node;
        }
    }
    return nullptr;
}

void Graph::addNode(Node node, int id) {
    if(id == -1)
        node.id = nodes.size();
    else
        node.id = id;
    nodes.push_back(node);
}

void Graph::removeNode(const Node *node) {
    nodes.remove_if( [node](const Node& i) {
        return &i == node;
    });
    edges.remove_if( [node](const Edge& i) {
        return ((&i.from == node) || (&i.to == node));
    });
}

void Graph::removeEdge(const Edge *edge) {
    edges.remove_if( [edge](const Edge& i) {
        return &i == edge;
    });
}

void Graph::removeAllNodes() {
    nodes.clear();
}

void Graph::removeAllEdges() {
    edges.clear();
}

void Graph::connect(Node &from, Node &to, int weight) {
    if(&from == &to) {
        return;
    }

    for(auto edge: getEdges()) {
        if((&(edge.from) == &from && &(edge.to) == &to) ||
                (&(edge.to) == &from && &(edge.from) == &to)) {
            return;
        }
    }
    edges.push_back(Edge(from, to, weight));

    from.connections.push_back(to);
    to.connections.push_back(from);
}

Graph::AdjacencyList Graph::getAdjacencyList() {
    AdjacencyList adjacencyList;
    for(size_t nodeIndex = 0; nodeIndex < nodes.size(); nodeIndex++) {
        std::list<std::pair<int, int>> adjacentNodes = getAdjacentNodes(*getNodeById(nodeIndex));
        adjacencyList.emplace(nodeIndex, adjacentNodes);
    }
    return adjacencyList;
}

std::list<std::pair<int, int>> Graph::getAdjacentNodes(const Node &node) {
    std::list<std::pair<int, int>> adjacentNodes;
    for(auto edge: edges) {
        std::pair<int, int> adjacent;
        if (node.id == edge.from.id) {
            adjacent.second = edge.to.id;
            adjacent.first = edge.weight;
        } else if(node.id == edge.to.id) {
            adjacent.second = edge.from.id;
            adjacent.first = edge.weight;
        } else {
            continue;
        }
        adjacentNodes.emplace_back(adjacent);
    }
    return adjacentNodes;
}

}
