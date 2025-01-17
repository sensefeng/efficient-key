// KeyMouseDlg.h : header file
//

#pragma once


// CKeyMouseDlg dialog
class CKeyMouseDlg : public CDialog
{
// Construction
public:
	CKeyMouseDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_KEYMOUSE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	HINSTANCE m_hInstanceTmp;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
