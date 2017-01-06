cplusplus {{
#include "veins/modules/heterogeneous/messages/HeterogeneousMessage_m.h"
}}

class HeterogeneousMessage;

message ScanRequest extends HeterogeneousMessage {
	// position
	// heading
	double heading;
} 
