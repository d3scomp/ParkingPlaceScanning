 #!/bin/sh

rou_template=prague.rou.xml.template
sumo_template=prague.sumo.cfg.template
omnet_template=omnetpp.ini.template
sumo_port=8888

if [ "$#" -ne 7 ]; then
  echo "Provide 7 positional parameters: CAR_PROBABILITY_FROM, CAR_PROBILITY_TO, CAR_PROBABILITY_STEP, PARKING_PROBABILITY_FROM, PARKING_PROBABILITY_TO, PARKING_PROBABILITY_STEP, RUNS as params" >&2
  exit 1
fi

car_from=${1}
car_to=${2}
car_step=${3}
parking_from=${4}
parking_to=${5}
parking_step=${6}
runs=${7}


echo "Generate configs with car probability from ${car_from} to ${car_to} with step ${car_step}"
echo "and parking probability from ${parking_from} to ${parking_to} with step ${parking_step}"
echo "and ${runs} run multiplicity ?"
echo "Press ENTER to proceed...";
read -r;

for i in $(seq -w ${runs}); do
	for parking_probability in $(seq -w ${parking_from} ${parking_step} ${parking_to}); do
		for car_probability in $(seq -w ${car_from} ${car_step} ${car_to}); do
		#	echo ${p};
			sumo_port=$((${sumo_port} + 1))
		#	echo ${sumo_port}
			
			rou_file=prague.rou.${car_probability}.xml
			sumo_file=prague.sumo.${car_probability}.cfg
			cat ${rou_template} | sed "s/{PROBABILITY}/${car_probability}/" > ${rou_file}
			cat ${sumo_template} | sed "s/{PROBABILITY}/${car_probability}/" > ${sumo_file}
			
			echo;
			sumo_file=$(echo "$sumo_file" | sed 's/\//\\\//')
			cat ${omnet_template} | sed "s/{SUMO_CONFIG}/${sumo_file}/;s/{SUMO_PORT}/${sumo_port}/;s/{PARKING_PROBABILITY}/${parking_probability}/" > omnetpp-car-${car_probability}-parking-${parking_probability}-run-${i}.ini
		done;
	done;
done;
