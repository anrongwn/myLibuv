// libuv_test.cpp : 定义控制台应用程序的入口点。
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
/* Die with fatal error. */
#define FATAL(msg)                                        \
  do {                                                    \
    fprintf(stderr,                                       \
            "Fatal error in %s on line %d: %s\n",         \
            __FILE__,                                     \
            __LINE__,                                     \
            msg);                                         \
    fflush(stderr);                                       \
    abort();                                              \
  } while (0)

/* Have our own assert, so we are sure it does not get optimized away in
* a release build.
*/
#define ASSERT(expr)                                      \
 do {                                                     \
  if (!(expr)) {                                          \
    fprintf(stderr,                                       \
            "Assertion failed in %s on line %d: %s\n",    \
            __FILE__,                                     \
            __LINE__,                                     \
            #expr);                                       \
    abort();                                              \
  }                                                       \
 } while (0)

/*//tcp echo 服务*/
typedef enum {
	TCP = 0,
	UDP,
	PIPE
} stream_type;

typedef struct {
	uv_write_t req;
	uv_buf_t buf;
}write_req_t;

static uv_loop_t * loop;
static int server_closed;
static stream_type serverType;
static uv_tcp_t tcpServer;
static uv_handle_t * server;


static void after_write(uv_write_t* req, int status);
static void after_read(uv_stream_t*, ssize_t nread, const uv_buf_t* buf);
static void on_close(uv_handle_t* peer);
static void on_server_close(uv_handle_t* handle);
static void on_connection(uv_stream_t*, int status);

static void after_write(uv_write_t * req, int status)
{
	write_req_t * wr = (write_req_t*)req;
	free(wr->buf.base);
	free(wr);

	if (0 == status)
	{
		return;
	}

	cout << "uv_write error:" << uv_err_name(status) << ", " \
		<< uv_strerror(status);

}
static void on_close(uv_handle_t* peer) {
	free(peer);
}

static void after_shutdown(uv_shutdown_t* req, int status) {
	uv_close((uv_handle_t*)req->handle, on_close);
	free(req);
}

static void after_read(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf)
{
	int i;
	write_req_t *wr;
	uv_shutdown_t* sreq;

	if (nread < 0) {
		
		//Error or EOF 
		ASSERT(nread == UV_EOF);

		uv_close((uv_handle_t*)handle, NULL);
		free(buf->base);
		sreq = (uv_shutdown_t*)malloc(sizeof* sreq);
		ASSERT(0 == uv_shutdown(sreq, handle, after_shutdown));
		return;
		
	}

	if (nread == 0) {
		/* Everything OK, but nothing read. */
		free(buf->base);
		return;
	}

	/*
	* Scan for the letter Q which signals that we should quit the server.
	* If we get QS it means close the stream.
	*/
	if (!server_closed) {
		for (i = 0; i < nread; i++) {
			if (buf->base[i] == 'Q') {
				if (i + 1 < nread && buf->base[i + 1] == 'S') {
					free(buf->base);
					uv_close((uv_handle_t*)handle, on_close);
					return;
				}
				else {
					uv_close(server, on_server_close);
					server_closed = 1;
				}
			}
		}
	}

	wr = (write_req_t*)malloc(sizeof *wr);
	ASSERT(wr != NULL);
	wr->buf = uv_buf_init(buf->base, nread);

	if (uv_write(&wr->req, handle, &wr->buf, 1, after_write)) {
		FATAL("uv_write failed");
	}
}


static void echo_alloc(uv_handle_t* handle,
	size_t suggested_size,
	uv_buf_t* buf) 
{
	buf->base = (char *)malloc(suggested_size);
	buf->len = suggested_size;
}


static void on_connection(uv_stream_t* server, int status) {
	uv_stream_t* stream=0;
	int r=0;
	struct sockaddr peername;
	int namelen = 0;

	if (status != 0) {
		fprintf(stderr, "Connect error %s\n", uv_err_name(status));
	}
	ASSERT(status == 0);

	switch (serverType) {
	case TCP:
		stream = (uv_stream_t*)malloc(sizeof(uv_tcp_t));
		ASSERT(stream != NULL);
		r = uv_tcp_init(loop, (uv_tcp_t*)stream);
		ASSERT(r == 0);
		break;

	case PIPE:
		stream = (uv_stream_t*)malloc(sizeof(uv_pipe_t));
		ASSERT(stream != NULL);
		r = uv_pipe_init(loop, (uv_pipe_t*)stream, 0);
		ASSERT(r == 0);
		break;

	default:
		ASSERT(0 && "Bad serverType");
		abort();
	}

	/* associate server with stream */
	stream->data = server;

	r = uv_accept(server, stream);
	ASSERT(r == 0);

	//
	namelen = sizeof peername;
	sockaddr_in sin;
	r = uv_tcp_getpeername((uv_tcp_t*)stream, &peername, &namelen);
	if (0 == r)
	{
		
		char sender[17] = { 0 };
		uv_ip4_name((const struct sockaddr_in *) &peername, sender, 16);
		memcpy(&sin, &peername, sizeof(sin));
		
		fprintf(stdout, "Accept  %s : %d connection.\n", sender, sin.sin_port);
		
	}

	r = uv_read_start(stream, echo_alloc, after_read);
	ASSERT(r == 0);
}


static void on_server_close(uv_handle_t* handle) {
	ASSERT(handle == server);
}


static int tcp4_echo_start(int port) {
	struct sockaddr_in addr;
	int r;

	ASSERT(0 == uv_ip4_addr("0.0.0.0", port, &addr));

	server = (uv_handle_t*)&tcpServer;
	serverType = TCP;

	r = uv_tcp_init(loop, &tcpServer);
	uv_tcp_nodelay(&tcpServer, 1);

	if (r) {
		/* TODO: Error codes */
		fprintf(stderr, "Socket creation error\n");
		return 1;
	}

	r = uv_tcp_bind(&tcpServer, (const struct sockaddr*) &addr, 0);
	if (r) {
		/* TODO: Error codes */
		fprintf(stderr, "Bind error\n");
		return 1;
	}

	r = uv_listen((uv_stream_t*)&tcpServer, SOMAXCONN, on_connection);
	if (r) {
		/* TODO: Error codes */
		fprintf(stderr, "Listen error %s\n", uv_err_name(r));
		return 1;
	}

	return 0;
}
///////////////////////////////////////////////////////////////

//uv_queue_work
#define FIB_UNTIL 10
uv_work_t req[FIB_UNTIL];
long fib_(long t) {
	if ((0 == t) || (1 == t)) {
		return 1;
	}
	else {
		return fib_(t - 1) + fib_(t - 2);
	}
}

void fib(uv_work_t * req) {
	int n = *(int *)req->data;
	uv_thread_t id = uv_thread_self();
	printf("thread id:%lu.\n", id);

	fprintf(stderr, "%dth fibonacci\n", n);
	int i = 0;
	srand(time(NULL));

	if (rand() % 2)
	{
		i = 5;
		Sleep(5);
	}
	else
	{
		i = 10;
		Sleep(10);
	}

	long fib = fib_(n);
	fprintf(stderr, "%dth fibonacci is %lu---costTime %ds\n", n, fib, i);

}

void after_fib(uv_work_t *req, int status)
{
	fprintf(stderr, "Done calculating %dth fibonacci\n", *(int *) req->data);
}
/////////////////////////////////////////////////

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

void signal_handler(uv_signal_t
	* req, int signum){
	printf("Signal received!\n");
	int i;
	for (i = 0; i < FIB_UNTIL; i++) {
		uv_cancel((uv_req_t *)&req[i]);
		
	}
	uv_signal_stop(req);
}
/////////////////////////////////////////////////////////////

/*//文件读取*/
uv_fs_t fs_open_req;
uv_fs_t fs_read_req;
char buffer[10] = { 0x0 };
uv_buf_t iov = { 0x00 };

void fs_on_read(uv_fs_t* req) {
	uv_thread_t id = uv_thread_self();
	printf("fs_on_read thread id:%lu.\n", id);

	uv_fs_req_cleanup(req);
	if (req->result < 0) {
		fprintf(stderr, "Read error: %s\n", uv_strerror(req->result));
	}
	else if (req->result == 0) {
		uv_fs_t close_req;
		// synchronous
		uv_fs_close(loop, &close_req, fs_open_req.result, NULL);

		fprintf(stderr, "Read end: %s\n", uv_strerror(req->result));
	}
	else {
		fprintf(stderr, "Read file %d: %s, %d\n",iov.len, iov.base, req->result);

		memset(buffer, 0x00, sizeof(buffer));
		iov = uv_buf_init(buffer, sizeof(buffer));
		uv_fs_read(loop, &fs_read_req, fs_open_req.result,
			&iov, 1, -1, fs_on_read);

	}
}

void fs_on_open(uv_fs_t* req) {
	uv_thread_t id = uv_thread_self();
	printf("fs_on_open thread id:%lu.\n", id);

	if (req->result != -1) {
		iov = uv_buf_init(buffer, sizeof(buffer));
		uv_fs_read(loop, &fs_read_req, req->result,
			&iov, 1, -1, fs_on_read);
	}
	else {
		fprintf(stderr, "error opening file: %d\n", req->result);
	}
	uv_fs_req_cleanup(req);

}
////////////////////////////////////////////////////////////////

/*
线程间通信
*/
uv_async_t async;

void fake_download(uv_work_t *req) {

	uv_thread_t id = uv_thread_self();
	printf("fake_download thread id:%lu.\n", id);

	int size = *((int*)req->data);
	int downloaded = 0;
	double percentage=0;

	srand(time(NULL));
	while (downloaded < size) {
		percentage = downloaded*100.0 / size;
		async.data = (void*)&percentage;
		uv_async_send(&async);

		Sleep(1);
		downloaded += (200 + rand()) % 1000; // can only download max 1000bytes/sec,
											   // but at least a 200;
	}
}

void print_progress(uv_async_t *handle) {
	uv_thread_t id = uv_thread_self();
	printf("print_progress thread id:%lu.\n", id);

	double percentage = *((double*)handle->data);
	fprintf(stderr, "Downloaded %.2f%%\n", percentage);
}
void after(uv_work_t *req, int status) {
	uv_thread_t id = uv_thread_self();
	printf("after thread id:%lu.\n", id);

	fprintf(stderr, "Download complete\n");
	uv_close((uv_handle_t*)&async, NULL);
}
////////////////////////////////////////////////////////

int main()
{
	BOOL b = SetConsoleCtrlHandler(ctrHandler, TRUE);

	cout << "====libuv test app starting..." << endl;
	uv_thread_t id = uv_thread_self();
	printf("main thread id:%lu.\n", id);


	loop = uv_default_loop();

	//tcp echo 通信
	if (tcp4_echo_start(TEST_PORT))
		return 1;
		

	/*//工作线程队列
	int data[FIB_UNTIL];
	
	int i;
	for (i = 0; i < FIB_UNTIL; i++)
	{
		data[i] = i;
		req[i].data = (void *)&data[i];
		uv_queue_work(loop, &req[i], fib, after_fib);
	}

	uv_signal_t sig;
	uv_signal_init(loop, &sig);
	uv_signal_start(&sig, signal_handler, SIGINT);
	*/

	/*//文件读取
	int r = uv_fs_open(loop, &fs_open_req, \
		"D:\\MyTest\\2018_C++\\myLibuv\\doc\\test-2.txt", O_RDONLY, \
		0, fs_on_open);
		*/
		

	/*//线程间通信
	uv_work_t req;
	int size = 10240;
	req.data = (void*)&size;

	uv_async_init(loop, &async, print_progress);
	uv_queue_work(loop, &req, fake_download, after);
	*/
	

	//start un_run
	uv_run(loop, UV_RUN_DEFAULT);
	uv_loop_close(loop);

	cout << "====stop server, exits." << endl;
	system("pause");
    return 0;
}

