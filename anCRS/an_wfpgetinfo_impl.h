#pragma once
#include "an_Commander.h"
class an_wfpgetinfo_impl :
	public an_Commander
{
public:
	an_wfpgetinfo_impl() = delete;
	an_wfpgetinfo_impl(an_xfs_caps *caps, an_xfs_status *status, \
		an_xfs_cu_info *cuinfo);
	~an_wfpgetinfo_impl();



	virtual HRESULT execute(an_WFP_header * arg, an_xfs_result *lpresult);
private:
	an_xfs_caps *caps_;
	an_xfs_status *status_;
	an_xfs_cu_info *cuinfo_;
};

