#pragma once

namespace ScreenCap
{

  bool HookMouse();
  void UnhookMouse();

  void SetMousePrecision(bool bPrecise);

  LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

}