#include <math.h>

#include "ParkingPlaceManagerApp.h"
#include "../messages/CarStatusMessage_m.h"
#include "../messages/ScanRequestMessage_m.h"
#include "ParkingPlaceCommon.h"

Define_Module(ParkingPlaceManagerApp);

ParkingPlaceManagerApp::ParkingPlaceManagerApp() {}

ParkingPlaceManagerApp::~ParkingPlaceManagerApp() {}

void ParkingPlaceManagerApp::initialize(int stage){
	ApplicationBase::initialize(stage);
	if(stage == 0){
		manager = Veins::TraCIScenarioManagerAccess().get();
		ASSERT(manager);
		
		scheduleAt(simTime() + uniform(0, 1), &ensembleMsg);
	}
}

void ParkingPlaceManagerApp::finish(){
}


void ParkingPlaceManagerApp::handleMessageWhenUp(cMessage *msg){
	if(msg->isSelfMessage()) {
		if(msg == &ensembleMsg) {
			ensemble();
			scheduleAt(SimTime(simTime()) + SimTime(ENSEMBLE_PERIOD_MS, SIMTIME_MS), &ensembleMsg);
		}
		return;
	}
	
	// Handle generic heterogenous message
	HeterogeneousMessage* heterogeneousMessage = dynamic_cast<HeterogeneousMessage*>(msg);
	if(heterogeneousMessage){
		std::string sourceAddress = heterogeneousMessage->getSourceAddress();
		std::cout << "### Server received LTE Message: " << msg->getFullName() << " from: " << sourceAddress << std::endl;

		// Server replies with a simple message. Note that no additional parameters (like exact
		// message size) are set and therefore transmission will more likely succeed. If you use
		// this function set it correctly to get realistic results.
		HeterogeneousMessage *reply = new HeterogeneousMessage("Server Reply");
		IPv4Address address = manager->getIPAddressForID(sourceAddress);
		reply->setSourceAddress("server");
		std::cout << "Sending Message back to " << address << std::endl;
		socket.sendTo(reply, address, 4242);
	}
	
	// Handle status message
	CarStatusMessage *status = dynamic_cast<CarStatusMessage*>(msg);
	if(status) {
		std::cout << "### Server received status message from: " << status->getId() << " mode: " << static_cast<CarMode>(status->getMode()) << " pos: " << status->getPosition() << " head: " << status->getHeading() << " road: " << status->getRoad() << std::endl;
		
		CarRecord record = {simTime(), status->getId(), static_cast<CarMode>(status->getMode()), status->getPosition(), status->getRoad(), status->getHeading(), status->getSpeed()};
		records[record.name] = record;
	}
	
	delete msg;
}

void ParkingPlaceManagerApp::dumpRecords() {
	std::cout << "### Records: " << std::endl;
	for(auto record: records) {
		const CarRecord &car = record.second;
		//std::cout << record.first << ": mode: " << car.mode << " pos: " << car.position << " road: " << car.road << " heading: " << car.heading << " speed: " << car.speed << std::endl;
		std::cout << car.toString() << std::endl;
	}
	std::cout << "### End records: " << std::endl;
}

bool ParkingPlaceManagerApp::handleNodeStart(IDoneCallback *doneCallback){
	socket.setOutputGate(gate("udpOut"));
	int localPort = par("localPort");
	socket.bind(localPort);
	return true;
}

bool ParkingPlaceManagerApp::handleNodeShutdown(IDoneCallback *doneCallback){
	return true;
}

void ParkingPlaceManagerApp::handleNodeCrash(){}

void ParkingPlaceManagerApp::ensemble() {
	// Remove old car records
	std::vector<std::string> tooOld;
	for(auto record: records) {
		const CarRecord &car = record.second;
		if(car.timestamp > (simTime() + SimTime(RECORD_VALIDITY_MS, SIMTIME_MS))) {
			tooOld.push_back(car.name);
		}
	}
	for(auto key: tooOld) {
		records.erase(key);
	}
	
	dumpRecords();
	std::cout << "### Mapping cars to ensembles" << std::endl;
	
	// Erase old mapping
	scanToRequester.clear();
	
	// Determine mapping
	for(auto record: records) {
		const CarRecord &car = record.second;
		
		// Determine scanner for parking car
		if(car.mode == PARKING) {
			std::cout << car.name << " requests parking assistance" << std::endl;			
			//std::cout << "current position: " << car.position << " heading: " << car.heading << " speed: " << car.speed << std::endl;
			std::cout << car.toString() << std::endl;
			
			// Determine prefered scan position
			Coord scanPos = car.position;
			double lookahed_m = car.speed * SCAN_LOOKAHEAD_MS / 1000;
			scanPos.x += lookahed_m * cos(car.heading);
			scanPos.y += lookahed_m * sin(car.heading);
			std:: cout << "prefered scan position : " << scanPos << std::endl;
			
			// Determine scanner (closest car to prefered scan position)
			CarRecord closest = car;
			for(auto rec: records) {
				const CarRecord &c = rec.second;
	
				if(closest.position.distance(scanPos) > c.position.distance(scanPos)) {
					closest = c;
				}
			}
			std::cout << "determined scanner: " << closest.toString() << std::endl;
			scanToRequester[car.name] = closest.name;
			sendInitiateScan(closest.name);
		}
	}
}

void ParkingPlaceManagerApp::sendInitiateScan(std::string carId) {
	ScanRequestMessage *requestMsg = new ScanRequestMessage("Scan request");
	
	requestMsg->setDuration((simTime() + SimTime(SCAN_REQUEST_DURATION_MS, SIMTIME_MS)));
	
	requestMsg->setByteLength(1);
	requestMsg->setNetworkType(LTE);
	requestMsg->setDestinationAddress(carId.c_str());
	requestMsg->setSourceAddress("server");
	
	IPv4Address address = manager->getIPAddressForID(carId.c_str());
	socket.sendTo(requestMsg, address, 4242);
}
