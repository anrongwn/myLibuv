#pragma once
#include <stdlib.h>
#include <memory.h>
#include "../libuv/include/uv.h"
#include <mutex>
#include <condition_variable>

#pragma pack(push, 1)

typedef USHORT HSERVICE;

//消息处理回调
typedef int(*an_result_handle)(void * lpxfs);

/*
//内存分配器
class CanAllotter {
public:
	static void * an_malloc(size_t size) {
		return malloc(size);
	}

	static void an_free(void *buf) {
		free(buf);
	}
};
*/

/*
//自动锁
class CanLock {
public:
	explicit CanLock(uv_mutex_t * handle) : proxy(handle) {
		uv_mutex_lock(proxy);
	}
	~CanLock() {
		uv_mutex_unlock(proxy);
	}
private:
	CanLock();
	CanLock(const CanLock&);
	CanLock& operator=(const CanLock&);
	
	uv_mutex_t * proxy;
};
#define ANLOCK(h)	CanLock lock(&h);
*/

//重边次数
#define AN_RECONNECT_COUNT	(1000)

class an_waitconnect_semaphore {
public:
	an_waitconnect_semaphore(const an_waitconnect_semaphore&) = delete;
	an_waitconnect_semaphore& operator=(const an_waitconnect_semaphore&) = delete;

	//默认无信号
	an_waitconnect_semaphore() : count{ 0 }, status{ 0 } {
	}
	/*
	//默认无信号
	explicit an_waitconnect_semaphore(int value = 0) : count{ value }, status{ 0 } {
	}
	*/
	~an_waitconnect_semaphore() {

	}

	void reset() {
		count = 0;
		status = 0;
	}

	int get_count() {
		return count;
	}

	int get_status() {
		return status;
	}

	void signal_once() {
		std::lock_guard<std::mutex>lock{ mutex };
		if (++count >= AN_RECONNECT_COUNT) {// 超过两次
			status = 2;
			condition.notify_one();// notify one !
		}
	}

	void signal() {
		std::lock_guard<std::mutex>lock{ mutex };
		count = 1;
		status = 1;
		condition.notify_one();// notify one !
	}

	void wait() {
		std::unique_lock<std::mutex>lock{ mutex };
		if (count < AN_RECONNECT_COUNT) {//少于两次
			condition.wait(lock, [&]()->bool {return status>0; });// suspend and wait ...
			//--status;// ok, me wakeup !

		}
	}


private:
	std::mutex mutex;
	std::condition_variable condition;
	volatile int count;	//连接次数，不得超过2次
	volatile int status; //连接状态，0--未能连接，1--连接成功，2--连接超时（失败）
};

#pragma pack(pop)