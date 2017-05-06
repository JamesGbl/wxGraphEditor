/***************************************************************
 * Name:      Frame.cpp
 * Purpose:   Code for Application Frame
 * Author:    Evgeny Kalenchuk (zhenchik35026@gmail.com)
 * Created:   2017-02-28
 * Copyright: Evgeny Kalenchuk (github.com/Osyotr)
 * License:   GNU GPLv3
 **************************************************************/

#include "Frame.hpp"

#include <wx/wfstream.h>
#include "defs.h"
#include "NodeVisualizer.hpp"
#include "NodeProperties.hpp"
#include <wx/xml/xml.h>

namespace GraphStructure {
wxBEGIN_EVENT_TABLE(Frame, wxFrame)
    EVT_MENU(wxID_NEW, Frame::onNew)
    EVT_MENU(wxID_OPEN, Frame::onOpen)
    EVT_MENU(wxID_SAVE, Frame::onSave)
    EVT_MENU(wxID_SAVEAS, Frame::onSaveAs)
    EVT_MENU(wxID_EXIT, Frame::onQuit)
    EVT_MENU(wxID_ABOUT, Frame::onAbout)
    EVT_CLOSE(Frame::onClose)
wxEND_EVENT_TABLE()

Frame::Frame(const wxSize &windowSize) :
    wxFrame(NULL, wxID_ANY, wxT(APP_NAME), wxDefaultPosition, windowSize) {
    viewContainer.nodeProperties = new NodeProperties(this);
        viewContainer.nodeVisualizer = new NodeVisualizer(this, *graph, *viewContainer.nodeProperties);
    viewContainer.InstrumentsPanel = new InstrumentsPanel(this, viewContainer.nodeVisualizer);

    wxMenuBar *mbar = new wxMenuBar();
    wxMenu *fileMenu = new wxMenu(_T(""));
    fileMenu->Append(wxID_NEW, _("&New\tCtrl-N"), _("New graph"));
    fileMenu->Append(wxID_OPEN, _("&Open\tCtrl-O"), _("Open graph"));
    fileMenu->Append(wxID_SAVE, _("&Save\tCtrl-S"), _("Save graph"));
    fileMenu->Append(wxID_SAVEAS, _("&Save as\tCtrl-S"), _("Save graph as"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, _("&Quit\tAlt-F4"), _("Quit GraphEditor"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu *viewMenu = new wxMenu(_T(""));
    viewMenu->AppendCheckItem((int)Window_MenuID::ViewInfo, wxT("Information"), wxT("Shows information about the graph"));
    viewMenu->AppendCheckItem((int)Window_MenuID::ViewNodeProp, _("Node Properties"), _("Shows information about the selected node"));
    mbar->Append(viewMenu, _("&View"));

    wxMenu *helpMenu = new wxMenu(_T(""));
    helpMenu->Append(wxID_ABOUT);
    mbar->Append(helpMenu, _("&Help"));
    SetMenuBar(mbar);

    CreateStatusBar(1);
    SetStatusText(_("Ready"), 0);
    SetIcon(wxIcon("aaaa"));
}

void Frame::setupLayout() {
    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(
        viewContainer.views[WindowViews::InstrumentsPanel],
        false, ///do not resize
        wxEXPAND);
    sizer->Add(
        viewContainer.views[WindowViews::NodeVisualizer],
        true,
        wxEXPAND
    );

    wxBoxSizer *sizerV = new wxBoxSizer(wxVERTICAL);
    sizerV->Add(
        viewContainer.views[WindowViews::NodeProperties],
        true,
        wxEXPAND
    );

    sizer->Add(sizerV, false, wxEXPAND);
    SetSizer(sizer);
}

void Frame::onNew(wxCommandEvent &event) {
    graph->removeAllEdges();
    graph->removeAllNodes();
}

void Frame::onOpen(wxCommandEvent &WXUNUSED(event)) {
    if (!saved) {
        if (wxMessageBox(_("Current content has not been saved! Proceed?"), _("Please confirm"),
                         wxICON_QUESTION | wxYES_NO, this) == wxNO )
            return;
    }

    wxFileDialog openFileDialog(this, _("Open graph file"), "", "",
                                "Graph files (*.xml)|*.xml", wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxXmlDocument graphXML;
    if (!graphXML.Load(openFileDialog.GetPath()) && (graphXML.GetRoot()->GetName() != "Graph")) {
        wxMessageBox(_("Error loading file"), _("Error!"));
        return;
    }

    graph->removeAllEdges();
    graph->removeAllNodes();

    wxXmlNode *graphItem = graphXML.GetRoot()->GetChildren();
    while (graphItem) {
        if (graphItem->GetName() == _("Nodes")) {
            wxXmlNode *graphNode = graphItem->GetChildren();
            while (graphNode) {
                graph->addNode(Node(wxPoint(wxAtoi(graphNode->GetAttribute(_("posX"))),
                                                wxAtoi(graphNode->GetAttribute(_("posY")))),
                                    graphNode->GetAttribute(_("label"))),
                                    wxAtoi(graphNode->GetAttribute(_("id"))));
                graphNode = graphNode->GetNext();
            }

        } else if (graphItem->GetName() == _("Edges")) {
            wxXmlNode *graphEdge = graphItem->GetChildren();
            while (graphEdge) {
                graph->connect(*(graph->getNodeById(wxAtoi(graphEdge->GetAttribute(_("from"))))),
                                   *(graph->getNodeById(wxAtoi(graphEdge->GetAttribute(_("to"))))),
                                   wxAtoi(graphEdge->GetAttribute(_("weight"))), wxAtoi(graphEdge->GetAttribute(_("id"))));
                graphEdge = graphEdge->GetNext();
            }
        }
        graphItem = graphItem->GetNext();
    }
    Refresh();
}

void Frame::onSave(wxCommandEvent &event) {
    if(filePath.IsEmpty()){
        onSaveAs(event);
        return;
    }

    saveGraph(filePath);
    saved = true;
}

void Frame::onSaveAs(wxCommandEvent &WXUNUSED(event)) {
    wxFileDialog saveFileDialog(this, _("Save graph file"), "", "",
                                "Graph files (*.xml)|*.xml", wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;

    filePath = saveFileDialog.GetPath();
    saveGraph(filePath);
    saved = true;
}

void Frame::saveGraph(wxString path){
    wxXmlDocument graphXML;

    wxXmlNode *root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, _("Graph"));
    graphXML.SetRoot(root);

    wxXmlNode *edges = new wxXmlNode (root, wxXML_ELEMENT_NODE, "Edges");
    for(auto graphEdge: graph->getEdges()) {
        wxXmlNode *edge = new wxXmlNode(edges, wxXML_ELEMENT_NODE, _("Edge"));
        edge->AddAttribute("from", wxString::Format(_("%i"), graphEdge.from.id));
        edge->AddAttribute("to", wxString::Format(_("%i"), graphEdge.to.id));
        edge->AddAttribute("weight", wxString::Format(_("%i"), graphEdge.weight));
        edge->AddAttribute("id", wxString::Format(_("%i"), graphEdge.id));
    }

    wxXmlNode *nodes = new wxXmlNode (root, wxXML_ELEMENT_NODE, _("Nodes"));
    for(auto graphNode: graph->getNodes()) {
        wxXmlNode *node = new wxXmlNode(nodes, wxXML_ELEMENT_NODE, _("Node"));
        node->AddAttribute("posX", wxString::Format(_("%i"), graphNode.pos.x));
        node->AddAttribute("posY", wxString::Format(_("%i"), graphNode.pos.y));
        node->AddAttribute("label", graphNode.label);
        node->AddAttribute("id", wxString::Format(_("%i"), graphNode.id));
    }

    graphXML.Save(path);
}

void Frame::onQuit(wxCommandEvent &event) {
    Destroy();
}

void Frame::onAbout(wxCommandEvent &event) {
    wxMessageBox(_("GraphEditor\nversion ") + _(GRAPH_EDITOR_VERSION));
}

void Frame::onClose(wxCloseEvent &event) {
    Destroy();
}
}
