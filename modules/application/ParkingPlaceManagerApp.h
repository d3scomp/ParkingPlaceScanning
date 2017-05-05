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

#include "../messages/ScanDataMessage_m.h"

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
		std::string server; // Server the car is connetced to
		
		std::string toString() const {
			std::stringstream stream;

			stream << name << ": mode: " << mode << " pos: " << position << " road: " << road << " heading: " << heading << " speed: " << speed;

			return stream.str();
		}
	};
	
	struct ScanServerForward {
		std::string server;
		double until;
	};

public:
	static const uint64_t ENSEMBLE_PERIOD_MS = 1000; // How often to pair parking cars and scanning cars
	static const uint64_t SCAN_REQUEST_DURATION_MS = 3000; // How log should the the scanning car send scan data until asked again
	static const uint64_t SCAN_LOOKAHEAD_MS = 5000; // How long does it take the car to park, ideally scan from position that will be reached in this time
	static const uint64_t RECORD_VALIDITY_MS = 5000; // How long to remember cars
	static const uint64_t SCAN_PROCESSING_TIME_MS = 50;
	static const uint64_t SCAN_PROCESSING_PARALLEL = 1;
	
	static const std::vector<std::string> SERVERS;
	
	static const uint32_t PORT = 4242;

	
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
	std::map<std::string, std::string> scanToRequester; // scanner car -> scan requesting car
	std::map<std::string, std::list<ScanServerForward> > scanToServer; // scanner car -> server managing requesting car
	
	std::vector<std::string> getLocalRequestingCars(std::string scanningCar);
	
	void dumpRecords();
	
	void ensemble();
	
	void sendInitiateScan(const CarRecord &where);
	void sendScanDataACK(std::string to);
	
	void process();
	std::queue<ScanDataMessage*> toProcess;
	
	const char* getServerName();
	
	cMessage *ensembleMsg;
	cMessage *processingMsg;
	
};

#endif /* PARKINGPLACEMANAGERAPP_H */
