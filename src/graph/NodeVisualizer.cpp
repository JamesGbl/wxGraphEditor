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
NodeVisualizer::NodeVisualizer(wxFrame *parent, Graph &graph, NodeProperties &nodeInfo):
    GLPane(parent), graph(graph), nodeInfo(nodeInfo), mouseDrag(false), mouseDragInitiationDistance(8), x(0.0f),
    y(0.0f), scale(1.0f), minScale(0.25f), maxScale(4) {
    contextMenuNode = new wxMenu;

    m_timer = new wxTimer();
    m_timer->SetOwner(this);

    contextMenuNode->Append((int)ItemContextMenuID::UNKNOWN, "<Node>")->Enable(false);
    contextMenuNode->AppendSeparator();
    contextMenuNode->Append((int)ItemContextMenuID::CHANGE_IDENTIFIER, "Change identifier");
    contextMenuNode->AppendSeparator();
    contextMenuNode->Append((int)ItemContextMenuID::REMOVE, "Remove");

    Bind(wxEVT_LEFT_DOWN, &NodeVisualizer::onMouseLeftDown, this);
    Bind(wxEVT_LEFT_UP, &NodeVisualizer::onMouseLeftUp, this);
    Bind(wxEVT_LEFT_DCLICK, &NodeVisualizer::onMouseDClick, this);
    Bind(wxEVT_MOTION, &NodeVisualizer::onMouseMove, this);
    Bind(wxEVT_RIGHT_DOWN, &NodeVisualizer::onMouseRightDown, this);
    Bind(wxEVT_MOUSEWHEEL, &NodeVisualizer::onMouseWheel, this);
    Bind(wxEVT_KEY_DOWN, &NodeVisualizer::onKeyDown, this);
    Bind(wxEVT_CONTEXT_MENU, &NodeVisualizer::onContextMenu, this);
    Bind(wxEVT_TIMER, &NodeVisualizer::dispatchActionList, this);

    m_timer->Start(500);
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

    auto edge = getEdgeAt(mouseClickPos);
    auto node = getNodeAt(mouseClickPos);

    if(node) {
        if(!event.ShiftDown())
            deselectNodes();
        selectNode(*node);
        nodeInfo.showNode(*node);
        mouseClickType = NVMouseClickType::SELECT_NODE;
        deselectEdges();
    } else if(edge) {
        if(!event.ShiftDown())
            deselectEdges();
        selectEdge(*edge);
        nodeInfo.showEdge(*edge);
        mouseClickType = NVMouseClickType::SELECT_EDGE;
        deselectNodes();
    } else {
        mouseClickType = NVMouseClickType::EMPTYSPACE;
    }

    Refresh();
    event.Skip();
}

void NodeVisualizer::onMouseLeftUp(wxMouseEvent &event) {
    if(mouseDrag) {
        mouseDrag = false;
    } else if(mouseClickType == NVMouseClickType::EMPTYSPACE) {
        if(hasNodeSelected() || hasEdgeSelected()) {
            deselectNodes();
            deselectEdges();
        }
    }
    if(drawingEdge == true) {
        drawingEdge = false;
        auto node = getNodeAt(mouseCurrentPos);
        if(node && node != getSelectedNodes().front()) {
            graph.connect(*getSelectedNodes().front(), *node);
        }
    }

    Refresh();
    mouseClickType = NVMouseClickType::NONE;
    event.Skip();
}

void NodeVisualizer::onMouseMove(wxMouseEvent &event) {
    const wxPoint mouseCurrentAbsolutePos = wxGetMousePosition();

    if(event.Dragging()) {
        if(mouseDrag) {
            if(selectedItem == NVSelectedInstrument::NODE_SELECTED) {
                switch(mouseClickType) {
                    case NVMouseClickType::EMPTYSPACE:
                        moveView(mouseCurrentAbsolutePos.x - mousePreviousAbsolutePos.x,
                                 mouseCurrentAbsolutePos.y - mousePreviousAbsolutePos.y);
                        break;
                    case NVMouseClickType::SELECT_NODE: {
                            mouseCurrentPos = getMouseEventPosition(event);
                            //!@TODO: Implement moving all selected nodes
                            getSelectedNodes().front()->pos = mouseCurrentPos;
                        }
                        break;

                    default:
                        break;
                }
                Refresh();
            } else {
                if(mouseClickType == NVMouseClickType::SELECT_NODE) {
                    mouseCurrentPos = getMouseEventPosition(event);

                    mouseStartPos = wxPoint(getSelectedNodes().front()->pos.x +x, getSelectedNodes().front()->pos.y +y);
                    drawingEdge = true;
                    Refresh();
                }
            }
        } else {
            if(!point_insideRadius(mouseCurrentAbsolutePos, mouseClickAbsolutePos, mouseDragInitiationDistance))
                mouseDrag = true;
        }
    }

    mousePreviousAbsolutePos = mouseCurrentAbsolutePos;
    event.Skip();
}

void NodeVisualizer::onMouseDClick(wxMouseEvent &event) {
    mouseClickAbsolutePos = getMouseEventAbsolutePosition(event);
    const wxPoint mouseClickPos = getMouseEventPosition(event);

    auto node = getNodeAt(mouseClickPos);
    auto edge = getEdgeAt(mouseClickPos);

    if(node) {
        mouseClickType = NVMouseClickType::SELECT_NODE;
        if(!event.ShiftDown())
            deselectNodes();
        deselectEdges();
        selectNode(*node);
    } else if(edge) {
        mouseClickType = NVMouseClickType::SELECT_EDGE;
        if(!event.ShiftDown())
            deselectEdges();
        deselectNodes();
        selectEdge(*edge);
    } else {
        deselectNodes();
        deselectEdges();

        mouseCurrentPos = getMouseEventPosition(event);
        graph.addNode(Node(mouseCurrentPos, _("")));
        Refresh();
    }

    mouseClickType = NVMouseClickType::NONE;

    event.Skip();
}

void NodeVisualizer::onMouseRightDown(wxMouseEvent &event) {
    const wxPoint mouseClickPos = getMouseEventPosition(event);

    //If clicked on node
    auto node = getNodeAt(mouseClickPos);
    if(node) {
        PopupMenu(contextMenuNode);
        selectNode(*node);
    }

    Refresh();
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

void NodeVisualizer::onKeyDown(wxKeyEvent &event) {
    switch(event.GetKeyCode()) {
        case WXK_SPACE:
            if(hasNodeSelected() == true) {
                graph.connect(*getSelectedNodes().front(), *getSelectedNodes().back());
                Refresh();
            }
            break;
        case WXK_DELETE:
            if(hasNodeSelected()) {
                for(auto node = getSelectedNodes().begin(); node != getSelectedNodes().end(); ++node) {
                    graph.removeNode(*node);
                }
            }
            if(hasEdgeSelected()) {
                for(auto edge = getSelectedEdges().begin(); edge != getSelectedEdges().end(); ++edge) {
                    graph.removeEdge(*edge);
                }
            }
            deselectNodes();
            Refresh();

            break;
        case 'I': {
                if(hasNodeSelected() == true) {
                    auto node = getSelectedNodes().begin();
                    wxString newLabel = wxGetTextFromUser(_T("Enter node label:"), _T("Label"));
                    (*node)->label = newLabel;
                }
                if(hasEdgeSelected() == true) {
                    auto edge = getSelectedEdges().begin();
                    int newWeight = wxAtoi(wxGetTextFromUser(_T("Enter edge weight:"), _T("Weight")));
                    (*edge)->weight = newWeight;
                }
            }
            Refresh();
            break;

        default:
            break;
    }

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
        edge.render(isEdgeSelected(&edge));
    });

    ///Render nodes
    std::for_each(graph.getNodes().begin(), graph.getNodes().end(), [=](const Node& node) {
        node.render(isNodeSelected(&node));
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
//////////////////////////////////////////////////
const std::list<Node *> &NodeVisualizer::getSelectedNodes() const {
    return selectedNodes;
}

const std::list<Edge *> &NodeVisualizer::getSelectedEdges() const {
    return selectedEdges;
}

void NodeVisualizer::selectNode(Node &node) {
    if(!isNodeSelected(&node))
        selectedNodes.push_back(&node);
}

void NodeVisualizer::selectEdge(Edge &edge) {
    if(!isEdgeSelected(&edge))
        selectedEdges.push_back(&edge);
}

void NodeVisualizer::deselectNodes() {
    selectedNodes.clear();
}

void NodeVisualizer::deselectEdges() {
    selectedEdges.clear();
}

Node *NodeVisualizer::getNodeAt(wxPoint pos) {
    for(auto node = graph.getNodes().begin(); node != graph.getNodes().end(); ++node) {
        if( hypot(node->pos.x - pos.x, node->pos.y - pos.y) < node->radius ) {
            return &*node;
        }
    }
    return nullptr;
}

Edge *NodeVisualizer::getEdgeAt(wxPoint pos) {
    for(auto edge = graph.getEdges().begin(); edge != graph.getEdges().end(); ++edge) {
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

bool NodeVisualizer::isNodeSelected(const Node *node) const {
    return std::find( std::begin(getSelectedNodes()), std::end(getSelectedNodes()), node) != std::end(getSelectedNodes());
}

bool NodeVisualizer::isEdgeSelected(const Edge *edge) const {
    return std::find( std::begin(getSelectedEdges()), std::end(getSelectedEdges()), edge) != std::end(getSelectedEdges());
}

bool NodeVisualizer::hasNodeSelected() const {
    return !getSelectedNodes().empty();
}

bool NodeVisualizer::hasEdgeSelected() const {
    return !getSelectedEdges().empty();
}

void NodeVisualizer::setActions(std::vector<int> actions) {
    this->actions.clear();
    this->actions = actions;
    this->algorithmStarted = true;
}

void NodeVisualizer::dispatchActionList(wxTimerEvent &event) {
    if(algorithmStarted) {
        static size_t elementNumber = 0;
        if(elementNumber == actions.size()) {
            algorithmStarted = false;
            elementNumber = 0;
            return;
        }

        if(graph.getNodeById(getAction(elementNumber))) {
            selectNode(*graph.getNodeById(getAction(elementNumber)));
        } else if(graph.getEdgeById(getAction(elementNumber))) {
            selectEdge(*graph.getEdgeById(getAction(elementNumber)));
        }
        elementNumber++;
        Refresh();
    }
}

}
