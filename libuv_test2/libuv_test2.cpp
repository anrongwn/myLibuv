// libuv_test2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <malloc.h>
#include <winsock2.h>
#include <time.h>
#include "..//libuv//include//uv.h"
#include <iostream>

using namespace std;

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"IPHLPAPI.lib")
#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"Userenv.lib")
#if _DEBUG
#pragma comment(lib, "..//Debug//libuv.lib")
#else
#pragma comment(lib, "..//Release//libuv.lib")
#endif


#define TEST_PORT 50001
uv_loop_t * loop;
uv_tcp_t client;
uv_connect_t req;


/*ctr+c handler*/
BOOL WINAPI  ctrHandler(DWORD key)
{
	BOOL ret = FALSE;
	switch (key) {
	case CTRL_C_EVENT:
		cout << "====User press : ctrl+c, exit loop." << endl;
		uv_stop(loop);
		//Sleep(2000);
		break;
	case CTRL_BREAK_EVENT:
		break;
	case CTRL_CLOSE_EVENT:
		cout << "====User close cmd, exit loop." << endl;
		uv_stop(loop);
		break;
	case CTRL_LOGOFF_EVENT:
		break;
	case CTRL_SHUTDOWN_EVENT:
		break;
	default:
		break;
	}

	return ret;
}
void echo_alloc(uv_handle_t* handle,
	size_t suggested_size,
	uv_buf_t* buf)
{
	buf->base = (char *)malloc(suggested_size);
	buf->len = suggested_size;
}

void on_read(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf)
{
	printf("==on_read uv_read data len:%d\n", nread);
	if (nread)
	{
		char * data = (char *)malloc(nread + 1);
		memset(data, 0x00, nread + 1);

		memcpy(data, buf->base, nread);

		printf("==on_read echo_server data :%s\n", data);

		free(data);
		free(buf->base);
	}
	
	//
	uv_close((uv_handle_t *)handle, NULL);

	uv_stop(loop);

}


void on_write(uv_write_t *req, int status)
{
	printf("==on_write uv_write status:%d\n", status);
	
	if (status >= 0)
	{
		int r = uv_read_start(req->handle, echo_alloc, on_read);
		if (r) {
			fprintf(stderr, "uv_read_start error %s\n", uv_strerror(r));
		}

	}
	else if (-1 == status)
	{
		printf("==on_write uv_write status:%s\n", uv_strerror(status));
	}

	free(req);
}

char *raw = "Hello,wangjr--libuv_test2";
void on_connect(uv_connect_t* req, int status)
{
	printf("===connect status :%lu.\n", status);

	if (-1 == status) return;

	//
	uv_write_t *wr = (uv_write_t *)malloc(sizeof(uv_write_t));
	uv_buf_t buf = uv_buf_init(raw, strlen(raw));

	int r = uv_write(wr, req->handle, &buf, 1, on_write);
	
}

int main()
{
	cout << "===libuv_test2 start...===" << endl;
	BOOL b = SetConsoleCtrlHandler(ctrHandler, TRUE);

	loop = uv_default_loop();

	
	uv_tcp_init(loop, &client);
	struct sockaddr_in dest;
	uv_ip4_addr("0.0.0.0", TEST_PORT, &dest);

	uv_tcp_connect(&req, &client, (const struct sockaddr *)&dest, on_connect);

	uv_run(loop, UV_RUN_DEFAULT);
	//uv_close((uv_handle_t *)&client, NULL);

	uv_loop_close(loop);

	system("pause");

    return 0;
}

