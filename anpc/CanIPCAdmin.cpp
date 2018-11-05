#include "stdafx.h"
#include "CanIPCAdmin.h"
#include "CanIPC.h"


CanIPCAdmin::CanIPCAdmin()
{
	uv_mutex_init(&m_mutex);
}


CanIPCAdmin::~CanIPCAdmin()
{
	uv_mutex_destroy(&m_mutex);

	clear_map();
}

void CanIPCAdmin::clear_map() {
	m_serviceMap.clear();

	auto f = [](map<string, class CanIPC*>::reference it) {
		if (it.second) delete it.second;
	};
	for_each(m_IPCMap.begin(), m_IPCMap.end(), f);

	m_IPCMap.clear();
}

void CanIPCAdmin::remove_map(HSERVICE service) {
	SERVICEMAP_ITER it = m_serviceMap.find(service);
	if (it != m_serviceMap.end()) {

		IPCMAP_ITER it2 = m_IPCMap.find(it->second);
		if (it2 != m_IPCMap.end()) {
			if (it2->second)delete it2->second;
			m_IPCMap.erase(it2);
		}

		m_serviceMap.erase(it);
	}


}
CanIPC * CanIPCAdmin::get_ipc(const string &szServerName) {
	CanIPC * r = nullptr;
	IPCMAP_ITER it = m_IPCMap.find(szServerName);
	if (it != m_IPCMap.end()) {
		r = it->second;
	}
	return r;
}

CanIPC * CanIPCAdmin::get_ipc(HSERVICE service) {
	CanIPC * r = nullptr;

	SERVICEMAP_ITER it = m_serviceMap.find(service);
	if (it != m_serviceMap.end()) {
		r = get_ipc(it->second);
	}

	return r;
}
int CanIPCAdmin::create_ipc(HSERVICE service, const char * szServerName, an_mssage_handle cb) {
	int r = 0;
	/*//
	string strname(szServerName);
	bool exist = false;
	auto f = [strname, &exist](map<string, class CanIPC*>::reference it) {
		if (0 == strname.compare(it.first)) {
			exist |= true;
		}
		else {
			exist |= false;
		}
	};
	for_each(m_IPCMap.begin(), m_IPCMap.end(), f);

	//新建 ipc 对应
	CanIPC * ipc = nullptr;
	if (false == exist) {
		ipc = new CanIPC(service, szServerName, cb);
		if (ipc) {
			r=ipc->run();
		}

		this->m_IPCMap.insert(map<string, CanIPC*>::value_type(strname, ipc));
	}
	else {
		ipc = get_ipc(strname);

		if (ipc) ipc->add_service(service);
	}
	this->m_serviceMap.insert(map<HSERVICE, string>::value_type(service, strname));
	*/

	return r;
}
int CanIPCAdmin::delete_ipc(HSERVICE service) {
	int r = 0;
	/*
	CanIPC * ipc = get_ipc(service);
	if (ipc) {
		ipc->remove_service(service);

		size_t n = ipc->get_service_size();
		if (0 == n) {
			delete ipc;
		}
	}
	*/
	return r;
}