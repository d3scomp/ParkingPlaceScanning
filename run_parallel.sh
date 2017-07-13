#!/bin/sh

max_processes=3

inis=$(ls omnetpp-probability-*.ini)

echo ${inis}

(for ini in ${inis}; do echo ${ini}; done;) | while read ini; do
	echo ${ini};
	
	while (( $(jobs | wc -l) >= ${max_processes} )); do
		:
	done;
	
	logdir=${ini}.logs
	
	mkdir -p ${logdir}
	
	(echo "Running ${ini}"; ./ParkPlaceScanning.sh -u Cmdenv -f ${ini} > ${logdir}/log.txt ) &
	
done;
