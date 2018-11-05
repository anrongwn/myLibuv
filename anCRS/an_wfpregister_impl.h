#pragma once
#include "an_Commander.h"

class an_wfpregister_impl : public an_Commander
{
public:
	an_wfpregister_impl();
	~an_wfpregister_impl();

	virtual HRESULT execute(an_WFP_header * arg, an_xfs_result *lpresult);
};

class an_wfpderegister_impl : public an_Commander
{
public:
	an_wfpderegister_impl();
	~an_wfpderegister_impl();

	virtual HRESULT execute(an_WFP_header * arg, an_xfs_result *lpresult);
};

