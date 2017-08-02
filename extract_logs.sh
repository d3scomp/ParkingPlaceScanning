#!/bin/sh

base=$(pwd)

for log in *.logs; do
	cd ${base}/${log};
	echo "Extracting in $(pwd)";
	
	cat log.txt | grep "^# EtE lattency #" | cut -d "#" -f 3,4,5 --output-delimiter=" " | sed 's/[[:space:]]\+/ /g;s/^[[:space:]]//' > log.ete-lattency.txt
	cat log.txt | grep "^# EtE distance #" | cut -d "#" -f 3,4,5 --output-delimiter=" " | sed 's/[[:space:]]\+/ /g;s/^[[:space:]]//' > log.ete-distance.txt
	cat log.txt | grep "^# num cars #" | cut -d "#" -f 3,5 --output-delimiter=" " | sed 's/[[:space:]]\+/ /g;s/^[[:space:]]//' > log.numcars.txt
	cat log.txt | grep "^# car scanning #" | cut -d "#" -f 3,4,5 --output-delimiter=" " | sed 's/[[:space:]]\+/ /g;s/^[[:space:]]//' > log.carscanning.txt
	cat log.txt | grep "^# server processing queue #" | cut -d "#" -f 3,4,5 --output-delimiter=" " | sed 's/[[:space:]]\+/ /g;s/^[[:space:]]//' > log.serverqueue.txt
	
done;
