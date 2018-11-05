#include "stdafx.h"

#include "item_pass_check.h"

CItem_pass_check::CItem_pass_check() : an_semaphore(), \
m_stop_flag(false) {

}
CItem_pass_check::~CItem_pass_check() {
	join();
}

void CItem_pass_check::run(CItem_pass_check *arg, fn_item_pass_check fn, void * fn_arg) {
	while (!arg->m_stop_flag) {
		arg->wait();	//等有信号后执行

						//退出
		if (true == arg->m_stop_flag) {
			break;
		}

		//执行
		if (fn) {
			fn(arg, fn_arg);
		}
	}

	::OutputDebugStringA("---CItem_pass_check::run() exit.\n");
}

void CItem_pass_check::start(fn_item_pass_check cb, void *arg) {

	m_thread = std::thread(CItem_pass_check::run, this, cb, arg);
}

inline void CItem_pass_check::stop() {
	m_stop_flag = true;

	//
	signal(1,1);
}

void CItem_pass_check::join() {
	stop();
	if (m_thread.joinable()) {
		m_thread.join();
	}
}
