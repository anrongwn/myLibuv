#include "stdafx.h"
#include "an_wfpregister_impl.h"


an_wfpregister_impl::an_wfpregister_impl() : an_Commander()
{
}


an_wfpregister_impl::~an_wfpregister_impl()
{
}

HRESULT an_wfpregister_impl::execute(an_WFP_header * arg, an_xfs_result *lpresult) {
	HRESULT hr = WFS_SUCCESS;

	an_WFPRegister * lpregister = reinterpret_cast<an_WFPRegister *>(arg);
	if (lpregister) {
		lpresult->hResult = WFS_SUCCESS;
		lpresult->hService = lpregister->hService;
		lpresult->RequestID = lpregister->ReqID;
		lpresult->u.dwCommandCode = WFS_REGISTER_COMPLETE;
		lpresult->hWnd = lpregister->hWnd;
		lpresult->dwMsg = WFS_REGISTER_COMPLETE;
	}

	return hr;
}

an_wfpderegister_impl::an_wfpderegister_impl() : an_Commander()
{
}


an_wfpderegister_impl::~an_wfpderegister_impl() 
{
}

HRESULT an_wfpderegister_impl::execute(an_WFP_header * arg, an_xfs_result *lpresult) {
	HRESULT hr = WFS_SUCCESS;

	an_WFPDeregister * lpregister = reinterpret_cast<an_WFPDeregister *>(arg);
	if (lpregister) {
		lpresult->hResult = WFS_SUCCESS;
		lpresult->hService = lpregister->hService;
		lpresult->RequestID = lpregister->ReqID;
		lpresult->u.dwCommandCode = WFS_DEREGISTER_COMPLETE;
		lpresult->hWnd = lpregister->hWnd;
		lpresult->dwMsg = WFS_DEREGISTER_COMPLETE;
	}

	return hr;
}