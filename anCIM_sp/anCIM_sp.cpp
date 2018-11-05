// anCIM_sp.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "an_CIM_spi.h"
#include "../anpc/anpc.h"
#include "message_pack.h"
#include "xfs_handler.h"
#include <mutex>
#include "../anpc/anpc_error.h"
#include "an_cim_execute_cmd_data.h"

#pragma comment(lib,"xfs_supp.lib")
#if _DEBUG
#pragma comment(lib, "..//Debug//anpc.lib")
//#pragma comment(lib, "..//tcmalloc//libtcmalloc_minimal-debug.lib")
#else
#pragma comment(lib, "..//Release//anpc.lib")
//#pragma comment(lib, "..//tcmalloc//libtcmalloc_minimal.lib")
#endif

//全局锁
std::mutex g_mtx;

//全局 service info
CanServiceInfo_Admi g_service_info;



//发送命令
inline int send_xfs_cmd(const std::stringstream &buffer, HSERVICE hService) {
	int r = ANPC_AN_SUCCESS;

	size_t len = buffer.str().length();
	void * cmd = anpc_malloc(len);
	memcpy(cmd, buffer.str().c_str(), len);

	//发送命令
	r = anpc_send_cmd(hService, cmd, len);
	
	return r;
}

HRESULT WINAPI WFPCancelAsyncRequest(HSERVICE hService, REQUESTID RequestID) {
	HRESULT hr = WFS_SUCCESS;

	std::unique_lock<std::mutex> lock(g_mtx);

	//service 是否存在？
	hr = g_service_info.find_service(hService);
	if (hr) return hr;

	std::stringstream buffer = serialize_cmd<an_WFPCancelAsyncRequest>(hService, RequestID);
	
	int r = send_xfs_cmd(buffer, hService);
	if (ANPC_AN_SUCCESS == r) {
	}
	else {
		hr = WFS_ERR_CONNECTION_LOST;
	}
	return hr;
}

HRESULT WINAPI WFPClose(HSERVICE hService, HWND hWnd, REQUESTID ReqID) {
	HRESULT hr = WFS_SUCCESS;

	std::unique_lock<std::mutex> lock(g_mtx);

	//service 是否存在？
	hr = g_service_info.find_service(hService);
	if (hr) return hr;

	std::stringstream buffer = serialize_cmd<an_WFPClose>(hService, (unsigned long)hWnd, ReqID);

	int r = send_xfs_cmd(buffer, hService);
	if (ANPC_AN_SUCCESS == r) {
		anpc_close_ipc(hService);

		//g_service_info.close_service(hService);
	}
	else {
		hr = WFS_ERR_CONNECTION_LOST;
	}
	return hr;
}

HRESULT WINAPI WFPDeregister(HSERVICE hService, DWORD dwEventClass, HWND hWndReg, HWND hWnd, REQUESTID ReqID) {
	HRESULT hr = WFS_SUCCESS;

	std::unique_lock<std::mutex> lock(g_mtx);

	//反注册信息
	hr = g_service_info.deregister_event(hService, dwEventClass, hWndReg);
	if (hr) return hr;

	std::stringstream buffer = serialize_cmd<an_WFPDeregister>(hService, dwEventClass, \
		(unsigned long)hWndReg, (unsigned long)hWnd, ReqID);

	int r = send_xfs_cmd(buffer, hService);
	if (ANPC_AN_SUCCESS == r) {
	}
	else {
		hr = WFS_ERR_CONNECTION_LOST;
	}
	return hr;
}

HRESULT WINAPI WFPExecute(HSERVICE hService, DWORD dwCommand, LPVOID lpCmdData, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID) {
	HRESULT hr = WFS_SUCCESS;

	std::unique_lock<std::mutex> lock(g_mtx);

	//service 是否存在？
	hr = g_service_info.find_service(hService);
	if (hr) return hr;

	std::stringstream cmd_data;

	switch (dwCommand) {
	case WFS_CMD_CIM_CASH_IN_START:
		cmd_data = serialize_cmd<an_wfs_cim_cash_in_start>((LPWFSCIMCASHINSTART)lpCmdData);
		break;
	case WFS_CMD_CIM_CASH_IN:
		break;
	}

	std::stringstream buffer = serialize_cmd<an_WFPExecute>(hService, dwCommand, \
		cmd_data.str().c_str(), cmd_data.str().length(), dwTimeOut, (unsigned long)hWnd, ReqID);

	int r = send_xfs_cmd(buffer, hService);
	if (ANPC_AN_SUCCESS == r) {
	}
	else {
		hr = WFS_ERR_CONNECTION_LOST;
	}


	return hr;
}

HRESULT WINAPI WFPGetInfo(HSERVICE hService, DWORD dwCategory, LPVOID lpQueryDetails, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID) {
	HRESULT hr = WFS_SUCCESS;

	std::unique_lock<std::mutex> lock(g_mtx);

	//service 是否存在？
	hr = g_service_info.find_service(hService);
	if (hr) return hr;

	std::stringstream os;
	std::string details;

	switch (dwCategory) {
	case WFS_INF_CIM_STATUS:
		break;
	case WFS_INF_CIM_CAPABILITIES:
		break;
	}

	std::stringstream buffer = serialize_cmd<an_WFPGetInfo>(hService, dwCategory, \
		details.c_str(), details.length(), dwTimeOut, (unsigned long)hWnd, ReqID);

	int r = send_xfs_cmd(buffer, hService);
	if (ANPC_AN_SUCCESS == r) {
	}
	else {
		hr = WFS_ERR_CONNECTION_LOST;
	}

	return hr;
}

HRESULT WINAPI WFPLock(HSERVICE hService, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID) {
	HRESULT hr = WFS_SUCCESS;

	std::unique_lock<std::mutex> lock(g_mtx);

	//service 是否存在？
	hr = g_service_info.find_service(hService);
	if (hr) return hr;

	std::stringstream buffer = serialize_cmd<an_WFPLock>(hService, dwTimeOut, \
		(unsigned long)hWnd, ReqID);

	int r = send_xfs_cmd(buffer, hService);
	if (ANPC_AN_SUCCESS == r) {
	}
	else {
		hr = WFS_ERR_CONNECTION_LOST;
	}

	return hr;
}

//spi 版本匹配
#define AN_SPI_VER_DES ("anXFS SPI 3.00-3.30")
HRESULT spi_version_match(const DWORD dwSPIVersionsRequired, const DWORD dwSrvcVersionsRequired, \
	LPWFSVERSION &lpSPIVersion, LPWFSVERSION &lpSrvcVersion)
{
	HRESULT hr = WFS_SUCCESS;

	if (lpSPIVersion)
	{
		memset(lpSPIVersion, 0, sizeof(WFSVERSION));
		lpSPIVersion->wVersion = 0x0003;//0x0303; //当前版本3.03
		lpSPIVersion->wLowVersion = 0x0003; //支持的最低版本3.00
		lpSPIVersion->wHighVersion = 0x0003;//0x1e03;    //支持的最高版本3.30
		memcpy(lpSPIVersion->szDescription, AN_SPI_VER_DES, strlen(AN_SPI_VER_DES));
	}

	if (lpSrvcVersion)
	{
		memset(lpSrvcVersion, 0, sizeof(WFSVERSION));
		lpSrvcVersion->wVersion = 0x0203;//0x0303; //当前版本3.03
		lpSrvcVersion->wLowVersion = 0x0003; //支持的最低版本3.00
		lpSrvcVersion->wHighVersion = 0x0203;//0x1e03;    //支持的最高版本3.30
		memcpy(lpSrvcVersion->szDescription, AN_SPI_VER_DES, strlen(AN_SPI_VER_DES));
	}

	WORD wSpi_HV = 0, wSpi_LV = 0, wSrv_HV = 0, wSrv_LV = 0;

	//spi要求最高版本(低16位)（0xHHHHLLLL）
	wSpi_HV = LOWORD(dwSPIVersionsRequired);
	//spi 要求最低版本（高16位）（0xHHHHLLLL）
	wSpi_LV = HIWORD(dwSPIVersionsRequired);

	//srv要求最高版本(低16位)（0xHHHHLLLL）
	wSrv_HV = LOWORD(dwSrvcVersionsRequired);
	//srv 要求最低版本（高16位）（0xHHHHLLLL）
	wSrv_LV = HIWORD(dwSrvcVersionsRequired);

	//判断版本要求
	if (wSpi_HV > 0xff03)   ///spi 最高版本支持3.30（0x1e03）
		hr = WFS_ERR_SPI_VER_TOO_HIGH;
	else if (wSrv_HV > 0xff03)  ///srv 最高版本支持3.30（0x1e03）
		hr = WFS_ERR_SRVC_VER_TOO_HIGH;
	else if (wSpi_LV < 0x0001)
		hr = WFS_ERR_SPI_VER_TOO_LOW; ///spi 最低版本支持3.00(0x0003 )
	else if (wSrv_LV < 0x0001)
		hr = WFS_ERR_SRVC_VER_TOO_LOW;  ///srv 最高版本支持3.00(0x0003 )

	return hr;

}

HRESULT WINAPI WFPOpen(HSERVICE hService, LPSTR lpszLogicalName, HAPP hApp, \
	LPSTR lpszAppID, DWORD dwTraceLevel, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID, \
	HPROVIDER hProvider, DWORD dwSPIVersionsRequired, LPWFSVERSION lpSPIVersion, \
	DWORD dwSrvcVersionsRequired, LPWFSVERSION lpSrvcVersion) {

	std::unique_lock<std::mutex> lock(g_mtx);
	HRESULT hr = WFS_SUCCESS;

	//版本判断
	hr = spi_version_match(dwSPIVersionsRequired, dwSrvcVersionsRequired, \
		lpSPIVersion, lpSrvcVersion);
	if (WFS_SUCCESS != hr)
	{
		return hr;
	}

	//增加
	g_service_info.push(hService, lpszAppID, dwSrvcVersionsRequired);

	//
	std::stringstream buffer = serialize_cmd<an_WFPOpen>(hService, lpszLogicalName, (unsigned long)hApp, \
		lpszAppID, dwTraceLevel, dwTimeOut, (unsigned long)hWnd, ReqID, (unsigned long)hProvider, dwSPIVersionsRequired, dwSrvcVersionsRequired);

	int ret = ANPC_AN_SUCCESS;

	//启动通信
	ret = anpc_create_ipc(hService, lpszLogicalName, xfs_result_handler);
	if (ANPC_AN_SUCCESS == ret) {
		//发送命令
		ret = send_xfs_cmd(buffer, hService);
		if (ANPC_AN_SUCCESS == ret) {
		}
		else {
			hr = WFS_ERR_CONNECTION_LOST;
		}
	}

	return hr;
}

HRESULT WINAPI WFPRegister(HSERVICE hService, DWORD dwEventClass, HWND hWndReg, HWND hWnd, REQUESTID ReqID) {
	HRESULT hr = WFS_SUCCESS;

	std::unique_lock<std::mutex> lock(g_mtx);

	//注册信息
	hr = g_service_info.register_event(hService, dwEventClass, hWndReg);
	if (hr) return hr;

	std::stringstream buffer = serialize_cmd<an_WFPRegister>(hService, dwEventClass, \
		(unsigned long)hWndReg, (unsigned long)hWnd, ReqID);

	int r = send_xfs_cmd(buffer, hService);
	if (ANPC_AN_SUCCESS == r) {
	}
	else {
		hr = WFS_ERR_CONNECTION_LOST;
	}
	return hr;
}

HRESULT WINAPI WFPSetTraceLevel(HSERVICE hService, DWORD dwTraceLevel) {
	HRESULT hr = WFS_SUCCESS;

	std::unique_lock<std::mutex> lock(g_mtx);

	//service 是否存在？
	hr = g_service_info.find_service(hService);
	if (hr) return hr;

	std::stringstream buffer = serialize_cmd<an_WFPSetTraceLevel>(hService, dwTraceLevel);

	int r = send_xfs_cmd(buffer, hService);
	if (ANPC_AN_SUCCESS == r) {
	}
	else {
		hr = WFS_ERR_CONNECTION_LOST;
	}
	return hr;
}

HRESULT WINAPI WFPUnloadService() {
	HRESULT hr = WFS_SUCCESS;

	std::unique_lock<std::mutex> lock(g_mtx);

	return hr;
}

HRESULT WINAPI WFPUnlock(HSERVICE hService, HWND hWnd, REQUESTID ReqID) {
	HRESULT hr = WFS_SUCCESS;

	std::unique_lock<std::mutex> lock(g_mtx);

	//service 是否存在？
	hr = g_service_info.find_service(hService);
	if (hr) return hr;

	std::stringstream buffer = serialize_cmd<an_WFPUnlock>(hService, (unsigned long)hWnd, ReqID);

	return hr;
}

