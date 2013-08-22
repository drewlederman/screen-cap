#include "stdafx.h"
#include "MultiMon.h"

namespace ScreenCap
{

  CRect GetVirtualScreenRect()
  {
    CRect rc;
    rc.left   = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    rc.top    = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    rc.right  = rc.left + ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    rc.bottom = rc.top  + ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
    return rc;
  }

  BOOL CALLBACK MonitorEnumProc(HMONITOR hMon, HDC hdcMon, LPRECT lprcMon, LPARAM dwData)
  {
    MonitorInfoList* plstMonitors = reinterpret_cast<MonitorInfoList*>(dwData);
    if (plstMonitors)
    {
      MONITORINFOEX info;
      info.cbSize = sizeof(info);
      
      if (GetMonitorInfo(hMon, &info))
        plstMonitors->push_back(info);
    }

    return TRUE;
  }

  bool GetMultiMonitorInfo(MonitorInfoList& lstMonitors)
  {
    return (EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&lstMonitors)) ? true : false);
  }

  bool GetPrimaryMonitorInfo(MONITORINFOEX* pInfo)
  {
    MonitorInfoList lstMonitors;
    if (!GetMultiMonitorInfo(lstMonitors))
      return false;

    MonitorInfoList::iterator mon;
    for (mon = lstMonitors.begin(); mon != lstMonitors.end(); ++mon)
    {
      if (mon->dwFlags == MONITORINFOF_PRIMARY && pInfo)
      {
        *pInfo = *mon;
        return true;
      }
    }

    return false;
  }

  //////////////////////////////////////////////////////////////////////////
  // CScreenDC
  //////////////////////////////////////////////////////////////////////////

  CScreenDC::CScreenDC()
  {
    m_pbmpStock = NULL;
  }

  CScreenDC::~CScreenDC()
  {
    CleanUp();
  }

  bool CScreenDC::CaptureScreen()
  {
    // Store cursor info
    GetCursorPos(&m_ptCursor);

    CURSORINFO ci = {0};
    ci.cbSize = sizeof(ci);
    GetCursorInfo(&ci);

    m_hCursor = ci.hCursor;

    // Capture screen
    CDC dcScreen;
    if (dcScreen.CreateDC(L"DISPLAY", NULL, NULL, NULL))
    {
      bool bSuccess = true;

      if (m_DC.CreateCompatibleDC(&dcScreen))
      {
        CRect rc = GetVirtualScreenRect();

        CBitmap bmpScreen;
        if (bmpScreen.CreateCompatibleBitmap(&dcScreen, rc.Width(), rc.Height()))
        {
          m_pbmpStock = m_DC.SelectObject(&bmpScreen);

          m_DC.BitBlt(0, 0, rc.Width(), rc.Height(), &dcScreen, rc.left, rc.top, SRCCOPY);
        }
        else
        {
          bSuccess = false;
        }
      }
      else
      {
        bSuccess = false;
      }

      dcScreen.DeleteDC();

      return bSuccess;
    }
    else
    {
      return false;
    }
  }

  void CScreenDC::InsertCursor()
  {
    ICONINFO ii = {0};
    GetIconInfo(m_hCursor, &ii);

    int x = m_ptCursor.x - ii.xHotspot;
    int y = m_ptCursor.y - ii.yHotspot;

    m_DC.DrawIcon(x, y, m_hCursor);
  }

  void CScreenDC::Blt(HDC dcDst, const CRect& rcDst, const CPoint& ptSrc)
  {
    ::BitBlt(dcDst, rcDst.left, rcDst.top, rcDst.Width(), rcDst.Height(), m_DC.m_hDC, ptSrc.x, ptSrc.y, SRCCOPY);
  }

  void CScreenDC::CleanUp()
  {
    if (m_pbmpStock)
    {
      CBitmap* pbmpScreen = m_DC.SelectObject(m_pbmpStock);
      if (pbmpScreen)
      {
        pbmpScreen->DeleteObject();
      }
    }
  }

}