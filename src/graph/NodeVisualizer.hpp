#pragma once

#include "../wx.hpp"
#include "../GLPane.hpp"
#include "Graph.hpp"
#include "InstrumentsPanel.h"
#include <list>

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

namespace ItemContextMenuID {
enum {
    UNKNOWN = 1,
    CHANGE_IDENTIFIER,
    CHANGE_WEIGTH,
    REMOVE
};
}

enum class NVMouseReleaseType {
    DESELECT_NODE,
    DESELECT_NODES,
    DESELECT_EDGE,
    DESELECT_EDGES
};

class NodeVisualizer : public GLPane {
  private:
    //Mouse control
    wxPoint mouseClickAbsolutePos,
            mousePreviousAbsolutePos;
    NVMouseClickType mouseClickType;
    bool mouseDrag;
    unsigned short mouseDragInitiationDistance;
    wxPoint mouseCurrentPos;
    wxPoint mouseStartPos;
    //View
    float x, y;
    float scale;
    float minScale;
    float maxScale;
    bool drawingEdge = false;


    //Data
  public:
    Graph &graph;

  private:
    wxMenu *contextMenuNode;

    wxDECLARE_EVENT_TABLE();

  protected:
    Node *getNodeAt(wxPoint pos);

  public:
    NodeVisualizer(wxFrame *parent, Graph &graph);
    virtual ~NodeVisualizer();

    NVSelectedInstrument selectedItem = NVSelectedInstrument::NODE_SELECTED;
    void render(wxPaintEvent &event);

    void onMouseMove(wxMouseEvent &event);
    void onMouseDClick(wxMouseEvent &event);
    void onMouseLeftDown(wxMouseEvent &event);
    void onMouseLeftUp(wxMouseEvent &event);
    void onMouseRightDown(wxMouseEvent &event);
    void onMouseRightUp(wxMouseEvent &event);
    void onMouseWheel(wxMouseEvent &event);
    void onMouseLeaveWindow(wxMouseEvent &event);
    void onKeyDown(wxKeyEvent &event);
    void onKeyUp(wxKeyEvent &event);
    void onContextMenu(wxContextMenuEvent &event);

    void moveView(float x, float y);

    wxPoint getMouseEventPosition(wxMouseEvent &event)const;
    wxPoint getMouseEventAbsolutePosition(wxMouseEvent &event)const;

    void renderTempEdge();

    void zoomIn(float scaling, wxPoint center);
    void zoomOut(float scaling, wxPoint center);

    Edge getMinEdge(Node& from);
    void primAlgorithm();
};
}
