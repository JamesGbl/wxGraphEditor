#include "NodeVisualizer.hpp"

#include "../gl.hpp"
#include "Node.hpp"
#include "Edge.hpp"
#include <wx/textdlg.h>
#include <queue>


static bool point_insideRadius(wxPoint a, wxPoint b, float radius) {
    return a.x > b.x - radius && a.x < b.x+radius && a.y > b.y-radius && a.y < b.y+radius;
}

namespace GraphStructure {

wxBEGIN_EVENT_TABLE(NodeVisualizer, GLPane)
    EVT_MOTION      (NodeVisualizer::onMouseMove)
    EVT_LEFT_DCLICK (NodeVisualizer::onMouseDClick)
    EVT_LEFT_DOWN   (NodeVisualizer::onMouseLeftDown)
    EVT_LEFT_UP     (NodeVisualizer::onMouseLeftUp)
    EVT_RIGHT_DOWN  (NodeVisualizer::onMouseRightDown)
    EVT_RIGHT_UP    (NodeVisualizer::onMouseRightUp)
    EVT_LEAVE_WINDOW(NodeVisualizer::onMouseLeaveWindow)
    EVT_KEY_DOWN    (NodeVisualizer::onKeyDown)
    EVT_KEY_UP      (NodeVisualizer::onKeyUp)
    EVT_MOUSEWHEEL  (NodeVisualizer::onMouseWheel)
    EVT_CONTEXT_MENU(NodeVisualizer::onContextMenu)
wxEND_EVENT_TABLE()

NodeVisualizer::NodeVisualizer(wxFrame *parent, NodeStatus &nodeStatus):
    GLPane(parent), mouseDrag(false), mouseDragInitiationDistance(8), x(0.0f),
    y(0.0f), scale(1.0f), minScale(1/4), maxScale(4), nodeStatus(nodeStatus) {
    contextMenuNode = new wxMenu;

    contextMenuNode->Append(ItemContextMenuID::UNKNOWN, "<Node>")->Enable(false);
    contextMenuNode->AppendSeparator();
    contextMenuNode->Append(ItemContextMenuID::CHANGE_IDENTIFIER, "Change identifier");
    contextMenuNode->AppendSeparator();
    contextMenuNode->Append(ItemContextMenuID::REMOVE, "Remove");
}

NodeVisualizer::~NodeVisualizer() {
    ///Free all allocated nodes
    nodeStatus.removeAllNodes();
    nodeStatus.removeAllEdges();

    ///Free menu
    delete contextMenuNode;
}

void NodeVisualizer::onMouseLeftDown(wxMouseEvent &event) {
    mouseClickAbsolutePos = getMouseEventAbsolutePosition(event);
    const wxPoint mouseClickPos = getMouseEventPosition(event);

    auto edge = nodeStatus.getEdgeAt(mouseClickPos);
    auto node = nodeStatus.getNodeAt(mouseClickPos);

    //node ? node : (edge ? edge : nullptr)
    if(node) {
        if(!event.ShiftDown())
            nodeStatus.deselectNodes();
        nodeStatus.selectNode(*node);
        mouseClickType = NVMouseClickType::SELECT_NODE;
        nodeStatus.deselectEdges();
    } else if(edge) {
        if(!event.ShiftDown())
            nodeStatus.deselectEdges();
        nodeStatus.selectEdge(*edge);
        mouseClickType = NVMouseClickType::SELECT_EDGE;
        nodeStatus.deselectNodes();
    } else {
        mouseClickType = NVMouseClickType::EMPTYSPACE;
    }

    event.Skip();
}

void NodeVisualizer::onMouseLeftUp(wxMouseEvent &event) {
    if(mouseDrag) {
        mouseDrag = false;
    } else {
        if(mouseClickType == NVMouseClickType::EMPTYSPACE) {
            if(nodeStatus.hasNodeSelected() || nodeStatus.hasEdgeSelected()) {
                nodeStatus.deselectNodes();
                nodeStatus.deselectEdges();
            }
        }
    }
    if(drawingEdge == true) {
        drawingEdge = false;
        Refresh();
        auto node = nodeStatus.getNodeAt(mouseCurrentPos);
        if(node && node != nodeStatus.getSelectedNodes().front()) {
            nodeStatus.connect(*nodeStatus.getSelectedNodes().front(), *node);
        }
    }

    mouseClickType = NVMouseClickType::NONE;
    event.Skip();
}

void NodeVisualizer::onMouseDClick(wxMouseEvent &event) {
    mouseClickAbsolutePos = getMouseEventAbsolutePosition(event);
    const wxPoint mouseClickPos = getMouseEventPosition(event);

    auto node = nodeStatus.getNodeAt(mouseClickPos);
    if(node) {
        mouseClickType = NVMouseClickType::SELECT_NODE;
        if(!event.ShiftDown())
            nodeStatus.deselectNodes();
        nodeStatus.deselectEdges();
        nodeStatus.selectNode(*node);
    } else {
        if(nodeStatus.hasNodeSelected()) {
            nodeStatus.deselectNodes();
            nodeStatus.deselectEdges();
        }

        mouseCurrentPos = getMouseEventPosition(event);
        nodeStatus.addNode(Node(mouseCurrentPos, _(""))); ///Create new node at mouse position
        Refresh();
    }

    mouseClickType = NVMouseClickType::NONE;

    event.Skip();
}

void NodeVisualizer::onMouseRightDown(wxMouseEvent &event) {
    const wxPoint mouseClickPos = getMouseEventPosition(event);

    //If clicked on node
    auto node = nodeStatus.getNodeAt(mouseClickPos);
    if(node) {
        PopupMenu(contextMenuNode);
    } else {
        event.Skip();
    }
}

void NodeVisualizer::onMouseRightUp(wxMouseEvent &event) {
    event.Skip();
}

void NodeVisualizer::onMouseWheel(wxMouseEvent &event) {
    if(event.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL) {
        if(event.GetWheelRotation() > 0)
            zoomIn(2.0f, wxPoint(getWidth()/2.0f, getHeight()/2.0f));
        else
            zoomOut(2.0f, wxPoint(getWidth()/2.0f, getHeight()/2.0f));
        Refresh();
    }

    event.Skip();
}

void NodeVisualizer::onMouseLeaveWindow(wxMouseEvent &event) {
    event.Skip();
}

void NodeVisualizer::onKeyDown(wxKeyEvent &event) {
    switch(event.GetKeyCode()) {
        case WXK_SPACE:
            if(nodeStatus.hasNodeSelected() == true) {
                nodeStatus.connect(*nodeStatus.getSelectedNodes().front(), *nodeStatus.getSelectedNodes().back());
                Refresh();
            }
            break;

        case WXK_DELETE:
            if(nodeStatus.hasNodeSelected()){
                for(auto node = nodeStatus.getSelectedNodes().begin(); node != nodeStatus.getSelectedNodes().end(); ++node) {
                    nodeStatus.removeNode(*node);
                }
            }
            if(nodeStatus.hasEdgeSelected()){
                for(auto edge = nodeStatus.getSelectedEdges().begin(); edge != nodeStatus.getSelectedEdges().end(); ++edge) {
                    nodeStatus.removeEdge(*edge);
                }
            }

            nodeStatus.deselectNodes();

            break;

        default: {
                auto chr = event.GetUnicodeKey();

                if(chr == 'I') {
                    if(nodeStatus.hasNodeSelected() == true) {
                        auto node = nodeStatus.getSelectedNodes().begin();
                        wxString newLabel = wxGetTextFromUser(_T("Enter node label:"), _T("Label"));
                        (*node)->label = newLabel;
                        Refresh();
                    }
                    if(nodeStatus.hasEdgeSelected() == true) {
                        auto edge = nodeStatus.getSelectedEdges().begin();
                        int newWeight = wxAtoi(wxGetTextFromUser(_T("Enter edge weight:"), _T("Weight")));
                        (*edge)->weight = newWeight;
                        Refresh();
                    }
                }
                break;
            }
    }

    event.Skip();
}

void NodeVisualizer::onKeyUp(wxKeyEvent &event) {
    event.Skip();
}

void NodeVisualizer::onMouseMove(wxMouseEvent &event) {
    const wxPoint mouseCurrentAbsolutePos = wxGetMousePosition();

    if(event.Dragging() && event.LeftIsDown()) {
        if(selectedItem == NVSelectedInstrument::NODE_SELECTED) {
            switch(mouseClickType) {
                case NVMouseClickType::SELECT_EDGE:
                case NVMouseClickType::EMPTYSPACE:
                    moveView(mouseCurrentAbsolutePos.x - mousePreviousAbsolutePos.x,
                             mouseCurrentAbsolutePos.y - mousePreviousAbsolutePos.y);
                    break;
                case NVMouseClickType::SELECT_NODE: {
                        mouseCurrentPos = getMouseEventPosition(event);
                        //!@TODO: Implement moving all selected nodes
                        nodeStatus.getSelectedNodes().front()->pos = mouseCurrentPos;
                    }
                    break;

                default:
                    break;
            }
            Refresh();
        } else {
            if(mouseClickType == NVMouseClickType::SELECT_NODE) {
                mouseCurrentPos = getMouseEventPosition(event);

                mouseStartPos = wxPoint(nodeStatus.getSelectedNodes().front()->pos.x +x, nodeStatus.getSelectedNodes().front()->pos.y +y);
                drawingEdge = true;
                Refresh();
            }
        }
    } else {
        if(!point_insideRadius(mouseCurrentAbsolutePos, mouseClickAbsolutePos, mouseDragInitiationDistance))
            mouseDrag = true;
    }

    mousePreviousAbsolutePos = mouseCurrentAbsolutePos;
    event.Skip();
}

void NodeVisualizer::onContextMenu(wxContextMenuEvent &event) {
    //PopupMenu(contextMenuGraph);
    event.Skip();
}

void NodeVisualizer::render(wxPaintEvent &event) {
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    ///Render edges
    std::for_each(nodeStatus.getEdges().begin(), nodeStatus.getEdges().end(), [=](const Edge& edge) {
        edge.render(nodeStatus.isEdgeSelected(&edge));
    });

    ///Render nodes
    std::for_each(nodeStatus.getNodes().begin(), nodeStatus.getNodes().end(), [=](const Node& node) {
        node.render(nodeStatus.isNodeSelected(&node));
    });

    if(drawingEdge == true) {
        this->renderTempEdge();
    }

    glScalef(1.0f/scale, 1.0f/scale, 1.0f);
    glTranslatef(-x, -y, 0.0f);
}

void NodeVisualizer::moveView(float x, float y) {
    this->x += x;
    this->y += y;
}

wxPoint NodeVisualizer::getMouseEventPosition(wxMouseEvent &event)const {
    wxPoint pos = getMouseEventAbsolutePosition(event);
    pos.x = (pos.x-x)/scale;
    pos.y = (pos.y-y)/scale;

    return pos;
}

wxPoint NodeVisualizer::getMouseEventAbsolutePosition(wxMouseEvent &event) const {
    return event.GetLogicalPosition(wxClientDC(const_cast<NodeVisualizer *>(this)));
}

void NodeVisualizer::zoomIn(float scaling, wxPoint center) {
    if(scale < maxScale) {
        //Scale n x
        scale *= scaling;

        //Correct translation for zooming in to the center
        x -= float(center.x)/scale;
        y -= float(center.y)/scale;
    }
}

void NodeVisualizer::zoomOut(float scaling, wxPoint center) {
    if(scale > minScale) {
        //Correct translation for zooming out from the center
        x += float(center.x)/scale;
        y += float(center.y)/scale;

        //Scale 1/n x
        scale /= scaling;
    }
}

void NodeVisualizer::renderTempEdge() {
    glLineWidth(4);
    glColor3ub(0, 0, 0);

    glBegin(GL_LINES);
    glVertex2f(mouseStartPos.x -x, mouseStartPos.y -y);
    glVertex2f(mouseCurrentPos.x, mouseCurrentPos.y);
    glEnd();
}

}
