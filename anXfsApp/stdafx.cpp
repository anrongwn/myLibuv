
// stdafx.cpp : 只包括标准包含文件的源文件
// anXfsApp.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

#pragma comment(lib, "msxfs.lib")
#pragma comment(lib,"xfs_conf.lib")


using namespace zsummer::log4z;

//全局日志ID
LoggerId g_logid = LOG4Z_INVALID_LOGGER_ID;
int log_init() {
	int r = 0;

	ILog4zManager::getInstance()->config("D:\\MyTest\\2018_C++\\myLibuv\\Debug\\anXfsApp_log.cfg");
	g_logid = ILog4zManager::getInstance()->findLogger("Main");

	r = ILog4zManager::getInstance()->start();

	return r;
}
int log_stop() {
	int r = 0;

	ILog4zManager::getInstance()->stop();

	return r;
}