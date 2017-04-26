#include "Graph.hpp"

#include <algorithm>
//#include <cmath>
#include "Node.hpp"
#include "Edge.hpp"

namespace GraphStructure {
const std::list<Node> &Graph::getNodes() const {
    return nodes;
}

const std::list<Edge> &Graph::getEdges() const {
    return edges;
}

const std::list<Node *> &Graph::getSelectedNodes() const {
    return selectedNodes;
}

const std::vector<Edge *> &Graph::getSelectedEdges() const {
    return selectedEdges;
}

void Graph::selectNode(Node &node) {
    if(!isNodeSelected(&node))
        selectedNodes.push_back(&node);
}

void Graph::selectEdge(Edge &edge) {
    if(!isEdgeSelected(&edge))
        selectedEdges.push_back(&edge);
}

/*void Graph::selectNodes() {
    deselectNodes();
    for(auto node = nodes.begin(); node != nodes.end(); ++node) {
        selectedNodes.push_back(&*node);
    }
}

void Graph::selectEdges() {
    deselectEdges();
    for(auto edge = edges.begin(); edge != edges.end(); ++edge) {
        selectedEdges.push_back(&*edge);
    }
}*/

void Graph::deselectNodes() {
    selectedNodes.clear();
}

void Graph::deselectEdges() {
    selectedEdges.clear();
}

Node *Graph::getNodeAt(wxPoint pos) {
    for(auto node = nodes.begin(); node != nodes.end(); ++node) {
        if( hypot(node->pos.x - pos.x, node->pos.y - pos.y) < node->radius ) {
            return &*node;
        }
    }
    return nullptr;
}

Node* Graph::getNodeById(int id){
    for(auto node = nodes.begin(); node != nodes.end(); ++node) {
        if( id == node->id ) {
            return &*node;
        }
    }
    return nullptr;
}

Edge *Graph::getEdgeAt(wxPoint pos) {
    for(auto edge = edges.begin(); edge != edges.end(); ++edge) {
        ///test if the point c is inside a pre-defined distance from the line
        double distance = 0;
        distance = fabs(double((pos.y - edge->to.pos.y) * edge->from.pos.x -
                               (pos.x - edge->to.pos.x) * edge->from.pos.y +
                               (pos.x * edge->to.pos.y) - pos.y * edge->to.pos.x)) /
                   sqrt(double((pow(pos.y - edge->to.pos.y, 2)) + (pow(pos.x - edge->to.pos.x, 2))));

        if (distance > 10) {
            continue;
        }

        ///test if the point c is between a and b
        int dotproduct = (pos.x - edge->from.pos.x) * (edge->to.pos.x - edge->from.pos.x) +
                         (pos.y - edge->from.pos.y) * (edge->to.pos.y - edge->from.pos.y);
        if(dotproduct < 0) {
            continue;
        }

        int squaredlengthba = (edge->to.pos.x - edge->from.pos.x) * (edge->to.pos.x - edge->from.pos.x) +
                              (edge->to.pos.y - edge->from.pos.y) * (edge->to.pos.y - edge->from.pos.y);
        if(dotproduct > squaredlengthba) {
            continue;
        }

        return &*edge;
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

bool Graph::isNodeSelected(const Node *node) const {
    return std::find( std::begin(getSelectedNodes()), std::end(getSelectedNodes()), node) != std::end(getSelectedNodes());
}

bool Graph::isEdgeSelected(const Edge *edge) const {
    return std::find( std::begin(getSelectedEdges()), std::end(getSelectedEdges()), edge) != std::end(getSelectedEdges());
}

bool Graph::hasNodeSelected() const {
    return !getSelectedNodes().empty();
}

bool Graph::hasEdgeSelected() const {
    return !getSelectedEdges().empty();
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
    for(int nodeIndex = 0; nodeIndex < nodes.size(); nodeIndex++)
    {
        std::list<std::pair<int ,int>> adjacentNodes = getAdjacentNodes(*getNodeById(nodeIndex));
        adjacencyList.emplace(nodeIndex, adjacentNodes);
    }
    return adjacencyList;
}

std::list<std::pair<int ,int>> Graph::getAdjacentNodes(const Node& node)
{
    std::list<std::pair<int ,int>> adjacentNodes;
    for(auto edge: edges)
    {
        std::pair<int ,int> adjacent;
        if (node.id == edge.from.id){
            adjacent.second = edge.to.id;
            adjacent.first = edge.weight;
        }
        else if(node.id == edge.to.id){
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
