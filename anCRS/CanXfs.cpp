#include "stdafx.h"
#include "CanXfs.h"
#include "anCRS_utility.h"
#include <sstream>
#include "an_wfpopen_impl.h"
#include "an_wfpregister_impl.h"
#include "an_wfpexecute_impl.h"
//#include "Executer.h"
#include "an_xfscim.h"


#pragma comment(lib, "msxfs.lib")
#pragma comment(lib, "xfs_supp.lib")

CanXfs::CanXfs(an_exit_observesr obs) : obs_(obs)
{
	_commander[e_an_WFPCancelAsyncRequest]= std::move(std::make_unique<an_wfpopen_impl>(&_caps, &_status, &_cuinfo, &_dev, \
		[this](HSERVICE h)->size_t {return push_service(h); }));
	_commander[e_an_WFPClose] = std::move(std::make_unique<an_wfpclose_impl>(&_caps, &_status, &_cuinfo, &_dev, \
		[this](HSERVICE h)->size_t {return remove_service(h); }));
	_commander[e_an_WFPDeregister] = std::move(std::make_unique<an_wfpderegister_impl>());
	_commander[e_an_WFPExecute] = std::move(std::make_unique<an_wfpexecute_impl>(&_caps, &_status, &_cuinfo, &_dev));
	_commander[e_an_WFPLock] = std::move(std::make_unique<an_wfpopen_impl>(&_caps, &_status, &_cuinfo, &_dev, \
		[this](HSERVICE h)->size_t {return push_service(h); }));
	_commander[e_an_WFPOpen] = std::move(std::make_unique<an_wfpopen_impl>(&_caps, &_status, &_cuinfo, &_dev, \
		[this](HSERVICE h)->size_t {return push_service(h); }));
	_commander[e_an_WFPRegister] = std::move(std::make_unique<an_wfpregister_impl>());
	_commander[e_an_WFPSetTraceLevel] = std::move(std::make_unique<an_wfpopen_impl>(&_caps, &_status, &_cuinfo, &_dev, \
		[this](HSERVICE h)->size_t {return push_service(h); }));
	_commander[e_an_WFPUnlock] = std::move(std::make_unique<an_wfpopen_impl>(&_caps, &_status, &_cuinfo, &_dev, \
		[this](HSERVICE h)->size_t {return push_service(h); }));
	
}


CanXfs::~CanXfs()
{
}

size_t CanXfs::push_service(HSERVICE service) {
	this->service_table_.push_back(service);
	return 1;
	//return this->service_table_.size();
}
size_t CanXfs::remove_service(HSERVICE service) {
	auto it = std::find(service_table_.begin(), service_table_.end(), service);
	if (it != service_table_.end()) {
		service_table_.erase(it);
	}

	return service_table_.size();
}

HRESULT CanXfs::spi_getinfo(an_buf *arg, CanEventHandler * handler) {
	HRESULT hr = WFS_SUCCESS;
	std::stringstream os, logdata;
	logdata << "CanXfs::spi_getinfo(hService=" << arg->u.m_cmd->hService << ", ReqID=" << arg->u.m_cmd->ReqID \
		<< ", Type=" << e_an_wfp_type_name[arg->u.m_cmd->eType] << ")";
	LOGD(logdata.str());

	an_xfs_result result;
	an_WFPGetInfo * lpgetinfo = reinterpret_cast<an_WFPGetInfo *>(arg->u.m_cmd);
	if (lpgetinfo) {
		result.hResult = WFS_SUCCESS;
		result.hService = lpgetinfo->hService;
		result.RequestID = lpgetinfo->ReqID;
		result.hWnd = lpgetinfo->hWnd;
		result.dwMsg = WFS_GETINFO_COMPLETE;
		result.u.dwCommandCode = lpgetinfo->dwCategory;
		//result.lpBuffer = "";

		std::stringstream os;
		switch (lpgetinfo->dwCategory) {
		case WFS_INF_CIM_CAPABILITIES:
		{
			os = serialize_result<an_xfs_caps>(this->_caps);
			/*
			{
				cereal::BinaryOutputArchive bi_ar(os);
				bi_ar(this->_caps);
			}
			*/

			result.lpBuffer = os.str();
			break;
		}
		case WFS_INF_CIM_STATUS:
		{
			os = serialize_result<an_xfs_status>(this->_status);
			/*
			{
				cereal::BinaryOutputArchive bi_ar(os);
				bi_ar(this->_status);
			}
			*/

			result.lpBuffer = os.str();
			break;
		}
		default:
			result.hResult = WFS_ERR_UNSUPP_CATEGORY;
			break;
		}
	}

	os = serialize_result<an_xfs_result>(result);
	/*
	os.str("");
	{
		cereal::BinaryOutputArchive bi_ar(os);
		bi_ar(result);
	}
	*/

	if (handler) {
		void * lpresult = nullptr;
		size_t len = os.str().length();
		lpresult = CanAllotter::an_malloc(len);
		memcpy(lpresult, os.str().c_str(), len);

		an_notice_result * cmd = an_notice_result::malloc_an_notice_result(lpresult, len, arg->m_client);

		handler->send_completed_notice(cmd);
	}

	logdata << " completed.";
	LOGD(logdata.str());

	return hr;
}
/*
HRESULT CanXfs::spi_execute(an_WFPExecute *arg, CanEventHandler * handler) {
	HRESULT hr = WFS_SUCCESS;

	return hr;
}

HRESULT CanXfs::spi_cancel(an_WFPCancelAsyncRequest *arg, CanEventHandler * handler) {
	HRESULT hr = WFS_SUCCESS;

	return hr;
}
HRESULT CanXfs::spi_close(an_WFPClose *arg, CanEventHandler * handler) {
	HRESULT hr = WFS_SUCCESS;

	return hr;
}
HRESULT CanXfs::spi_deregister(an_WFPDeregister *arg, CanEventHandler * handler) {
	HRESULT hr = WFS_SUCCESS;

	return hr;
}
HRESULT CanXfs::spi_lock(an_WFPLock *arg, CanEventHandler * handler) {
	HRESULT hr = WFS_SUCCESS;

	return hr;
}
HRESULT CanXfs::spi_unlock(an_WFPUnlock *arg, CanEventHandler * handler) {
	HRESULT hr = WFS_SUCCESS;

	return hr;
}
HRESULT CanXfs::spi_open(an_WFPOpen *arg, CanEventHandler * handler) {
	HRESULT hr = WFS_SUCCESS;

	return hr;
}
HRESULT CanXfs::spi_register(an_WFPRegister *arg, CanEventHandler * handler) {
	HRESULT hr = WFS_SUCCESS;

	return hr;
}
HRESULT CanXfs::spi_settracelevel(an_WFPSetTraceLevel *arg, CanEventHandler * handler) {
	HRESULT hr = WFS_SUCCESS;

	return hr;
}
*/

void CanXfs::item_pass_check(void *arg, void * handler) {
	CItem_pass_check * pItem = reinterpret_cast<CItem_pass_check*>(arg);

	//判断设备状态
	//

	/*//测试发送状态改变事件*/
	an_wfs_cim_position_info position;
	std::stringstream os = serialize_result<an_wfs_cim_position_info>(position);

	an_xfs_result result;
	result.dwMsg = WFS_SERVICE_EVENT;
	result.u.dwEventID = WFS_SRVE_CIM_ITEMSTAKEN;
	result.hResult = WFS_SUCCESS;
	result.hService = pItem->_cur_service;
	result.RequestID = pItem->_cur_reqid;
	result.hWnd = 0;
	result.lpBuffer = os.str();

	os = serialize_result<an_xfs_result>(result);

	//生成发送的buffer
	void * lpresult = nullptr;
	size_t len = os.str().length();
	lpresult = CanAllotter::an_malloc(len);
	memcpy(lpresult, os.str().c_str(), len);

	an_notice_result * notice = an_notice_result::malloc_an_notice_result(lpresult, len, nullptr);
	if (handler) {
		(static_cast<CanEventHandler*>(handler))->send_completed_notice(notice);
	}
	
	//测试
	::Sleep(1000);
}
void CanXfs::start_item_check(CanEventHandler * handler) {
	this->m_item_pass_check.start(CanXfs::item_pass_check, handler);
}

void CanXfs::stop_item_check() {
	this->m_item_pass_check.stop();
}

void CanXfs::warkup_item_check(int type, HSERVICE service, REQUESTID reqid) {
	if (e_an_WFPExecute == type) {
		this->m_item_pass_check.signal(service, reqid);
	}
}
void CanXfs::pause_item_check(int type, HSERVICE service, REQUESTID reqid) {
	if (e_an_WFPExecute == type) {
		::Sleep(5000);//测试用模拟硬件动作

		this->m_item_pass_check.reset();
	}
}

HRESULT CanXfs::cmd_handler(an_buf * arg, CanEventHandler * handler) {
	HRESULT hr = WFS_SUCCESS;
	std::stringstream logdata;
	logdata << "CanXfs::cmd_handler(hService=" << arg->u.m_cmd->hService << ", ReqID=" << arg->u.m_cmd->ReqID \
		<< ", Type=" << e_an_wfp_type_name[arg->u.m_cmd->eType] << ")";
	LOGD(logdata.str());

	//唤醒item check线程
	warkup_item_check(arg->u.m_cmd->eType, arg->u.m_cmd->hService, arg->u.m_cmd->ReqID);

	an_xfs_result result;
	hr = _commander[arg->u.m_cmd->eType]->execute(arg->u.m_cmd, &result);
	std::stringstream os = serialize_result<an_xfs_result>(result);
	
	if (handler) {
		void * lpresult = nullptr;
		size_t len = os.str().length();
		lpresult = CanAllotter::an_malloc(len);
		memcpy(lpresult, os.str().c_str(), len);

		an_notice_result * cmd = an_notice_result::malloc_an_notice_result(lpresult, len, arg->m_client);
		
		//最后一个wfpclose 执行完成，设置退出标志
		if (1 == _commander[arg->u.m_cmd->eType]->get_flag()) {
			cmd->set_notify_exit_flag(true);
		}

		handler->send_completed_notice(cmd);
	}

	//挂起item check线程
	pause_item_check(arg->u.m_cmd->eType, arg->u.m_cmd->hService, arg->u.m_cmd->ReqID);

	return hr;
}
