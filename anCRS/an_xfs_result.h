#pragma once
//#include <XFSAPI.H>
#include <string>
#include <sstream>
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
//#include <cereal\archives\json.hpp>
#include <cereal\types\array.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>
//#include <cereal\types\list.hpp>
//#include <cereal\types\map.hpp>
#include "../libuv/include/uv.h"
#include "anAllotter.h"

#pragma pack(push, 1)
typedef ULONG REQUESTID;
typedef LONG HRESULT;
typedef USHORT HSERVICE;

struct _an_systemtime : public SYSTEMTIME {
	_an_systemtime() = default;
	~_an_systemtime() = default;

	template<typename Archive>
	void serialize(Archive & ar) {
		ar(wYear, wMonth, wDayOfWeek, wDay, wHour, wMinute, wSecond, wMilliseconds);
	}
};

struct _an_wfs_result {
	REQUESTID       RequestID;
	HSERVICE        hService;
	_an_systemtime  tsTimestamp;
	HRESULT         hResult;
	union {
		DWORD       dwCommandCode;
		DWORD       dwEventID;
	} u;
	std::string     lpBuffer;
	unsigned long	hWnd;	//完成消息窗口
	DWORD			dwMsg;	// Message-No

	_an_wfs_result() {
		RequestID = 0;
		hService = 0;
		::GetLocalTime(&tsTimestamp);
		hResult = 0;
		u.dwCommandCode = 0;
		hWnd = 0;
		dwMsg = 0;
	}
	~_an_wfs_result() = default;
	
	template<typename Archive>
	void serialize(Archive & ar) {
		ar(RequestID, hService, tsTimestamp, hResult, \
			u.dwCommandCode, lpBuffer, hWnd, dwMsg);
	}
};


class an_xfs_result : public _an_wfs_result
{
public:
	an_xfs_result() :_an_wfs_result() {
	}
	~an_xfs_result() {

	}

	an_xfs_result(const an_xfs_result&) = delete;
	an_xfs_result& operator=(const an_xfs_result&) = delete;
};


//用于包装 完成消息、事件的 uv_async_t->data
struct an_notice_result {
	uv_buf_t	_buf;
	uv_stream_t	* _client;//通信会话
	bool _notify_exit_flag;	//通知IPC服务退出标志，只有最后一个 wfsclose完成消息，才会设置

	an_notice_result():_client(nullptr),_notify_exit_flag(false){
		_buf = uv_buf_init(nullptr, 0);
	}
	an_notice_result(void *lpxfs, size_t len, uv_stream_t	*client)\
		: _client(client), _notify_exit_flag(false) {
		_buf = uv_buf_init((char*)lpxfs, len);
	}
	bool set_notify_exit_flag(bool b) {
		_notify_exit_flag = b;
		return _notify_exit_flag;
	}
	~an_notice_result() = default;
	an_notice_result(const an_notice_result&) = default;
	an_notice_result& operator=(const an_notice_result&) = default;

	static an_notice_result * malloc_an_notice_result(void *lpxfs, size_t len, uv_stream_t	*client) {
		return (new an_notice_result(lpxfs, len, client));
	}

	static an_notice_result * malloc_an_notice_result() {
		return (new an_notice_result());
	}

	static void free_an_notice_result(an_notice_result * buffer) {
		delete buffer;
		buffer = nullptr;
	}

	static an_notice_result* clone(an_notice_result *source) {
		an_notice_result * dst = an_notice_result::malloc_an_notice_result();
		if ((dst)&&(source)) {
			void * tmp = CanAllotter::an_malloc(source->_buf.len);
			memcpy(tmp, source->_buf.base, source->_buf.len);
			dst->_client = nullptr;
			dst->_buf = uv_buf_init((char*)tmp, source->_buf.len);
			dst->_notify_exit_flag = source->_notify_exit_flag;
		}

		return dst;
	}

	static void destroy(an_notice_result **source) {
		if (*source) {
			CanAllotter::an_free((*source)->_buf.base);
			an_notice_result::free_an_notice_result((*source));
			(*source) = nullptr;
		}
	}

};

//反序列化返回结果
template<class R>
inline void unserialize_result(const std::string &buffer, R &result) {
	std::stringstream is(buffer);
	{
		cereal::BinaryInputArchive ar(is);
		ar(result);
	}
}

#pragma pack(pop)