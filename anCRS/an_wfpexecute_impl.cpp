#include "stdafx.h"
#include "an_wfpexecute_impl.h"


an_wfpexecute_impl::an_wfpexecute_impl(an_xfs_caps *caps, an_xfs_status *status, \
	an_xfs_cu_info *cuinfo, an_xfs_dev *dev) : an_Commander(), \
	_caps(caps), _status(status),_cuinfo(cuinfo),_dev(dev)
{
}


an_wfpexecute_impl::~an_wfpexecute_impl()
{
}

HRESULT an_wfpexecute_impl::execute(an_WFP_header * arg, an_xfs_result *lpresult) {
	HRESULT hr = WFS_SUCCESS;

	an_WFPExecute * lpexecute = reinterpret_cast<an_WFPExecute *>(arg);

	/*//Ó²¼þ²Ù×÷
	hr = _dev->
	*/

	//
	if (lpexecute) {
		lpresult->hResult = hr;
		lpresult->hService = lpexecute->hService;
		lpresult->RequestID = lpexecute->ReqID;
		lpresult->u.dwCommandCode = lpexecute->dwCommand;
		lpresult->hWnd = lpexecute->hWnd;
		lpresult->dwMsg = WFS_EXECUTE_COMPLETE;
		//lpresult->lpBuffer = 
	}

	return hr;
}