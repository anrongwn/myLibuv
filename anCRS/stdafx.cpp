// stdafx.cpp : 只包括标准包含文件的源文件
// anCRS.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中引用任何所需的附加头文件，
//而不是在此文件中引用

//全局日志ID
LoggerId	g_logid = LOG4Z_INVALID_LOGGER_ID;

using namespace zsummer::log4z;

int anlog_init() {
	int r = 0;

	ILog4zManager::getInstance()->config("D:\\MyTest\\2018_C++\\myLibuv\\Debug\\anCRS_log.cfg");
	g_logid = ILog4zManager::getInstance()->findLogger("Main");

	r = ILog4zManager::getInstance()->start();

	return r;
}

int anlog_stop() {
	int r = 0;

	ILog4zManager::getInstance()->stop();

	return r;
}