
// ScreenCapTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScreenCapTest.h"
#include "ScreenCapTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ScreenCapTestDlg dialog

ScreenCapTestDlg::ScreenCapTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ScreenCapTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ScreenCapTestDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_CAPTURETYPE, m_cmbType);
  DDX_Control(pDX, IDC_SPINDELAY,   m_spinDelay);
}

BEGIN_MESSAGE_MAP(ScreenCapTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_CAPTURE, &ScreenCapTestDlg::OnBnClickedCapture)
END_MESSAGE_MAP()


// ScreenCapTestDlg message handlers

BOOL ScreenCapTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Extra initialization
  m_cmbType.AddString(L"Fullscreen");
  m_cmbType.AddString(L"Current Window");
  m_cmbType.SetCurSel(0);

  m_spinDelay.SetRange(0, 10);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void ScreenCapTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR ScreenCapTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void ScreenCapTestDlg::OnBnClickedCapture()
{
  ScreenCap::CaptureType Type = static_cast<ScreenCap::CaptureType>(m_cmbType.GetCurSel());

  int nDelay = (m_spinDelay.GetPos() * 1000);

  ScreenCap::DoCapture(Type, nDelay, boost::bind(&ScreenCapTestDlg::CaptureCallback, this, _1, _2)); 
}

void ScreenCapTestDlg::CaptureCallback(ScreenCap::ErrorType err, CString strFilePath)
{
  if (err == ScreenCap::ErrorNone)
  {
    ShellExecute(NULL, L"open", strFilePath, NULL, NULL, SW_SHOWNORMAL);
  } 
  else if (err == ScreenCap::ErrorCaptureFailed)
  {
    MessageBox(L"An error occurred while trying to capture your screen. This is typically due to insufficient system resources.", NULL, MB_ICONSTOP);
  }
} 