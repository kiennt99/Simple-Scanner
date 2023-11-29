
// SimpleScannerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "SimpleScanner.h"
#include "SimpleScannerDlg.h"
#include "afxdialogex.h"
#include "CListBoxDetailsDlg.h"
#include <fstream>
#include <string>
#include <afx.h>
#include <afxwin.h>
#include <sstream>
#include "sqlite3.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSimpleScannerDlg dialog



CSimpleScannerDlg::CSimpleScannerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIMPLESCANNER_DIALOG, pParent)
	, m_directoryPath(_T(""))
	, m_hexCodeSignature(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimpleScannerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EditDirPath, m_directoryPath);
	DDX_Text(pDX, IDC_EditSignature, m_hexCodeSignature);
	DDX_Control(pDX, IDC_LIST1, m_ctrlListBox);
}

BEGIN_MESSAGE_MAP(CSimpleScannerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Select, &CSimpleScannerDlg::OnBnClickedSelect)
	ON_BN_CLICKED(IDC_Start, &CSimpleScannerDlg::OnBnClickedStart)
	ON_MESSAGE(WM_SCAN_DIRECTORY, &CSimpleScannerDlg::OnScanDirectory)
	ON_MESSAGE(WM_SCAN_DIRECTORY_WITH_DATABASE, &CSimpleScannerDlg::OnScanDirectoryWithDatabase)
	ON_BN_CLICKED(IDC_Exit, &CSimpleScannerDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_CHOOSE_FROM_DB, &CSimpleScannerDlg::OnBnClickedChooseFromDb)
END_MESSAGE_MAP()



// CSimpleScannerDlg message handlers

BOOL CSimpleScannerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	m_customButtonStart.SubclassDlgItem(IDC_Start, this);
	m_customButtonExit.SubclassDlgItem(IDC_Exit, this);
	m_customButtonDirPath.SubclassDlgItem(IDC_EditDirPath, this);
	m_customButtonSignature.SubclassDlgItem(IDC_EditSignature, this);



	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSimpleScannerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSimpleScannerDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calas this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSimpleScannerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSimpleScannerDlg::reset()
{
	CSimpleScannerDlg::m_validFiles = 0;
	m_ctrlListBox.ResetContent();
	m_msgListBox.clear();
}



//Hàm ScanDirectory nhận đường dẫn thư mục và signature, sau đó duyệt các file trong thư mục (bao gồm cả thư mục con)
//xem file nào chứa signature và thêm vào list box
void CSimpleScannerDlg::ScanDirectory(CString directoryPath, CString hexCodeSignature)
{
	CFileFind fileFinder;
	CString searchPath = directoryPath + "\\*.*";
	BOOL bWorking = fileFinder.FindFile(searchPath);
	if (!bWorking)
	{
		AfxMessageBox(_T("Không tìm thấy thư mục cần quét"), MB_OK);
		fileFinder.Close();
		return;
	}
	while (bWorking)
	{
		bWorking = fileFinder.FindNextFile();

		if (fileFinder.IsDots())
			continue;
		if (fileFinder.IsDirectory())
		{
			CString subDir = fileFinder.GetFilePath();
			ScanDirectory(subDir, hexCodeSignature);
		}
		else
		{
			CString filePath = fileFinder.GetFilePath();
			if (ScanFile(filePath, hexCodeSignature)) {
				m_ctrlListBox.AddString(_T("[+]Đã quét xong file ") + filePath);
				m_msgListBox.push_back(filePath);
				m_validFiles++;
			}
			else
				m_ctrlListBox.AddString(_T("[-]Đã quét xong file ") + filePath);
		}
	}
	m_announcementText.Format(_T("Chương trình đã quét xong, tìm thấy %d file chứa signature"), m_validFiles);
	fileFinder.Close();
}

//Hàm ScanFile là hàm helper giúp kiểm tra xem 1 file có chứa signature hay ko

BOOL CSimpleScannerDlg::ScanFile(CString filePath, CString hexCodeSignature)
{
	std::ifstream file(filePath);
	if (!file.is_open()) {
		AfxMessageBox(_T("Failed to open the file."));
		return FALSE;
	}
	
	

	std::string hexCode;
	for (int i = 0; i < hexCodeSignature.GetLength(); i += 2) {
		CString hexByte = hexCodeSignature.Mid(i, 2);
		int hexValue = strtol(CStringA(hexByte), nullptr, 16);
		hexCode.push_back(static_cast<char>(hexValue));
	}

	std::string line;
	while (std::getline(file, line))
	{
		if (line.find(hexCode) != std::string::npos)
		{
			file.close();
			return TRUE;
		}
	}
	file.close();
	return FALSE;
}

//Hàm scan signature kiểm tra xem signature bị sai định dạng hay ko

BOOL CSimpleScannerDlg::ScanSignature(CString hexCodeSignature)
{

	if (hexCodeSignature.IsEmpty())
	{
		AfxMessageBox(_T("Signature sai định dạng (xâu ký tự rỗng)"), MB_OK);
		return FALSE;
	}
	int length = hexCodeSignature.GetLength();
	if (length % 2 != 0)
	{
		AfxMessageBox(_T("Signature sai định dạng (số ký tự không chẵn)"), MB_OK);
		return FALSE;
	}
	std::string hexCode = CStringA(hexCodeSignature);
	for (int i = 0; i < hexCode.length(); i++)
	{
		if (!isxdigit(hexCode[i])) {
			AfxMessageBox(_T("Signature sai định dạng (chứa ký tự không hợp lệ)"), MB_OK);
			return FALSE;
		}
	}
	return TRUE;
}

//Hàm ScanSignatureFromDatabase duyệt tất cả các signature trong database và đưa vào 1 vector
BOOL CSimpleScannerDlg::ScanSignatureFromDatabase()
{
	sqlite3* db;
	//int result = sqlite3_open("D:\\Training Security\\SimpleScanner\\x64\\Release\\SignaturesDB.db", & db);
	int result = sqlite3_open("..\\SignaturesDB.db", & db);
	if (result != SQLITE_OK) {
		AfxMessageBox(_T("Failed to open the database"), MB_OK);
		return FALSE;
	}
	const char* sql = "SELECT DISTINCT hexCodeSignature FROM signatures";
	sqlite3_stmt* stmt;
	result = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (result != SQLITE_OK) {
		AfxMessageBox(_T("Failed to prepare the SQL statement.") , MB_OK);
		sqlite3_close(db);
		return FALSE;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		CString hexCodeSignature = CString((const char*)sqlite3_column_text(stmt, 0));
		m_signatures.push_back(hexCodeSignature);
	}

	if (m_signatures.empty())
	{
		AfxMessageBox(_T("Empty database"), MB_OK);
		return FALSE;
	}
	// Finalize the statement and close the database
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return TRUE;
}


//Hàm ScanDirectoryWithSignature giống với hàm ScanDirectory, dùng để scan với mỗi signature trong database
void CSimpleScannerDlg::ScanDirectoryWithSignature(CString directoryPath, CString hexCodeSignature)
{
	CFileFind fileFinder;
	CString searchPath = directoryPath + "\\*.*";
	BOOL bWorking = fileFinder.FindFile(searchPath);
	if (!bWorking)
	{
		AfxMessageBox(_T("Không tìm thấy thư mục cần quét"), MB_OK);
		fileFinder.Close();
		return;
	}
	while (bWorking)
	{
		bWorking = fileFinder.FindNextFile();

		if (fileFinder.IsDots())
			continue;
		if (fileFinder.IsDirectory())
		{
			CString subDir = fileFinder.GetFilePath();
			ScanDirectoryWithSignature(subDir, hexCodeSignature);
		}
		else
		{
			CString filePath = fileFinder.GetFilePath();
			if (ScanFile(filePath, hexCodeSignature)) {
				m_ctrlListBox.AddString(filePath + _T(" chứa signature ") + hexCodeSignature);
				m_msgListBox.push_back(filePath + _T(" chứa signature ") + hexCodeSignature);
				m_validFiles++;
			}
		}
	}
	fileFinder.Close();
}

//Hàm ScanDirectoryWithDatabase scan thư mục với tất cả signature trong database
void CSimpleScannerDlg::ScanDirectoryWithDatabase(CString directoryPath, std::vector<CString> signatures)
{
	for (auto hexCodeSignature : signatures)
		CSimpleScannerDlg::ScanDirectoryWithSignature(directoryPath, hexCodeSignature);
	m_announcementText.Format(_T("Chương trình đã quét xong "));
}

//Khi click "chọn" hộp thoại windows explorer mở ra cho phép người dùng chọn đường dẫn thư mục
void CSimpleScannerDlg::OnBnClickedSelect()
{
	UpdateData();
	m_directoryPath.Format(_T(""));
	CFolderPickerDialog folderDialog;
	CString m_tmpValue = m_hexCodeSignature;
	if (folderDialog.DoModal() == IDOK)
	{
		m_directoryPath = folderDialog.GetFolderPath();
	}
	m_hexCodeSignature = m_tmpValue;
	UpdateData(FALSE);
}


//hàm lock và unlock controls khóa điều khiển các nút khi chương trình đang quét file
void CSimpleScannerDlg::lockControls()
{
	auto pEditDirectory = GetDlgItem(IDC_EditDirPath);
	auto pEditSignature = GetDlgItem(IDC_EditSignature);
	auto pBtnSelect = GetDlgItem(IDC_Select);
	auto pBtnStart = GetDlgItem(IDC_Start);
	auto pBtnChooseFromDB = GetDlgItem(IDC_CHOOSE_FROM_DB);
	pEditDirectory->EnableWindow(FALSE);
	pEditSignature->EnableWindow(FALSE);
	pBtnSelect->EnableWindow(FALSE);
	pBtnStart->EnableWindow(FALSE);
	pBtnChooseFromDB->EnableWindow(FALSE);
}

void CSimpleScannerDlg::unlockControls()
{
	auto pEditDirectory = GetDlgItem(IDC_EditDirPath);
	auto pEditSignature = GetDlgItem(IDC_EditSignature);
	auto pBtnSelect = GetDlgItem(IDC_Select);
	auto pBtnChooseFromDB = GetDlgItem(IDC_CHOOSE_FROM_DB);
	auto pBtnStart = GetDlgItem(IDC_Start);
	pEditDirectory->EnableWindow();
	pEditSignature->EnableWindow();
	pBtnSelect->EnableWindow();
	pBtnStart->EnableWindow();
	pBtnChooseFromDB->EnableWindow();
}


//Thread mới này sẽ thực hiện ScanDirectory để quét các file trong thư mục và list ra các file chứa signature
UINT CSimpleScannerDlg::CalculateThreadProc(LPVOID Param){
	CSimpleScannerDlg* ourWnd = reinterpret_cast<CSimpleScannerDlg*>(Param);
	if (!ourWnd->ScanSignature(ourWnd->m_hexCodeSignature))
		return 0;
	ourWnd->reset();
	ourWnd->lockControls();
	CTime startTime = CTime::GetCurrentTime();
	COleDateTime startDate = COleDateTime::GetCurrentTime();
	CString strTimeStart = startTime.Format(_T("%H:%M:%S"));
	CString strDateStart = startDate.Format(_T("%d/%m/%Y"));
	
	ourWnd->m_ctrlListBox.AddString(_T("Chương trình bắt đầu quét từ ") + strTimeStart + _T(" ") + strDateStart 
		+ _T(", tại thư mục ") + ourWnd->m_directoryPath);
	ourWnd->ScanDirectory(ourWnd->m_directoryPath, ourWnd->m_hexCodeSignature);

	int nIndex = ourWnd->m_ctrlListBox.GetCount();
	CTime endTime = CTime::GetCurrentTime();
	COleDateTime endDate = COleDateTime::GetCurrentTime();
	CString strTimeEnd = endTime.Format(_T("%H:%M:%S"));
	CString strDateEnd = endDate.Format(_T("%d/%m/%Y"));
	CString msg;
	msg.Format(_T("Chương trình quét xong lúc %s %s, tìm thấy %d file chứa signature"), strTimeEnd, strDateEnd, ourWnd->m_validFiles);
	ourWnd->m_ctrlListBox.InsertString(nIndex, msg);
	ourWnd->PostMessageW(WM_SCAN_DIRECTORY);
	ourWnd->unlockControls();
	
	return 0;
}



afx_msg LRESULT CSimpleScannerDlg::OnScanDirectory(WPARAM wParam, LPARAM lParam)
{

	CListBoxDetailsDlg Dlg;
	if(m_ctrlListBox.GetCount() != 0)
		Dlg.DoModal();
	return 0;
}


//Khi nhấn "Bắt đầu" sẽ tạo 1 thread mới để bắt đăù quét file
void CSimpleScannerDlg::OnBnClickedStart()
{
	UpdateData();
	AfxBeginThread(CalculateThreadProc, this);
}


void CSimpleScannerDlg::OnBnClickedExit()
{
	EndDialog(IDOK);
}


//Thread mới này quét tất cả các file trong thư mục lần lượt với từng signature trong database
UINT CSimpleScannerDlg::DatabaseThreadProc(LPVOID Param)
{	
	CSimpleScannerDlg* ourWnd = reinterpret_cast<CSimpleScannerDlg*>(Param);
	ourWnd->m_signatures.clear();
	if (ourWnd->ScanSignatureFromDatabase())
	{
		ourWnd->reset();
		ourWnd->lockControls();
		ourWnd->ScanDirectoryWithDatabase(ourWnd->m_directoryPath, ourWnd->m_signatures);
		ourWnd->PostMessageW(WM_SCAN_DIRECTORY_WITH_DATABASE);
		ourWnd->unlockControls();
	}
	return 0;
}


//Hàm này tạo 1 object của class CListBoxDetailsDlg để tạo 1 cửa số thông báo về kết quả quét
afx_msg LRESULT CSimpleScannerDlg::OnScanDirectoryWithDatabase(WPARAM wParam, LPARAM lParam)
{
	CListBoxDetailsDlg DLg;
	if (m_ctrlListBox.GetCount() != 0)
		DLg.DoModal();
	return 0;
}

//Khi chọn "Quét từ database" sẽ tạo ra 1 thread mới
void CSimpleScannerDlg::OnBnClickedChooseFromDb()
{
	AfxBeginThread(DatabaseThreadProc, this);
}