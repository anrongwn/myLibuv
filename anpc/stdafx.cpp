// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// anpc.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H �������κ�����ĸ���ͷ�ļ���
//�������ڴ��ļ�������
LoggerId g_logid = LOG4Z_INVALID_LOGGER_ID;

using namespace zsummer::log4z;

int anlog_init() {
	int r = 0;

	ILog4zManager::getInstance()->config("D:\\MyTest\\2018_C++\\myLibuv\\Debug\\anpc_log.cfg");
	g_logid = ILog4zManager::getInstance()->findLogger("Main");

	r = ILog4zManager::getInstance()->start();

	return r;
}

int anlog_stop() {
	int r = 0;

	ILog4zManager::getInstance()->stop();

	return r;
}