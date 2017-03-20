#include <cmath>

#include "ParkingPlaceManagerApp.h"
#include "../messages/CarStatusMessage_m.h"
#include "../messages/ScanRequestMessage_m.h"
#include "../messages/ScanDataMessage_m.h"
#include "../messages/ScanResultMessage_m.h"
#include "ParkingPlaceCommon.h"

Define_Module(ParkingPlaceManagerApp);

const std::vector<std::string> ParkingPlaceManagerApp::SERVERS = std::vector<std::string>({"server1", "server2", "server3"});

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
		} else {
			ScanResultMessage *resultMsg = dynamic_cast<ScanResultMessage *>(msg);
			if(resultMsg) {
				std:: cout << "### " << getServerName() << " Processing time passed, redirecting result message to " << resultMsg->getDestinationAddress() << std::endl;
				IPv4Address address = manager->getIPAddressForID(resultMsg->getDestinationAddress());
				if(!address.isUnspecified()) {
					socket.sendTo(resultMsg, address, PORT);
				}
			} else {
				std::cout << "SERVER RECEIVED UNHLANDLED SELF MESSAGE" << std::endl;
				delete msg;
			}
		}
		return;
	}
	
	// Handle generic heterogenous message
	HeterogeneousMessage* heterogeneousMessage = dynamic_cast<HeterogeneousMessage*>(msg);
	if(heterogeneousMessage){
		std::string sourceAddress = heterogeneousMessage->getSourceAddress();
		std::cout << "### " << getServerName() << " received LTE Message: " << msg->getFullName() << " from: " << sourceAddress << " transmission: " << heterogeneousMessage->getArrivalTime() - heterogeneousMessage->getCreationTime() << std::endl;
	}
	
	// Handle status message
	CarStatusMessage *status = dynamic_cast<CarStatusMessage*>(msg);
	if(status) {
		std::cout << "### " << getServerName() << " received status message from: " << status->getId() << " mode: " << static_cast<CarMode>(status->getMode()) << " pos: " << status->getPosition() << " head: " << status->getHeading() << " road: " << status->getRoad() << std::endl;
		
		// Reate record for the car
		CarRecord record = {simTime(), status->getId(), static_cast<CarMode>(status->getMode()), status->getPosition(), status->getRoad(), status->getHeading(), status->getSpeed(), status->getServer()};
		records[record.name] = record;
		
		// Forward status to other servers
		if(strcmp(status->getServer(), getServerName()) == 0) {
			for(const std::string &server: SERVERS) {
				if(server != getServerName()) {
					std::cout << "#### Forwarding to " << server << std::endl;
					auto address = IPvXAddressResolver().resolve(server.c_str());
					std::cout << "Address: " << address << std::endl;
					socket.sendTo(status->dup(), address, PORT);
				}
			}
		}
		
		delete msg;
		return;
	}
	
	// Handle scan data
	ScanDataMessage *scan = dynamic_cast<ScanDataMessage *>(msg);
	if(scan) {
		std::cout << "Received scan data from position " << scan->getDataPosition() << " at " << scan->getDataTimestamp() << std::endl;
		
		// Schedule delivery of result to requesting car (if any)
		std::map<std::string, std::string>::iterator requester = scanToRequester.find(scan->getSourceAddress());
		if(requester != scanToRequester.end()) {
			// Create result message and deliver it with delay to self, it will be resend later to recipient
			ScanResultMessage *resultMsg = new ScanResultMessage("Scan result");
			
			resultMsg->setDataTimestamp(scan->getDataTimestamp());
			resultMsg->setDataPosition(scan->getDataPosition());
			
			resultMsg->setByteLength(64);
			resultMsg->setNetworkType(LTE);
			resultMsg->setDestinationAddress(requester->second.c_str());
			resultMsg->setSourceAddress(getServerName());
			
			// Send message to self, will be redirected afther processing time to requester
			scheduleAt(SimTime(simTime()) + SimTime(SCAN_PROCESSING_TIME_MS, SIMTIME_MS), resultMsg);
		}
		
		// Forward scan data to requesting server (if any)
		std::map<std::string, std::list<ScanServerForward> >::iterator requestingServer = scanToServer.find(scan->getSourceAddress());
		if(requestingServer != scanToServer.end()) {
			for(ScanServerForward &record: requestingServer->second) {
				if(record.until > simTime().dbl()) {
					auto address = IPvXAddressResolver().resolve(record.server.c_str());
					std::cout << "DEBUG: " << record.server << " addr:" << address <<std::endl;
					socket.sendTo(scan->dup(), address, PORT);
				}
			}
		}
		
		delete msg;
		return;
	}
	
	// Handle initiate scan from other servers
	ScanRequestMessage *request = dynamic_cast<ScanRequestMessage *>(msg);
	if(request) {
		std::cout << "Forwarding initiate scan request to car: " << request->getCar() << std::endl;
		request->setDestinationAddress(request->getCar());
		auto address = manager->getIPAddressForID(request->getCar());
		if(!address.isUnspecified()) {
			socket.sendTo(request->dup(), address, PORT);
		}
		
		// Remember mapping in order to forward results later
		scanToServer[request->getCar()].push_back({request->getRequestingServer(), request->getUntil()});
		
		delete msg;
		return;
	}
}

void ParkingPlaceManagerApp::dumpRecords() {
	std::cout << "### " << getServerName() << " Records: " << std::endl;
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
	std::cout << "### " << getServerName() << " Mapping cars to ensembles" << std::endl;
	
	// Erase old mapping
	scanToRequester.clear();
	
	// Determine mapping
	for(auto record: records) {
		const CarRecord &car = record.second;
		
		// Determine scanner for parking cars managed by this server
		if(car.mode == PARKING && car.server == getServerName()) {
			std::cout << car.name << " requests parking assistance" << std::endl;			
			//std::cout << "current position: " << car.position << " heading: " << car.heading << " speed: " << car.speed << std::endl;
			std::cout << car.toString() << std::endl;
			
			// Determine prefered scan position
			Coord scanPos = car.position;
			double lookahed_m = car.speed * SCAN_LOOKAHEAD_MS / 1000;
			scanPos.x += lookahed_m * std::cos(car.heading);
			scanPos.y += lookahed_m * std::sin(car.heading);
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
			scanToRequester[closest.name] = car.name;
			sendInitiateScan(closest);
		}
	}
}

void ParkingPlaceManagerApp::sendInitiateScan(const CarRecord &where) {
	ScanRequestMessage *requestMsg = new ScanRequestMessage("Scan request");
	
	double until = (simTime() + SimTime(SCAN_REQUEST_DURATION_MS, SIMTIME_MS)).dbl();
	std::cout << "### " << getServerName() << " requesting scan on " << where.name << " until " << until << std::endl;
 	requestMsg->setUntil(until);
	requestMsg->setCar(where.name.c_str());
	requestMsg->setCarServer(where.server.c_str());
	requestMsg->setRequestingServer(getServerName());
	requestMsg->setByteLength(32);
	requestMsg->setSourceAddress(getServerName());
	requestMsg->setNetworkType(LTE);
		
	if(where.server == getServerName()) {
		// Send message directly to car
		requestMsg->setDestinationAddress(where.name.c_str());
		auto address = manager->getIPAddressForID(where.name.c_str());
		if(!address.isUnspecified()) {
			socket.sendTo(requestMsg, address, PORT);
		}
	} else {
		// Send message to server with access to car
		requestMsg->setDestinationAddress(where.server.c_str());
		auto address = IPvXAddressResolver().resolve(where.server.c_str());
		if(!address.isUnspecified()) {
			socket.sendTo(requestMsg, address, PORT);
		}
	}
}

const char* ParkingPlaceManagerApp::getServerName() {
	return getParentModule()->getName();
}
