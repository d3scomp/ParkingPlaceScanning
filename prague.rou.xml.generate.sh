 #!/bin/sh

rou_template=prague.rou.xml.template
sumo_template=prague.sumo.cfg.template
omnet_template=omnetpp.ini.template
sumo_port=8888

if [ "$#" -ne 3 ]; then
  echo "Provide FROM and TO and STEP as params" >&2
  exit 1
fi

from=${1}
to=${2}
step=${3}

echo "Generate route configs with probability from ${from} to ${to} with step ${step} ? Enter to proceed...";
read -r;

for p in $(seq ${from} ${step} ${to}); do
#	echo ${p};
	sumo_port=$((${sumo_port} + 1))
#	echo ${sumo_port}
	
	rou_file=prague.rou.${p}.xml
	sumo_file=prague.sumo.${p}.cfg
	cat ${rou_template} | sed "s/{PROBABILITY}/${p}/" > ${rou_file}
	cat ${sumo_template} | sed "s/{PROBABILITY}/${p}/" > ${sumo_file}
	
	echo;
	sumo_file=$(echo "$sumo_file" | sed 's/\//\\\//')
	cat ${omnet_template} | sed "s/{SUMO_CONFIG}/${sumo_file}/;s/{SUMO_PORT}/${sumo_port}/" > omnetpp-probability-${p}.ini
done;
