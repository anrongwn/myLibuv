#pragma once
#ifdef ANCIM_SP_EXPORTS
#define ANCIM_SP_API __declspec(dllexport)
#else
#define ANCIM_SP_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <xfsapi.h>

	typedef HANDLE HPROVIDER;

#include <xfsconf.h>
#include <xfsadmin.h>

	/*   be aware of alignment   */
#pragma pack(push,1)

	/****** SPI functions ********************************************************/

	ANCIM_SP_API HRESULT WINAPI WFPCancelAsyncRequest(HSERVICE hService, REQUESTID RequestID);

	ANCIM_SP_API HRESULT WINAPI WFPClose(HSERVICE hService, HWND hWnd, REQUESTID ReqID);

	ANCIM_SP_API HRESULT WINAPI WFPDeregister(HSERVICE hService, DWORD dwEventClass, HWND hWndReg, HWND hWnd, REQUESTID ReqID);

	ANCIM_SP_API HRESULT WINAPI WFPExecute(HSERVICE hService, DWORD dwCommand, LPVOID lpCmdData, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID);

	ANCIM_SP_API HRESULT WINAPI WFPGetInfo(HSERVICE hService, DWORD dwCategory, LPVOID lpQueryDetails, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID);

	ANCIM_SP_API HRESULT WINAPI WFPLock(HSERVICE hService, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID);

	ANCIM_SP_API HRESULT WINAPI WFPOpen(HSERVICE hService, LPSTR lpszLogicalName, HAPP hApp, LPSTR lpszAppID, DWORD dwTraceLevel, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID, HPROVIDER hProvider, DWORD dwSPIVersionsRequired, LPWFSVERSION lpSPIVersion, DWORD dwSrvcVersionsRequired, LPWFSVERSION lpSrvcVersion);

	ANCIM_SP_API HRESULT WINAPI WFPRegister(HSERVICE hService, DWORD dwEventClass, HWND hWndReg, HWND hWnd, REQUESTID ReqID);

	ANCIM_SP_API HRESULT WINAPI WFPSetTraceLevel(HSERVICE hService, DWORD dwTraceLevel);

	ANCIM_SP_API HRESULT extern WINAPI WFPUnloadService();

	ANCIM_SP_API HRESULT extern WINAPI WFPUnlock(HSERVICE hService, HWND hWnd, REQUESTID ReqID);


	/*   restore alignment   */
#pragma pack(pop)

#ifdef __cplusplus
}    /*extern "C"*/
#endif