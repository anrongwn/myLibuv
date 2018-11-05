#include "stdafx.h"
#include "xfs_handler.h"

#include <XFSADMIN.H>
#include <XFSAPI.H>
#include <XFSCDM.H>
#include <XFSCIM.H>
#include "../anCRS/an_xfs_result.h"
#include "../anCRS/an_xfscim.h"
#include "../anCRS/an_xfs_status.h"

/*
//反序列化返回结果
template<class R>
inline void unserialize_result(const std::string &buffer, R &result) {
	std::stringstream is(buffer);
	{
		cereal::BinaryInputArchive ar(is);
		ar(result);
	}
}
*/

//getinfo
static int getinfo_handler(const an_xfs_result * cmd, LPWFSRESULT *lpResult) {
	int r = 0;
	//std::stringstream is;
	HRESULT hr = WFS_SUCCESS;
	
	switch (cmd->u.dwCommandCode) {
	case WFS_INF_CIM_CAPABILITIES:
	{
		an_xfs_caps caps;
		//caps.wMaxCashInItems = 300;
		unserialize_result<an_xfs_caps>(cmd->lpBuffer, caps);

		/*
		std::stringstream is(cmd->lpBuffer);
		{
			cereal::BinaryInputArchive ar(is);
			ar(caps);
		}
		*/

		LPWFSCIMCAPS lpcaps = nullptr;
		hr = WFMAllocateMore(sizeof(WFSCIMCAPS), *lpResult, (void**)&lpcaps);
		if (lpcaps) {
			(*lpResult)->lpBuffer = lpcaps;

			lpcaps->wClass = caps.wClass;
			lpcaps->fwType = caps.fwType;
			lpcaps->wMaxCashInItems = caps.wMaxCashInItems;
			lpcaps->bCompound = caps.bCompound;
			lpcaps->bShutter = caps.bShutter;
			lpcaps->bShutterControl = caps.bShutterControl;
			lpcaps->bSafeDoor = caps.bSafeDoor;
			lpcaps->bCashBox = caps.bCashBox;
			lpcaps->bRefill = caps.bRefill;
			lpcaps->fwIntermediateStacker = caps.fwIntermediateStacker;
			lpcaps->bItemsTakenSensor = caps.bItemsTakenSensor;
			lpcaps->bItemsInsertedSensor = caps.bItemsInsertedSensor;
			lpcaps->fwPositions = caps.fwPositions;
			lpcaps->fwExchangeType = caps.fwExchangeType;
			lpcaps->fwRetractAreas = caps.fwRetractAreas;
			lpcaps->fwRetractTransportActions = caps.fwRetractTransportActions;
			lpcaps->fwRetractStackerActions = caps.fwRetractStackerActions;
			//lpszExtra;
			memcpy(lpcaps->dwGuidLights, caps.dwGuidLights, sizeof(lpcaps->dwGuidLights));
			lpcaps->dwItemInfoTypes = caps.dwItemInfoTypes;
			lpcaps->bCompareSignatures = caps.bCompareSignatures;
			lpcaps->bPowerSaveControl = caps.bPowerSaveControl;
			lpcaps->bReplenish = caps.bReplenish;
			lpcaps->fwCashInLimit = caps.fwCashInLimit;
			lpcaps->fwCountActions = caps.fwCountActions;
			lpcaps->bDeviceLockControl = caps.bDeviceLockControl;
			lpcaps->wMixedMode = caps.wMixedMode;
			lpcaps->bMixedDepositAndRollback = caps.bMixedDepositAndRollback;
			lpcaps->bAntiFraudModule = caps.bAntiFraudModule;
			lpcaps->bDeplete = caps.bDeplete;
			lpcaps->bBlacklist = caps.bBlacklist;
			lpcaps->lpdwSynchronizableCommands = 0;

		}

		hr = WFMAllocateMore(caps.lpszExtra.length(), *lpResult, (void**)&lpcaps->lpszExtra);
		if (lpcaps->lpszExtra) {
			memcpy(lpcaps->lpszExtra, caps.lpszExtra.c_str(), caps.lpszExtra.length());
		}

		break;
	}
	case WFS_INF_CIM_STATUS:
		an_xfs_status status;
		unserialize_result<an_xfs_status>(cmd->lpBuffer, status);
		/*
		std::stringstream is(cmd->lpBuffer);
		{
			cereal::BinaryInputArchive ar(is);
			ar(status);
		}
		*/

		LPWFSCIMSTATUS lpstatus = nullptr;
		hr = WFMAllocateMore(sizeof(WFSCIMSTATUS), *lpResult, (void**)&lpstatus);
		if (lpstatus) {
			(*lpResult)->lpBuffer = lpstatus;

			lpstatus->bDropBox = status.bDropBox;
			memcpy(lpstatus->dwGuidLights, status.dwGuidLights, sizeof(status.dwGuidLights));
			lpstatus->fwAcceptor = status.fwAcceptor;
			lpstatus->fwBanknoteReader = status.fwBanknoteReader;
			lpstatus->fwDevice = status.fwDevice;
			lpstatus->fwIntermediateStacker = status.fwIntermediateStacker;
			lpstatus->fwSafeDoor = status.fwSafeDoor;
			lpstatus->usPowerSaveRecoveryTime = status.usPowerSaveRecoveryTime;
			lpstatus->wAntiFraudModule = status.wAntiFraudModule;
			lpstatus->wDevicePosition = status.wDevicePosition;
			lpstatus->wMixedMode = status.wMixedMode;
			lpstatus->fwStackerItems = status.fwStackerItems;
			
		}
		hr = WFMAllocateMore(status.lpszExtra.length(), *lpResult, (void**)&lpstatus->lpszExtra);
		if (lpstatus->lpszExtra) {
			memcpy(lpstatus->lpszExtra, status.lpszExtra.c_str(), status.lpszExtra.length());
		}

		hr = WFMAllocateMore(sizeof(LPWFSCIMINPOS)*(AN_POSITION_SIZE+1), *lpResult, (void**)&lpstatus->lppPositions);
		if (lpstatus->lppPositions) {
			for (int i = 0; i < AN_POSITION_SIZE; ++i) {
				WFMAllocateMore(sizeof(WFSCIMINPOS), *lpResult, (void**)&(lpstatus->lppPositions[i]));
				if (lpstatus->lppPositions[i]) {
					memcpy(lpstatus->lppPositions[i], &status.lppPositions[i], sizeof(status.lppPositions[i]));
				}
			}
		}

		break;
	}

	return r;
}

//发送命令完成消息
static int send_completed_message(an_xfs_result * cmd) {
	int r = 0;
	LPWFSRESULT lpResult = nullptr;
	HRESULT hr = WFMAllocateBuffer(sizeof(WFSRESULT), WFS_MEM_SHARE | WFS_MEM_ZEROINIT, (void**)&lpResult);
	if (WFS_SUCCESS == hr) {
		lpResult->hResult = cmd->hResult;
		lpResult->hService = cmd->hService;
		lpResult->RequestID = cmd->RequestID;
		memcpy(&lpResult->tsTimestamp, &cmd->tsTimestamp, sizeof(lpResult->tsTimestamp));
		lpResult->u.dwCommandCode = cmd->u.dwCommandCode;
		lpResult->lpBuffer = nullptr;

		size_t  buflen = cmd->lpBuffer.length();
		if (buflen) {
			switch (cmd->dwMsg) {
			case WFS_GETINFO_COMPLETE:
				r = getinfo_handler(cmd, &lpResult);
				break;
			case WFS_EXECUTE_COMPLETE:
				break;
			default:
				break;
			}
		}
	}

	//
	::PostMessage((HWND)cmd->hWnd, cmd->dwMsg, 0, reinterpret_cast<LPARAM>(lpResult));

	//删除注册信息
	if (WFS_CLOSE_COMPLETE == cmd->u.dwCommandCode) {
		g_service_info.close_service(cmd->hService);
	}

	return r;
}

//发送system_events\service_events 事件
static int send_events(an_xfs_result * cmd, DWORD eventclass) {
	return g_service_info.send_events(cmd, eventclass);
}

//消息处理总泵
int xfs_result_handler(void * arg) {
	int r = 0;
	an_xfs_result * cmd = reinterpret_cast<an_xfs_result*>(arg);
	int eventclass = an_get_xfsmessage_class(cmd->dwMsg);

	switch (eventclass) {
	case 0:
		r = send_completed_message(cmd);
		break;
	case SERVICE_EVENTS:
	case SYSTEM_EVENTS:
		r = send_events(cmd, eventclass);
		break;
	case EXECUTE_EVENTS:
		break;
	
	case USER_EVENTS:
		break;
	}

	/*
	LPWFSRESULT lpResult = nullptr;
	HRESULT hr = WFMAllocateBuffer(sizeof(WFSRESULT), WFS_MEM_SHARE | WFS_MEM_ZEROINIT, (void**)&lpResult);
	if (WFS_SUCCESS == hr) {
		lpResult->hResult = cmd->hResult;
		lpResult->hService = cmd->hService;
		lpResult->RequestID = cmd->RequestID;
		memcpy(&lpResult->tsTimestamp, &cmd->tsTimestamp, sizeof(lpResult->tsTimestamp));
		lpResult->u.dwCommandCode = cmd->u.dwCommandCode;
		lpResult->lpBuffer = nullptr;

		size_t  buflen = cmd->lpBuffer.length();
		if (buflen) {
			//测试用
			if (cmd->dwMsg == WFS_SYSTEM_EVENT) {
				

				an_wfs_devstatus devstatus;
				std::stringstream is(cmd->lpBuffer);
				cereal::BinaryInputArchive ar(is);
				ar(devstatus);

				LPWFSDEVSTATUS lpdev = nullptr;
				hr = WFMAllocateMore(sizeof(WFSDEVSTATUS), lpResult, (void**)&lpdev);
				if (lpdev) {
					lpResult->lpBuffer = lpdev;
					lpdev->dwState = devstatus.dwState;

					LPSTR   lpszPhysicalName = nullptr;
					LPSTR	lpszWorkstationName = nullptr;
					WFMAllocateMore(devstatus.lpszPhysicalName.length(), lpResult, (void**)&lpszPhysicalName);
					if (lpszPhysicalName) {
						lpdev->lpszPhysicalName = lpszPhysicalName;
						memcpy(lpdev->lpszPhysicalName, devstatus.lpszPhysicalName.c_str(), devstatus.lpszPhysicalName.length());
					}
					WFMAllocateMore(devstatus.lpszWorkstationName.length(), lpResult, (void**)&lpszWorkstationName);
					if (lpszWorkstationName) {
						lpdev->lpszWorkstationName = lpszWorkstationName;
						memcpy(lpdev->lpszWorkstationName, devstatus.lpszWorkstationName.c_str(), devstatus.lpszWorkstationName.length());
					}
				}
			}
		}
	}

	//
	::PostMessage((HWND)cmd->hWnd, cmd->dwMsg, 0, reinterpret_cast<LPARAM>(lpResult));
	*/

	return r;
}