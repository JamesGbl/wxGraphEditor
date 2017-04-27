#pragma once

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#include <wx/app.h>
#include <wx/wx.h>
#include "../wx.hpp"

namespace GraphStructure {

class App : public wxApp{
  public:
    virtual bool OnInit();
};
}
