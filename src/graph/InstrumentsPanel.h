#pragma once

#include "../wx.hpp"
#include "NodeVisualizer.hpp"

namespace GraphStructure {
class NodeVisualizer;

class InstrumentsPanel : public wxPanel {
  public:
    InstrumentsPanel(wxFrame *parent, NodeVisualizer *nv);

    char instrumentSelected;
};
}
