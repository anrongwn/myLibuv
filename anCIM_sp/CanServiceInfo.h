#pragma once

#include <XFSADMIN.H>
#include <XFSAPI.H>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <map>
#include <functional>

#include "../anCRS/an_xfs_result.h"

class CanServiceInfo
{
public:
	CanServiceInfo();
	~CanServiceInfo();
	CanServiceInfo(HSERVICE hService, LPSTR lpszAppID, \
		DWORD dwSrvcVersionsRequired = 0x0003);

	CanServiceInfo(const CanServiceInfo &a);
	CanServiceInfo& operator=(const CanServiceInfo &a);

	CanServiceInfo(CanServiceInfo &&a);
	CanServiceInfo& operator=(CanServiceInfo &&a);
	void clear();

	friend bool operator==(std::unique_ptr<CanServiceInfo> &a, const HSERVICE hService);

private:
	HSERVICE	hService_;
	HWND		hWndReg_;
	DWORD		dwEventClass_;
	std::string	strAppId_;
	DWORD		dwSrvcVersionsRequired_;

	friend class CanServiceInfo_Admi;
};

inline bool operator==(std::unique_ptr<CanServiceInfo> &a, const HSERVICE hService) {
	if (a.get()->hService_ == hService) {
		return true;
	}
	else {
		return false;
	}
}

class CanServiceInfo_Admi {
public:
	CanServiceInfo_Admi();
	~CanServiceInfo_Admi();

	CanServiceInfo_Admi(const CanServiceInfo_Admi&) = delete;
	CanServiceInfo_Admi& operator=(const CanServiceInfo_Admi&) = delete;

	void push(HSERVICE hService, LPSTR lpszAppID, DWORD dwSrvcVersionsRequired);
	HRESULT find_service(HSERVICE hService);
	HRESULT register_event(HSERVICE hService, DWORD dwEventClass, HWND hWndReg);
	HRESULT deregister_event(HSERVICE hService, DWORD dwEventClass, HWND hWndReg);
	HRESULT close_service(HSERVICE hService);
	//HWND get_wnd(HSERVICE hService, DWORD dwEventClass);
	

	int send_events(an_xfs_result * cmd, DWORD eventclass);
private:
	void * system_device_status(const std::string &buffer, LPWFSRESULT * lpresult);
	void * service_cim_itemtaken(const std::string &buffer, LPWFSRESULT * lpresult);
	
	void register_event_handler();
private:
	std::vector<std::unique_ptr<CanServiceInfo>> service_;
	std::mutex mtx_;

	typedef std::map<int, std::function<void*(const std::string &, LPWFSRESULT *)>> EVENT_HANDLER_MAP;
	EVENT_HANDLER_MAP event_handler_table_;
	//std::map<int, std::function<void*(const std::string &, LPWFSRESULT *)>> event_handler_table_;
};