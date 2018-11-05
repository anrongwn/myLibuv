#pragma once
#include "an_Commander.h"

class an_wfpexecute_impl :
	public an_Commander
{
public:
	an_wfpexecute_impl() = delete;
	an_wfpexecute_impl(an_xfs_caps *caps, an_xfs_status *status, \
		an_xfs_cu_info *cuinfo, an_xfs_dev *dev);
	~an_wfpexecute_impl();

	virtual HRESULT execute(an_WFP_header * arg, an_xfs_result *lpresult);

private:
	an_xfs_caps * _caps;
	an_xfs_status * _status;
	an_xfs_cu_info * _cuinfo;
	an_xfs_dev * _dev;
};

