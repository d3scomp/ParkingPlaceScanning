cplusplus {{
#include "veins/modules/heterogeneous/messages/HeterogeneousMessage_m.h"
#include "veins/base/utils/Coord.h"
}}

class HeterogeneousMessage;
class noncobject Coord;

message ScanResultMessage extends HeterogeneousMessage {
	Coord dataPosition;
	double dataTimestamp;
} 
