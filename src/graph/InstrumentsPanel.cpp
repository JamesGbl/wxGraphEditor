#include "InstrumentsPanel.h"
#include "PrimAlgorithm.h"
#include "Node.hpp"

namespace GraphStructure {
InstrumentsPanel::InstrumentsPanel(wxFrame *parent, NodeVisualizer *nv) :
    wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(50, 100), wxTAB_TRAVERSAL|wxNO_BORDER) {
    wxImage::AddHandler( new wxPNGHandler );

    wxButton *newNodeButton = new wxButton(this, wxID_ANY);
    wxButton *newEdgeButton = new wxButton(this, wxID_ANY);
    wxButton *algorithmButton = new wxButton(this, wxID_ANY);
    wxButton *adjacencyListButton = new wxButton(this, wxID_ANY, _("ADJ"));
    newNodeButton->SetBitmap(wxBITMAP_PNG(newNodeButtonImage));
    newEdgeButton->SetBitmap(wxBITMAP_PNG(newEdgeButtonImage));
    algorithmButton->SetBitmap(wxBITMAP_PNG(algorithmButtonImage));

    wxBoxSizer *sizerV = new wxBoxSizer(wxVERTICAL);
    sizerV->Add(newNodeButton, false, wxEXPAND);
    sizerV->Add(newEdgeButton, false, wxEXPAND);
    sizerV->Add(algorithmButton, false, wxEXPAND);
    sizerV->Add(adjacencyListButton, false, wxEXPAND);
    ///Add more buttons if needed.

    newNodeButton->Bind(wxEVT_BUTTON, [=](wxCommandEvent &) {
        nv->selectedItem = NVSelectedInstrument::NODE_SELECTED;
    });
    newEdgeButton->Bind(wxEVT_BUTTON, [=](wxCommandEvent &) {
        nv->selectedItem = NVSelectedInstrument::EDGE_SELECTED;
    });
    algorithmButton->Bind(wxEVT_BUTTON, [=](wxCommandEvent &) {
        PrimAlgorithm prim;
        nv->setActions(prim.execute(nv->graph));
    });
    adjacencyListButton->Bind(wxEVT_BUTTON, [=](wxCommandEvent &) {
        std::map<int, std::list<std::pair<int, int>>> adjList = nv->graph.getAdjacencyList();
        wxString adjListStr;
        for(auto node: nv->graph.getNodes()) {
            adjListStr += wxString::Format(_("%i: {"), node.id);
            for(auto adjacent: adjList[node.id]) {
                adjListStr += wxString::Format(_("id%i "), adjacent.second);
            }
            adjListStr += _("} ");
        }
        wxGetTextFromUser(_("Adjacency list:"), _("Adjacency list"), adjListStr);
    });

    SetSizer(sizerV);
}
}
