#include "modules/application/CarApp.h"

Define_Module(CarApp);

void CarApp::initialize(int stage) {
	if (stage == 0) {
		toDecisionMaker = findGate("toDecisionMaker");
		fromDecisionMaker = findGate("fromDecisionMaker");
		
		mobility = dynamic_cast<Veins::TraCIMobility *>(getParentModule()->getSubmodule("veinsmobility"));
		ASSERT(mobility);
		
		// Choose car mode
		if (dblrand() < 0.1) {
			mode = PARKING;
		} else {
			mode = NORMAL;
		}
		
		// Schedule parking assistance request
		scheduleAt(simTime() + uniform(0, 1), new cMessage("CallForAssistance"));
	}
}

void CarApp::handleMessage(cMessage *msg) {
if (msg->isSelfMessage()) {
		/*
		 * Send a message to a random node in the network. Note that only the most necessary values
		 * are set. Size of the message have to be set according to the real message (aka your used
		 * .msg file). The values here are just a temporary placeholder.
		 */
		HeterogeneousMessage *testMessage = new HeterogeneousMessage();
		testMessage->setNetworkType(DONTCARE);
		testMessage->setName("Heterogeneous Test Message");
		testMessage->setByteLength(10);

		/* choose a random other car to send the message to */
		Veins::TraCIScenarioManager* manager = Veins::TraCIScenarioManagerAccess().get();
		std::map<std::string, cModule*> hosts = manager->getManagedHosts();
		std::map<std::string, cModule*>::iterator it = hosts.begin();
		std::advance(it, intrand(hosts.size()));
		std::string destination("node[" + it->first + "]");
		std::cout << "Sending message to " << destination << std::endl;
		testMessage->setDestinationAddress(destination.c_str());

		/* Finish the message and send it */
		testMessage->setSourceAddress(mobility->getExternalId().c_str());
		send(testMessage, toDecisionMaker);

		/*
		 * At 25% of the time send also a message to the main server. This message is sent via LTE
		 * and is then simply handed to the decision maker.
		 */
		if(dblrand() < 0.25){
			std::cout << "Sending message also to server" << std::endl;
			HeterogeneousMessage* serverMessage = new HeterogeneousMessage();
			serverMessage->setName("Server Message Test");
			testMessage->setByteLength(10);
			serverMessage->setNetworkType(LTE);
			serverMessage->setDestinationAddress("server");
			serverMessage->setSourceAddress(mobility->getExternalId().c_str());
			send(serverMessage, toDecisionMaker);
		}

		scheduleAt(simTime() + 1, new cMessage("Send"));
	} else {
		HeterogeneousMessage *testMessage = dynamic_cast<HeterogeneousMessage *>(msg);
		std::cout << "Received message " << msg->getFullName() << " from " << testMessage->getSourceAddress() << std::endl;
	}
}
