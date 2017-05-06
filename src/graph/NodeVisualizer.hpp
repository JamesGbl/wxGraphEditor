#pragma once

#include "../wx.hpp"
#include "../GLPane.hpp"
#include "Graph.hpp"
#include "InstrumentsPanel.h"
#include "NodeProperties.hpp"
#include <list>
#include <vector>

namespace GraphStructure {
class Node;

enum class NVMouseClickType {
    NONE,
    SELECT_NODE,
    SELECT_NODES,
    SELECT_EDGE,
    SELECT_EDGES,
    EMPTYSPACE
};

enum class NVSelectedInstrument {
    NODE_SELECTED,
    EDGE_SELECTED
};

enum class ItemContextMenuID : int {
    UNKNOWN = 1,
    CHANGE_IDENTIFIER,
    CHANGE_WEIGTH,
    REMOVE
};

enum class NVMouseReleaseType {
    DESELECT_NODE,
    DESELECT_NODES,
    DESELECT_EDGE,
    DESELECT_EDGES
};

class NodeVisualizer : public GLPane {
  public:
    Graph &graph;
    NodeProperties &nodeInfo;
    NodeVisualizer(wxFrame *parent, Graph &graph, NodeProperties &nodeInfo);

    std::list<Edge *> selectedEdges;
    std::list<Node *> selectedNodes;

    const std::list<Node *> &getSelectedNodes() const;
    const std::list<Edge *> &getSelectedEdges() const;

    void selectNode(Node &node);
    void selectEdge(Edge &edge);
    void deselectNodes();
    void deselectEdges();

    Node *getNodeAt(wxPoint pos);
    Edge *getEdgeAt(wxPoint pos);
    bool isNodeSelected(const Node *node) const;
    bool isEdgeSelected(const Edge *edge) const;
    bool hasNodeSelected() const;
    bool hasEdgeSelected() const;

    void render(wxPaintEvent &event);
    void renderTempEdge();

    NVSelectedInstrument selectedItem = NVSelectedInstrument::NODE_SELECTED;

    void onMouseMove(wxMouseEvent &event);
    void onMouseDClick(wxMouseEvent &event);
    void onMouseLeftDown(wxMouseEvent &event);
    void onMouseLeftUp(wxMouseEvent &event);
    void onMouseRightDown(wxMouseEvent &event);
    void onMouseWheel(wxMouseEvent &event);
    void onKeyDown(wxKeyEvent &event);
    void onContextMenu(wxContextMenuEvent &event);

    wxPoint getMouseEventPosition(wxMouseEvent &event)const;
    wxPoint getMouseEventAbsolutePosition(wxMouseEvent &event)const;

    void moveView(float x, float y);
    void zoomIn(float scaling, wxPoint center);
    void zoomOut(float scaling, wxPoint center);

    void dispatchActionList(wxTimerEvent& event);
    void setActions(std::vector<int> actions);
    int getAction(int id) { return actions[id]; };
    std::vector<int> getActions() { return actions; };

    virtual ~NodeVisualizer();
  private:
    //Mouse control
    NVMouseClickType mouseClickType;

    wxPoint mouseClickAbsolutePos,
            mousePreviousAbsolutePos;
    bool mouseDrag;
    unsigned short mouseDragInitiationDistance;
    wxPoint mouseCurrentPos, mouseStartPos;

    //View
    float x, y;
    float scale;
    float minScale;
    float maxScale;
    bool drawingEdge = false;
    wxMenu *contextMenuNode;

    wxTimer* m_timer;
    std::vector<int> actions;
    bool algorithmStarted = false;
};
}
