#pragma once

#include <array>
#include "../wx.hpp"

namespace GraphStructure {
class InstrumentsPanel;
class NodeVisualizer;
class NodeProperties;

namespace WindowViews {
enum {
    InstrumentsPanel,
    NodeVisualizer,
    NodeProperties,
    length
};
}

union WindowViewContainer {
    wxWindow *views[WindowViews::length];
    struct {
        InstrumentsPanel *InstrumentsPanel;
        NodeVisualizer   *nodeVisualizer;
        NodeProperties   *nodeProperties;
    };
};
}
