
// anXfsAppDlg.h : 头文件
//

#pragma once
#include <memory>
#include <vector>
#include <sstream>

class item {
public:
	item() : _value (0){
		std::stringstream logdata;
		logdata << "===item() : _value (0)" << std::endl;

		::OutputDebugStringA(logdata.str().c_str());
	}
	~item() {
		std::stringstream logdata;
		logdata << "===~item()" << std::endl;

		::OutputDebugStringA(logdata.str().c_str());
	}
	item(const item& a) {
		_value = a._value;

		std::stringstream logdata;
		logdata << "===item(const item& a)" << std::endl;

		::OutputDebugStringA(logdata.str().c_str());
	}
	item(item&& a) {
		std::stringstream logdata;
		logdata << "===item(item&& a)" << std::endl;

		::OutputDebugStringA(logdata.str().c_str());

		_value = a._value;
	}
	item& operator=(const item& a) {
		std::stringstream logdata;
		logdata << "===item& operator=(const item& a)" << std::endl;

		::OutputDebugStringA(logdata.str().c_str());
		if (this != &a) {
			_value = a._value;
		}
		return (*this);
	}
	item& operator=(item&& a) {
		std::stringstream logdata;
		logdata << "===item& operator=(item&& a)" << std::endl;

		::OutputDebugStringA(logdata.str().c_str());

		if (this != &a) {
			_value = a._value;
		}
		return (*this);
	}
private:
	int _value;
};

// CanXfsAppDlg 对话框
class CanXfsAppDlg : public CDialogEx
{
// 构造
public:
	CanXfsAppDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANXFSAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
