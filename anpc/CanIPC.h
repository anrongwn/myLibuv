#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <thread>
#include <mutex>
#include "anUtility.h"
#include "CanContext.h"

using namespace std;

#pragma pack(push, 1)

typedef enum { anpc_cmd_req = 0, anpc_cmd_completed, anpc_event, }anpc_data_type;

//async cmd pack
typedef struct {
	anpc_data_type type;
	HSERVICE service;
	void * data;
	int len;
	void * handle;
}an_cmd_t, *p_an_cmd_t;

//result
typedef struct
{
	anpc_data_type	type;
	unsigned short	service;
	void *			lpxfs;
} anpc_result, *p_anpc_result;



typedef struct {
	uv_write_t uv_req;
	uv_buf_t buf;
}an_write_t, *p_an_write_t;

class CanIPC : public CanContext
{
public:
	~CanIPC();

	static CanIPC * get_instance(const char * strServiceName);
	
	int run(void *arg = 0);
	int close(HSERVICE service);
	int stop(bool bforce=false);
	int sendcmd(HSERVICE service, void * data, size_t len);
	inline an_result_handle register_result_callback(an_result_handle cb) {
		m_result_cb = cb;
		return m_result_cb;
	}
	
	//static uv callback function
	static void on_send_cmd(uv_async_t * handle);
	static void on_connect(uv_connect_t * req, int status);
	static void on_write(uv_write_t * req, int status);
	static void on_read(uv_stream_t * stream, ssize_t nread, const uv_buf_t * buf);
	static void on_close(uv_handle_t* handle);

	//read buffer allocer
	static void alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf);
	
	//work thread
	static void worker(void * arg);
private:
	explicit CanIPC(const char * strServiceName);
	CanIPC();
	CanIPC(const CanIPC&);
	CanIPC& operator= (const CanIPC&);

	int init();
	int connect();
	int write(void * data, size_t len);
	//bool isExist(HSERVICE service);

private:
	uv_loop_t m_loop;
	uv_pipe_t m_pipe;
	uv_connect_t m_connect;
	std::thread m_thread;
	uv_process_t m_sp_process_req;
	uv_process_options_t m_sp_process_options;

	string m_szServerName;

	static CanIPC * s_pInstance;
	an_waitconnect_semaphore m_wait_connect;
	an_result_handle m_result_cb;
};

#pragma pack(pop)
