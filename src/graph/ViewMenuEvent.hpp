#pragma once

#include "wx.hpp"

//https://code.google.com/p/freetype-gl/
//http://wiki.wxwidgets.org/Custom_Events
//http://wiki.wxwidgets.org/Custom_Events_Tutorial
class ViewMenuEvent : public wxMenuEvent {
  public:
    ViewMenuEvent(wxEventType type = wxEVT_NULL, int id = 0, wxMenu *menu = nullptr);
    ~ViewMenuEvent();
};
