#pragma once
#include "an_Commander.h"

//
typedef std::function<size_t(HSERVICE)> an_observesr;

class an_wfpopen_impl :
	public an_Commander
{
public:
	an_wfpopen_impl() = delete;
	an_wfpopen_impl(an_xfs_caps *caps, an_xfs_status *status, \
		an_xfs_cu_info *cuinfo, an_xfs_dev *dev, an_observesr obs);
	~an_wfpopen_impl();

	virtual HRESULT execute(an_WFP_header * arg, an_xfs_result *lpresult);
private:
	an_observesr obs_;
};

class an_wfpclose_impl :
	public an_Commander
{
public:
	an_wfpclose_impl() = delete;
	an_wfpclose_impl(an_xfs_caps *caps, an_xfs_status *status, \
		an_xfs_cu_info *cuinfo, an_xfs_dev *dev, an_observesr obs);
	~an_wfpclose_impl();

	virtual HRESULT execute(an_WFP_header * arg, an_xfs_result *lpresult);
private:
	an_observesr obs_;
};

