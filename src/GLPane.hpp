#pragma once

#include "wx.hpp"
#include <wx/glcanvas.h>

class GLPane : public wxGLCanvas {
  private:
    wxGLContext *m_context;
    wxDECLARE_EVENT_TABLE();

    void onRender(wxPaintEvent &event);

  public:
    GLPane(wxFrame *parent);
    virtual ~GLPane();

    void onWindowResize(wxSizeEvent &event);

    int getWidth()const;
    int getHeight()const;
    virtual void render(wxPaintEvent &event) = 0;
};
