#pragma once
#include <string>
#include <map>
#include <algorithm>
#include "anAllotter.h"

using namespace std;


class CanIPC;

class CanIPCAdmin
{
public:
	CanIPCAdmin();
	~CanIPCAdmin();

	int create_ipc(HSERVICE service, const char * szServerName, an_mssage_handle cb);
	int delete_ipc(HSERVICE service);
private:
	CanIPC * get_ipc(const string &szServerName);
	CanIPC * get_ipc(HSERVICE service);
	void clear_map();
	void remove_map(HSERVICE service);

private:
	CanIPCAdmin(const CanIPCAdmin&);
	CanIPCAdmin& operator=(const CanIPCAdmin&);

private:
	map<string, class CanIPC*> m_IPCMap;
	typedef map<string, class CanIPC*>::iterator IPCMAP_ITER;

	map<HSERVICE, string> m_serviceMap;
	typedef map<HSERVICE, string>::iterator SERVICEMAP_ITER;
		
	uv_mutex_t m_mutex;
};
