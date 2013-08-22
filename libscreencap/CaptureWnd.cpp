#include "stdafx.h"
#include "CaptureWnd.h"

#include "resource.h"
#include "MouseHook.h"
#include "Conversions.h"
#include "Encoder.h"

#include "..\..\common\KayakoMFCUtil.h"
#include "..\..\common\KayakoFileUtil.h"

using namespace Gdiplus;
using namespace Kayako;

namespace ScreenCap
{

  const UINT WM_CAPTURE = WM_USER + 0x1;

  BEGIN_MESSAGE_MAP(CaptureWnd, CWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_WM_KILLFOCUS()
    ON_MESSAGE(WM_CAPTURE, OnCapture)
  END_MESSAGE_MAP()

  CaptureWnd::CaptureWnd()
    : m_cursor(MAKEINTRESOURCE(IDC_CROSS))
  {
    m_bSelecting = false;
    m_bClosing   = false;
  }

  CaptureWnd::~CaptureWnd()
  {
  }

  BOOL CaptureWnd::Create()
  {
    CRect rc = GetVirtualScreenRect();

    DWORD dwExFlags = 0;
#ifndef _DEBUG
    dwExFlags = WS_EX_TOPMOST;
#endif

    return CWnd::CreateEx(dwExFlags, NULL, NULL, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 
      rc, NULL, 0);
  }

  void CaptureWnd::SetSelection(CRect rc, bool bRedraw /* = false */)
  {
    m_Sel.Set(rc);
    
    if (bRedraw)
    {
      m_rcDirty = m_rcClient;

      Invalidate();
    }
  }

  void CaptureWnd::Capture(Callback cb)
  {
    m_cb = cb;

    SendMessage(WM_CAPTURE);
  }

  void CaptureWnd::CreateToolWindows()
  {
    MONITORINFOEX info;
    GetPrimaryMonitorInfo(&info);

    m_wndZoom.Create(this, info.rcMonitor);

    g_wndDebug.Create(this);
  }

  void CaptureWnd::DestroyToolWindows()
  {
    m_wndZoom.DestroyWindow();
  }

  void CaptureWnd::RenderZoomWnd(const CPoint& ptCursor)
  {
    CDC* pDC = GetDC();

    m_wndZoom.Render(pDC, ptCursor);

    ReleaseDC(pDC);
  }

  void CaptureWnd::SetCursor(const CPoint& ptCursor)
  {
    if (m_bSelecting)
      return;

    LPCTSTR lpCursor = IDC_CROSS;

    m_Sel.HitTestCursor(ptCursor, lpCursor);

    m_cursor.Set(lpCursor);
  }

  void CaptureWnd::Invalidate()
  {
    InvalidateRect(m_rcDirty, FALSE);
    UpdateWindow();
  }

  void CaptureWnd::OnEscape()
  {
    m_bClosing = true;

    if (m_cb)
      m_cb(ErrorCanceled, CString());

    DestroyWindow();
  }

  void CaptureWnd::OnEnter()
  {
    m_bClosing = true;

    ShowWindow(SW_HIDE);

    CString strFile;

    ErrorType err = (SaveFile(strFile) ? ErrorNone : ErrorSaveFailed);

    if (m_cb)
      m_cb(err, strFile);

    DestroyWindow();
  }

  void CaptureWnd::ToggleDebugWnd()
  {
    if (g_wndDebug.IsWindowVisible())
      g_wndDebug.ShowWindow(SW_HIDE);
    else
      g_wndDebug.ShowWindow(SW_SHOW);
  }

  bool CaptureWnd::SaveFile(CString& strFileOut)
  {
    strFileOut.Format(L"%s%s.png", Path::GetTempFolderPath(), CreateGUIDString());
    
    m_dcScreen.InsertCursor();

    CRect rcCrop = (m_Sel.IsEmpty() ? m_rcClient : m_Sel.GetRect());

    return SavePngFile(m_dcScreen.GetDC(), rcCrop, strFileOut);
  }

  BOOL CaptureWnd::PreCreateWindow(CREATESTRUCT& cs)
  {
    cs.lpszClass = AfxRegisterWndClass(NULL);

    return TRUE;
  }

  void CaptureWnd::PostNcDestroy()
  {
    delete this;
  }

  int CaptureWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
    CWnd::OnCreate(lpCreateStruct);

    GetClientRect(m_rcClient);

    CreateToolWindows();

    HookMouse();

    m_Sel.SetWnd(this);
    m_Sel.SetBounds(m_rcClient);

    return 0;
  }

  void CaptureWnd::OnDestroy()
  {
    DestroyToolWindows();

    UnhookMouse();

    CWnd::OnDestroy();
  }

  void CaptureWnd::OnPaint()
  {
    PAINTSTRUCT ps;
    BeginPaint(&ps);

    CRect rcPaint(ps.rcPaint);

    // Copy screen bitmap
    m_dcScreen.Blt(ps.hdc, rcPaint, rcPaint.TopLeft());

    if (!m_Sel.IsNull())
    {
      // Render selection
      Graphics gfx(ps.hdc);

      gfx.ExcludeClip(Rect(Convert(m_Sel.GetRect())));

      SolidBrush brush(Color(150, 0, 0, 0));
      gfx.FillRectangle(&brush, rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height());
    }
    
    EndPaint(&ps);
  }

  void CaptureWnd::OnLButtonDown(UINT nFlags, CPoint point)
  {
    SetCapture();

    bool bNoSelection = m_Sel.IsNull();

    if (bNoSelection || (m_Sel.HitTest(point) == Selection::GripNone))
    {
      m_Sel.Begin(point);
    }
    else
    {
      int nHitTest = m_Sel.HitTest(point);

      if (nHitTest != Selection::GripNone)
      {
        m_Sel.BeginAdjust(point, nHitTest);
      }
    }

    m_bSelecting = true;
  }

  void CaptureWnd::OnLButtonUp(UINT nFlags, CPoint point)
  {
    ReleaseCapture();

    m_Sel.End();

    if (m_Sel.IsEmpty())
    {
      m_Sel.Clear();

      m_rcDirty = m_rcClient;

      Invalidate();
    }

    m_bSelecting = false;
  }

  void CaptureWnd::OnMouseMove(UINT nFlags, CPoint point)
  {
    // Are we making a selection?
    if (m_bSelecting)
    {
      m_Sel.Track(point);

      // Set the "dirty" rect to the union of the last
      // selection and the current selection
      m_rcDirty.UnionRect(m_rcDirty, m_Sel.GetRect());

      Invalidate();

      m_rcDirty = m_Sel.GetRect();
    }

    // Reposition zoom window if necessary
    CRect rcZoomWnd;
    m_wndZoom.GetWindowRect(&rcZoomWnd);
    ScreenToClient(&rcZoomWnd);
    if (rcZoomWnd.PtInRect(point))
    {
      m_wndZoom.Reposition();
    }

    RenderZoomWnd(point);
    
    SetCursor(point);

    CWnd::OnMouseMove(nFlags, point);
  }

  void CaptureWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
    switch (nChar)
    {
    case VK_CONTROL:
      SetMousePrecision(true);
      break;

    case VK_F1:
      ToggleDebugWnd();
      break;

    case VK_ESCAPE:
      OnEscape();
      return;

    case VK_RETURN:
      OnEnter();
      return;
    }

    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
  }

  void CaptureWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
    if (nChar == VK_CONTROL)
    {
      SetMousePrecision(false);
    }

    CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
  }

  void CaptureWnd::OnKillFocus(CWnd* pNewWnd)
  {
    CWnd::OnKillFocus(pNewWnd);

#ifndef _DEBUG
    if (!m_bClosing)
      OnEscape();
#endif
  }

  LRESULT CaptureWnd::OnCapture(WPARAM wParam, LPARAM lParam)
  {
    // Capture the screen to our memory DC
    if (!m_dcScreen.CaptureScreen())
    {
      if (m_cb)
        m_cb(ErrorCaptureFailed, CString());

      DestroyWindow();

      return 0;
    }

    if (m_Sel.IsEmpty())
      m_rcDirty = m_rcClient;
    else
      m_rcDirty = m_Sel.GetRect();

    ShowWindow(SW_SHOWNORMAL);

    SetForegroundWindow();

    ::PlaySound(MAKEINTRESOURCE(IDR_CAPTURE), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);

    return 0;
  }

} // !namespace ScreenCap
