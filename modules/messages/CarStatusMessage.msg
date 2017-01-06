cplusplus {{
#include "veins/modules/heterogeneous/messages/HeterogeneousMessage_m.h"
#include "veins/base/utils/Coord.h"
}}

class HeterogeneousMessage;
class noncobject Coord;

message CarStatusMessage extends HeterogeneousMessage {
	string id;
	Coord position;
	double heading;
	string road;
}
