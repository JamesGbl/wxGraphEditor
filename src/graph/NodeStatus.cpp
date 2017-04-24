#include "NodeStatus.hpp"

#include <algorithm>
//#include <cmath>
#include "Node.hpp"
#include "Edge.hpp"

namespace GraphStructure {
const std::list<Node> &NodeStatus::getNodes() const {
    return nodes;
}

const std::list<Edge> &NodeStatus::getEdges() const {
    return edges;
}

const std::list<Node *> &NodeStatus::getSelectedNodes() const {
    return selectedNodes;
}

const std::vector<Edge *> &NodeStatus::getSelectedEdges() const {
    return selectedEdges;
}

void NodeStatus::selectNode(Node &node) {
    if(!isNodeSelected(&node))
        selectedNodes.push_back(&node);
}

void NodeStatus::selectEdge(Edge &edge) {
    if(!isEdgeSelected(&edge))
        selectedEdges.push_back(&edge);
}

/*void NodeStatus::selectNodes() {
    deselectNodes();
    for(auto node = nodes.begin(); node != nodes.end(); ++node) {
        selectedNodes.push_back(&*node);
    }
}

void NodeStatus::selectEdges() {
    deselectEdges();
    for(auto edge = edges.begin(); edge != edges.end(); ++edge) {
        selectedEdges.push_back(&*edge);
    }
}*/

void NodeStatus::deselectNodes() {
    selectedNodes.clear();
}

void NodeStatus::deselectEdges() {
    selectedEdges.clear();
}

Node *NodeStatus::getNodeAt(wxPoint pos) {
    for(auto node = nodes.begin(); node != nodes.end(); ++node) {
        if( hypot(node->pos.x - pos.x, node->pos.y - pos.y) < node->radius ) {
            return &*node;
        }
    }
    return nullptr;
}

Node* NodeStatus::getNodeById(int id){
    for(auto node = nodes.begin(); node != nodes.end(); ++node) {
        if( id == node->id ) {
            return &*node;
        }
    }
    return nullptr;
}

Edge *NodeStatus::getEdgeAt(wxPoint pos) {
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

void NodeStatus::addNode(Node node, int id) {
    if(id == -1)
        node.id = nodes.size();
    else
        node.id = id;
    nodes.push_back(node);
}

void NodeStatus::removeNode(const Node *node) {
    nodes.remove_if( [node](const Node& i) {
        return &i == node;
    });
    edges.remove_if( [node](const Edge& i) {
        return ((&i.from == node) || (&i.to == node));
    });
}

void NodeStatus::removeEdge(const Edge *edge) {
    edges.remove_if( [edge](const Edge& i) {
        return &i == edge;
    });
}

bool NodeStatus::isNodeSelected(const Node *node) const {
    return std::find( std::begin(getSelectedNodes()), std::end(getSelectedNodes()), node) != std::end(getSelectedNodes());
}

bool NodeStatus::isEdgeSelected(const Edge *edge) const {
    return std::find( std::begin(getSelectedEdges()), std::end(getSelectedEdges()), edge) != std::end(getSelectedEdges());
}

bool NodeStatus::hasNodeSelected() const {
    return !getSelectedNodes().empty();
}

bool NodeStatus::hasEdgeSelected() const {
    return !getSelectedEdges().empty();
}

void NodeStatus::removeAllNodes() {
    nodes.clear();
}

void NodeStatus::removeAllEdges() {
    edges.clear();
}

void NodeStatus::connect(Node &from, Node &to, int weight) {
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

NodeStatus::AdjacencyList NodeStatus::getAdjacencyList() {
    AdjacencyList adjacencyList;
    for(int nodeIndex = 0; nodeIndex < nodes.size(); nodeIndex++)
    {
        std::list<std::pair<int ,int>> adjacentNodes = getAdjacentNodes(*getNodeById(nodeIndex));
        adjacencyList.emplace(nodeIndex, adjacentNodes);
    }
    return adjacencyList;
}

std::list<std::pair<int ,int>> NodeStatus::getAdjacentNodes(const Node& node)
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
