
// anXfsAppDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "anXfsApp.h"
#include "anXfsAppDlg.h"
#include "afxdialogex.h"
#include <XFSSPI.H>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CanXfsAppDlg 对话框



CanXfsAppDlg::CanXfsAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ANXFSAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CanXfsAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CanXfsAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CanXfsAppDlg 消息处理程序

BOOL CanXfsAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	/*
#define XX(v)	#v
	char * p = XX(WFS_SUCCESS);
	*/

	
	item * ps = new item();
	std::unique_ptr<item> pi(ps);
	//delete pi.get();
	//delete pi.release();

	

	std::vector<std::unique_ptr<item>> vpi;
	vpi.push_back(std::move(pi));
	size_t count = vpi.size();



	for (auto & it : vpi) {
		/*
		if (nullptr==it.get()) {
			break;
		}
		*/
		//delete it.release();
	}
	count = vpi.size();

	for (auto & it : vpi) {
		/*
		if (nullptr == it.get()) {
			break;
		}
		*/
		//delete it.release();
	}

	if (nullptr == pi.get()) {
		int i = 0;
	}

	std::string str1{ "wangjr" };
	str1 += '\0';
	str1 += 'a';
	//std::string str2 = std::move(str1);
	//std::string str3 = str2;

	size_t len = str1.length();
	size_t size = str1.size();

	HMODULE hm = LoadLibraryA("D:\\MyTest\\2018_C++\\myLibuv\\Release\\anpc.dll");
	if (NULL == hm) {
		DWORD err = ::GetLastError();
	}
	

	if (hm)	FreeLibrary(hm);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CanXfsAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CanXfsAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CanXfsAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

