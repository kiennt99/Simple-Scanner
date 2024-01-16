#include "pch.h"
#include "CListBoxDetailsDlg.h"
#include "SimpleScannerDlg.h"
#include "resource.h"
#include <vector>

CListBoxDetailsDlg::CListBoxDetailsDlg(CWnd* pParent /*= nullptr*/)
	: CDialogEx(IDD_LISTBOX_DETAILS_DIALOG, pParent)
	, m_announcementText(_T(""))
	, m_editControl(_T(""))
{

}


//Cứa sổ này sẽ hiện ra list các file chứa signature đã được quét
BOOL CListBoxDetailsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_btnShowDetails.SubclassDlgItem(IDC_SHOW_DETAILS_BUTTON, this);

	m_editBox.ShowWindow(SW_HIDE);
	CSimpleScannerDlg* pMainDlg = static_cast<CSimpleScannerDlg*>(GetParent());
	std::vector<CString>& msgListBox = pMainDlg->m_msgListBox;
	int itemCount = msgListBox.size();
	for (int i = 0; i < itemCount; i++)
	{
		//CString itemText;
		//msgListBox.GetText(i, itemText);
		if (i == 0)
		{
			m_editControl.AppendFormat(_T("\r%s"), msgListBox[i]);
		}
		else
		{
			m_editControl.AppendFormat(_T("\r\n%s"), msgListBox[i]);
		}
		UpdateData(FALSE);
	}
	m_announcementText.Format(pMainDlg->m_announcementText);
	UpdateData(FALSE);
	return TRUE;
}

void CListBoxDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OK, m_btnOK);
	DDX_Text(pDX, IDC_ANNOUNCEMENT_TEXT, m_announcementText);
	DDX_Text(pDX, IDC_EDIT1, m_editControl);
	DDX_Control(pDX, IDC_EDIT1, m_editBox);
}

BEGIN_MESSAGE_MAP(CListBoxDetailsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SHOW_DETAILS_BUTTON, &CListBoxDetailsDlg::OnBnClickedShowDetailsButton)
	ON_BN_CLICKED(IDC_OK, &CListBoxDetailsDlg::OnBnClickedOk)
END_MESSAGE_MAP()

//
void CListBoxDetailsDlg::OnBnClickedShowDetailsButton()
{

	BOOL bVisible = m_editBox.IsWindowVisible();
	m_editBox.ShowWindow(bVisible ? SW_HIDE : SW_SHOW);
	m_btnShowDetails.SetWindowTextW(bVisible ? _T("Show details") : _T("Hide details"));
}



void CListBoxDetailsDlg::OnBnClickedOk()
{
	EndDialog(IDOK);
}
