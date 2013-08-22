#include "stdafx.h"
#include "Encoder.h"

#include "Conversions.h"

using namespace Gdiplus;


namespace ScreenCap
{

  bool SavePngFile(CDC* pDC, CRect rcCrop, const CString& strFile)
  {
    bool bSuccess = false;

    // Create a Bitmap
    if (pDC)
    {
      CBitmap*  pBitmap  = pDC->GetCurrentBitmap();
      CPalette* pPalette = pDC->GetCurrentPalette();

      if (pBitmap && pPalette)
      {
        Bitmap* pbmScreen = Bitmap::FromHBITMAP(*pBitmap, *pPalette);
        if (pbmScreen)
        {
          Bitmap* pbmCrop = pbmScreen->Clone(Convert(rcCrop), pbmScreen->GetPixelFormat());
          if (pbmCrop)
          {
            // Find PNG encoder CLSID
            CLSID clsidPng;
            unsigned int num  = 0;
            unsigned int size = 0;

            GetImageEncodersSize(&num, &size);
            if (size != 0)
            {
              ImageCodecInfo* imageCodecInfo = new ImageCodecInfo[size];
              GetImageEncoders(num, size, imageCodecInfo);

              for (unsigned int i = 0; i < num; ++i)
              {
                if (wcscmp(imageCodecInfo[i].MimeType, L"image/png") == 0)
                {
                  clsidPng = imageCodecInfo[i].Clsid;
                  break;
                }    
              }
              delete[] imageCodecInfo;
            }

            // Save image
            bSuccess = (pbmCrop->Save(strFile, &clsidPng, NULL) == Ok);

            delete pbmCrop;
          }

          delete pbmScreen;
        }
      }
    }

    return bSuccess;
  }

}