#include "stdafx.h"
#include "Executer.h"
#include <functional>
#include <random>

/*
CExecuter::CExecuter() : xfs_cmd_queue(), m_main_loop(nullptr) {

}
*/

CExecuter::CExecuter(uv_loop_t *loop, fn_completed_cb cb) : xfs_cmd_queue(), CanThread(){
	m_xfs_proxy = nullptr;
	m_evnet_handler.init(loop, cb);
}

CExecuter::~CExecuter(){
	
}

/*
CExecuter::CExecuter(CExecuter&& a) {
	if (this != &a) {
		this->m_main_loop = a.m_main_loop;
		a.m_main_loop = nullptr;
		this->m_cmd_queue = a.m_cmd_queue;

	}
}
*/

int CExecuter::start() {
	int r = 0;
	//启动自身线程
	r = CanThread::start();

	//启动item_pass_check 线程
	this->m_xfs_proxy->start_item_check(&this->m_evnet_handler);

	return r;
}

void CExecuter::stop() {
	this->m_xfs_proxy->stop_item_check();

	CanThread::stop();
}
int CExecuter::cmd_end_processed(an_buf &cmd) {
	int r = 0;

	if (!cmd.u.m_cmd) return 0;

	

	return r;
}
int CExecuter::cmd_pre_processed(an_buf  &cmd) {
	int r = 0;

	if (!cmd.u.m_cmd) return 0;

	if (e_an_WFPCancelAsyncRequest == cmd.u.m_cmd->eType) {
		std::function<void(std::deque<an_buf>::reference)> fn;
		if (0 == cmd.u.m_cmd->ReqID) {
			//取消所有
			auto f1 = [&](std::deque<an_buf>::reference it) {
				if (e_an_WFPCancelAsyncRequest != it.u.m_cmd->eType) {
					if (cmd.u.m_cmd->hService == it.u.m_cmd->hService) {
						it.u.m_cmd->eStatus = e_an_cmd_cancelled;
					}
				}
			};

			fn = std::bind(f1, std::placeholders::_1);
		}
		else {
			//取消指定reqid
			auto f2 = [&](std::deque<an_buf>::reference it) {
				if (e_an_WFPCancelAsyncRequest != it.u.m_cmd->eType) {
					if ((cmd.u.m_cmd->hService == it.u.m_cmd->hService)&&\
						(cmd.u.m_cmd->ReqID==it.u.m_cmd->ReqID)) {
						it.u.m_cmd->eStatus = e_an_cmd_cancelled;
					}
				}
			};
			fn = std::bind(f2, std::placeholders::_1);
		}

		std::for_each(this->m_cmd_deque.begin(), this->m_cmd_deque.end(), fn);
	}
	
	return r;
}
int CExecuter::run(void *arg) {
	int r = 0;

	size_t len = get_size_cmd_deque();
	if (0 == len) {
		return r;
	}
	else {
		m_current_cmd = get_front_cmd_deque();

		//是否为取消命令处理
		r = cmd_pre_processed(m_current_cmd);

		//命令处理
		r = this->m_xfs_proxy->cmd_handler(&m_current_cmd, &this->m_evnet_handler);
		
		//
		//r = cmd_end_processed(m_current_cmd);

		//清除命令队列
		destroy_cmd(m_current_cmd.u.m_cmd);
		pop_front_cmd_deque();
		
	}

	return r;
}
int CExecuter::idle(void *arg) {
	int r = 0;

	/*//测试发送状态改变事件*/
	an_wfs_devstatus devStatus;
	devStatus.lpszPhysicalName = "anCRS SP";
	devStatus.lpszWorkstationName = "anCRS";

	//测试用
	std::default_random_engine random(time(nullptr));
	std::uniform_int_distribution<unsigned> u(1, 10);

	if (0 == (u(random) % 2)) {
		devStatus.dwState = WFS_STAT_DEVOFFLINE;
	}
	else {
		devStatus.dwState = WFS_STAT_DEVONLINE;
	}

	std::stringstream os = serialize_result<an_wfs_devstatus>(devStatus);
	/*
	{
		cereal::BinaryOutputArchive bi_ar(os);
		bi_ar(devStatus);
	}
	*/

	an_xfs_result result;
	result.dwMsg = WFS_SYSTEM_EVENT;
	result.u.dwEventID = WFS_SYSE_DEVICE_STATUS;
	result.hResult = WFS_SUCCESS;
	result.hService = 0;
	result.RequestID = 0;
	result.hWnd = 0;
	result.lpBuffer = os.str();

	os = serialize_result<an_xfs_result>(result);
	/*
	os.str("");
	{
		cereal::BinaryOutputArchive bi_ar(os);
		bi_ar(result);
	}
	*/

	//生成发送的buffer
	void * lpresult = nullptr;
	size_t len = os.str().length();
	lpresult = CanAllotter::an_malloc(len);
	memcpy(lpresult, os.str().c_str(), len);

	an_notice_result * notice = an_notice_result::malloc_an_notice_result(lpresult, len, nullptr);
	this->m_evnet_handler(notice);

	return r;

}