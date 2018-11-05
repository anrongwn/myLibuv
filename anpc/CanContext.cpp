#include "stdafx.h"
#include "CanContext.h"


CanContext::CanContext()
{
	
}


CanContext::~CanContext()
{
	m_service_cb.clear();
	m_service_name.clear();
}


void CanContext::insert_service_cb(const string strlogicname, an_result_handle cb) {
	bool exist = false;
	auto f = [strlogicname, &exist](map<string, an_result_handle>::reference it) {
		if (0 == strlogicname.compare(it.first)) {
			exist |= true;
		}
		else {
			exist |= false;
		}
	};
	for_each(m_service_cb.begin(), m_service_cb.end(), f);

	if (false == exist) {
		m_service_cb.insert(map<string, an_result_handle>::value_type(strlogicname, cb));
	}
}
void CanContext::reomve_service_cb(const string strlogicname) {
	serivcecb_iterator it = m_service_cb.find(strlogicname);
	if (it != m_service_cb.end()) {
		m_service_cb.erase(it);
	}
}
an_result_handle CanContext::get_service_cb(const string strlogicname) {
	an_result_handle cb = nullptr;

	serivcecb_iterator it = m_service_cb.find(strlogicname);
	if (it != m_service_cb.end()) {
		cb = it->second;
	}

	return cb;
}

void CanContext::insert_service_name(HSERVICE service, const string strlogicname) {
	m_service_name.insert(map<HSERVICE, string>::value_type(service, strlogicname));
}
void CanContext::remove_service_name(HSERVICE service) {
	auto it = m_service_name.find(service);
	if (it != m_service_name.end()) {
		m_service_name.erase(it);
	}
}
string CanContext::get_service_name(HSERVICE service) {
	string name;

	auto it = m_service_name.find(service);
	if (it != m_service_name.end()) {
		name = it->second;
	}

	return name;
}

size_t CanContext::get_service_size(const string strlogicname){
	size_t num = 0;

	auto f = [strlogicname, &num](map<HSERVICE, string>::reference it) {
		if (0 == it.second.compare(strlogicname)) {
			if (it.first) ++num;
		}
	};

	for_each(m_service_name.begin(), m_service_name.end(), f);

	return num;
}