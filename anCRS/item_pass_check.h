#pragma once
#include <thread>
#include <atomic>
#include "an_sem.h"

typedef void(*fn_item_pass_check)(void *arg, void *handler);
class CItem_pass_check : public an_semaphore {
public:
	CItem_pass_check(const CItem_pass_check&) = delete;
	CItem_pass_check& operator=(const CItem_pass_check&) = delete;

	CItem_pass_check();
	~CItem_pass_check();
	void start(fn_item_pass_check cb, void *arg);
	void stop();
	void join();


	static void run(CItem_pass_check *arg, fn_item_pass_check fn, void * fn_arg);
private:
	std::thread m_thread;
	std::atomic<bool> m_stop_flag;
};

