#pragma once
#include <string>
#include <map>
#include <algorithm>
#include "anUtility.h"

using namespace std;

class CanContext
{
public:
	CanContext();
	virtual ~CanContext();
private:
	CanContext(const CanContext&);
	CanContext& operator=(const CanContext&);
public:
	void insert_service_cb(const string strlogicname, an_result_handle cb);
	void reomve_service_cb(const string strlogicname);
	an_result_handle get_service_cb(const string strlogicname);

	void insert_service_name(HSERVICE service, const string strlogicname);
	void remove_service_name(HSERVICE service);
	string get_service_name(HSERVICE service);
	size_t get_service_size(const string strlogicname);

private:
	map<string, an_result_handle> m_service_cb;
	typedef map<string, an_result_handle>::iterator serivcecb_iterator;

	//service ”Î logicname ∂‘”¶
	map<HSERVICE, string> m_service_name;
	typedef map<HSERVICE, string>::iterator servicename_iterator;
};

