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
public:
	static double constexpr PARKING_PROBABILITY = 0.2;
	
	ParkingPlaceScanningApp();
	~ParkingPlaceScanningApp();

protected:
	int toDecisionMaker;
	int fromDecisionMaker;
	Veins::TraCIMobility* mobility;
	
	CarMode mode = NORMAL;

	virtual void initialize(int stage);
	virtual void handleMessage(cMessage *msg);
	
	std::string getId();

private:
	std::string SERVER;
	
	const uint64_t CALL_DELAY_S = 1;
	
	const uint64_t STATUS_DELAY_S = 1;
	/// Status message size
	const size_t STATUS_SIZE = 48;
	
	/// Interframe delay @ 24 FPS
	const uint64_t SCAN_DELAY_MS = 42; 
	/// 648x480 @ 24bit color raw
	//const uint64_t SCAN_SIZE = 640*480*3; 
	/// 648x480 @ 24bit color, 30% jpeg, sample image
	const uint64_t SCAN_SIZE = 27685;
	/// Split each frame into N packets
	const uint64_t SCAN_SPLIT = 10; 
	
	double scanUntil = 0;
	bool canSendScan = false;
	
	

	
	cMessage *reportStatusMsg;
	cMessage *scanTriggerMsg;
	
	void reportStatus();
	void callForScan();
	void scan();
};

#endif // PARKINGPLACESCANNINGAPP_H
