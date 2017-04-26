#include "App.hpp"

#include "Frame.hpp"
#include "NodeVisualizer.hpp"
#include "NodeProperties.hpp"
#include "InstrumentsPanel.h"

wxIMPLEMENT_APP(GraphStructure::App);

namespace GraphStructure {
bool App::OnInit() {
    //Creates the frame
    Frame *frame = new Frame(wxSize(800, 600), *this);
    frame->SetMinSize(wxSize(500, 400));

    //Prepare views
    views.nodeVisualizer = new NodeVisualizer(frame, *this);
    views.InstrumentsPanel = new InstrumentsPanel(frame, views.nodeVisualizer);

    views.nodeProperties = new NodeProperties(frame);

    frame->setupLayout(views);

    //Show the frame
    frame->Show(true);
    frame->Update(); ///fix black screen on nodeVisualizer view at startup

    return true;
}

bool App::isModified()const {
    return modified;
}

void App::selectNode(Node &node) {
    views.nodeProperties->showNode(node);
    Graph::selectNode(node);
    views.nodeVisualizer->Refresh();
}

void App::deselectNodes() {
    Graph::deselectNodes();
    views.nodeVisualizer->Refresh();
}

void App::addNode(Node node) {
    modified = true;
    Graph::addNode(node);
    views.nodeVisualizer->Refresh();
}

void App::removeNode(const Node *node) {
    modified = true;
    Graph::removeNode(node);
    views.nodeVisualizer->Refresh();
}

void App::removeAllNodes() {
    modified = true;
    Graph::removeAllNodes();
    views.nodeVisualizer->Refresh();
}

void App::connect(Node &from, Node &to) {
    Graph::connect(from, to);
    views.nodeVisualizer->Refresh();
}

}
