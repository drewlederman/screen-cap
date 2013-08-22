#pragma once


namespace ScreenCap
{

  class Selection
  {
  public:
    enum GripType
    {
      GripNone        = 0x00,
      GripLeft        = 0x01,
      GripTop         = 0x02,
      GripRight       = 0x04,
      GripBottom      = 0x08,
      GripCenter      = 0x10,
      GripTopLeft     = GripTop | GripLeft,
      GripTopRight    = GripTop | GripRight,
      GripBottomRight = GripBottom | GripRight,
      GripBottomLeft  = GripBottom | GripLeft
    };

  public:
    Selection();

    void SetWnd(CWnd* pWnd);
    void SetBounds(CRect rcBounds);

    int  HitTest(CPoint pt);
    void HitTestCursor(CPoint pt, LPCTSTR& lpCursor);

    void Set(CRect rc);
    void Begin(CPoint pt);
    void BeginAdjust(CPoint pt, int nGripType);
    void Track(CPoint pt);
    void End();
    
    bool IsEmpty();
    bool IsNull();
    void Clear();

    CRect GetRect();

  private:
    void SnapCursor(CPoint pt, int nGripType);

  private:
    CWnd* m_pWnd;
    CRect m_rcBounds;
    CRect m_rcSelection;
    CPoint m_ptRef;
    int m_gripType;
  };

}