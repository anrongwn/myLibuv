// libuv_pipe_client.cpp : 定义控制台应用程序的入口点。
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

#ifdef _WIN32
#define PIPENAME "\\\\?\\pipe\\anrong_wn.echo.sock"
#else
#define PIPENAME "/tmp/anrong_wn.echo.sock"
#endif

uv_loop_t * loop = NULL;
uv_tty_t tty_stdin, tty_stdout;
uv_pipe_t server;
uv_connect_t conn;


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

void write_to_stdout_cb(uv_write_t * req, int status) {
	if (status) {
		fprintf(stderr, "write_to_stdout error %s\n", uv_strerror(status));
		exit(0);
	}
	free_write_req(req);
}

void read_from_pipe_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	write_req_t *wri = (write_req_t *)malloc(sizeof(write_req_t));
	wri->buf = uv_buf_init(buf->base, nread);

	int r = uv_write((uv_write_t*)wri, (uv_stream_t*)&tty_stdout, &wri->buf, 1, write_to_stdout_cb);
	if (r) {
		fprintf(stderr, "echo write_to_stdout error %s\n", uv_strerror(r));
	}

}

void write_to_pipe_cb(uv_write_t* req, int status) {
	if (status) {
		fprintf(stderr, "Write error %s\n", uv_strerror(status));
		exit(0);
	}
	int r = 0;

	/*
	r = uv_read_start((uv_stream_t*)&server, alloc_buffer, read_from_pipe_cb);//再一次构造缓冲区
	if (r) {
		fprintf(stderr, "uv_read_start error %s\n", uv_strerror(r));
	}
	*/

	free_write_req(req);//释放动态分配的所有数据
}

void read_from_input_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	write_req_t *wri = (write_req_t *)malloc(sizeof(write_req_t));//实例化新结构，主要2个作用：产生write所需的uv_write_t;临时存储buf（同时提供给回调函数析构方法）便于该数据的及时free
	wri->buf = uv_buf_init(buf->base, nread);//buf复制

	int r = uv_write((uv_write_t*)wri, (uv_stream_t*)&server, &wri->buf, 1, write_to_pipe_cb);//需要注意的是write调用的时候&wri->buf必须依然有效！所以这里直接用buf会出现问题！
	if (r) {
		fprintf(stderr, "uv_write error %s\n", uv_strerror(r));
	}
	//write完成之后当前缓冲区也就失去了意义，于是向回调函数传递buf指针，并由回调函数负责析构该缓冲区
}

uv_process_t child_req;
uv_process_options_t options;
void connect_pipe_server();

void on_connect(uv_connect_t* req, int status) {
	int r = 0;

	if (status) {
		char* args[3];
		args[0] = "D:\\MyTest\\2018_C++\\myLibuv\\Debug\\libuv_pipe_server.exe";
		args[1] = NULL;
		args[2] = NULL;
		options.exit_cb = NULL;
		options.cwd = "D:\\MyTest\\2018_C++\\myLibuv\\Debug\\";
		options.args = args;
		options.file= "D:\\MyTest\\2018_C++\\myLibuv\\Debug\\libuv_pipe_server.exe";
		options.flags = UV_PROCESS_DETACHED;

		r = uv_spawn(loop, &child_req, &options);
		if(r) {
			fprintf(stderr, "%s\n", uv_strerror(r));
			exit(0);
		}
		uv_unref((uv_handle_t *)&child_req);

		connect_pipe_server();


		/*fprintf(stderr, "connect pipe error %s\n", uv_strerror(status));
		exit(0);
		*/
	}
	else if(status==0)
		r = uv_read_start((uv_stream_t*)req->handle, alloc_buffer, read_from_pipe_cb);
}

static int s_test = 0;

void connect_pipe_server() {
	conn.data = &s_test;

	uv_pipe_connect(&conn, &server, PIPENAME, on_connect);
}

int main()
{
	cout << "===libuv_pipe_client start...===" << endl;
	BOOL b = SetConsoleCtrlHandler(ctrHandler, TRUE);

	loop = uv_default_loop();

	uv_pipe_init(loop, &server, 0);
	uv_tty_init(loop, &tty_stdin, 0, 1);
	uv_tty_init(loop, &tty_stdout, 1, 0);

	int r = 0;
	connect_pipe_server();


	r = uv_read_start((uv_stream_t*)&tty_stdin, alloc_buffer, read_from_input_cb);
	if (r) {
		fprintf(stderr, "server uv_read_start error %s\n", uv_strerror(r));
	}


	return uv_run(loop, UV_RUN_DEFAULT);
}

