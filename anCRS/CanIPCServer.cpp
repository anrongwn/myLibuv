#include "stdafx.h"
#include "CanIPCServer.h"
#include "../anCIM_sp/message_pack.h"
#include <sstream>
#include "Executer.h"


CanIPCServer* CanIPCServer::s_pInstance = nullptr;
std::vector<std::unique_ptr<uv_pipe_t>> CanIPCServer::s_client_vector;
CanIPCServer * CanIPCServer::get_instance(const char *strServerName) {
	//双检测，互斥
	if (nullptr == s_pInstance) {
		//ANLOCK(g_mutex);
		if (nullptr == s_pInstance) {
			static CanIPCServer server(strServerName);

			s_pInstance = &server;
		}
	}

	return s_pInstance;
}
/*
CanIPCServer::CanIPCServer() 
	: m_loop(nullptr), m_strServerName(""), \
	m_cmd_executer(nullptr), m_xfs_proxy(nullptr)
{
	
}
*/
CanIPCServer::CanIPCServer(const char * strServerName) \
	: m_loop(nullptr), m_strServerName(strServerName), \
	m_cmd_executer(nullptr) {
	m_xfs_proxy = std::make_unique<CanXfs>([this](void*arg)->int {return notify_exit_server(arg); });
}

CanIPCServer::~CanIPCServer()
{
	if (m_cmd_executer) {
		delete m_cmd_executer;
	}

	/*
	if (m_xfs_proxy) {
		delete m_xfs_proxy;
	}
	*/
}

int CanIPCServer::notify_exit_server(void *arg) {
	int r = 0;

	//r = uv_loop_alive(m_loop);

	//
	uv_stop(m_loop);

	//
	uv_close((uv_handle_t*)&this->m_handle, CanIPCServer::on_close);
	
	std::stringstream logdata;
	logdata << "CanIPCServer::notify_exit_server()";
	LOGD(logdata.str());

	return r;
}
void CanIPCServer::alloc_buffer(uv_handle_t * handle, size_t suggested_size, uv_buf_t * buf) {
	buf->base = (char *)malloc(suggested_size);
	buf->len = suggested_size;
}

void CanIPCServer::on_write(uv_write_t * req, int status) {
	std::stringstream logdata;

	logdata << "CanIPCServer::on_write(" << std::hex << (void*)req \
		<< ", " << status << ")--";

	if (status < 0) {
		//fprintf(stderr, "Write error %s\n", uv_err_name(status));
		LOGE(logdata.str());
		return;
	}

	//清理用户数据
	an_notice_result *result = (an_notice_result*)req->data;
	if (result) {
		
		//是否有退出服务标志(最后一个close 完成消息写完)
		if (result->_notify_exit_flag) {
			CanIPCServer * pThis = reinterpret_cast<CanIPCServer *>(result->_client->data);
			if (pThis) {
				pThis->notify_exit_server(nullptr);
			}
		}

		CanAllotter::an_free(result->_buf.base);
		logdata << "CanAllotter::an_free(" << std::hex << (void*)result->_buf.base << "),";
		an_notice_result::free_an_notice_result(result);//an_notice_result
		logdata << "free_an_notice_result(" << std::hex << (void*)result << "),";
	}

	CanAllotter::an_free(req);
	logdata << "CanAllotter::an_free(" << std::hex << (void*)req << ") completed.";
	LOGD(logdata.str());
}

void CanIPCServer::erase_client_vector(void * client) {
	std::stringstream logdata;
	logdata << "CanIPCServer::erase_client_vector client=" << std::hex << client;
	void * des = nullptr;
	for (auto & it : CanIPCServer::s_client_vector) {
		des = static_cast<void *>(it.get());
		if (des == client) {
			logdata << ", des=" << std::hex << des;
			delete it.release();
			
			LOGD(logdata.str());
			break;
		}
	}
}

void CanIPCServer::on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
	std::stringstream logdata;

	logdata << "CanIPCServer::on_read(), nread=" << nread;
	LOGD(logdata.str());

	CanIPCServer * pThis = reinterpret_cast<CanIPCServer *>(client->data);
	if (nread > 0) {
		//读取包头
		std::stringstream is(std::string(buf->base, nread));
		cereal::BinaryInputArchive ar_header(is);
		an_WFP_header header;
		ar_header(header);
		
		logdata.str("");
		logdata << "CanIPCServer::on_read()--received:(hService=" << header.hService << ", ReqID=" \
			<< header.ReqID << ", Type=" << e_an_wfp_type_name[header.eType] << ")";
		LOGD(logdata.str());

		//生成命令包
		is.seekg(std::ios::beg);
		an_WFP_header * cmd = g_cmd_builder[header.eType](is);
		an_buf tmp(client, cmd);

		//
		if (e_an_WFPGetInfo == cmd->eType) {
			//委托xfs
			pThis->m_xfs_proxy->spi_getinfo(&tmp, &pThis->m_evnet_handler);

			destroy_cmd(cmd);
		}
		else {
			//加入命令队列中
			pThis->m_cmd_executer->push_back_cmd_deque(std::move(tmp));
		}
		
	}

	if (nread < 0) {
		logdata << "CanIPCServer::on_read() failed, nread=" << nread;
		if (!uv_is_closing((uv_handle_t*)client)) {

			uv_close(reinterpret_cast<uv_handle_t*>(client), CanIPCServer::on_close);

			logdata << "--uv_close() session handle=" << std::hex << (void*)client;
		}
		else {
			logdata << "session handle=" << std::hex << (void*)client << "is closed(closing)!!!";
		}
		LOGE(logdata.str());
	}

	//
	CanAllotter::an_free(buf->base);
}
void CanIPCServer::on_new_connection(uv_stream_t* server, int status) {
	std::stringstream logdata;

	if (status < 0) {
		
		uv_close(reinterpret_cast<uv_handle_t*>(server), nullptr);

		logdata << "CanIPCServer::on_new_connection status=" << uv_strerror(status);
		LOGE(logdata.str());

		return;
	}

	CanIPCServer * pThis = reinterpret_cast<CanIPCServer *>(server->data);

	
	
	std::unique_ptr<uv_pipe_t> client(new uv_pipe_t);
	client.get()->data = pThis;
	uv_pipe_init(pThis->m_loop, client.get(), 0);
	int r = 0;

	logdata << "CanIPCServer::on_new_connection new client=" << std::hex << (void*)client.get();
	LOGD(logdata.str());

	r = uv_accept(server, reinterpret_cast<uv_stream_t*>(client.get()));
	if (0 == r) {
		/*
		char buf[1024] = { 0x00 };
		size_t len = 1024;
		r = uv_pipe_getpeername(client.get(), buf, &len);
		fprintf(stderr, "getpeername pipename: %s\n", buf);
		*/

		r = uv_read_start(reinterpret_cast<uv_stream_t*>(client.get()), \
			CanIPCServer::alloc_buffer, CanIPCServer::on_read);
		if (r) {
			logdata << "CanIPCServer::on_new_connection()--uv_read_start error=" << uv_strerror(r);
			LOGE(logdata.str());
		}

		CanIPCServer::s_client_vector.push_back(std::move(client));
	} else {
		//fprintf(stderr, "accept error %s\n", uv_err_name(r));
		uv_close(reinterpret_cast<uv_handle_t*>(client.get()), NULL);

		logdata << "CanIPCServer::on_new_connection()--uv_accept error=" << uv_strerror(r);
		LOGE(logdata.str());

	}
}

void CanIPCServer::on_close(uv_handle_t* handle) {
	std::stringstream logdata;
	logdata << "CanIPCServer::on_close(" << std::hex << (void*)handle << ")";

	if (UV_ASYNC == handle->type) {
		//清除 每次的uv_async_t对象
		CanAllotter::an_free(handle);
		logdata << "CanAllotter::an_free(" << std::hex << (void*)handle << ")";
		LOGD(logdata.str());
	}
	else if (UV_NAMED_PIPE == handle->type) {
		//从连接中清除
		CanIPCServer * pThis = reinterpret_cast<CanIPCServer *>(handle->data);
		pThis->erase_client_vector((void *)handle);

	}

}

/*
typedef struct {
	uv_write_t uv_req;
	uv_buf_t buf;
}an_write_t, *p_an_write_t;
*/

int CanIPCServer::send_notify(uv_stream_t* client, an_notice_result * lpresult) {
	std::stringstream logdata;
	int r = 0;
	logdata << "CanIPCServer::send_notify(lpresult=" << std::hex << (void*)lpresult << ")--";
	LOGD(logdata.str());

	if (!uv_is_closing((uv_handle_t*)client)) {
		uv_write_t * req = (uv_write_t *)CanAllotter::an_malloc(sizeof(uv_write_t));
		req->data = lpresult;

		r = uv_write((uv_write_t *)req, (uv_stream_t*)client, \
			&lpresult->_buf, 1, CanIPCServer::on_write);

		logdata << "uv_write(req=" << std::hex << (void*)req << " ,len=" << std::dec << lpresult->_buf.len << ", base=" << std::hex << \
			(void*)lpresult->_buf.base << ")";
		LOGD(logdata.str());

		//发送失败处理
		if (0 != r) {
			CanAllotter::an_free(req);

			logdata << ",failed." << uv_strerror(r);
			LOGE(logdata.str());
		}
	}
	else {
		logdata << "client=" << std::hex << (void*)client << " is closed!!!";
		LOGE(logdata.str());

	}

	return r;
}
int CanIPCServer::broadcast_notify(an_notice_result * lpresult) {
	int r = 0;
	std::stringstream logdata;
	logdata << "CanIPCServer::broadcast_notify(lpresult=" << std::hex << (void*)lpresult << ")--";

	auto f = [&](std::vector<std::unique_ptr<uv_pipe_t>>::reference ref) {
		an_notice_result * tmp = an_notice_result::clone(lpresult);
		if (ref.get()) {
			r = CanIPCServer::send_notify((uv_stream_t*)ref.get(), tmp);
			if (r) {
				an_notice_result::destroy(&tmp);
			}
		}
	};

	std::for_each(CanIPCServer::s_client_vector.begin(), \
		CanIPCServer::s_client_vector.end(), f);
	logdata << " completed.";
	LOGD(logdata.str());

	return r;
}

void CanIPCServer::on_notify(uv_async_t* handle) {
	std::stringstream logdata;
	logdata << "CanIPCServer::on_notify(" << std::hex << (void*)handle << ")--";
	LOGD(logdata.str());
	int r = 0;

	//通知处理
	an_notice_result * buffer = (an_notice_result*)handle->data;
	if (buffer) {
		
		//广播事件
		if (nullptr == buffer->_client) {

			r = CanIPCServer::broadcast_notify(buffer);
			
			//
			CanAllotter::an_free(buffer->_buf.base);
			an_notice_result::free_an_notice_result(buffer);//an_notice_result
		}
		else {	//完成消息
			r = CanIPCServer::send_notify(buffer->_client, buffer);
			if (0!=r) {
				CanAllotter::an_free(buffer->_buf.base);
				an_notice_result::free_an_notice_result(buffer);//an_notice_result
			}
			/*
			if (!uv_is_closing((uv_handle_t*)buffer->_client)) {
				uv_write_t * req = (uv_write_t *)CanAllotter::an_malloc(sizeof(uv_write_t));
				req->data = buffer;

				r = uv_write((uv_write_t *)req, (uv_stream_t*)buffer->_client, \
					&buffer->_buf, 1, CanIPCServer::on_write);
				
				logdata << "uv_write(len=" << buffer->_buf.len << ", base=" << std::hex << \
					(void*)buffer->_buf.base << ")";
				LOGD(logdata.str());

				//发送失败处理
				if (0 != r) {
					CanAllotter::an_free(buffer->_buf.base);
					an_notice_result::free_an_notice_result(buffer);//an_notice_result
					CanAllotter::an_free(req);

					logdata << ",failed." << uv_strerror(r);
					LOGE(logdata.str());
				}
				
			}
			else {
				logdata << "client=" << std::hex << (void*)buffer->_client << " is closed!!!";
				LOGE(logdata.str());

				CanAllotter::an_free(buffer->_buf.base);
				an_notice_result::free_an_notice_result(buffer);//an_notice_result
			}
			*/
			
		}
	}

	//清除
	uv_close((uv_handle_t*)handle, CanIPCServer::on_close);
}

int CanIPCServer::run() {
	int r = 0;
	std::stringstream logdata;

	m_loop = uv_default_loop();
	
	m_handle.data = this;
	r = uv_pipe_init(m_loop, &m_handle, 0);
	if (r) {
		logdata << "CanIPCServer::run()--uv_pipe_init, error=" << uv_strerror(r);
		LOGE(logdata.str());
		return r;
	}

	r = uv_pipe_bind(&m_handle, m_strServerName.c_str());
	if (r) {
		logdata << "CanIPCServer::run()--uv_pipe_bind " << m_strServerName <<", error=" << uv_strerror(r);
		LOGE(logdata.str());
		return r;
	}
	
	r = uv_listen(reinterpret_cast<uv_stream_t*>(&m_handle), 128, CanIPCServer::on_new_connection);
	if (r) {
		logdata << "CanIPCServer::run()--uv_listen, error=" << uv_strerror(r);
		LOGE(logdata.str());
		return r;
	}
		
	//注册getinfo 完成事件处理器
	m_evnet_handler.init(m_loop, CanIPCServer::on_notify);

	//创建执行命令线程
	m_cmd_executer = new CExecuter(m_loop, CanIPCServer::on_notify);
	m_cmd_executer->register_xfs_proxy(this->m_xfs_proxy.get());
	m_cmd_executer->start();
	
	return uv_run(m_loop, UV_RUN_DEFAULT);
}

int CanIPCServer::stop() {
	if (m_cmd_executer) {
		m_cmd_executer->stop();
		m_cmd_executer->join();
	}

	return 0;
}