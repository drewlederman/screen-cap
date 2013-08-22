#pragma once

#include "ScreenCapDefs.h"

namespace ScreenCap
{

  void Initialize(bool bInitGDIPlus = true);
  void Uninitialize();

  void DoCapture(CaptureType Type, UINT nDelay, Callback cb);

}