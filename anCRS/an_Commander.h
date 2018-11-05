#pragma once
#include <XFSCDM.H>
#include <XFSCIM.H>
#include "../anCIM_sp/message_pack.h"
#include "an_xfs_cu_info.h"
#include "an_xfs_dev.h"
#include "an_xfs_status.h"
#include "an_xfs_result.h"
#include <functional>

class an_Commander {
public:
	an_Commander() : _status(nullptr), _dev(nullptr), \
		_cuinfo(nullptr),_caps(nullptr),_flag(0){

	}

	virtual ~an_Commander() {

	}

	virtual HRESULT execute(an_WFP_header * arg, an_xfs_result *lpresult) {
		return WFS_SUCCESS;
	}
	inline int get_flag() {
		return _flag;
	}
	inline void set_flag(const int f) {
		_flag = f;
	}
	an_Commander(const an_Commander& a) = delete;
	an_Commander& operator=(const an_Commander& a) = delete;
protected:
	an_xfs_caps	* _caps;	//sp������Ϣ
	an_xfs_status * _status;	//sp״̬��Ϣ
	an_xfs_cu_info * _cuinfo;	//Ǯ����Ϣ

	an_xfs_dev	* _dev;		//�豸����
	volatile int _flag;		//��ǰ����ִ������־��Ŀǰֻ��wfpclose ʹ�ã�1ʱ�˳�
};