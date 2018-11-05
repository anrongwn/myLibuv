
// anXfsAppDlg.h : ͷ�ļ�
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

// CanXfsAppDlg �Ի���
class CanXfsAppDlg : public CDialogEx
{
// ����
public:
	CanXfsAppDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANXFSAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
