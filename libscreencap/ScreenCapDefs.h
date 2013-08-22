#pragma once

namespace ScreenCap
{
  
  enum ErrorType
  {
    ErrorNone = 0,
    ErrorCaptureFailed,
    ErrorSaveFailed,
    ErrorCanceled
  };

  typedef boost::function<void (ErrorType err, CString strFile)> Callback;

  enum CaptureType
  {
    CaptureTypeFullScreen,
    CaptureTypeCurrentWindow
  };

}