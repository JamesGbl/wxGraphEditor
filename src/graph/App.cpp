#include "App.hpp"

#include "WindowViews.hpp"
#include "Frame.hpp"

wxIMPLEMENT_APP(GraphStructure::App);

namespace GraphStructure {
bool App::OnInit() {
    //Creates the frame
    Frame *frame = new Frame(wxSize(800, 600));
    frame->SetMinSize(wxSize(500, 400));
    frame->setupLayout();

    //Show the frame
    frame->Show(true);
    frame->Update(); ///fix black screen on nodeVisualizer view at startup

    return true;
}
}
