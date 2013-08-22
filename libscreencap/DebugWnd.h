#pragma once

#include "stdarg.h"

#include "..\..\common\KayakoFontUtil.h"

namespace ScreenCap
{

  class DebugWnd : public CEdit
  {
  public:
    BOOL Create(CWnd* pParent);

    inline void PrintV(const wchar_t* strFormat, ...)
    {
#ifdef _DEBUG
      if (!::IsWindow(m_hWnd))
        return;

      va_list args;
      va_start(args, strFormat);

      CString strPrint;
      strPrint.FormatV(strFormat, args);
      strPrint.Append(L"\r\n");

      int len = GetWindowTextLength();
      SetSel(len, len);

      ReplaceSel(strPrint);

      va_end(args);
#endif
    }

    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
    Kayako::SystemFont m_Font;
  };

  extern DebugWnd g_wndDebug;

};