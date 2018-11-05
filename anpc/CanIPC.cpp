#include "stdafx.h"
#include "CanIPC.h"
#include <sstream>
#include "../anCRS/an_xfs_result.h"
#include "anpc_error.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"IPHLPAPI.lib")
#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"Userenv.lib")

#if _DEBUG
#pragma comment(lib, "..//Debug//libuv.lib")
//#pragma comment(lib, "..//tcmalloc//libtcmalloc_minimal-debug.lib")
#else
#pragma comment(lib, "..//Release//libuv.lib")
//#pragma comment(lib, "..//tcmalloc//libtcmalloc_minimal.lib")
#endif

/*//
class service_exist {
public:
	explicit service_exist(HSERVICE id): _value(id), _status(false) {

	}
	~service_exist() {

	}
	operator bool() {
		return _status;
	}

	void operator() (HSERVICE id) {
		if (_value == id) {
			_status |= true;
		}
		else {
			_status |= false;
		}
	}
private:
	HSERVICE _value;
	bool _status;
};
*/

//初始化
CanIPC * CanIPC::s_pInstance=nullptr;
CanIPC * CanIPC::get_instance(const char * strServiceName) {
	//双检测，互斥
	if (nullptr == s_pInstance) {
		
		if (nullptr == s_pInstance) {
			static CanIPC ipc(strServiceName);
			s_pInstance = &ipc;
			//CanIPC * ipc = new CanIPC(strServiceName);
			//s_pInstance = ipc;
			
		}
	}

	return s_pInstance;
}

//线程函数
void CanIPC::worker(void * arg) {
	CanIPC * pThis = static_cast<CanIPC*>(arg);

	
	std::stringstream logdata;
	logdata << "====CanIPC::worker's threadid=" << std::hex << std::this_thread::get_id();
	LOGD(logdata.str());
	::OutputDebugStringA(logdata.str().c_str());
	

	int r = ANPC_AN_SUCCESS;
	r = pThis->init();
	if (r) return;

	r = pThis->connect();
	if (r) return;

	r = uv_run(&pThis->m_loop, UV_RUN_DEFAULT);

	logdata << ", uv_run() exit, CanIPC::worker() finished!!!";
	LOGD(logdata.str());
	::OutputDebugString(L"====CanIPC::worker exit...\n");
	
}
CanIPC::CanIPC(const char * strServiceName)
	:CanContext(), m_szServerName(strServiceName), m_result_cb(nullptr)
{
	
}
int CanIPC::stop(bool bforce) {
	int r = ANPC_AN_SUCCESS;
	std::stringstream logdata;
	logdata << "CanIPC::stop()--";
	if (m_thread.joinable()) {
		logdata << "m_thread.joinable(),";
		//
		uv_stop(&m_loop);

		//
		if (!uv_is_closing((uv_handle_t*)&m_pipe)) {
			//只作为 激活uv_run 的信号，并通知退出 
			uv_close((uv_handle_t*)&m_pipe, NULL);
			logdata << "uv_close(),";
		}
		
		//主要防止 DLL_PROCESS_DETACH 时，std::thread join 与 dllmain 死锁的问题。（std::thread join的bug）
		if (false==bforce) {
			m_thread.join();
			logdata << "m_thread.join(),";
		}
		else {
			//分离线程，不能使用join(),如上
			m_thread.detach();
			logdata << "m_thread.detach(),";
		}
	}

	LOGD(logdata.str());
		
	return 0;
}
CanIPC::~CanIPC()
{
	/*//
	//stop(true);
	if (m_thread.joinable()) m_thread.detach();

	uv_loop_close(&m_loop);
	*/
}

int CanIPC::close(HSERVICE service) {
	int r = ANPC_AN_SUCCESS;

	/*//没必要，暂不做任何操作。会造成的 service map项没有清除
	string logicname = get_service_name(service);

	remove_service_name(service);
	size_t num = get_service_size(logicname);
	if (0 == num) {
		reomve_service_cb(logicname);

		
		//停止线程
		stop(false);
		
	}
	*/

	return r;
}


int CanIPC::init() {
	int r = ANPC_AN_SUCCESS;
	std::stringstream logdata;

	r = uv_loop_init(&m_loop);
	if (r) {
		logdata << "uv_loop_init error=" << uv_strerror(r);
		LOGE(logdata.str());

		return r;
	}

	r = uv_pipe_init(&m_loop, &m_pipe, 0);
	if (r) {
		logdata << "uv_pipe_init error=" << uv_strerror(r);
		LOGE(logdata.str());
	}
	
	return r;
}

void CanIPC::on_close(uv_handle_t* handle) {
	if (UV_ASYNC == handle->type) {
		//清除 每次的uv_async_t对象
		if(handle->data)
			CanAllotter::an_free(handle->data);

		CanAllotter::an_free(handle);

	}
	else if (UV_NAMED_PIPE == handle->type) {
		//anCRS端终止连接后，工作线程uv_run退出，分离当前线程 
		CanIPC * pThis = static_cast<CanIPC*>(handle->data);
		if ((&pThis->m_pipe) == (void*)(handle))
		{
			//防此线程退出后，还可以joinable
			pThis->m_thread.detach();
		}
		
	}
	
}


int CanIPC::sendcmd(HSERVICE service, void *data, size_t len) {
	int r = ANPC_AN_SUCCESS;
	std::stringstream logdata;

	logdata << "CanIPC::sendcmd(hService=" << service << ",len=" << len\
		<< ")";
	LOGD(logdata.str());

	if (!uv_is_active((uv_handle_t*)&m_pipe)) {
		logdata << "service is inactive!!!";
		LOGE(logdata.str());
		
		return ANPC_AN_FAILED;
	}

	uv_async_t * async_req = (uv_async_t*)CanAllotter::an_malloc(sizeof(uv_async_t));
	p_an_cmd_t cmd = (p_an_cmd_t)CanAllotter::an_malloc(sizeof(an_cmd_t));

	cmd->type = anpc_cmd_req;
	cmd->service = service;
	cmd->data = data;
	cmd->len = len;
	cmd->handle = this;

	async_req->data = cmd;

	uv_async_init(&m_loop, async_req, on_send_cmd);
	r = uv_async_send(async_req);
	if (0 != r) {
		CanAllotter::an_free(async_req);
		CanAllotter::an_free(cmd->data); //xfs cmd
		CanAllotter::an_free(cmd);

		logdata << ", uv_async_init failed." << uv_strerror(r);
		LOGE(logdata.str());
	}

	return r;
}

void CanIPC::on_send_cmd(uv_async_t * handle) {
	p_an_cmd_t req = static_cast<p_an_cmd_t>(handle->data);
	CanIPC * pThis = static_cast<CanIPC*>(req->handle);

	pThis->write(req->data, req->len);

	uv_close((uv_handle_t*)handle, CanIPC::on_close);

}


void CanIPC::on_connect(uv_connect_t* req, int status) {
	CanIPC * pThis = static_cast<CanIPC*>(req->data);
	std::stringstream logdata;
	int r = ANPC_AN_SUCCESS;

	if ((status)){

		if (0 == pThis->m_wait_connect.get_count()) {
			char* args[3];
			args[0] = "D:\\MyTest\\2018_C++\\myLibuv\\Debug\\anCRS.exe";
			args[1] = NULL;
			args[2] = NULL;
			pThis->m_sp_process_options.exit_cb = NULL;
			pThis->m_sp_process_options.cwd = "D:\\MyTest\\2018_C++\\myLibuv\\Debug\\";
			pThis->m_sp_process_options.args = args;
			pThis->m_sp_process_options.file = "D:\\MyTest\\2018_C++\\myLibuv\\Debug\\anCRS.exe";
			pThis->m_sp_process_options.flags = UV_PROCESS_DETACHED;

			r = uv_spawn(&pThis->m_loop, &pThis->m_sp_process_req, &pThis->m_sp_process_options);
			if (r) {
				
				logdata << "CanIPC::on_connect--uv_spawn(" << pThis->m_sp_process_options.file << ") failed."\
					<< uv_strerror(r);
				LOGE(logdata.str());
				return;
			}
			uv_unref((uv_handle_t *)&pThis->m_sp_process_req);
		}
		::Sleep(10);
		pThis->connect();
		pThis->m_wait_connect.signal_once();
	}
	else if (0 == status) {
		req->handle->data = pThis;

		r = uv_read_start((uv_stream_t *)req->handle, alloc_buffer, on_read);
		if (r) {
			logdata << "CanIPC::on_connect--uv_read_start failed."\
				<< uv_strerror(r);
			LOGE(logdata.str());
		}
		else {
			logdata << "CanIPC::on_connect connect service session=" << std::hex << (void*)req->handle << " is success!!!";
			LOGI(logdata.str());
		}
		pThis->m_wait_connect.signal();
	}

}
void CanIPC::on_write(uv_write_t* req, int status) {
	an_write_t * wr = (an_write_t*)(req);

	CanAllotter::an_free(wr->buf.base); //xfs cmd 
	CanAllotter::an_free(wr);
}

void CanIPC::alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf) {
	buf->base = (char *)CanAllotter::an_malloc(suggested_size);
	buf->len = suggested_size;
}

void CanIPC::on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	std::stringstream logdata;
	logdata << "CanIPC::on_read(), nread=" << nread;
	LOGD(logdata.str());

	if (nread > 0) {
		CanIPC * pThis = static_cast<CanIPC*>(stream->data);
		
		//读取xfs_result
		std::stringstream is(std::string(buf->base, nread));
		cereal::BinaryInputArchive ar(is);
		an_xfs_result result;
		ar(result);

		//发送xfs 事件或完成消息
		if (pThis->m_result_cb) {
			int r = pThis->m_result_cb(&result);
		}

		/*//
		if (result.hService) {
			string strlogicname = pThis->get_service_name(result.hService);
			an_result_handle cb = pThis->get_service_cb(strlogicname);

			//发送xfs 事件或完成消息
			if (cb) {
				int r = cb(&result);
			}
			
		}
		*/
	}
	else {
		logdata << "CanIPC::on_read() failed, nread=" << nread;
		if (!uv_is_closing((uv_handle_t*)stream)) {
			uv_close(reinterpret_cast<uv_handle_t*>(stream), CanIPC::on_close);

			logdata << "--uv_close() session handle=" << std::hex << (void*)stream;
		}
		else {
			logdata << "session handle=" << std::hex << (void*)stream << "is closed(closing)!!!";
		}
		
	}
	logdata << " completed.";
	LOGD(logdata.str());
	CanAllotter::an_free(buf->base);

}


int CanIPC::connect() {
	int r = ANPC_AN_SUCCESS;

	m_connect.data = this;
	
	uv_pipe_connect(&m_connect, &m_pipe, m_szServerName.c_str(), CanIPC::on_connect);

	return r;
}

int CanIPC::write(void * data, size_t len) {
	int r = ANPC_AN_SUCCESS;
	std::stringstream logdata;
	logdata << "CanIPC::write(len=" << len << ")";
	LOGD(logdata.str());

	if (uv_is_active((uv_handle_t*)&m_pipe)) {
		an_write_t * req = (an_write_t *)CanAllotter::an_malloc(sizeof(an_write_t));
		req->uv_req.data = this;
		req->buf = uv_buf_init((char *)data, len);

		r = uv_write((uv_write_t *)req, (uv_stream_t*)&m_pipe, &req->buf, 1, CanIPC::on_write);
		if (0 != r) {
			CanAllotter::an_free(req->buf.base); // xfs cmd

			CanAllotter::an_free(req);

			logdata << ", uv_write failed." << uv_strerror(r);
			LOGE(logdata.str());
		}
	}
	else {
		logdata << "service is inactive!!!";
		LOGE(logdata.str());
		r = 1;
	}


	return r;
}
int CanIPC::run(void *arg) {
	int r = ANPC_AN_SUCCESS;
	std::stringstream logdata;
	logdata << "CanIPC::run() ";

	if (false == m_thread.joinable()) {
		m_wait_connect.reset();

		m_thread = std::thread(CanIPC::worker, this);

		//等待连接
		if (m_thread.joinable()) {
			m_wait_connect.wait();

			//连接不成功
			if (1 != m_wait_connect.get_status()) {
				logdata << "connect failed. num=" << m_wait_connect.get_count()\
					<< ", status=" << m_wait_connect.get_status();
				r = 1;
			}
		}
	}
	/*
	if (NULL==m_worker) {
		r = uv_thread_create(&m_worker, CanIPC::worker, this);
		uv_thread_join(&m_worker);
	}
	*/
	logdata << "),=" << r;
	LOGD(logdata.str());

	return r;
}


/*
bool CanIPC::isExist(HSERVICE service) {
	
	service_exist exist(service);
	for_each(m_serviceV.begin(), m_serviceV.end(), exist);

	return exist;
}
*/

