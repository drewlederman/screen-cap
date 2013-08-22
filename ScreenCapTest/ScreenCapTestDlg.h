
// ScreenCapTestDlg.h : header file
//

#pragma once

#include "..\libscreencap\ScreenCap.h"


// ScreenCapTestDlg dialog
class ScreenCapTestDlg : public CDialog
{
// Construction
public:
	ScreenCapTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SCREENCAPTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

  void CaptureCallback(ScreenCap::ErrorType err, CString strFilePath);

// Implementation
protected:
  CComboBox m_cmbType;
  CSpinButtonCtrl m_spinDelay;

	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedCapture();
};
