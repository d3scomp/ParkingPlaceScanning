#!/bin/sh

base=$(pwd)

for log in *.logs; do
	cd ${base}/${log};
	echo "Extracting in $(pwd)";
	
	xzcat log.txt.xz | grep "^# EtE lattency #" | cut -d "#" -f 3,4,5 --output-delimiter=" " | sed 's/[[:space:]]\+/ /g;s/^[[:space:]]//' > log.ete-lattency.txt
	xzcat log.txt.xz | grep "^# EtE distance #" | cut -d "#" -f 3,4,5 --output-delimiter=" " | sed 's/[[:space:]]\+/ /g;s/^[[:space:]]//' > log.ete-distance.txt
	xzcat log.txt.xz | grep "^# num cars #" | cut -d "#" -f 3,5 --output-delimiter=" " | sed 's/[[:space:]]\+/ /g;s/^[[:space:]]//' > log.numcars.txt
	xzcat log.txt.xz | grep "^# car scanning #" | cut -d "#" -f 3,4,5 --output-delimiter=" " | sed 's/[[:space:]]\+/ /g;s/^[[:space:]]//' > log.carscanning.txt
	
	xzcat log.txt.xz | grep "^# server processing queue #" | cut -d "#" -f 3,4,5 --output-delimiter=" " | sed 's/[[:space:]]\+/ /g;s/^[[:space:]]//' > log.serverqueue.txt
	
	xzcat log.txt.xz | grep "^# parking probability #" | cut -d "#" -f 3,4,5 --output-delimiter=" " | sed 's/[[:space:]]\+/ /g;s/^[[:space:]]//' > log.parkingprobability.txt
done;


# Extract simulation performance
#grep -R "simsec/sec" *.logs | sed 's/[ \t][ \t]*/ /g' | cut -d " " -f 4 | sed 's/simsec\/sec=//' > simsec-sec.txt

