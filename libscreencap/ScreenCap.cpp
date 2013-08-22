#include "stdafx.h"
#include "ScreenCap.h"

#include "CaptureWnd.h"

using namespace Gdiplus;

namespace ScreenCap
{

  ULONG_PTR gdiplusToken;

  CaptureType g_Type;
  Callback    g_Callback;

  void Initialize(bool bInitGDIPlus /* = true */)
  {
    if (bInitGDIPlus)
    {
      GdiplusStartupInput gdiplusStartupInput;
      GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    }
    else
    {
      gdiplusToken = 0;
    }
  }

  void Uninitialize()
  {
    if (gdiplusToken)
    {
      GdiplusShutdown(gdiplusToken);
    }
  }

  CRect GetForegroundWindowRect()
  {
    CRect rcCurWnd;

    CWnd* pCurWnd = CWnd::GetForegroundWindow();
    if (pCurWnd)
    {
      pCurWnd->GetWindowRect(&rcCurWnd);

      // Special case for dialogs
      if ((pCurWnd->GetStyle() & DS_MODALFRAME) == DS_MODALFRAME)
      {
        int cx = GetSystemMetrics(SM_CXDLGFRAME) + GetSystemMetrics(SM_CXBORDER);
        int cy = GetSystemMetrics(SM_CYDLGFRAME) + GetSystemMetrics(SM_CYBORDER);

        rcCurWnd.InflateRect(cx, cy);
      }
    }

    return rcCurWnd;
  }

  void _DoCapture(CaptureWnd* pwndCap)
  {
    if (g_Type == CaptureTypeCurrentWindow)
      pwndCap->SetSelection(GetForegroundWindowRect());

     pwndCap->Capture(g_Callback);
  }

  void CALLBACK TimerCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
  {
    CaptureWnd* pwndCap = reinterpret_cast<CaptureWnd*>(dwUser);
    if (!pwndCap)
      return;

    _DoCapture(pwndCap);
  }

  void DoCapture(CaptureType Type, UINT nDelay /* = 0 */, Callback cb /* = 0 */)
  {
    CaptureWnd* pwndCap = new CaptureWnd();
    if (!pwndCap)
      return;
    if (!pwndCap->Create())
      return;

    g_Type     = Type;
    g_Callback = cb;

    if (nDelay > 0)
      timeSetEvent(nDelay, 100, &TimerCallback, reinterpret_cast<DWORD_PTR>(pwndCap), TIME_ONESHOT);
    else
      _DoCapture(pwndCap);
  }

}