#pragma once
/*
#include <XFSAPI.H>
#include <XFSCDM.H>
#include <XFSCIM.H>
*/

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

typedef unsigned long DWORD;
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

	an_wfs_hwerror(const an_wfs_hwerror&) = delete;
	an_wfs_hwerror& operator=(const an_wfs_hwerror&) = delete;

	template<typename Archive>
	void serialize(Archive & ar) {
		ar(lpszLogicalName, lpszPhysicalName, lpszWorkstationName, lpszAppID, \
			dwAction, dwSize, lpbDescription);
	}
};

#pragma pack(pop)