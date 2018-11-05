#pragma once
#include "anCRS_utility.h"
#include "../anCIM_sp/message_pack.h"
#include "build_cmd.h"
#include "CanXfs.h"
#include "an_xfscim.h"


class CExecuter : public xfs_cmd_queue, public CanThread
{
public:
	CExecuter(const CExecuter& a) = delete;
	CExecuter& operator=(const CExecuter& a) = delete;
	//CExecuter(CExecuter&& a);
	//CExecuter& operator=(CExecuter&& a);

	explicit CExecuter(uv_loop_t *loop, fn_completed_cb cb);
	CExecuter() = delete;
	~CExecuter();

	virtual int run(void *arg = nullptr);
	virtual int idle(void *arg = nullptr);
	virtual int start();
	virtual void stop();

	inline void register_xfs_proxy(CanXfs * proxy) {
		m_xfs_proxy = proxy;
	}

	friend class CanIPCServer;
private:
	int cmd_pre_processed(an_buf  &cmd);
	int cmd_end_processed(an_buf &cmd);
private:
	CanEventHandler m_evnet_handler;	//xfs exectute/event completed 消息发送代理
	CanXfs * m_xfs_proxy;	//xfs 命令处理代理
	an_buf m_current_cmd;				//当前执行命令
};

