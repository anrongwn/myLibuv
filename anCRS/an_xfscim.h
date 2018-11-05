#pragma once
#include <XFSCIM.H>
#include <string>
#include <sstream>
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
//#include <cereal\archives\json.hpp>
#include <cereal\types\array.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>
//#include <cereal\types\list.hpp>
//#include <cereal\types\map.hpp>
#pragma pack(push, 1)
//
struct an_wfs_cim_position_info : public _wfs_cim_position_info {
	an_wfs_cim_position_info() {
		wPosition = WFS_CIM_POSINFRONT;
		wAdditionalBunches = 0;
		usBunchesRemaining = 0;
	}

	~an_wfs_cim_position_info() {

	}

	template<typename Archive>
	void serialize(Archive & ar) {
		ar(this->wPosition, this->wAdditionalBunches,this->usBunchesRemaining);
	}
};

struct an_wfs_hwerror {
	std::string lpszLogicalName;
	std::string lpszPhysicalName;
	std::string lpszWorkstationName;
	std::string lpszAppID;
	DWORD dwAction;
	DWORD dwSize;
	std::string lpbDescription;

	an_wfs_hwerror() {
		dwAction = 0;
		dwSize = 0;
	}
	~an_wfs_hwerror() {

	}

	template<typename Archive>
	void serialize(Archive & ar) {
		ar(lpszLogicalName, lpszPhysicalName, lpszWorkstationName, lpszAppID, \
			dwAction, dwSize, lpbDescription);
	}
};

struct an_wfs_devstatus {
	std::string lpszPhysicalName;
	std::string lpszWorkstationName;
	DWORD dwState;

	an_wfs_devstatus() {
		dwState = 0;
	}
	~an_wfs_devstatus() {

	}
	template<typename Archive>
	void serialize(Archive & ar) {
		ar(lpszPhysicalName, lpszWorkstationName, dwState);
	}
};























#pragma pack(pop)