#include "stdafx.h"
#include "an_wfpgetinfo_impl.h"


an_wfpgetinfo_impl::an_wfpgetinfo_impl(an_xfs_caps *caps, an_xfs_status *status, \
	an_xfs_cu_info *cuinfo) : caps_(caps), status_(status), cuinfo_(cuinfo)
{
}


an_wfpgetinfo_impl::~an_wfpgetinfo_impl()
{
}

HRESULT an_wfpgetinfo_impl::execute(an_WFP_header * arg, an_xfs_result *lpresult) {
	HRESULT hr = WFS_SUCCESS;

	an_WFPGetInfo * lpgetinfo = reinterpret_cast<an_WFPGetInfo *>(arg);
	if (lpgetinfo) {
		lpresult->hResult = WFS_SUCCESS;
		lpresult->hService = lpgetinfo->hService;
		lpresult->RequestID = lpgetinfo->ReqID;
		lpresult->hWnd = lpgetinfo->hWnd;
		lpresult->dwMsg = WFS_GETINFO_COMPLETE;
		lpresult->u.dwCommandCode = lpgetinfo->dwCategory;
		//lpresult->lpBuffer = "";

		std::stringstream os;
		

		switch (lpgetinfo->dwCategory) {
		case WFS_INF_CIM_CAPABILITIES:
		{
			{
				cereal::BinaryOutputArchive bi_ar(os);
				bi_ar(*caps_);
			}

			lpresult->lpBuffer = os.str();
			break;
		}
		case WFS_INF_CIM_STATUS:
			break;
		default:
			lpresult->hResult = WFS_ERR_UNSUPP_CATEGORY;
			break;
		}
	}

	return hr;
}