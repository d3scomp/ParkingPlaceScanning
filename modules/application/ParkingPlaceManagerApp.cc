#include "ParkingPlaceManagerApp.h"
#include "../messages/CarStatusMessage_m.h"
#include "ParkingPlaceCommon.h"

Define_Module(ParkingPlaceManagerApp);

ParkingPlaceManagerApp::ParkingPlaceManagerApp() {}

ParkingPlaceManagerApp::~ParkingPlaceManagerApp() {}

void ParkingPlaceManagerApp::initialize(int stage){
	ApplicationBase::initialize(stage);
	if(stage == 0){
		manager = Veins::TraCIScenarioManagerAccess().get();
		ASSERT(manager);
	}
}

void ParkingPlaceManagerApp::finish(){
}


void ParkingPlaceManagerApp::handleMessageWhenUp(cMessage *msg){
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
		
		CarRecord record = {status->getId(), static_cast<CarMode>(status->getMode()), status->getPosition(), status->getRoad(), status->getHeading()};
		records[record.name] = record;
		
		dumpRecords();
	}
	
	
	delete msg;
}

void ParkingPlaceManagerApp::dumpRecords() {
	std::cout << "### Records: " << std::endl;
	for(auto record: records) {
		std::cout << record.first << ": mode: " << record.second.mode << " pos: " << record.second.position << " road: " << record.second.road << " heading: " << record.second.heading << std::endl;
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
