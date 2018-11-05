// anpc.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "anpc.h"

#include "anUtility.h"
#include "CanIPC.h"
#include "anpc_error.h"
#include "../anCRS/anAllotter.h"

#define AN_SERVER_NAME_PREFIX	"\\\\?\\pipe\\{11DC2369-6C30-4378-9D8C-D3B10747D20D}"
//#define AN_SERVER_NAME_PREFIX	"\\\\?\\pipe\\anrong_wn.echo.sock"

std::mutex g_mutex;

//ipc 实例
CanIPC * g_pipc = nullptr;// CanIPC::get_instance(AN_SERVER_NAME_PREFIX);

void * WINAPI anpc_malloc(size_t size) {
	std::unique_lock<std::mutex> lock(g_mutex);

	return CanAllotter::an_malloc(size);
}

void WINAPI anpc_free(void * buf) {
	std::unique_lock<std::mutex> lock(g_mutex);
	int r = ANPC_AN_SUCCESS;
	CanAllotter::an_free(buf);
}

int WINAPI	anpc_create_ipc(unsigned short service, const char * strlogicname, an_result_handle cb) {
	std::unique_lock<std::mutex> lock(g_mutex);
	int r = ANPC_AN_SUCCESS;

	if (nullptr == g_pipc)
		g_pipc = CanIPC::get_instance(AN_SERVER_NAME_PREFIX);

	string logicname(strlogicname);
	g_pipc->register_result_callback(cb);
	//g_pipc->insert_service_cb(logicname, cb);
	g_pipc->insert_service_name(service, logicname);
	

	r = g_pipc->run();

	return r;
}

int WINAPI	anpc_send_cmd(unsigned short service, void * cmd, size_t len) {
	std::unique_lock<std::mutex> lock(g_mutex);
	int r = ANPC_AN_SUCCESS;

	if (g_pipc) {
		r = g_pipc->sendcmd(service, cmd, len);
	}

	return r;
}

int WINAPI	anpc_close_ipc(unsigned short service) {
	std::unique_lock<std::mutex> lock(g_mutex);
	int r = ANPC_AN_SUCCESS;

	if (g_pipc) {
		r = g_pipc->close(service);
	}


	return r;
}

int WINAPI	anpc_stop(bool bforce) {
	std::unique_lock<std::mutex> lock(g_mutex);
	int r = ANPC_AN_SUCCESS;

	if (g_pipc) {
		r = g_pipc->stop(bforce);
		//g_pipc = nullptr;
	}

	return r;
}

