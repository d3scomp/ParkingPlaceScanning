#ifndef PARKINGPLACECOMMON_H_
#define PARKINGPLACECOMMON_H_

#include <string>


/**
 * Car operation mode
 */
enum CarMode {
	NORMAL, /// Car is driving from A to B, normal traffic
	PARKING /// Car is willing to park, will request assitance from others
};

#endif // PARKINGPLACECOMMON_H_
