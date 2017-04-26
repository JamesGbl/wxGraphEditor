#pragma once

#include "wx.hpp"
#include "Node.hpp"
#include "Graph.hpp"
#include "WindowViews.hpp"
#include <list>
#include "App.hpp"

#define GRAPH_EDITOR_VERSION "0.9rc"

namespace GraphStructure {
class Frame : public wxFrame {
  public:
    Frame(const wxSize &size, Graph &graph);

    void setupLayout(const WindowViewContainer &viewContainer);
  private:
    Graph &graph;
    bool saved = false;
    wxString filePath = _("");

    bool showSaveConfirmation();

    void onNew   (wxCommandEvent &event);
    void onClose (wxCloseEvent &event);
    void onOpen  (wxCommandEvent &event);
    void onSave  (wxCommandEvent &event);
    void onSaveAs(wxCommandEvent &event);
    void onQuit  (wxCommandEvent &event);
    void onAbout (wxCommandEvent &event);

    void saveGraph(wxString path);

    wxDECLARE_EVENT_TABLE();
};

enum class Window_MenuID : int {
    ViewInfo = 1,
    ViewNodeProp
};
}
