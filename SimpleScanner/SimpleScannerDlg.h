
// SimpleScannerDlg.h : header file
//

#pragma once

#include "afxwin.h"
#include <vector>
#include "sqlite3.h"
// CSimpleScannerDlg dialog

#define WM_SCAN_DIRECTORY (WM_USER + 1)
#define WM_SCAN_DIRECTORY_WITH_DATABASE (WM_USER + 2)


class CCustomButton : public CButton
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg) override
	{
		if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
		{
			return TRUE;
		}

		return CButton::PreTranslateMessage(pMsg);
	}
};

class CSimpleScannerDlg : public CDialogEx
{
// Construction
public:
	CSimpleScannerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMPLESCANNER_DIALOG };
#endif

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
	CString m_directoryPath;
	CString m_hexCodeSignature;
	CListBox m_ctrlListBox;
	std::vector<CString> m_msgListBox;
	int m_validFiles;
	void ScanDirectory(CString directoryPath, CString hexCodeSignature);
	BOOL ScanFile(CString filePath, CString hexCodeSignature);
	BOOL ScanSignature(CString hexCodeSignature);
	void ScanDirectoryWithSignature(CString directoryPath, CString hexCodeSignature);
	void ScanDirectoryWithDatabase(CString directoryPath, std::vector<CString> signatures);
	BOOL ScanSignatureFromDatabase();
	std::vector<CString> m_signatures;
	CString m_announcementText;
	void lockControls();
	void unlockControls();
	void reset();
	static UINT CalculateThreadProc(LPVOID Param);
	static UINT DatabaseThreadProc(LPVOID Param);
	CCustomButton m_customButtonStart;
	CCustomButton m_customButtonExit;
	CCustomButton m_customButtonDirPath;
	CCustomButton m_customButtonSignature;
	CCustomButton m_customButtonListBox;

public:
	afx_msg void OnBnClickedSelect();
	afx_msg void OnBnClickedStart();
protected:
	afx_msg LRESULT OnScanDirectory(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScanDirectoryWithDatabase(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedChooseFromDb();
};



