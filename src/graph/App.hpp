#pragma once

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#include <wx/app.h>
#include <wx/wx.h>
#include "../wx.hpp"
#include <list>
#include "Graph.hpp"
#include "WindowViews.hpp"

namespace GraphStructure {
class Node;
class Edge;

class App : public wxApp, public Graph {
  public:
    WindowViewContainer views;
    bool modified;

    virtual bool OnInit();

    virtual bool isModified() const;

    virtual void selectNode(Node &node);
    virtual void deselectNodes();
    virtual void addNode(Node node);
    virtual void removeNode(const Node *node);
    virtual void removeAllNodes();
    virtual void connect(Node &from, Node &to);
};
}
