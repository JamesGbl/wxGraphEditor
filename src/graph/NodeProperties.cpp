#include "NodeProperties.hpp"

#include <sstream>
#include "Node.hpp"

namespace GraphStructure {
NodeProperties::NodeProperties(wxFrame *parent) :
    wxDataViewListCtrl(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 200)) {
    AppendTextColumn("Field");
    AppendTextColumn("Value");
}

void NodeProperties::showNode(const Node &node) {
    DeleteAllItems();

    std::stringstream strStream;

    wxVector<wxVariant> data;
    data.push_back(wxVariant("Type"));
    data.push_back(wxVariant("Node"));
    AppendItem(data);
    data.clear();
    data.push_back(wxVariant("Label"));
    data.push_back(wxVariant(node.label));
    AppendItem(data);
    data.clear();
    data.push_back(wxVariant("ID"));
    data.push_back(wxVariant(wxString::Format(_("%i"), node.id)));
    AppendItem(data);
    data.clear();
    data.push_back(wxVariant("x"));
    data.push_back(wxVariant(wxString::Format(_("%i"), node.pos.x)));
    AppendItem(data);
    data.clear();
    data.push_back(wxVariant("y"));
    data.push_back(wxVariant(wxString::Format(_("%i"), node.pos.y)));
    AppendItem(data);
    data.clear();
}

void NodeProperties::showEdge(const Edge &edge) {
    DeleteAllItems();

    std::stringstream strStream;

    wxVector<wxVariant> data;
    data.push_back(wxVariant("Type"));
    data.push_back(wxVariant("Edge"));
    AppendItem(data);
    data.clear();
    data.push_back(wxVariant("From"));
    data.push_back(wxVariant(wxString::Format(_("%i"), edge.from.id)));
    AppendItem(data);
    data.clear();
    data.push_back(wxVariant("To"));
    data.push_back(wxVariant(wxString::Format(_("%i"), edge.to.id)));
    AppendItem(data);
    data.clear();
    data.push_back(wxVariant("Weight"));
    data.push_back(wxVariant(wxString::Format(_("%i"), edge.weight)));
    AppendItem(data);
    data.clear();
    data.push_back(wxVariant("ID"));
    data.push_back(wxVariant(wxString::Format(_("%i"), edge.id)));
    AppendItem(data);
    data.clear();
}
}
