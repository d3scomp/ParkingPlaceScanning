#include "modules/application/CarApp.h"

using Veins::TraCIMobilityAccess;
using Veins::AnnotationManagerAccess;

Define_Module(CarApp);

void CarApp::initialize(int stage) {
	BaseWaveApplLayer::initialize(stage);
	if (stage == 0) {
		mobility = TraCIMobilityAccess().get(getParentModule());
		traci = mobility->getCommandInterface();
		traciVehicle = mobility->getVehicleCommandInterface();
		annotations = AnnotationManagerAccess().getIfExists();
		ASSERT(annotations);
	}
	
	// Choose car mode
	if (dblrand() < 0.1) {
		mode = PARKING;
	} else {
		mode = NORMAL;
	}
}

void CarApp::onBeacon(WaveShortMessage* wsm) {
}

void CarApp::onData(WaveShortMessage* wsm) {
}

void CarApp::sendMessage(std::string blockedRoadId) {
}
void CarApp::receiveSignal(cComponent* source, simsignal_t signalID, cObject* obj, cObject* details) {
}
void CarApp::handleParkingUpdate(cObject* obj) {
}

void CarApp::handlePositionUpdate(cObject* obj) {
}
void CarApp::sendWSM(WaveShortMessage* wsm) {
}
