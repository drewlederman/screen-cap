#pragma once

#include "ScreenCapDefs.h"
#include "ZoomWnd.h"
#include "MultiMon.h"
#include "Selection.h"

#include "..\..\common\KayakoCursor.h"

namespace ScreenCap
{

  class CaptureWnd : public CWnd
  {

  public:
    CaptureWnd();
    virtual ~CaptureWnd();

    BOOL Create();

    void Capture(Callback cb);

    void SetSelection(CRect rc, bool bRedraw = false);

  protected:
    void CreateToolWindows();
    void DestroyToolWindows();

    void RenderZoomWnd(const CPoint& ptCursor);

    void SetCursor(const CPoint& ptCursor);

    void Invalidate();

    void OnEscape();
    void OnEnter();

    void ToggleDebugWnd();

    bool SaveFile(CString& strFileOut);

    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void PostNcDestroy();

    DECLARE_MESSAGE_MAP();
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg LRESULT OnCapture(WPARAM wParam, LPARAM lParam);

  private:
    ZoomWnd m_wndZoom;

    CScreenDC m_dcScreen;

    Kayako::Cursor m_cursor;

    Selection m_Sel;
    bool m_bSelecting;

    CRect m_rcClient;
    CRect m_rcDirty;

    Callback m_cb;

    bool m_bClosing;
  };

}