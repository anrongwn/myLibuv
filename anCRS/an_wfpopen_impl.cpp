#include "stdafx.h"
#include "an_wfpopen_impl.h"


an_wfpopen_impl::an_wfpopen_impl(an_xfs_caps *caps, an_xfs_status *status, \
	an_xfs_cu_info *cuinfo, an_xfs_dev *dev, an_observesr obs) \
	: an_Commander(), obs_(obs)
{
	_caps = caps;
	_status = status;
	_cuinfo = cuinfo;
	_dev = dev;
}


an_wfpopen_impl::~an_wfpopen_impl()
{
}

HRESULT an_wfpopen_impl::execute(an_WFP_header * arg, an_xfs_result *lpresult) {
	HRESULT hr = WFS_SUCCESS;

	an_WFPOpen * lpopen = reinterpret_cast<an_WFPOpen *>(arg);
	if (lpopen) {
		lpresult->hResult = WFS_SUCCESS;
		lpresult->hService = lpopen->hService;
		lpresult->RequestID = lpopen->ReqID;
		lpresult->u.dwCommandCode = WFS_OPEN_COMPLETE;
		lpresult->hWnd = lpopen->hWnd;
		lpresult->dwMsg = WFS_OPEN_COMPLETE;
	}
	
	obs_(arg->hService);

	return hr;
}


an_wfpclose_impl::an_wfpclose_impl(an_xfs_caps *caps, an_xfs_status *status, \
	an_xfs_cu_info *cuinfo, an_xfs_dev *dev, an_observesr obs) \
	: an_Commander(), obs_(obs)
{
	_caps = caps;
	_status = status;
	_cuinfo = cuinfo;
	_dev = dev;
}
an_wfpclose_impl::~an_wfpclose_impl() {

}

HRESULT an_wfpclose_impl::execute(an_WFP_header * arg, an_xfs_result *lpresult) {
	HRESULT hr = WFS_SUCCESS;

	an_WFPClose * lpclose = reinterpret_cast<an_WFPClose *>(arg);
	if (lpclose) {
		lpresult->hResult = WFS_SUCCESS;
		lpresult->hService = lpclose->hService;
		lpresult->RequestID = lpclose->ReqID;
		lpresult->u.dwCommandCode = WFS_CLOSE_COMPLETE;
		lpresult->hWnd = lpclose->hWnd;
		lpresult->dwMsg = WFS_CLOSE_COMPLETE;
	}


	//
	if (0 == obs_(arg->hService)) {
		this->_flag = 1;//退出服务标志;
	}

	return hr;
}