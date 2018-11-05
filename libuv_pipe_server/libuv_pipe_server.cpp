// libuv_pipe_server.cpp : 定义控制台应用程序的入口点。
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
#include <memory>
#include <vector>

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

#ifdef _WIN32
#define PIPENAME "\\\\?\\pipe\\anrong_wn.echo.sock"
#else
#define PIPENAME "/tmp/anrong_wn.echo.sock"
#endif
uv_loop_t * loop = NULL;


/*ctr+c handler*/
BOOL WINAPI  ctrHandler(DWORD key)
{
	BOOL ret = FALSE;
	switch (key) {
	case CTRL_C_EVENT:
		cout << "====User press : ctrl+c, exit loop." << endl;

		uv_fs_t req;
		uv_fs_unlink(loop, &req, PIPENAME, NULL);

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

typedef struct {
	uv_write_t req;
	uv_buf_t buf;
}write_req_t;

void free_write_req(uv_write_t * req) {
	write_req_t * wr = (write_req_t*)req;
	free(wr->buf.base);
	free(wr);
}

void alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf) {
	buf->base = (char *)malloc(suggested_size);
	buf->len = suggested_size;
}

void echo_write(uv_write_t * req, int status) {
	if (status < 0) {
		fprintf(stderr, "Write error %s\n", uv_err_name(status));
	}

	free_write_req(req);
}

void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
	if (nread > 0) {
		write_req_t * req = (write_req_t*)malloc(sizeof(write_req_t));
		req->buf = uv_buf_init(buf->base, nread);

		char * data = (char *)malloc(buf->len + 1);
		data[buf->len] = 0x00;
		memcpy(data, buf->base, buf->len);

		fprintf(stderr, "recive data: %s\n", data);
		free(data);

		uv_write((uv_write_t *)req, client, &req->buf, 1, echo_write);
		return;
	}

	if (nread < 0) {
		if (nread != UV_EOF)
			fprintf(stderr, "Read error %s\n", uv_err_name(nread));
		uv_close((uv_handle_t*)client, NULL);
	}
	free(buf->base);
}


void on_new_connect(uv_stream_t * server, int status) {
	if (status == -1) {
		fprintf(stderr, "connect error %s\n", uv_err_name(status));
		return;
	}

	
	uv_pipe_t * client = (uv_pipe_t *)malloc(sizeof(uv_pipe_t));
	uv_pipe_init(loop, client, 0);
	int r = 0;
	r = uv_accept(server, (uv_stream_t*)client);
	
	if (0 == r) {
		char buf[1024] = { 0x00 };
		size_t len = 1024;
		r = uv_pipe_getpeername(client, buf, &len);
		fprintf(stderr, "getpeername pipename: %s\n", buf);

		r = uv_read_start((uv_stream_t*)client, alloc_buffer, echo_read);
	}
	else {
		fprintf(stderr, "accept error %s\n", uv_err_name(r));
		uv_close((uv_handle_t*)client, NULL);
	}
}
int main()
{
	cout << "===libuv_pipe_server start...===" << endl;
	BOOL b = SetConsoleCtrlHandler(ctrHandler, TRUE);

	loop = uv_default_loop();

	uv_pipe_t server;
	uv_pipe_init(loop, &server, 0);

	int r = 0;
	r = uv_pipe_bind(&server, PIPENAME);
	if (r) {
		fprintf(stderr, "Bind error %s\n", uv_err_name(r));
		return 1;
	}

	r = uv_listen((uv_stream_t*)&server, 128, on_new_connect);
	if (r) {
		fprintf(stderr, "Listen error %s\n", uv_err_name(r));
		return 2;
	}

    return uv_run(loop, UV_RUN_DEFAULT);
}

