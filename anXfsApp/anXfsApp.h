
// anXfsApp.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CanXfsAppApp: 
// �йش����ʵ�֣������ anXfsApp.cpp
//

class CanXfsAppApp : public CWinApp
{
public:
	CanXfsAppApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CanXfsAppApp theApp;