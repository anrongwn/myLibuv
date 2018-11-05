#pragma once

#include <XFSAPI.H>
#include <XFSCIM.H>

#include <string>
#include <sstream>
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
//#include <cereal\archives\json.hpp>
//#include <cereal\types\array.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\vector.hpp>
//#include <cereal\types\list.hpp>
//#include <cereal\types\map.hpp>

#pragma pack(push, 1)

struct an_wfs_cim_cash_in_start : public _wfs_cim_cash_in_start {
	an_wfs_cim_cash_in_start() {
		usTellerID = 0;
		bUseRecycleUnits = FALSE;
		fwOutputPosition = 0;
		fwInputPosition = 0;
	}
	an_wfs_cim_cash_in_start(const LPWFSCIMCASHINSTART lp) {
		usTellerID = lp->usTellerID;
		bUseRecycleUnits = lp->bUseRecycleUnits;
		fwOutputPosition = lp->fwOutputPosition;
		fwInputPosition = lp->fwInputPosition;
	}

	~an_wfs_cim_cash_in_start() {

	}

	template<typename Archive>
	void serialize(Archive & ar) {
		ar(usTellerID, \
			bUseRecycleUnits, \
			fwOutputPosition, \
			fwInputPosition
		);
	}
};

#pragma pack(pop)