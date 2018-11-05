#pragma once
#include "anCRS_utility.h"
#include <string>
#include <memory>
#include <vector>
#include "build_cmd.h"
#include "CanXfs.h"

class CanIPCServer
{
public:
	CanIPCServer() = delete;
	~CanIPCServer();
	int run();
	int stop();
	void erase_client_vector(void * client);

	CanIPCServer(const CanIPCServer&) = delete;
	CanIPCServer& operator=(const CanIPCServer&) = delete;

	int notify_exit_server(void *arg);
public:
	static CanIPCServer* get_instance(const char * strServerName);
	static void on_new_connection(uv_stream_t* server, int status);
	static void alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf);
	static void on_write(uv_write_t * req, int status);
	static void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
	static void on_notify(uv_async_t* handle);
	static void on_close(uv_handle_t* handle);

private:
	explicit CanIPCServer(const char * strServerName);
	static int send_notify(uv_stream_t* client, an_notice_result * lpresult);
	static int broadcast_notify(an_notice_result * lpresult);

private:
	static CanIPCServer* s_pInstance;
	static std::vector<std::unique_ptr<uv_pipe_t>> s_client_vector;

	std::string m_strServerName;
	uv_loop_t * m_loop;
	uv_pipe_t	m_handle;
	//uv_mutex_t m_mtx

	
	std::unique_ptr<CanXfs> m_xfs_proxy; //xfs 命令处理代理
	CanEventHandler m_evnet_handler;	//xfs getinfo completed 消息发送代理
	class CExecuter * m_cmd_executer;	//命令队列
};

