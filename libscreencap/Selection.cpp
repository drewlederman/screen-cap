#include "stdafx.h"
#include "Selection.h"

namespace ScreenCap
{

  const UINT nGripSize = 5; // 5px selection margin

  inline CString GripTypeToString(int nGripType)
  {
#ifdef _DEBUG
    switch (nGripType)
    {
    case Selection::GripNone:        return L"None";
    case Selection::GripLeft:        return L"Left";
    case Selection::GripTop:         return L"Top";
    case Selection::GripRight:       return L"Right";
    case Selection::GripBottom:      return L"Bottom";
    case Selection::GripCenter:      return L"Center";
    case Selection::GripTopLeft:     return L"TopLeft";
    case Selection::GripTopRight:    return L"TopRight";
    case Selection::GripBottomRight: return L"BottomRight";
    case Selection::GripBottomLeft:  return L"BottomLeft";
    }
#endif
    return CString();
  }

  Selection::Selection()
  {
    m_pWnd = NULL;

    m_rcSelection.SetRect(0, 0, 0, 0);
    m_ptRef.SetPoint(0, 0);

    m_gripType = GripNone;
  }

  void Selection::SetWnd(CWnd* pWnd)
  {
    m_pWnd = pWnd;
  }

  void Selection::SetBounds(CRect rcBounds)
  {
    m_rcBounds = rcBounds;
  }

  int Selection::HitTest(CPoint pt)
  {
    CRect rcSelection = m_rcSelection;
    rcSelection.InflateRect(nGripSize, nGripSize);

    if (!rcSelection.PtInRect(pt))
    {
      return GripNone;
    }
   
    CRect rcL(m_rcSelection.left,  m_rcSelection.top,    m_rcSelection.left,  m_rcSelection.bottom);
    CRect rcT(m_rcSelection.left,  m_rcSelection.top,    m_rcSelection.right, m_rcSelection.top);
    CRect rcR(m_rcSelection.right, m_rcSelection.top,    m_rcSelection.right, m_rcSelection.bottom);
    CRect rcB(m_rcSelection.left,  m_rcSelection.bottom, m_rcSelection.right, m_rcSelection.bottom);

    rcL.InflateRect(nGripSize, nGripSize);
    rcT.InflateRect(nGripSize, nGripSize);
    rcR.InflateRect(nGripSize, nGripSize);
    rcB.InflateRect(nGripSize, nGripSize);

    int ret = GripNone;
    
    // Left
    if (rcL.PtInRect(pt))
    {
      ret |= GripLeft;
    }
    // Top
    if (rcT.PtInRect(pt))
    {
      ret |= GripTop;
    }
    // Right
    if (rcR.PtInRect(pt))
    {
      ret |= GripRight;
    }
    // Bottom
    if (rcB.PtInRect(pt))
    {
      ret |= GripBottom;
    }
    // Center
    if (ret == GripNone)
    {
      ret = GripCenter;
    }

    return ret;
  }

  void Selection::HitTestCursor(CPoint pt, LPCTSTR& lpCursor)
  {
    if (IsEmpty())
      return;

    int nHitTest = HitTest(pt);

    // Center
    if (nHitTest == GripCenter)
    {
      lpCursor = IDC_SIZEALL;
    }
    // Left and Right
    else if (nHitTest == GripLeft ||
             nHitTest == GripRight)
    {
      lpCursor = IDC_SIZEWE;
    }
    // Top and Bottom
    else if (nHitTest == GripTop || 
             nHitTest == GripBottom)
    {
      lpCursor = IDC_SIZENS;
    }
    // Top-Left and Bottom-Right
    else if (nHitTest == GripTopLeft || 
             nHitTest == GripBottomRight)
    {
      lpCursor = IDC_SIZENWSE;
    }
    // Top-Right and Bottom-Left
    else if (nHitTest == GripTopRight || 
             nHitTest == GripBottomLeft)
    {
      lpCursor = IDC_SIZENESW;
    }
  }

  void Selection::Set(CRect rc)
  {
    m_rcSelection = rc;
  }

  void Selection::Begin(CPoint pt)
  {
    m_rcSelection.SetRect(pt, pt);

    m_gripType = GripRight | GripBottom;
  }

  void Selection::BeginAdjust(CPoint pt, int nGripType)
  {
    SnapCursor(pt, nGripType);

    m_ptRef = pt - m_rcSelection.TopLeft();
    
    m_gripType = nGripType;
  }

  void Selection::Track(CPoint pt)
  {
    if (m_gripType == GripNone)
    {
      return;
    }
    if (m_gripType & GripLeft)
    {
      if (pt.x >= m_rcBounds.left)
        m_rcSelection.left = pt.x;
    }
    if (m_gripType & GripTop)
    {
      if (pt.y >= m_rcBounds.top)
        m_rcSelection.top = pt.y;
    }
    if (m_gripType & GripRight)
    {
      if (pt.x <= m_rcBounds.right)
        m_rcSelection.right = pt.x;
    }
    if (m_gripType & GripBottom)
    {
      if (pt.y <= m_rcBounds.bottom)
        m_rcSelection.bottom = pt.y;
    }
    if (m_gripType == GripCenter)
    {      
      CRect rcAdjusted = m_rcSelection;
      rcAdjusted.MoveToXY(CPoint(pt - m_ptRef));

      if (rcAdjusted.left < m_rcBounds.left)
        rcAdjusted.MoveToX(m_rcBounds.left);
      if (rcAdjusted.top < m_rcBounds.top)
        rcAdjusted.MoveToY(m_rcBounds.top);
      if (rcAdjusted.right > m_rcBounds.right)
        rcAdjusted.MoveToX(m_rcBounds.right - m_rcSelection.Width());
      if (rcAdjusted.bottom > m_rcBounds.bottom)
        rcAdjusted.MoveToY(m_rcBounds.bottom - m_rcSelection.Height());

      m_rcSelection = rcAdjusted;
    }
  }

  void Selection::End()
  {
    m_gripType = GripNone;

    m_rcSelection.NormalizeRect();
  }

  bool Selection::IsEmpty()
  {
    return (m_rcSelection.IsRectEmpty() ? true : false);
  }

  bool Selection::IsNull()
  {
    return (m_rcSelection.IsRectNull() ? true : false);
  }

  void Selection::Clear()
  {
    m_gripType = GripNone;

    m_rcSelection.SetRectEmpty();
  }

  CRect Selection::GetRect()
  {
    CRect rcSel = m_rcSelection;

    rcSel.NormalizeRect();

    return rcSel;
  }
  
  void Selection::SnapCursor(CPoint pt, int nGripType)
  {
    if (nGripType == GripCenter)
    {
      return;
    }
    if (nGripType & GripLeft)
    {
      pt.x = m_rcSelection.left;
    }
    if (nGripType & GripTop)
    {
      pt.y = m_rcSelection.top;
    }
    if (nGripType & GripRight)
    {
      pt.x = m_rcSelection.right;
    }
    if (nGripType & GripBottom)
    {
      pt.y = m_rcSelection.bottom;
    }
    
    if (m_pWnd)
      m_pWnd->ClientToScreen(&pt);

    SetCursorPos(pt.x, pt.y);
  }

} // !namespace ScreenCap