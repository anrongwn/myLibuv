// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>



// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "../libuv/include/uv.h"
#include "log4z\log4z.h"

extern LoggerId g_logid;

int anlog_init();
int anlog_stop();
/*
#include "CanIPCAdmin.h"

extern CanIPCAdmin g_ca;
*/
