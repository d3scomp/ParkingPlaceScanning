#include <simtime.h>

#include "ParkingPlaceScanningApp.h"
#include "../messages/CarStatusMessage_m.h"
#include "../messages/ScanDataMessage_m.h"
#include "../messages/ScanRequestMessage_m.h"

Define_Module(ParkingPlaceScanningApp);

void ParkingPlaceScanningApp::initialize(int stage) {
	if (stage == 0) {
		toDecisionMaker = findGate("toDecisionMaker");
		fromDecisionMaker = findGate("fromDecisionMaker");
		
		mobility = dynamic_cast<Veins::TraCIMobility *>(getParentModule()->getSubmodule("veinsmobility"));
		ASSERT(mobility);
		
		// Choose car mode
		if (dblrand() < 0.2) {
			mode = PARKING;
		} else {
			mode = NORMAL;
		}
		
		// Schedule status reporting
		scheduleAt(simTime() + uniform(0, 1), &reportStatusMsg);
		
		// Schedule scan, it is actually perfoemed only when scan until is in the future
		scheduleAt(simTime() + uniform(0, 1), &scanTriggerMsg);
		
		std::cout << getId() << ": INITIALIZED" << std::endl;
	}
}

std::string ParkingPlaceScanningApp::getId() {
	return mobility->getExternalId();
}

void ParkingPlaceScanningApp::reportStatus() {
	const std::string road = mobility->getRoadId();
	const Coord position = mobility->getCurrentPosition();
	const double heading = mobility->getAngleRad();
	const double speed = mobility->getSpeed();
	
	std:: cout << "### " << getId() << " reporting status: position: " << position << " heading: " << heading << " road: " << road << " speed:" << speed << std::endl;
	
	CarStatusMessage *statusMsg = new CarStatusMessage();
	statusMsg->setName("Car status message");
	
	statusMsg->setId(getId().c_str());
	statusMsg->setMode(mode);
	statusMsg->setRoad(road.c_str());
	statusMsg->setHeading(heading);
	statusMsg->setPosition(position);
	statusMsg->setSpeed(speed);
	
	statusMsg->setByteLength(STATUS_SIZE);
	
	statusMsg->setNetworkType(LTE);
	statusMsg->setDestinationAddress("server");
	statusMsg->setSourceAddress(getId().c_str());
	
	send(statusMsg, toDecisionMaker);
}

void ParkingPlaceScanningApp::callForScan() {
	// Call for parking assistance
	std::cout << getId() << ": Calling for assistance" << std::endl;
	HeterogeneousMessage *callMessage = new HeterogeneousMessage();
	callMessage->setNetworkType(DSRC);
	callMessage->setName("DSCR call for assistance message");
	callMessage->setByteLength(10);		
	callMessage->setDestinationAddress("0"); // BROADCAST
	callMessage->setSourceAddress(getId().c_str());
	send(callMessage, toDecisionMaker);
	
	HeterogeneousMessage* serverMessage = new HeterogeneousMessage();
	serverMessage->setName("LTE call for assistance message");
	serverMessage->setByteLength(10);
	serverMessage->setNetworkType(LTE);
	serverMessage->setDestinationAddress("server");
	serverMessage->setSourceAddress(getId().c_str());
	send(serverMessage, toDecisionMaker);
}

void ParkingPlaceScanningApp::scan() {
	if(simTime() > scanUntil) {
		return;
	}
	
	std:: cout << "### " << getId() << " sending scan data" << std::endl;
	
	ScanDataMessage *scanMsg = new ScanDataMessage();
	scanMsg->setName("Scan data");
	scanMsg->setByteLength(SCAN_SIZE / SCAN_SPLIT);
	scanMsg->setPosition(mobility->getCurrentPosition());
	scanMsg->setDataTimestamp(simTime().dbl());
	
	scanMsg->setNetworkType(LTE);
	scanMsg->setDestinationAddress("server");
	scanMsg->setSourceAddress(getId().c_str());
	
	send(scanMsg, toDecisionMaker);
}

void ParkingPlaceScanningApp::handleMessage(cMessage *msg) {
	if (msg->isSelfMessage()) {
		if(msg == &reportStatusMsg) {
			reportStatus();
			scheduleAt(simTime() + STATUS_DELAY_S, &reportStatusMsg);
		} else if (msg == &callForScanMsg) {
			callForScan();
			scheduleAt(simTime() + CALL_DELAY_S, &callForScanMsg);
		} else if (msg == &scanTriggerMsg) {
			scan();
			scheduleAt(SimTime(simTime()) + SimTime(SCAN_DELAY_MS / SCAN_SPLIT, SIMTIME_MS), &scanTriggerMsg);// TODO: terminate trigger
		} else {
			std::cerr << "Unknown self message received by " << getId() << std::endl;
		}
	} else {
		HeterogeneousMessage *testMessage = dynamic_cast<HeterogeneousMessage *>(msg);
		std::cout << "Received message " << msg->getFullName() << " from " << testMessage->getSourceAddress() << std::endl;
		
		ScanRequestMessage *requestMsg = dynamic_cast<ScanRequestMessage *>(msg);
		if(requestMsg) {
			std::cout << "received scan request until " << requestMsg->getUntil() << std::endl;
			scanUntil = requestMsg->getUntil();
		}
	}
}
