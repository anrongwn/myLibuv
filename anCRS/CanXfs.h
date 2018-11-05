#pragma once
#include <XFSAPI.H>
#include <XFSCIM.H>
#include <XFSCDM.H>
//#include "../anCIM_sp/message_pack.h"
//#include "../libuv/include/uv.h"
#include "build_cmd.h"
#include <array>
#include <memory>
#include <vector>
#include <functional>
#include "an_Commander.h"
#include "an_xfs_cu_info.h"
#include "item_pass_check.h"

typedef std::function<int(void *)> an_exit_observesr;
class CanEventHandler;
class CanXfs
{
public:
	CanXfs(const CanXfs&) = delete;
	CanXfs& operator=(const CanXfs&) = delete;

	CanXfs() = delete;
	explicit CanXfs(an_exit_observesr obs);
	~CanXfs();

	HRESULT cmd_handler(an_buf * arg, CanEventHandler * handler);
	HRESULT spi_getinfo(an_buf * arg, CanEventHandler * handler);
	void start_item_check(CanEventHandler * handler);
	void stop_item_check();


	size_t push_service(HSERVICE service);
	size_t remove_service(HSERVICE service);
	/*
protected:
	HRESULT spi_cancel(an_WFPCancelAsyncRequest * arg, CanEventHandler * handler);
	HRESULT spi_execute(an_WFPExecute *arg, CanEventHandler * handler);
	HRESULT spi_close(an_WFPClose *arg, CanEventHandler * handler);
	HRESULT spi_deregister(an_WFPDeregister *arg, CanEventHandler * handler);
	HRESULT spi_lock(an_WFPLock *arg, CanEventHandler * handler);
	HRESULT spi_unlock(an_WFPUnlock *arg, CanEventHandler * handler);
	HRESULT spi_open(an_WFPOpen *arg, CanEventHandler * handler);
	HRESULT spi_register(an_WFPRegister *arg, CanEventHandler * handler);
	HRESULT spi_settracelevel(an_WFPSetTraceLevel *arg, CanEventHandler * handler);
	*/
private:
	static void item_pass_check(void *arg, void * handler);

	void warkup_item_check(int type, HSERVICE service, REQUESTID reqid);
	void pause_item_check(int type, HSERVICE service, REQUESTID reqid);

private:
	std::array<std::unique_ptr<an_Commander>, e_an_WFP> _commander;
	an_xfs_caps _caps;
	an_xfs_status _status;
	an_xfs_cu_info _cuinfo;
	an_xfs_dev _dev;

	CItem_pass_check m_item_pass_check;	//钞票inserted或taken 事件检测线程

	std::vector<HSERVICE> service_table_;

	an_exit_observesr obs_;//暂不用
};

