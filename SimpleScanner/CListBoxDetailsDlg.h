#pragma once
#include <afxdialogex.h>
//Tạo 1 class mới để tạo cửa số thông báo kết quả
class CListBoxDetailsDlg :
    public CDialogEx
{
public:
    CListBoxDetailsDlg(CWnd* pParent = nullptr);
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_LISTBOX_DETAILS_DIALOG };
#endif // AFX_DESIGN_TIME
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    virtual BOOL OnInitDialog() override;
    CButton m_btnShowDetails;
public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedShowDetailsButton();
    CButton m_btnOK;
    afx_msg void OnBnClickedOk();
    CString m_announcementText;
    CEdit* m_pAnnouncementText;
    CString m_editControl;
    CEdit m_editBox;
};

