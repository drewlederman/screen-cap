#include "stdafx.h"
#include "ZoomWnd.h"

#include "MultiMon.h"

namespace ScreenCap
{

  BEGIN_MESSAGE_MAP(ScreenCap::ZoomWnd, CWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
  END_MESSAGE_MAP()

  
  const UINT nZoomFactor = 2; // We can change this later

  const UINT nWndSize    = 250;
  const UINT nWndMargin  = 50;

  bool ZoomWnd::Create(CWnd* pParent, CRect rcPrimaryMon)
  {
    m_rcPrimaryMon = rcPrimaryMon;

    return (CWnd::CreateEx(WS_EX_PALETTEWINDOW, NULL, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 
            CRect(0, 0, nWndSize, nWndSize), pParent, 0) != 0);
  }

  void ZoomWnd::Render(CDC* pdcSrc, const CPoint& ptCursor)
  {
    if (!pdcSrc)
      return;

    CRect rcClient;
    GetClientRect(&rcClient);

    int cxSrc = rcClient.Width()  / nZoomFactor;
    int cySrc = rcClient.Height() / nZoomFactor;
    int xSrc  = (ptCursor.x - (cxSrc / 2));
    int ySrc  = (ptCursor.y - (cySrc / 2));

    m_memDC.FillSolidRect(rcClient, RGB(0, 0, 0));

    rcClient.DeflateRect(2, 2);

    m_memDC.StretchBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), pdcSrc, xSrc, ySrc, cxSrc, cySrc, SRCCOPY);

    CPoint ptCenter = rcClient.CenterPoint();
    m_memDC.DrawIcon(ptCenter.x - m_iiCur.xHotspot - 1, ptCenter.y - m_iiCur.yHotspot - 1, m_hCur);

    InvalidateRect(rcClient);
    UpdateWindow();
  }

  void ZoomWnd::Reposition()
  {
    switch (m_pos)
    {
    case PositionTop:
      Reposition(PositionBottom);
      break;
    case PositionBottom:
      Reposition(PositionTop);
      break;
    }
  }

  void ZoomWnd::Reposition(Position pos)
  {
    int xOrig = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int yOrig = GetSystemMetrics(SM_YVIRTUALSCREEN);

    CPoint ptNewPos;

    switch (pos)
    {
    case PositionTop:
      {
        ptNewPos.x = abs(xOrig) + m_rcPrimaryMon.left + nWndMargin;
        ptNewPos.y = abs(yOrig) + m_rcPrimaryMon.top  + nWndMargin;
      }
      break;
    case PositionBottom:
      {
        ptNewPos.x = abs(xOrig) + m_rcPrimaryMon.left + nWndMargin;
        ptNewPos.y = abs(yOrig) + m_rcPrimaryMon.top  + (m_rcPrimaryMon.Height() - nWndSize - nWndMargin);
      }
      break;
    }

    SetWindowPos(NULL, ptNewPos.x, ptNewPos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    m_pos = pos;
  }

  BOOL ZoomWnd::PreCreateWindow(CREATESTRUCT& cs)
  {
    cs.lpszClass = AfxRegisterWndClass(NULL);

    return CWnd::PreCreateWindow(cs);
  }

  int ZoomWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
    if (CWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

    Reposition(PositionTop);

    // Load the 'cross' cursor
    m_hCur = LoadCursor(NULL, MAKEINTRESOURCE(IDC_CROSS));

    GetIconInfo(m_hCur, &m_iiCur);

    // Prepare our memory DC
    CDC* pDC = GetDC();

    if (m_memDC.CreateCompatibleDC(pDC))
    {
      CRect rcClient;
      GetClientRect(&rcClient);

      CBitmap bmp;
      if (bmp.CreateCompatibleBitmap(pDC, rcClient.Width(), rcClient.Height()))
      {
        m_pbmpStock = m_memDC.SelectObject(&bmp);
      }
    }

    ReleaseDC(pDC);
    
    return 0;
  }

  void ZoomWnd::OnDestroy()
  {
    // Clean up GDI stuff
    DestroyCursor(m_hCur);

    DeleteObject(m_iiCur.hbmColor);
    DeleteObject(m_iiCur.hbmMask);

    if (m_pbmpStock)
    {
      CBitmap* pbmp = m_memDC.SelectObject(m_pbmpStock);
      if (pbmp)
      {
        pbmp->DeleteObject();
      }
    }

    CWnd::OnDestroy();
  }

  void ZoomWnd::OnPaint()
  {
    PAINTSTRUCT ps;
    BeginPaint(&ps);

    CDC dc;
    dc.Attach(ps.hdc);

    CRect rcPaint(ps.rcPaint);

    // Draw contents
    dc.BitBlt(rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height(), &m_memDC, rcPaint.left, rcPaint.top, SRCCOPY);

    dc.Detach();
    EndPaint(&ps);
  }

  void ZoomWnd::OnMouseMove(UINT nFlags, CPoint point)
  {
    Reposition();

    CWnd::OnMouseMove(nFlags, point);
  }

} //!namespace ScreenCap