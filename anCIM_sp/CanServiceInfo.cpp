#include "stdafx.h"
#include "CanServiceInfo.h"
#include "../anCRS/an_xfscim.h"
#include <algorithm>

CanServiceInfo::CanServiceInfo():hService_(0), hWndReg_(NULL), \
	dwEventClass_(0), strAppId_(""), dwSrvcVersionsRequired_(0x0003)
{
}
CanServiceInfo::CanServiceInfo(HSERVICE hService, LPSTR lpszAppID, DWORD dwSrvcVersionsRequired) : hService_(hService), hWndReg_(NULL), \
	dwEventClass_(0), strAppId_(lpszAppID), dwSrvcVersionsRequired_(dwSrvcVersionsRequired){
	
}

CanServiceInfo::~CanServiceInfo()
{
}
void CanServiceInfo::clear() {
	hService_ = 0;
	dwEventClass_ = 0;
	hWndReg_ = NULL;
	strAppId_ = "";
	dwSrvcVersionsRequired_ = 0;
}

CanServiceInfo::CanServiceInfo(const CanServiceInfo &a) {
	hService_ = a.hService_;
	dwEventClass_ = a.dwEventClass_;
	hWndReg_ = a.hWndReg_;
	strAppId_ = a.strAppId_;
	dwSrvcVersionsRequired_ = a.dwSrvcVersionsRequired_;
}

CanServiceInfo& CanServiceInfo::operator=(const CanServiceInfo &a) {
	if (this != &a) {
		hService_ = a.hService_;
		dwEventClass_ = a.dwEventClass_;
		hWndReg_ = a.hWndReg_;
		strAppId_ = a.strAppId_;
		dwSrvcVersionsRequired_ = a.dwSrvcVersionsRequired_;
	}

	return (*this);
}

CanServiceInfo::CanServiceInfo(CanServiceInfo &&a) {
	hService_ = a.hService_;
	dwEventClass_ = a.dwEventClass_;
	hWndReg_ = a.hWndReg_;
	strAppId_ = std::move(a.strAppId_);
	dwSrvcVersionsRequired_ = a.dwSrvcVersionsRequired_;
}

CanServiceInfo& CanServiceInfo::operator=(CanServiceInfo &&a) {
	if (this != &a) {
		hService_ = a.hService_;
		dwEventClass_ = a.dwEventClass_;
		hWndReg_ = a.hWndReg_;
		strAppId_ = std::move(a.strAppId_);
		dwSrvcVersionsRequired_ = a.dwSrvcVersionsRequired_;
	}

	return (*this);
}

CanServiceInfo_Admi::CanServiceInfo_Admi() {
	register_event_handler();
}

CanServiceInfo_Admi::~CanServiceInfo_Admi() {

}

void CanServiceInfo_Admi::push(HSERVICE hService, LPSTR lpszAppID, \
	DWORD dwSrvcVersionsRequired) {
	std::lock_guard<std::mutex> lock(mtx_);

	service_.push_back(std::make_unique<CanServiceInfo>(hService, lpszAppID, \
		dwSrvcVersionsRequired));//c++14
}

HRESULT CanServiceInfo_Admi::find_service(HSERVICE hService) {
	std::lock_guard<std::mutex> lock(mtx_);
	HRESULT hr = WFS_ERR_SERVICE_NOT_FOUND;

	for (auto &ref : service_) {
		if ((ref.get()->hService_) == hService) {
			hr = WFS_SUCCESS;
			break;
		}
	}

	return hr;
}

HRESULT CanServiceInfo_Admi::register_event(HSERVICE hService, DWORD dwEventClass, \
	HWND hWndReg) {

	std::lock_guard<std::mutex> lock(mtx_);

	HRESULT hr = WFS_ERR_SERVICE_NOT_FOUND;

	for (auto &ref : service_) {
		if ((ref.get()->hService_) == hService) {
			ref.get()->hWndReg_ = hWndReg;
			ref.get()->dwEventClass_ |= dwEventClass;
			
			hr = WFS_SUCCESS;
			break;
		}
	}
	
	return hr;
}

HRESULT CanServiceInfo_Admi::deregister_event(HSERVICE hService, DWORD dwEventClass, \
	HWND hWndReg) {
	std::lock_guard<std::mutex> lock(mtx_);

	HRESULT hr = WFS_ERR_SERVICE_NOT_FOUND;

	for (auto &ref : service_) {
		if ((ref.get()->hService_) == hService) {
			//ref.get()->hWndReg_ = hWndReg;
			ref.get()->dwEventClass_ &= (~dwEventClass);
			hr = WFS_SUCCESS;
			break;
		}
	}

	return hr;
}

HRESULT CanServiceInfo_Admi::close_service(HSERVICE hService) {
	std::lock_guard<std::mutex> lock(mtx_);

	HRESULT hr = WFS_ERR_SERVICE_NOT_FOUND;

	//删除
	auto it = std::find(service_.begin(), service_.end(), hService);
	if (it != service_.end()) {
		service_.erase(it);
		hr = WFS_SUCCESS;
	}

	return hr;

	/*//清空
	for (auto &ref : service_) {
		if ((ref.get()->hService_) == hService) {
			
			ref.get()->clear();

			hr = WFS_SUCCESS;
			break;
		}
	}

	return hr;
	*/
}
/*
HWND CanServiceInfo_Admi::get_wnd(HSERVICE hService, DWORD dwEventClass) {
	HWND hWnd = NULL;

	for (auto &ref : service_) {
		if (((ref.get()->hService_) == hService) && \
			(ref.get()->dwEventClass_ & dwEventClass)){
			hWnd = ref.get()->hWndReg_;
			break;
		}
	}
	
	return hWnd;
}
*/
void * CanServiceInfo_Admi::service_cim_itemtaken(const std::string &buffer, LPWFSRESULT * lpresult) {
	an_wfs_cim_position_info position;;
	unserialize_result<an_wfs_cim_position_info>(buffer, position);

	LPWFSCIMPOSITIONINFO lppos = nullptr;
	HRESULT hr = WFMAllocateMore(sizeof(WFSCIMPOSITIONINFO), *lpresult, (void**)&lppos);
	if (lppos) {
		(*lpresult)->lpBuffer = lppos;

		memcpy(lppos, &position, sizeof(WFSCIMPOSITIONINFO));
	}
	return lppos;
}
void * CanServiceInfo_Admi::system_device_status(const std::string &buffer, LPWFSRESULT * lpresult) {
	
	an_wfs_devstatus devstatus;
	unserialize_result<an_wfs_devstatus>(buffer, devstatus);

	LPWFSDEVSTATUS lpdev = nullptr;
	HRESULT hr = WFMAllocateMore(sizeof(WFSDEVSTATUS), *lpresult, (void**)&lpdev);
	if (lpdev) {
		(*lpresult)->lpBuffer = lpdev;
		lpdev->dwState = devstatus.dwState;

		LPSTR   lpszPhysicalName = nullptr;
		LPSTR	lpszWorkstationName = nullptr;
		WFMAllocateMore(devstatus.lpszPhysicalName.length() + 1, (*lpresult), (void**)&lpszPhysicalName);
		if (lpszPhysicalName) {
			lpdev->lpszPhysicalName = lpszPhysicalName;
			memcpy(lpdev->lpszPhysicalName, devstatus.lpszPhysicalName.c_str(), devstatus.lpszPhysicalName.length());
		}
		WFMAllocateMore(devstatus.lpszWorkstationName.length() + 1, (*lpresult), (void**)&lpszWorkstationName);
		if (lpszWorkstationName) {
			lpdev->lpszWorkstationName = lpszWorkstationName;
			memcpy(lpdev->lpszWorkstationName, devstatus.lpszWorkstationName.c_str(), devstatus.lpszWorkstationName.length());
		}
	}
	
	return lpdev;
}
int CanServiceInfo_Admi::send_events(an_xfs_result * cmd, DWORD eventclass) {
	std::lock_guard<std::mutex> lock(mtx_);

	int r = 0;
	HRESULT hr = WFS_SUCCESS;

	for (auto &ref : service_) {
		if ((ref.get()->dwEventClass_ & eventclass)) {
			LPWFSRESULT lpResult = nullptr;
			hr = WFMAllocateBuffer(sizeof(WFSRESULT), WFS_MEM_SHARE | WFS_MEM_ZEROINIT, (void**)&lpResult);
			if (WFS_SUCCESS == hr) {
				lpResult->hResult = cmd->hResult;
				lpResult->hService = ref.get()->hService_;
				lpResult->RequestID = cmd->RequestID;
				memcpy(&lpResult->tsTimestamp, &cmd->tsTimestamp, sizeof(lpResult->tsTimestamp));
				lpResult->u.dwEventID = cmd->u.dwEventID;
				lpResult->lpBuffer = nullptr;

				
				//调用事件处理器//性能相当switch...case
				auto f = event_handler_table_.find(cmd->u.dwEventID);
				if (f != event_handler_table_.end()) {
					f->second(cmd->lpBuffer, &lpResult);
				}
				
				/*//
				switch (cmd->u.dwEventID) {
				case WFS_SYSE_DEVICE_STATUS:
					system_device_status(cmd->lpBuffer, &lpResult);
					break;
				case WFS_SRVE_CIM_ITEMSTAKEN:
					service_cim_itemtaken(cmd->lpBuffer, &lpResult);
				default:
					break;
				}
				*/
				
			}

			//
			::PostMessage(ref.get()->hWndReg_, cmd->dwMsg, 0, reinterpret_cast<LPARAM>(lpResult));
		}
	}
	return r;
}

void CanServiceInfo_Admi::register_event_handler() {
	//WFS_SYSE_DEVICE_STATUS事件
	event_handler_table_.insert(EVENT_HANDLER_MAP::value_type(WFS_SYSE_DEVICE_STATUS, \
		[this](const std::string &buffer, LPWFSRESULT * lpresult)->void * \
	{return system_device_status(std::cref(buffer), lpresult); }));

	//WFS_SRVE_CIM_ITEMSTAKEN事件
	event_handler_table_.insert(EVENT_HANDLER_MAP::value_type(WFS_SRVE_CIM_ITEMSTAKEN, \
		[this](const std::string &buffer, LPWFSRESULT * lpresult)->void * \
	{return service_cim_itemtaken(std::cref(buffer), lpresult); }));

	/*//性能销差于 insert
	event_handler_table_[WFS_SYSE_DEVICE_STATUS] = \
		[this](const std::string &buffer, LPWFSRESULT * lpresult)->void * {return system_device_status(buffer, lpresult); };
	event_handler_table_[WFS_SRVE_CIM_ITEMSTAKEN] = \
		[this](const std::string &buffer, LPWFSRESULT * lpresult)->void * {return service_cim_itemtaken(buffer, lpresult); };
	*/
}