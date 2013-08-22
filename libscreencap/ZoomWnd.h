#pragma once

namespace ScreenCap
{

  class ZoomWnd : public CWnd
  {
    enum Position
    {
      PositionTop,
      PositionBottom
    };

  public:
    bool Create(CWnd* pParent, CRect rcPrimaryMon);

    void Render(CDC* pdcSrc, const CPoint& ptCursor);

    void Reposition();
    void Reposition(Position pos);

  protected:
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

   DECLARE_MESSAGE_MAP()
   afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnDestroy();
   afx_msg void OnPaint();
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);

  private:
    CDC m_memDC;
    CBitmap* m_pbmpStock;
    HCURSOR m_hCur;
    ICONINFO m_iiCur;
    CRect m_rcPrimaryMon;
    Position m_pos;
  };

}