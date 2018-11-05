#pragma once

#ifdef ANPC_EXPORTS
#define ANPC_API __declspec(dllexport)
#else
#define ANPC_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
#pragma pack(push, 1)

	//消息处理回调
	typedef int(*an_result_handle)(void * lpxfs);

	ANPC_API void * WINAPI anpc_malloc(size_t size);
	ANPC_API void WINAPI anpc_free(void * buf);

	ANPC_API int WINAPI	anpc_create_ipc(unsigned short service, const char * strlogicname, an_result_handle cb);
	ANPC_API int WINAPI	anpc_send_cmd(unsigned short service, void * cmd, size_t len);
	ANPC_API int WINAPI	anpc_close_ipc(unsigned short service);
	ANPC_API int WINAPI	anpc_stop(bool bforce);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif