 #!/bin/sh

template=prague.rou.xml.template
outdir=prague.rou

if [ "$#" -ne 3 ]; then
  echo "Provide FROM and TO and STEP as params" >&2
  exit 1
fi

from=${1}
to=${2}
step=${3}

echo "Generate route configs with probability from ${from} to ${to} with step ${step} ? Enter to proceed...";
read -r;

mkdir -p ${outdir}

for p in $(seq ${from} ${step} ${to}); do
	echo ${p};
	
	file=${outdir}/prague.rou.${p}.xml
	cat ${template} | sed "s/{PROBABILITY}/${p}/" > ${file}
done;
