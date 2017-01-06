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
	
	cMessage reportStatusMsg;
	cMessage callForScanMsg;
	
	void reportStatus();
	void callForScan();
};

#endif // PARKINGPLACESCANNINGAPP_H
