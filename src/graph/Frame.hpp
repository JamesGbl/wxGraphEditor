#pragma once

#include "wx.hpp"
#include "Node.hpp"
#include "Graph.hpp"
#include <list>
#include "App.hpp"
#include "WindowViews.hpp"

#define GRAPH_EDITOR_VERSION "1.0"

namespace GraphStructure {
class Frame : public wxFrame {
  public:
    Frame(const wxSize &windowSize);

    Graph *graph = new Graph();

    WindowViewContainer viewContainer;

    void setupLayout();

  private:
    bool saved = false;
    wxString filePath = _("");

    void onNew   (wxCommandEvent &event);
    void onClose (wxCloseEvent &event);
    void onOpen  (wxCommandEvent &event);
    void onSave  (wxCommandEvent &event);
    void onSaveAs(wxCommandEvent &event);
    void onQuit  (wxCommandEvent &event);
    void onAbout (wxCommandEvent &event);

    void saveGraph(wxString path);
};

enum class Window_MenuID : int {
    ViewInfo = 1,
    ViewNodeProp
};
}
