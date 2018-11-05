#pragma once
#include <mutex>
#include <condition_variable>
#include <functional>

class an_semaphore {
public:
	an_semaphore(const an_semaphore&) = delete;
	an_semaphore& operator=(const an_semaphore&) = delete;

	//Ä¬ÈÏÎÞÐÅºÅ
	/*
	an_semaphore() : count{ 0 }, wakeups{ 0 }{
	}
	*/
	an_semaphore() : _cur_reqid{ 0 }, _cur_service{ 0 } {
	}
	
	~an_semaphore() {

	}


	void signal(USHORT service, ULONG reqid) {
		std::lock_guard<std::mutex>lock{ mutex };

		_cur_service = service;
		_cur_reqid = reqid;

		if ((_cur_reqid) && (_cur_service)) {
			condition.notify_one();// notify one !
		}
		
		/*
		if (++count <= 0) {// have some thread suspended ?
			++wakeups;
			condition.notify_one();// notify one !
		}
		*/
		
		
	}

	void reset() {
		std::lock_guard<std::mutex>lock{ mutex };

		_cur_service = 0;
		_cur_reqid = 0;
	}

	
protected:
	std::mutex mutex;

	void wait() {
		std::unique_lock<std::mutex>lock{ mutex };

		/*
		if (--count<0) {// count is not enough ?
			condition.wait(lock, [&]()->bool {return wakeups>0; });// suspend and wait ...
			--wakeups;// ok, me wakeup !
		}
		*/

		if (_cur_service==0) {// count is not enough ?
			condition.wait(lock, [&]()->bool {return _cur_reqid>0; });// suspend and wait ...
		}
	}

private:
	std::condition_variable condition;
	//volatile	int count;
	//volatile	int wakeups;
public:
	volatile	ULONG       _cur_reqid;
	volatile	USHORT      _cur_service;

};