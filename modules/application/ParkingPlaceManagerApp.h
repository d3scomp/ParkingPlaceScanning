#ifndef PARKINGPLACEMANAGERAPP_H
#define PARKINGPLACEMANAGERAPP_H

#include <omnetpp.h>
#include "ApplicationBase.h"
#include "INETDefs.h"
#include "UDPSocket.h"
#include "IPv4Address.h"
#include "veins/modules/heterogeneous/messages/HeterogeneousMessage_m.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/base/utils/SimpleLogger.h"

#include "ParkingPlaceCommon.h"

class ParkingPlaceManagerApp: public ApplicationBase {
protected:
	UDPSocket socket;
	Veins::TraCIScenarioManager* manager;
	
	struct CarRecord {
		std::string name;
		CarMode mode;
		Coord position;
		std::string road;
		double heading;
	};

public:
	ParkingPlaceManagerApp();
	virtual ~ParkingPlaceManagerApp();

	virtual int numInitStages() const {
		return 4;
	}
	virtual void initialize(int stage);
	
	virtual void finish();
	virtual void handleMessageWhenUp(cMessage *msg);
	
	virtual bool handleNodeStart(IDoneCallback *doneCallback);
	virtual bool handleNodeShutdown(IDoneCallback *doneCallback);
	virtual void handleNodeCrash();

private:
	std::map<std::string, CarRecord> records;
	
	void dumpRecords();
	
};

#endif /* PARKINGPLACEMANAGERAPP_H */
