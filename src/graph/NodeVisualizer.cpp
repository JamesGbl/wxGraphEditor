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

NodeVisualizer::NodeVisualizer(wxFrame *parent, Graph &graph):
    GLPane(parent), mouseDrag(false), mouseDragInitiationDistance(8), x(0.0f),
    y(0.0f), scale(1.0f), minScale(1/4), maxScale(4), graph(graph) {
    contextMenuNode = new wxMenu;

    contextMenuNode->Append(ItemContextMenuID::UNKNOWN, "<Node>")->Enable(false);
    contextMenuNode->AppendSeparator();
    contextMenuNode->Append(ItemContextMenuID::CHANGE_IDENTIFIER, "Change identifier");
    contextMenuNode->AppendSeparator();
    contextMenuNode->Append(ItemContextMenuID::REMOVE, "Remove");
}

NodeVisualizer::~NodeVisualizer() {
    ///Free all allocated nodes
    graph.removeAllNodes();
    graph.removeAllEdges();

    ///Free menu
    delete contextMenuNode;
}

void NodeVisualizer::onMouseLeftDown(wxMouseEvent &event) {
    mouseClickAbsolutePos = getMouseEventAbsolutePosition(event);
    const wxPoint mouseClickPos = getMouseEventPosition(event);

    auto edge = graph.getEdgeAt(mouseClickPos);
    auto node = graph.getNodeAt(mouseClickPos);

    //node ? node : (edge ? edge : nullptr)
    if(node) {
        if(!event.ShiftDown())
            graph.deselectNodes();
        graph.selectNode(*node);
        mouseClickType = NVMouseClickType::SELECT_NODE;
        graph.deselectEdges();
    } else if(edge) {
        if(!event.ShiftDown())
            graph.deselectEdges();
        graph.selectEdge(*edge);
        mouseClickType = NVMouseClickType::SELECT_EDGE;
        graph.deselectNodes();
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
            if(graph.hasNodeSelected() || graph.hasEdgeSelected()) {
                graph.deselectNodes();
                graph.deselectEdges();
            }
        }
    }
    if(drawingEdge == true) {
        drawingEdge = false;
        Refresh();
        auto node = graph.getNodeAt(mouseCurrentPos);
        if(node && node != graph.getSelectedNodes().front()) {
            graph.connect(*graph.getSelectedNodes().front(), *node);
        }
    }

    mouseClickType = NVMouseClickType::NONE;
    event.Skip();
}

void NodeVisualizer::onMouseDClick(wxMouseEvent &event) {
    mouseClickAbsolutePos = getMouseEventAbsolutePosition(event);
    const wxPoint mouseClickPos = getMouseEventPosition(event);

    auto node = graph.getNodeAt(mouseClickPos);
    if(node) {
        mouseClickType = NVMouseClickType::SELECT_NODE;
        if(!event.ShiftDown())
            graph.deselectNodes();
        graph.deselectEdges();
        graph.selectNode(*node);
    } else {
        if(graph.hasNodeSelected()) {
            graph.deselectNodes();
            graph.deselectEdges();
        }

        mouseCurrentPos = getMouseEventPosition(event);
        graph.addNode(Node(mouseCurrentPos, _(""))); ///Create new node at mouse position
        Refresh();
    }

    mouseClickType = NVMouseClickType::NONE;

    event.Skip();
}

void NodeVisualizer::onMouseRightDown(wxMouseEvent &event) {
    const wxPoint mouseClickPos = getMouseEventPosition(event);

    //If clicked on node
    auto node = graph.getNodeAt(mouseClickPos);
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
            if(graph.hasNodeSelected() == true) {
                graph.connect(*graph.getSelectedNodes().front(), *graph.getSelectedNodes().back());
                Refresh();
            }
            break;

        case WXK_DELETE:
            if(graph.hasNodeSelected()){
                for(auto node = graph.getSelectedNodes().begin(); node != graph.getSelectedNodes().end(); ++node) {
                    graph.removeNode(*node);
                }
            }
            if(graph.hasEdgeSelected()){
                for(auto edge = graph.getSelectedEdges().begin(); edge != graph.getSelectedEdges().end(); ++edge) {
                    graph.removeEdge(*edge);
                }
            }

            graph.deselectNodes();

            break;

        default: {
                auto chr = event.GetUnicodeKey();

                if(chr == 'I') {
                    if(graph.hasNodeSelected() == true) {
                        auto node = graph.getSelectedNodes().begin();
                        wxString newLabel = wxGetTextFromUser(_T("Enter node label:"), _T("Label"));
                        (*node)->label = newLabel;
                        Refresh();
                    }
                    if(graph.hasEdgeSelected() == true) {
                        auto edge = graph.getSelectedEdges().begin();
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
                        graph.getSelectedNodes().front()->pos = mouseCurrentPos;
                    }
                    break;

                default:
                    break;
            }
            Refresh();
        } else {
            if(mouseClickType == NVMouseClickType::SELECT_NODE) {
                mouseCurrentPos = getMouseEventPosition(event);

                mouseStartPos = wxPoint(graph.getSelectedNodes().front()->pos.x +x, graph.getSelectedNodes().front()->pos.y +y);
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
    std::for_each(graph.getEdges().begin(), graph.getEdges().end(), [=](const Edge& edge) {
        edge.render(graph.isEdgeSelected(&edge));
    });

    ///Render nodes
    std::for_each(graph.getNodes().begin(), graph.getNodes().end(), [=](const Node& node) {
        node.render(graph.isNodeSelected(&node));
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
