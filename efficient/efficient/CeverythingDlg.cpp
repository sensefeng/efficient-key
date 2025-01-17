// CeverythingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "efficient.h"
#include "CeverythingDlg.h"
#include "afxdialogex.h"


// CeverythingDlg dialog

IMPLEMENT_DYNAMIC(CeverythingDlg, CDialog)

CeverythingDlg::CeverythingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_EVERYTHING, pParent)
{
	GetTimeZoneInformation(&m_TimeZoneInformation);
}

CeverythingDlg::~CeverythingDlg()
{
	TerminateThread(m_queryAndDisplayProcessHandler, 0); 
	releaseResources();
}

void CeverythingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_KEYWORDCOLLECTOR, m_EditBox);
	DDX_Control(pDX, IDC_SEARCHRESULTLIST, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CeverythingDlg, CDialog)
	ON_EN_CHANGE(IDC_KEYWORDCOLLECTOR, &CeverythingDlg::OnEnChangeKeywordcollector)
	ON_LBN_DBLCLK(IDC_SEARCHRESULTLIST, &CeverythingDlg::OnLbnDblclkSearchresultlist)
	ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CeverythingDlg message handlers

void CeverythingDlg::OnEnChangeKeywordcollector()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	// got the keyword from user input
	m_EditBox.GetWindowTextA(m_keyWordInEditBox);
	m_editBoxTextChange = TRUE;
}

inline void everythingErrCheck(void)
{
	int everythingErrorCode = Everything_GetLastError();
	if (everythingErrorCode != EVERYTHING_OK)
	{
		CString tmpErrorMsg = "";
		switch (everythingErrorCode)
		{
		case EVERYTHING_OK:
			tmpErrorMsg = "EVERYTHING: everthing is ok";
			break;
		case EVERYTHING_ERROR_MEMORY:
			tmpErrorMsg = "EVERYTHING: out of memory";
			break;
		case EVERYTHING_ERROR_IPC:
			tmpErrorMsg = "EVERYTHING: search client is not running";
			break;
		case EVERYTHING_ERROR_REGISTERCLASSEX:
			tmpErrorMsg = "EVERYTHING: unable to register window class";
			break;
		case EVERYTHING_ERROR_CREATEWINDOW:
			tmpErrorMsg = "EVERYTHING: unable to create listening window";
			break;
		case EVERYTHING_ERROR_CREATETHREAD:
			tmpErrorMsg = "EVERYTHING: unable to create listening thread";
			break;
		case EVERYTHING_ERROR_INVALIDINDEX:
			tmpErrorMsg = "EVERYTHING: invalid index";
			break;
		case EVERYTHING_ERROR_INVALIDCALL:
			tmpErrorMsg = "EVERYTHING: invalid call";
			break;
		case EVERYTHING_ERROR_INVALIDREQUEST:
			tmpErrorMsg = "EVERYTHING: invalid request data, request data first";
			break;
		case EVERYTHING_ERROR_INVALIDPARAMETER:
			tmpErrorMsg = "EVERYTHING: bad parameter";
			break;
		default:
			tmpErrorMsg.Format("EVERYTHING: error code %d", everythingErrorCode);
			break;
		}
		MessageBox(NULL, tmpErrorMsg, _T("Waring"), MB_OK | MB_ICONINFORMATION);
	}
}

DWORD CeverythingDlg::queryAndDisplayProcess(LPVOID pParam)
{
	CeverythingDlg* pThis = (CeverythingDlg*)pParam;
	FILETIME tmpFileTime;
	while (1)
	{
		if (pThis->m_editBoxTextChange)
		{
			pThis->m_editBoxTextChange = FALSE;
			//search by everything
			Everything_SetSearchA(pThis->m_keyWordInEditBox); 
			Everything_SetRequestFlags(EVERYTHING_REQUEST_FILE_NAME | EVERYTHING_REQUEST_PATH | EVERYTHING_REQUEST_SIZE | EVERYTHING_REQUEST_DATE_MODIFIED);
			//Everything_SetSort(EVERYTHING_SORT_SIZE_DESCENDING);
			Everything_QueryA(TRUE);
			everythingErrCheck();
			pThis->m_searchResult.clear();
			pThis->m_listCtrl.ResetContent();
			int displaycount = Everything_GetNumResults();
			if (displaycount > MAX_ITEM_SHOW_IN_LIST)
			{
				displaycount = MAX_ITEM_SHOW_IN_LIST;
			}
			for (int i = 0; i < displaycount; i++)
			{
				pThis->m_tmpSearchResult.fileName = Everything_GetResultFileNameA(i);
				pThis->m_tmpSearchResult.filePath = Everything_GetResultPathA(i);
				Everything_GetResultSize(i, &(pThis->m_filesize));
				//pThis->m_tmpSearchResult.size = int ((pThis->m_filesize.QuadPart >> 10) + \
				//	!(PathIsDirectoryA(pThis->m_tmpSearchResult.filePath +"\\" + pThis->m_tmpSearchResult.fileName)));
				if (pThis->m_filesize.QuadPart > 0)
					pThis->m_tmpSearchResult.size = int((pThis->m_filesize.QuadPart >> 10) + 1);
				else
					pThis->m_tmpSearchResult.size = 0;
				Everything_GetResultDateModified(i, &tmpFileTime);
				FileTimeToSystemTime(&tmpFileTime, &(pThis->m_tmpSearchResult.modifiytime));
				SystemTimeToTzSpecificLocalTime(&(pThis->m_TimeZoneInformation), &(pThis->m_tmpSearchResult.modifiytime), &(pThis->m_tmpSearchResult.modifiytime));
				pThis->m_searchResult.push_back(pThis->m_tmpSearchResult);
			}
			Everything_Reset();
			displaycount = displaycount < 10 ? displaycount : 10;
			pThis->MoveWindow(CRect(pThis->m_rc.left, pThis->m_rc.top, pThis->m_rc.right, (LONG)(pThis->m_rc.bottom + SEARCH_RESULT_ITEM_HEIGHT * displaycount)));
			pThis->m_listCtrl.MoveWindow(CRect(0, (pThis->m_rc.bottom - pThis->m_rc.top), (pThis->m_rc.right - pThis->m_rc.left), (LONG)(pThis->m_rc.bottom - pThis->m_rc.top + SEARCH_RESULT_ITEM_HEIGHT * displaycount)));

			for (int i = 0; i < pThis->m_searchResult.size(); i++)
			{
				pThis->m_tmpSearchResult = pThis->m_searchResult.at(i);
				if (pThis->m_editBoxTextChange && i > 10)
					break;
				CString subTitleStr = _T("");
				subTitleStr.Format("%04d%02d%02d %02d:%02d size: %dkb %s",
					pThis->m_tmpSearchResult.modifiytime.wYear,
					pThis->m_tmpSearchResult.modifiytime.wMonth,
					pThis->m_tmpSearchResult.modifiytime.wDay,
					pThis->m_tmpSearchResult.modifiytime.wHour,
					pThis->m_tmpSearchResult.modifiytime.wMinute,
					pThis->m_tmpSearchResult.size,
					pThis->m_tmpSearchResult.filePath);
				pThis->m_listCtrl.AppendString(pThis->m_tmpSearchResult.fileName, subTitleStr, RGB(192, 192, 192), RGB(0, 43, 54));
			}
			pThis->m_listCtrl.AppendString(MAGIC_STRING_FOR_LAST_DRAW, MAGIC_STRING_FOR_LAST_DRAW, RGB(192, 192, 192), RGB(0, 43, 54));
		}
		else
		{
			Sleep(1);
		}
	}
	return 0;
}

BOOL CeverythingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//set the main window place
	m_screenx = GetSystemMetrics(SM_CXSCREEN);
	m_screeny = GetSystemMetrics(SM_CYSCREEN);
	m_rc.top = (LONG)(m_screeny * 0.191);//(1-0.618)/2
	m_rc.bottom = m_rc.top + SEARCH_BOX_HEIGHT;
	m_rc.left = (LONG)(m_screenx * 0.22);
	m_rc.right = (LONG)(m_screenx * 0.76);
	moveEverythingDialogWindow();

	DWORD threadID;
	m_keyWordInEditBox = "";
	m_editBoxTextChange = FALSE;
	m_queryAndDisplayProcessHandler = CreateThread(NULL, 0, CeverythingDlg::queryAndDisplayProcess, this, 0, &threadID);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

extern HWND g_hWnd;
void CeverythingDlg::OnLbnDblclkSearchresultlist()
{
	// TODO: Add your control notification handler code here
	int nSel = m_listCtrl.GetCurSel();
	ShowWindow(SW_HIDE);
	ShellExecute(g_hWnd, _T("open"), m_searchResult[nSel].filePath + "\\" + m_searchResult[nSel].fileName, _T(""), _T(""), SW_SHOWNORMAL);
	releaseResources();
}


BOOL CeverythingDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (IDC_SEARCHRESULTLIST == (CWnd::GetFocus())->GetDlgCtrlID())
		{
			OnLbnDblclkSearchresultlist();
		}
		return true;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_SPACE)
	{
		if (IDC_SEARCHRESULTLIST == (CWnd::GetFocus())->GetDlgCtrlID())
		{
			ShowWindow(SW_HIDE);
			ShellExecute(g_hWnd, _T("open"), m_searchResult[m_listCtrl.GetCurSel()].filePath, _T(""), _T(""), SW_SHOWNORMAL);
			releaseResources();
		    return true;
		}
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		releaseResources();
		return true;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DOWN)
	{
		if (IDC_KEYWORDCOLLECTOR == (CWnd::GetFocus())->GetDlgCtrlID())
		{
			m_listCtrl.SetFocus();
			m_listCtrl.SetCurSel(0);
			return true;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
void CeverythingDlg::releaseResources()
{
	ShowWindow(SW_HIDE);
	m_searchResult.clear();
	m_listCtrl.ResetContent();
	m_EditBox.SetSel(0, -1);
	m_EditBox.Clear();
	//move the window to default place
	moveEverythingDialogWindow();

	Everything_CleanUp();
	CloseHandle(m_queryAndDisplayProcessHandler);
	m_queryAndDisplayProcessHandler = NULL;
}

HBRUSH CeverythingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if ((CTLCOLOR_EDIT == nCtlColor) && (IDC_KEYWORDCOLLECTOR == pWnd->GetDlgCtrlID()) )
	{
		pDC->SetBkColor(RGB(40, 86, 104));
		pDC->SetTextColor(RGB(192, 192, 192));
		hbr = CreateSolidBrush(RGB(0, 86, 104));
	}
	if (CTLCOLOR_DLG == nCtlColor)
	{
		pDC->SetBkColor(RGB(0, 43, 54));
		pDC->SetTextColor(RGB(192, 192, 192));
		hbr = CreateSolidBrush(RGB(0, 43, 54));
	}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


LRESULT CeverythingDlg::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
    UINT nHitTest = (UINT)CDialog::OnNcHitTest(point);
 	if(nHitTest==HTCLIENT)
	{ 	
		GetClientRect(&m_rc);
		ClientToScreen(m_rc);
		m_rc.bottom = m_rc.top + SEARCH_BOX_HEIGHT;
		return HTCAPTION;
	}
	else
		return nHitTest;
}

void CeverythingDlg::moveEverythingDialogWindow(void)
{
	MoveWindow(m_rc);
	m_listCtrl.MoveWindow(CRect (0, SEARCH_BOX_HEIGHT, 0, 0));
	m_EditBox.MoveWindow(CRect(SEARCH_BOX_BODER_HEIGHT, SEARCH_BOX_BODER_HEIGHT, \
		(m_rc.right - m_rc.left - SEARCH_BOX_BODER_HEIGHT), (m_rc.bottom - m_rc.top - SEARCH_BOX_BODER_HEIGHT)));
}

