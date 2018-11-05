#pragma once
#include <XFSAPI.H>
#include <XFSCIM.H>
#include <XFSADMIN.H>

#include <string>
#include <sstream>
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
//#include <cereal\archives\json.hpp>
//#include <cereal\types\array.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>
//#include <cereal\types\list.hpp>
//#include <cereal\types\map.hpp>

#pragma pack(push, 1)

//序例化命令参数
template<typename cmdType, typename... Args>
inline std::stringstream serialize_cmd(const Args&... rest) {
	std::stringstream os;

	cmdType cmd(rest...);
	{
		cereal::BinaryOutputArchive bi_ar(os);
		bi_ar(cmd);
	}
	return os; //c++11，充分使用了右值，move构造及赋值
}

typedef enum {
	e_an_WFPUNKNOW = 0, \
	e_an_WFPCancelAsyncRequest, \
	e_an_WFPClose, \
	e_an_WFPDeregister, \
	e_an_WFPExecute, \
	e_an_WFPGetInfo, \
	e_an_WFPLock, \
	e_an_WFPOpen, \
	e_an_WFPRegister, \
	e_an_WFPSetTraceLevel, \
	e_an_WFPUnloadService, \
	e_an_WFPUnlock, \
	e_an_WFP,
}e_an_wfp_type;

static const char *const e_an_wfp_type_name[] =
{
	"WFPUNKNOW",
	"WFPCancelAsyncRequest",
	"WFPClose ",
	"WFPDeregister ",
	"WFPExecute",
	"WFPGetInfo",
	"WFPLock",
	"WFPOpen",
	"WFPRegister",
	"WFPSetTraceLevel",
	"WFPUnloadService",
	"WFPUnlock",
	""
};

typedef enum {
	e_an_cmd_init=0,
	e_an_cmd_pending,
	e_an_cmd_completed,
	e_an_cmd_cancelled,
	e_an_cmd_timeout,
	e_an_cmd_unknow
}e_an_wfp_cmd_status;


struct an_WFP_header {
	size_t nLen;
	e_an_wfp_type eType;
	e_an_wfp_cmd_status eStatus;
	HSERVICE hService;
	//HWND hWnd;
	unsigned long hWnd;
	REQUESTID ReqID;
	DWORD dwStartTime;

	an_WFP_header() {
		nLen = 0;
		eType = e_an_WFPUNKNOW;
		eStatus = e_an_cmd_init;
		//hService = 0;
		hWnd = 0;
		//ReqID = 0;
		dwStartTime = ::GetTickCount();
	}

	an_WFP_header(HSERVICE service, REQUESTID reqid) : an_WFP_header() {
		hService = service;
		ReqID = reqid;
	}

	template<typename Archive>
	void serialize(Archive & ar) {
		ar(nLen, eType, eStatus, hService, hWnd, ReqID, dwStartTime);
	}
};

struct an_WFPCancelAsyncRequest : public an_WFP_header {
	an_WFPCancelAsyncRequest() :an_WFP_header() {
		eType = e_an_WFPCancelAsyncRequest;
	}

	an_WFPCancelAsyncRequest(HSERVICE hService, REQUESTID RequestID) 
		: an_WFP_header(hService, RequestID){
		eType = e_an_WFPCancelAsyncRequest;
	}
};

struct an_WFPClose : public an_WFP_header {
	an_WFPClose() :an_WFP_header() {
		eType = e_an_WFPClose;

	}

	an_WFPClose(HSERVICE hService, unsigned long wnd, REQUESTID ReqID)
		: an_WFP_header(hService, ReqID) {
		hWnd = wnd;
		eType = e_an_WFPClose;
	}
};

struct an_WFPDeregister : public an_WFP_header {
	DWORD dwEventClass;
	unsigned long hWndReg;

	an_WFPDeregister() : an_WFP_header() {
		dwEventClass = 0;
		hWndReg = 0;
		eType = e_an_WFPDeregister;
	}

	an_WFPDeregister(HSERVICE service, DWORD eventclass, unsigned long wndreg, \
		unsigned long wnd, REQUESTID reqid) : an_WFP_header(service, reqid) {
		this->dwEventClass = eventclass;
		this->hWnd = wnd;
		this->hWndReg = wndreg;
		eType = e_an_WFPDeregister;
	}
	template<typename Archive>
	void serialize(Archive & ar) {
		an_WFP_header::serialize(ar);
		ar(dwEventClass, hWndReg);
	}
};

struct an_WFPExecute : public an_WFP_header {
	DWORD dwCommand;
	std::string lpCmdData;
	DWORD dwTimeOut;

	an_WFPExecute() : an_WFP_header() {
		dwCommand = 0;
		dwTimeOut = 0;
		eType = e_an_WFPExecute;

		lpCmdData.clear();
	}

	an_WFPExecute(HSERVICE service, DWORD command, const char * lpcmddata, size_t data_len, \
		DWORD timeout, unsigned long hwnd, REQUESTID reqid) \
		: an_WFP_header(service, reqid){

		this->dwCommand = command;
		this->dwTimeOut = timeout;
		this->eType = e_an_WFPExecute;
		this->hWnd = hwnd;
		this->lpCmdData = std::string(lpcmddata, data_len);
		this->nLen = data_len;

	}
	template<typename Archive>
	void serialize(Archive & ar) {
		an_WFP_header::serialize(ar);
		ar(dwCommand, lpCmdData, dwTimeOut);
	}
};

struct an_WFPGetInfo : public an_WFP_header {
	DWORD dwCategory;
	std::string lpQueryDetails;
	DWORD dwTimeOut;

	an_WFPGetInfo() : an_WFP_header() {
		dwCategory = 0;
		dwTimeOut = 0;
		eType = e_an_WFPGetInfo;

		lpQueryDetails.clear();
	}

	an_WFPGetInfo(HSERVICE service, DWORD category, \
		const char* lpdetails, size_t details_len,  DWORD timeout, unsigned long hwnd, REQUESTID reqid) \
		: an_WFP_header(service, reqid) {
		this->dwCategory = category;
		this->dwTimeOut = timeout;
		this->eType = e_an_WFPGetInfo;
		this->hWnd = hwnd;
		this->lpQueryDetails = std::string(lpdetails, details_len);
		
	}
	template<typename Archive>
	void serialize(Archive & ar) {
		an_WFP_header::serialize(ar);
		ar(dwCategory, lpQueryDetails, dwTimeOut);
	}
};

struct an_WFPLock : public an_WFP_header {
	DWORD dwTimeOut;

	an_WFPLock() : an_WFP_header() {
		dwTimeOut = 0;
		eType = e_an_WFPLock;
	}

	an_WFPLock(HSERVICE service, DWORD timeout, unsigned long wnd, REQUESTID reqid) \
		: an_WFP_header(service, reqid) {
		dwTimeOut = timeout;
		hWnd = wnd;
		eType = e_an_WFPLock;
	}

	template<typename Archive>
	void serialize(Archive & ar) {
		an_WFP_header::serialize(ar);
		ar(dwTimeOut);
	}
};

struct an_WFPUnlock : public an_WFP_header {
	
	an_WFPUnlock() : an_WFP_header() {
		eType = e_an_WFPUnlock;
	}

	an_WFPUnlock(HSERVICE service, unsigned long wnd, REQUESTID reqid) \
		: an_WFP_header(service, reqid) {
		hWnd = wnd;
		eType = e_an_WFPUnlock;
	}

	template<typename Archive>
	void serialize(Archive & ar) {
		an_WFP_header::serialize(ar);
		
	}
};
struct an_WFPOpen : public an_WFP_header {
	std::string lpszLogicalName;
	unsigned long hApp;
	std::string lpszAppID;
	DWORD dwTraceLevel;
	DWORD dwTimeOut;
	unsigned long hProvider;
	DWORD dwSPIVersionsRequired;
	//LPWFSVERSION lpSPIVersion;
	DWORD dwSrvcVersionsRequired;
	//LPWFSVERSION lpSrvcVersion;

	an_WFPOpen() : an_WFP_header() {
		eType = e_an_WFPOpen;
		dwTraceLevel = 0;
		dwTimeOut = 0;
		hProvider = 0;
		dwSPIVersionsRequired = 0;
		dwSrvcVersionsRequired = 0;
	}

	an_WFPOpen(HSERVICE service, LPSTR logicname, unsigned long app, LPSTR appid, \
		DWORD tracelevel, DWORD timeout, unsigned long wnd, REQUESTID reqid, \
		unsigned long provider, DWORD spivr, DWORD srvvr) \
		: an_WFP_header(service, reqid) {
		eType = e_an_WFPOpen;

		hWnd = wnd;
		dwTraceLevel = tracelevel;
		dwTimeOut = timeout;
		hProvider = provider;
		dwSPIVersionsRequired = spivr;
		dwSrvcVersionsRequired = srvvr;
	}

	template<typename Archive>
	void serialize(Archive & ar) {
		an_WFP_header::serialize(ar);
		ar(lpszLogicalName, hApp, lpszAppID, dwTraceLevel, dwTimeOut, \
			hProvider, dwSPIVersionsRequired, dwSrvcVersionsRequired);
	}
};

struct an_WFPRegister : public an_WFP_header { 
	DWORD dwEventClass;
	unsigned long hWndReg;

	an_WFPRegister() : an_WFP_header() {
		this->eType = e_an_WFPRegister;
		dwEventClass = 0;
		hWndReg = 0;
	}

	an_WFPRegister(HSERVICE service, DWORD eventclass, unsigned long wndreg, \
		unsigned long wnd, REQUESTID reqid) : an_WFP_header(service, reqid) {
		this->eType = e_an_WFPRegister;
		dwEventClass = eventclass;
		hWndReg = wndreg;
		hWnd = wnd;
	}

	template<typename Archive>
	void serialize(Archive & ar) {
		an_WFP_header::serialize(ar);
		ar(dwEventClass, hWndReg);
	}
};

struct an_WFPSetTraceLevel : public an_WFP_header {
	DWORD dwTraceLevel;

	an_WFPSetTraceLevel() : an_WFP_header() {
		this->eType = e_an_WFPSetTraceLevel;
		dwTraceLevel = 0;
	}

	an_WFPSetTraceLevel(HSERVICE service, DWORD tracelevel) : an_WFP_header(service, 0) {
		this->eType = e_an_WFPSetTraceLevel;
		dwTraceLevel = tracelevel;
	}
	template<typename Archive>
	void serialize(Archive & ar) {
		an_WFP_header::serialize(ar);
		ar(dwTraceLevel);
	}
	
};

#pragma pack(pop)