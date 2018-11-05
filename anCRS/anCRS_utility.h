#pragma once
#include "stdafx.h"
#include "../libuv/include/uv.h"
#include <thread>
#include <atomic>
#include <sstream>
#include "an_xfs_result.h"
#include "anAllotter.h"

//�Զ���
class CanLock {
public:
	explicit CanLock(uv_mutex_t * handle) : proxy(handle) {
		uv_mutex_lock(proxy);
	}
	~CanLock() {
		uv_mutex_unlock(proxy);
	}

	CanLock() = delete;
	CanLock(const CanLock&) = delete;
	CanLock& operator=(const CanLock&) = delete;
private:
	uv_mutex_t * proxy;
};
#define ANLOCK(h)	CanLock lock(&h);

//�߳���
class CanThread {
public:
	CanThread(const CanThread&) = delete;
	CanThread& operator=(const CanThread&) = delete;

	CanThread() : m_stop_flag(false), m_handle(nullptr), m_idle_interval(30000){
		
	}
	~CanThread() {
		/*
		stop();

		join();
		*/
	}

	unsigned long set_idle_interval(const unsigned long interval) {
		m_idle_interval = interval;
		return m_idle_interval;
	}

	virtual int start() {
		return uv_thread_create(&m_handle, CanThread::thread_cb, this);
	}
	virtual void stop() {
		m_stop_flag = true;

		
	}
	int join() {
		int r = 0;
		if (nullptr!=m_handle) {
			r = uv_thread_join(&m_handle);
			m_stop_flag = false;
		}
		return r;
	}

	

	/*//run��������ֵҪ��
	// 0--��ʾû������ִ��
	// 1--��ʾ����ִ�гɹ�
	// ����
	*/
	virtual int run(void *arg = nullptr) {
		return 0;
	}
	virtual int idle(void *arg = nullptr) {
		return 0;
	}
	virtual int idle_handle(const int action) {
		static unsigned long s_begin = ::GetTickCount();
		int r = 0;

		//���¿�ʼʱ��
		if (1 == action) {
			s_begin = ::GetTickCount();
		}
		else {
			unsigned long current = ::GetTickCount();
			if ((current - s_begin) >= m_idle_interval) {

				//ִ��idle
				r = idle(nullptr);

				//���¼���
				s_begin = ::GetTickCount();
			}

			//��ֹCPUռ�ù���
			::Sleep(1);
			
		}
		
		return r;
	}


private:
	static void thread_cb(void *arg) {
		CanThread * pThis = reinterpret_cast<CanThread*>(arg);
		int r = 0;

		while (!pThis->m_stop_flag) {
			
			//ִ������
			r = pThis->run();

			//�������:ֻ��run()����0ʱ������г���m_idle_interval��ִ��idle
			r = pThis->idle_handle(r);
		}

		::OutputDebugStringA("---CanThread::thread_cb() exit.\n");
	}


private:
	std::atomic<bool> m_stop_flag;
	uv_thread_t m_handle;

	unsigned long m_idle_interval;
};

//
//�¼�������¼� ֪ͨ�ص�
typedef void(*fn_completed_cb)(uv_async_t*);
class CanEventHandler {
public:
	CanEventHandler() : m_loop(nullptr), m_completed_cb(nullptr){
	}

	virtual ~CanEventHandler() {

	}

	virtual int init(uv_loop_t* loop, fn_completed_cb cb) {
		m_loop = loop;
		m_completed_cb = cb;

		return 0;
	}

	int operator()(an_notice_result *result) {
		return send_completed_notice(result);
	}

	virtual int send_completed_notice(an_notice_result *result) {
		int r = 0;
		std::stringstream logdata;
		logdata << "CanEventHandler::send_completed_notice(result=" << std::hex<<(void*)result<<")--";
		LOGD(logdata.str());

		if ((m_loop)) {
			uv_async_t * async = (uv_async_t*)CanAllotter::an_malloc(sizeof(uv_async_t));
			async->data = result;
			
			r = uv_async_init(this->m_loop, async, this->m_completed_cb);
			r = uv_async_send(async);
			if (r) {
				CanAllotter::an_free(result->_buf.base);
				an_notice_result::free_an_notice_result(result);
				CanAllotter::an_free(async);

				logdata << " uv_async_send()failed.error=" << uv_strerror(r);
				
			}
			else {
				logdata << " uv_async_send(" << std::hex << (void*)async <<")";
			}
		}

		logdata << "  completed.";
		LOGD(logdata.str());
		
		return r;
	}

private:
	uv_loop_t * m_loop;
	fn_completed_cb m_completed_cb;	//����¼����¼��ص�
};