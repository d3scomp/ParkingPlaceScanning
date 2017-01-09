#ifndef PARKINGPLACESCANNINGAPP_H
#define PARKINGPLACESCANNINGAPP_H

#include "veins/modules/heterogeneous/messages/HeterogeneousMessage_m.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/base/utils/SimpleLogger.h"

#include "ParkingPlaceCommon.h"

/**
 * Car application module
 */
class ParkingPlaceScanningApp : public cSimpleModule {
protected:
	int toDecisionMaker;
	int fromDecisionMaker;
	Veins::TraCIMobility* mobility;
	
	CarMode mode = NORMAL;

	virtual void initialize(int stage);
	virtual void handleMessage(cMessage *msg);
	
	std::string getId();

private:
	const uint64_t CALL_DELAY_S = 1;
	const uint64_t STATUS_DELAY_S = 1;
	
	const uint64_t SCAN_DELAY_MS = 42; // @ 24 FPS
	//const uint64_t SCAN_SIZE = 640*480*3; // 648x480 @ 24bit color raw
	const uint64_t SCAN_SIZE = 27685; // 648x480 @ 24bit color, 30% jpeg, sample image
	const uint64_t SCAN_SPLIT = 10; // Split each frame into N packets

	
	cMessage reportStatusMsg;
	cMessage callForScanMsg;
	cMessage scanTriggerMsg;
	
	void reportStatus();
	void callForScan();
	void scan();
};

#endif // PARKINGPLACESCANNINGAPP_H
