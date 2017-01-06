#ifndef PARKINGPLACESCANNINGAPP_H
#define PARKINGPLACESCANNINGAPP_H

#include "veins/modules/heterogeneous/messages/HeterogeneousMessage_m.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/base/utils/SimpleLogger.h"

/**
 * Car operation mode
 */
enum CarMode {
	NORMAL, /// Car is driving from A to B, normal traffic
	PARKING /// Car is willing to park, will request assitance from others
};

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
};

#endif // PARKINGPLACESCANNINGAPP_H
