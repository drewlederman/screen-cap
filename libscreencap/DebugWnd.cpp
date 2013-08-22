#include "stdafx.h"
#include "DebugWnd.h"

namespace ScreenCap
{

  DebugWnd g_wndDebug;

  static const int cxWnd = 500;
  static const int cyWnd = 250;

  BEGIN_MESSAGE_MAP(DebugWnd, CEdit)
    ON_WM_CREATE()
  END_MESSAGE_MAP()


  BOOL DebugWnd::Create(CWnd* pParent)
  {
#ifdef _DEBUG
    return CEdit::CreateEx(WS_EX_CLIENTEDGE | WS_EX_TOPMOST, L"EDIT", NULL, 
      WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL | ES_READONLY,
      CRect(0, 0, cxWnd, cyWnd), pParent, 0);
#else
    return TRUE;
#endif
  }

  int DebugWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
    if (CEdit::OnCreate(lpCreateStruct) == -1)
      return -1;

    int xOrig = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int yOrig = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int xEdge = abs(xOrig) + GetSystemMetrics(SM_CXSCREEN);

    SetWindowPos(NULL, xEdge - cxWnd, yOrig, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    SetFont(&m_Font);

    return 0;
  }

}
