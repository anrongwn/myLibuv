#pragma once
#include <XFSCDM.H>
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


struct an_crs_position
{
	WORD                  fwPosition;
	WORD                  fwShutter;
	WORD                  fwPositionStatus;
	WORD                  fwTransport;
	WORD                  fwTransportStatus;
	WORD                  fwJammedShutterPosition;

	an_crs_position() {

	}

	~an_crs_position() {

	}

	template<typename Archive>
	void serialize(Archive & ar) {
		ar(fwPosition, \
			fwShutter, \
			fwPositionStatus, \
			fwTransport, \
			fwTransportStatus, \
			fwJammedShutterPosition
		);
	}
};
#define AN_POSITION_SIZE	(3)

struct an_xfs_status {
	//cdm
	WORD                  fwDevice;
	WORD                  fwSafeDoor;
	WORD                  fwDispenser;
	WORD                  fwIntermediateStacker;
	an_crs_position		  lppPositions[AN_POSITION_SIZE];
	std::string           lpszExtra;
	DWORD                 dwGuidLights[WFS_CDM_GUIDLIGHTS_SIZE];
	WORD                  wDevicePosition;
	USHORT                usPowerSaveRecoveryTime;
	WORD                  wAntiFraudModule;

	//cim
	WORD                   fwAcceptor;
	WORD                   fwStackerItems;
	WORD                   fwBanknoteReader;
	BOOL                   bDropBox;
	WORD                   wMixedMode;

	an_xfs_status() {
		fwDevice = WFS_CIM_DEVOFFLINE;
		fwSafeDoor = WFS_CIM_DOORCLOSED;
		fwDispenser = WFS_CDM_DISPCUSTOP;
		fwIntermediateStacker = WFS_CIM_ISEMPTY;
		lppPositions[0].fwPosition= WFS_CIM_POSINTOP;
		lppPositions[1].fwPosition = WFS_CIM_POSOUTCENTER;
		lppPositions[2].fwPosition = WFS_CIM_POSOUTTOP;

		lpszExtra = "LastErrorCode=1001";
		lpszExtra += '\0';
		lpszExtra += "Desc=HW not connected.";
		lpszExtra += '\0';
		lpszExtra += "Remark=...";
		lpszExtra += '\0';
		lpszExtra += '\0';

		memset(dwGuidLights, WFS_CIM_GUIDANCE_NOT_AVAILABLE, \
			sizeof(DWORD)*WFS_CIM_GUIDLIGHTS_SIZE);;
		wDevicePosition = WFS_CIM_DEVICEINPOSITION;
		usPowerSaveRecoveryTime = 0;
		wAntiFraudModule = WFS_CIM_AFMOK;
		fwAcceptor = WFS_CIM_ACCCUSTOP;
		fwStackerItems = WFS_CIM_NOITEMS;
		fwBanknoteReader = WFS_CIM_BNROK;
		bDropBox = FALSE;
		wMixedMode = WFS_CIM_IPMMIXEDMEDIA;

	};
	~an_xfs_status() {
	};

	template<typename Archive>
	void serialize(Archive & ar) {
		ar(fwDevice, \
			fwSafeDoor, \
			fwDispenser, \
			fwIntermediateStacker, \
			lppPositions, \
			lpszExtra, \
			dwGuidLights, \
			wDevicePosition, \
			usPowerSaveRecoveryTime, \
			wAntiFraudModule, \
			fwAcceptor, \
			fwStackerItems, \
			fwBanknoteReader, \
			bDropBox, \
			wMixedMode
		);
	}
};


//
struct an_xfs_caps
{
	//cdm
	WORD                  wClass;
	WORD                  fwType;
	WORD                  wMaxDispenseItems;
	BOOL                  bCompound;
	BOOL                  bShutter;
	BOOL                  bShutterControl;
	WORD                  fwRetractAreas;
	WORD                  fwRetractTransportActions;
	WORD                  fwRetractStackerActions;
	BOOL                  bSafeDoor;
	BOOL                  bCashBox;
	BOOL                  bIntermediateStacker;
	BOOL                  bItemsTakenSensor;
	WORD                  fwPositions;
	WORD                  fwMoveItems;
	WORD                  fwExchangeType;
	std::string           lpszExtra;
	DWORD                 dwGuidLights[WFS_CIM_GUIDLIGHTS_SIZE];
	BOOL                  bPowerSaveControl;
	BOOL                  bPrepareDispense;
	BOOL                  bAntiFraudModule;
	DWORD                 dwItemInfoTypes;
	BOOL                  bBlacklist;

	//cim
	WORD                  wMaxCashInItems;
	BOOL                  bRefill;
	WORD                  fwIntermediateStacker;
	BOOL                  bItemsInsertedSensor;
	BOOL                  bCompareSignatures;
	BOOL                  bReplenish;
	WORD                  fwCashInLimit;
	WORD                  fwCountActions;
	BOOL                  bDeviceLockControl;
	WORD                  wMixedMode;
	BOOL                  bMixedDepositAndRollback;
	BOOL                  bDeplete;
	DWORD				  lpdwSynchronizableCommands[WFS_CIM_GUIDLIGHTS_SIZE];

	an_xfs_caps(){
		
		wClass = WFS_SERVICE_CLASS_CIM;
		fwType = WFS_CIM_SELFSERVICEBILL;
		wMaxCashInItems = 100;
		wMaxDispenseItems = 100;
		bCompound = TRUE;
		bShutter = TRUE;
		bShutterControl = TRUE;
		bSafeDoor = TRUE;
		bCashBox = FALSE;
		bRefill = FALSE;
		fwIntermediateStacker = 1;
		bItemsTakenSensor = TRUE;
		bItemsInsertedSensor = TRUE;
		fwPositions = WFS_CIM_POSINFRONT | WFS_CIM_POSOUTFRONT;
		fwMoveItems = WFS_CDM_FROMCU | WFS_CDM_TOCU | WFS_CDM_TOTRANSPORT | WFS_CDM_TOSTACKER;
		fwExchangeType = WFS_CIM_EXBYHAND | WFS_CIM_EXTOCASSETTES | \
			WFS_CIM_CLEARRECYCLER | WFS_CIM_DEPOSITINTO;
		fwRetractAreas = WFS_CIM_RA_RETRACT;
		fwRetractTransportActions = WFS_CIM_RETRACT;
		fwRetractStackerActions = WFS_CIM_RETRACT;
		lpszExtra = "SP_Version=1.00";
		lpszExtra += '\0';
		lpszExtra += "FW_Version=1.00";
		lpszExtra += '\0';
		lpszExtra += "BV_Version=6767";
		lpszExtra += '\0';
		lpszExtra += '\0';

		memset(dwGuidLights, WFS_CIM_GUIDANCE_NOT_AVAILABLE, \
			sizeof(DWORD)*WFS_CIM_GUIDLIGHTS_SIZE);
		dwItemInfoTypes = WFS_CIM_ITEM_SERIALNUMBER;
		bCompareSignatures = TRUE;
		bPowerSaveControl = TRUE;
		bReplenish = TRUE;
		fwCashInLimit = WFS_CIM_LIMITBYTOTALITEMS | WFS_CIM_LIMITBYAMOUNT;
		fwCountActions = WFS_CIM_COUNTINDIVIDUAL | WFS_CIM_COUNTALL;
		bDeviceLockControl = FALSE;
		wMixedMode = WFS_CIM_IPMMIXEDMEDIA;
		bMixedDepositAndRollback = TRUE;
		bPrepareDispense = TRUE;
		bAntiFraudModule = TRUE;
		bDeplete = FALSE;
		bBlacklist = TRUE;
		memset(lpdwSynchronizableCommands, 0x00, \
			sizeof(DWORD)*WFS_CIM_GUIDLIGHTS_SIZE);
			
	}
	~an_xfs_caps() {

	}

	template<typename Archive>
	void serialize(Archive & ar) {
		ar(wClass, \
			fwType, \
			wMaxDispenseItems, \
			bCompound, \
			bShutter, \
			bShutterControl, \
			fwRetractAreas, \
			fwRetractTransportActions, \
			fwRetractStackerActions, \
			bSafeDoor, \
			bCashBox, \
			bIntermediateStacker, \
			bItemsTakenSensor, \
			fwPositions, \
			fwMoveItems, \
			fwExchangeType, \
			lpszExtra, \
			dwGuidLights, \
			bPowerSaveControl, \
			bPrepareDispense, \
			bAntiFraudModule, \
			dwItemInfoTypes, \
			bBlacklist, \
			wMaxCashInItems, \
			bRefill, \
			fwIntermediateStacker, \
			bItemsInsertedSensor, \
			bCompareSignatures, \
			bReplenish, \
			fwCashInLimit, \
			fwCountActions, \
			bDeviceLockControl, \
			wMixedMode, \
			bMixedDepositAndRollback, \
			bDeplete, \
			lpdwSynchronizableCommands
		);
	}
};