#include "stdafx.h"
#include "MouseHook.h"

namespace ScreenCap
{
  static HHOOK hookMouse;
  static POINT ptLast = {0};

  static bool bPrecision = false;

  static const int nPrecision = 2;

  bool HookMouse()
  {
    UnhookMouse();

    hookMouse = SetWindowsHookEx(WH_MOUSE, LowLevelMouseProc, NULL, GetCurrentThreadId());

    return (hookMouse != NULL);
  }

  void UnhookMouse()
  {
    if (hookMouse)
    {
      UnhookWindowsHookEx(hookMouse);
      hookMouse = NULL;
    }
  }

  void SetMousePrecision(bool bPrecise)
  {
    bPrecision = bPrecise;
  }

  LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
  {
    if (nCode >= 0)
    {
      if (MOUSEHOOKSTRUCT* phs = reinterpret_cast<MOUSEHOOKSTRUCT*>(lParam))
      {
        if (bPrecision && (ptLast.x != 0 && ptLast.y != 0))
        {
          int xDist = phs->pt.x - ptLast.x;
          int yDist = phs->pt.y - ptLast.y;

          if (xDist != 0)
            phs->pt.x = (ptLast.x + (xDist / nPrecision));
          if (yDist != 0)
            phs->pt.y = (ptLast.y + (yDist / nPrecision));

          SetCursorPos(phs->pt.x, phs->pt.y);
        }

        ptLast = phs->pt;
      }
    }

    return CallNextHookEx(hookMouse, nCode, wParam, lParam);
  }
}