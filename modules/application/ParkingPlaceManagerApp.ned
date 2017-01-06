package cz.cuni.d3s.parkingplacescanning.modules.application;

import inet.applications.IUDPApp;

simple ParkingPlaceManagerApp like IUDPApp {
	parameters:
		int localPort = default(4242);
	gates:
		input udpIn @labels(UDPControlInfo/up);
		output udpOut @labels(UDPControlInfo/down);
}
