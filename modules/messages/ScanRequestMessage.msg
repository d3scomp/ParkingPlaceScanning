cplusplus {{
#include "veins/modules/heterogeneous/messages/HeterogeneousMessage_m.h"
}}

class HeterogeneousMessage;

message ScanRequestMessage extends HeterogeneousMessage {
	double until;
	string car;
	string carServer;
	string requestingServer;
} 
