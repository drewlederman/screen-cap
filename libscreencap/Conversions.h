#pragma once

namespace ScreenCap
{

  inline Gdiplus::Rect Convert(CRect rcIn)
  {
    Gdiplus::Rect r;
    r.X      = rcIn.left;
    r.Y      = rcIn.top;
    r.Width  = rcIn.Width();
    r.Height = rcIn.Height();
    return r;
  }

}