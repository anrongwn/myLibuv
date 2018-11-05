#pragma once

#include "../anCIM_sp/message_pack.h"
#include "../libuv/include/uv.h"
#include <sstream>
#include <array>
#include <deque>
#include <memory>
#include <mutex>

struct an_buf {
	union {
		an_WFP_header * m_cmd;
		void * _lpxfs;
	}u;
	
	uv_stream_t * m_client;//通信会话

	an_buf() {
		this->u.m_cmd = nullptr;
		m_client = nullptr;
	}

	an_buf(uv_stream_t * client, an_WFP_header * ptr) {
		u.m_cmd = ptr;
		m_client = client;
	}
	an_buf(uv_stream_t * client, void * ptr) {
		u._lpxfs = ptr;
		m_client = client;
	}

	~an_buf() {
		//do nothing
	}

	static an_buf * malloc_an_buf() {
		return (new an_buf());
	}

	static void free_an_buf(an_buf * buffer) {
		delete buffer;
		buffer = nullptr;
	}

	static an_buf * malloc_an_buf(uv_stream_t	*client, void *lpxfs) {
		return (new an_buf(client, lpxfs));
	}
	
	an_buf(const an_buf& a) {
		u.m_cmd = a.u.m_cmd;
		m_client = a.m_client;
	}

	an_buf& operator=(const an_buf& a) {
		if (this != &a) {
			u.m_cmd = a.u.m_cmd;
			m_client = a.m_client;
		}
		return (*this);
	}
	

	an_buf(an_buf&& a) noexcept {
		u.m_cmd = a.u.m_cmd;
		m_client = a.m_client;

		a.u.m_cmd = nullptr;
		a.m_client = nullptr;
	}

	an_buf& operator=(an_buf&&a) noexcept {
		if (this != &a) {
			u.m_cmd = a.u.m_cmd;
			m_client = a.m_client;

			a.u.m_cmd = nullptr;
			m_client = nullptr;
		}
		return (*this);
	}
};

class xfs_cmd_queue {
public:
	xfs_cmd_queue() {

	}

	~xfs_cmd_queue() {

	}

	xfs_cmd_queue(const xfs_cmd_queue&) = delete;
	xfs_cmd_queue& operator=(const xfs_cmd_queue&) = delete;

	/*
	xfs_cmd_queue(xfs_cmd_queue&& a){
		m_cmd_deque = a.m_cmd_deque;
	}

	xfs_cmd_queue& operator=(xfs_cmd_queue&& a) {
		if (this != &a) {
			m_cmd_deque = a.m_cmd_deque;
		}
		return (*this);
	}
	*/


	void push_back_cmd_deque(const an_buf& a) {
		std::unique_lock<std::mutex> lock(m_mtx);

		m_cmd_deque.push_back(a);

	}
	void push_back_cmd_deque(an_buf&& a) {
		std::unique_lock<std::mutex> lock(m_mtx);

		m_cmd_deque.push_back(std::forward<an_buf>(a));
	}

	an_buf& get_front_cmd_deque() {
		std::unique_lock<std::mutex> lock(m_mtx);

		return m_cmd_deque.front();
	}

	void pop_front_cmd_deque() {
		std::unique_lock<std::mutex> lock(m_mtx);

		m_cmd_deque.pop_front();
	}

	size_t get_size_cmd_deque() {
		std::unique_lock<std::mutex> lock(m_mtx);

		return m_cmd_deque.size();
	}
protected:
	std::mutex m_mtx;
	std::deque<an_buf> m_cmd_deque;
};

typedef an_WFP_header * (*build_cmd_fn)(std::stringstream &);

template<typename T>
inline T * build_cmd_impl(std::stringstream &is) {
	T * obj = new T;
	cereal::BinaryInputArchive ar_body(is);
	ar_body(*obj);
	return obj;
}

inline void destroy_cmd(an_WFP_header*cmd) {
	delete cmd;
}

/*
template<typename T>
inline T * destroy_cmd(T *obj) {
	delete obj;
	obj = nullptr;
	return obj;
}
*/
static an_WFP_header * build_cmd_an_WFP_header(std::stringstream &is) {
	return build_cmd_impl<an_WFP_header>(is);
}
static an_WFP_header * build_cmd_an_WFPCancelAsyncRequest(std::stringstream &is) {
	return build_cmd_impl<an_WFPCancelAsyncRequest>(is);
}
static an_WFP_header * build_cmd_an_WFPClose(std::stringstream &is) {
	return build_cmd_impl<an_WFPClose>(is);
}
static an_WFP_header * build_cmd_an_WFPDeregister(std::stringstream &is) {
	return build_cmd_impl<an_WFPDeregister>(is);
}
static an_WFP_header * build_cmd_an_WFPExecute(std::stringstream &is) {
	return build_cmd_impl<an_WFPExecute>(is);
}
static an_WFP_header * build_cmd_an_WFPGetInfo(std::stringstream &is) {
	return build_cmd_impl<an_WFPGetInfo>(is);
}
static an_WFP_header * build_cmd_an_WFPLock(std::stringstream &is) {
	return build_cmd_impl<an_WFPLock>(is);
}
static an_WFP_header * build_cmd_an_WFPOpen(std::stringstream &is) {
	return build_cmd_impl<an_WFPOpen>(is);
}
static an_WFP_header * build_cmd_an_WFPRegister(std::stringstream &is) {
	return build_cmd_impl<an_WFPRegister>(is);
}
static an_WFP_header * build_cmd_an_WFPSetTraceLevel(std::stringstream &is) {
	return build_cmd_impl<an_WFPSetTraceLevel>(is);
}

static an_WFP_header * build_cmd_an_WFPUnloadService(std::stringstream &is) {
	return build_cmd_impl<an_WFP_header>(is);
}

static an_WFP_header * build_cmd_an_WFPUnlock(std::stringstream &is) {
	return build_cmd_impl<an_WFPUnlock>(is);
}

static std::array<build_cmd_fn, e_an_WFP> g_cmd_builder = { \
build_cmd_an_WFP_header,\
build_cmd_an_WFPCancelAsyncRequest, \
build_cmd_an_WFPClose,\
build_cmd_an_WFPDeregister ,\
build_cmd_an_WFPExecute ,\
build_cmd_an_WFPGetInfo ,\
build_cmd_an_WFPLock ,\
build_cmd_an_WFPOpen ,\
build_cmd_an_WFPRegister,\
build_cmd_an_WFPSetTraceLevel ,\
build_cmd_an_WFPUnloadService ,\
build_cmd_an_WFPUnlock };


//序列化命令结果
template<class R>
inline std::stringstream serialize_result(R &result) {
	std::stringstream os;
	{
		cereal::BinaryOutputArchive bi_ar(os);
		bi_ar(result);
	}

	return os;	//c++11，充分使用了右值，move构造及赋值
}
