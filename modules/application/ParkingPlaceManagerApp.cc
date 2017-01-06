#include "ParkingPlaceManagerApp.h"

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
	HeterogeneousMessage* heterogeneousMessage = dynamic_cast<HeterogeneousMessage*>(msg);
	if(heterogeneousMessage){
		std::string sourceAddress = heterogeneousMessage->getSourceAddress();
		std::cout << "Received LTE Message: " << msg->getFullName() << " from: " << sourceAddress << std::endl;

		// Server replies with a simple message. Note that no additional parameters (like exact
		// message size) are set and therefore transmission will more likely succeed. If you use
		// this function set it correctly to get realistic results.
		HeterogeneousMessage *reply = new HeterogeneousMessage("Server Reply");
		IPv4Address address = manager->getIPAddressForID(sourceAddress);
		reply->setSourceAddress("server");
		std::cout << "Sending Message back to " << address << std::endl;
		socket.sendTo(reply, address, 4242);
	}
	delete msg;
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
