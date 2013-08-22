#pragma once

namespace ScreenCap
{

  typedef std::list<MONITORINFOEX> MonitorInfoList;

  CRect GetVirtualScreenRect();

  bool GetMultiMonitorInfo(MonitorInfoList& lstMonitors);

  bool GetPrimaryMonitorInfo(MONITORINFOEX* pInfo);

  class CScreenDC
  {
  public:
    CScreenDC();
    virtual ~CScreenDC();

    bool CaptureScreen();

    void InsertCursor();

    void Blt(HDC dcDst, const CRect& rcDst, const CPoint& ptSrc);

    CDC* GetDC() { return &m_DC; }

    void CleanUp();

  private:
    CDC m_DC;

    CPoint m_ptCursor;
    HCURSOR m_hCursor;

    CBitmap* m_pbmpStock;
  };

}