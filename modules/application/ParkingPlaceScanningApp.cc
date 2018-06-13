#include <cmath>

#include <simtime.h>

#include "ParkingPlaceScanningApp.h"
#include "../messages/CarStatusMessage_m.h"
#include "../messages/ScanDataMessage_m.h"
#include "../messages/ScanDataACKMessage_m.h"
#include "../messages/ScanRequestMessage_m.h"
#include "../messages/ScanResultMessage_m.h"

Define_Module(ParkingPlaceScanningApp);

int ParkingPlaceScanningApp::numCars = 0;

ParkingPlaceScanningApp::ParkingPlaceScanningApp() {
	std::cout << "ParkingPlaceScanningApp: constructor" << std::endl;
	reportStatusMsg = new cMessage();
	scanTriggerMsg = new cMessage();
	numCars++;
	
	std:: cout << "# num cars # "<< simTime() << " # global # "<< numCars << std::endl;
}

ParkingPlaceScanningApp::~ParkingPlaceScanningApp() {
	std::cout << "ParkingPlaceScanningApp: destructor" << std::endl;
	numCars--;
	
	std:: cout << "# num cars # "<< simTime() << " # global # "<< numCars << std::endl;
}

void ParkingPlaceScanningApp::initialize(int stage) {
	std::cout << "ParkingPlaceScanningApp: initialize stage: " << stage << std::endl;
	if (stage == 0) {
		toDecisionMaker = findGate("toDecisionMaker");
		fromDecisionMaker = findGate("fromDecisionMaker");
		
		mobility = dynamic_cast<Veins::TraCIMobility *>(getParentModule()->getSubmodule("veinsmobility"));
		ASSERT(mobility);
		
		// Determine server name
		SERVER = "server" + getParentModule()->par("masterId").str();
		
		// Determine parking probability parameter
		PARKING_PROBABILITY = par("parkingProbability").longValue();
		std::cout << "Parking Probability: " << PARKING_PROBABILITY << std::endl;
		
		// Choose car mode
		if (dblrand() < PARKING_PROBABILITY) {
			mode = PARKING;
		} else {
			mode = NORMAL;
		}
		
		// Schedule status reporting
		scheduleAt(simTime() + uniform(0, 1), reportStatusMsg);
		
		// Schedule scan, it is actually perfoemed only when scan until is in the future
		scheduleAt(simTime() + uniform(0, 1), scanTriggerMsg);
		
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
	
	std:: cout << "### " << getId() << " at " << SERVER << " reporting status: position: " << position << " heading: " << heading << " road: " << road << " speed:" << speed << std::endl;
	
	std:: cout << "# car scanning # "<< simTime() << " # " << getId() << " # "<< (bool)(scanUntil < simTime().dbl()) << std::endl;
	
	CarStatusMessage *statusMsg = new CarStatusMessage();
	statusMsg->setName("Car status message");
	
	statusMsg->setId(getId().c_str());
	statusMsg->setMode(mode);
	statusMsg->setRoad(road.c_str());
	statusMsg->setHeading(heading);
	statusMsg->setPosition(position);
	statusMsg->setSpeed(speed);
	statusMsg->setServer(SERVER.c_str());
	
	statusMsg->setByteLength(STATUS_SIZE);
	
	statusMsg->setNetworkType(LTE);
	statusMsg->setDestinationAddress(SERVER.c_str());
	statusMsg->setSourceAddress(getId().c_str());
	
	send(statusMsg, toDecisionMaker);
}

void ParkingPlaceScanningApp::scan() {
	if(simTime() > scanUntil) {
		return;
	}
	
	std:: cout << "### " << getId() << " sending scan data" << std::endl;
	
	ScanDataMessage *scanMsg = new ScanDataMessage();
	scanMsg->setName("Scan data");
	scanMsg->setByteLength(SCAN_SIZE / SCAN_SPLIT);
	scanMsg->setDataPosition(mobility->getCurrentPosition());
	scanMsg->setDataTimestamp(simTime().dbl());
	scanMsg->setPart(scanCounter++);
	scanMsg->setForwarded(false);
	scanCounter %= SCAN_SPLIT;
	
	scanMsg->setNetworkType(LTE);
	scanMsg->setDestinationAddress(SERVER.c_str());
	scanMsg->setSourceAddress(getId().c_str());
	
	send(scanMsg, toDecisionMaker);
}

void ParkingPlaceScanningApp::handleMessage(cMessage *msg) {
	if (msg->isSelfMessage()) {
		if(msg == reportStatusMsg) {
			reportStatus();
			scheduleAt(simTime() + STATUS_DELAY_S, reportStatusMsg);
		} else if (msg == scanTriggerMsg) {
			if(canSendScan) {
				scan();
				canSendScan = false;
			}
			scheduleAt(SimTime(simTime()) + SimTime(SCAN_DELAY_MS / SCAN_SPLIT, SIMTIME_MS), scanTriggerMsg);// TODO: terminate trigger
		} else {
			std::cerr << "Unknown self message received by " << getId() << std::endl;
			delete msg;
		}
	} else {
		HeterogeneousMessage *heterogenousMessage = dynamic_cast<HeterogeneousMessage *>(msg);
		if(heterogenousMessage) {
			std::cout << getId() <<  " received message " << msg->getFullName() << " from " << heterogenousMessage->getSourceAddress() << " transmission: " << heterogenousMessage->getArrivalTime() - heterogenousMessage->getCreationTime() << std::endl;
		}
		
		ScanRequestMessage *requestMsg = dynamic_cast<ScanRequestMessage *>(msg);
		if(requestMsg) {
			std::cout << "### "<< getId() << " received scan request until " << requestMsg->getUntil() << std::endl;
			scanUntil = requestMsg->getUntil();
			scanCounter = 0;
			canSendScan = true;
		}
		
		ScanResultMessage *resultMsg = dynamic_cast<ScanResultMessage *>(msg);
		if(resultMsg) {
			double lattency = (simTime() - resultMsg->getDataTimestamp()).dbl();
			
			Coord dataPos = resultMsg->getDataPosition();
			Coord curPos = mobility->getCurrentPosition();
			double curHeading = mobility->getAngleRad();
			
			double distance = curPos.distance(dataPos);
			Coord diff = dataPos - curPos;
			double dataHeading = std::atan2(diff.x, diff.y);
			
			if(std::abs(dataHeading - curHeading) > M_PI) {
				distance *= -1;
			}
			
			std:: cout << getId() << " received scan result message (lattency, distance+ahead): " << lattency << " " << distance << std::endl;
			
			std:: cout << "# EtE lattency # "<< simTime() << " # " << getId() << " # "<< lattency << std::endl;
			std:: cout << "# EtE distance # "<< simTime() << " # " << getId() << " # "<< distance << std::endl;
			
			std::cout << "Cur Pos: " << curPos << " dataPos" << dataPos << std::endl;
		}
		
		ScanDataACKMessage *scanACKMsg = dynamic_cast<ScanDataACKMessage *>(msg);
		if(scanACKMsg) {
			std:: cout << "### "<< getId() << " received scan ACK" << std::endl;
			canSendScan = true;
		}
		
		delete msg;
	}
}
