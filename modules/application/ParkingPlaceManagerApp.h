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
	
	class CarRecord {
	public:
		SimTime timestamp;
		std::string name;
		CarMode mode;
		Coord position;
		std::string road;
		double heading;
		double speed;
		
		std::string toString() const {
			std::stringstream stream;

			stream << name << ": mode: " << mode << " pos: " << position << " road: " << road << " heading: " << heading << " speed: " << speed;

			return stream.str();
		}
	};

public:
	const uint64_t ENSEMBLE_PERIOD_MS = 1000; // How often to pair parking cars and scanning cars
	const uint64_t SCAN_REQUEST_DURATION_MS = 3000; // How log should the the scanning car send scan data until asked again
	const uint64_t SCAN_LOOKAHEAD_MS = 5000; // How long does it take the car to park, ideally scan from position that will be reached in this time
	const uint64_t RECORD_VALIDITY_MS = 5000; // How long to remember cars
	
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
	
	void ensemble();
	
	cMessage ensembleMsg;
	
};

#endif /* PARKINGPLACEMANAGERAPP_H */
