#!/bin/bash

# Author: Gustavo CIOTTO PINTON RA 117136
# Computer Architecture II MO601B

PROJECT_DIR=${PWD}
PINBALL_DIR=/home/gciotto/INTcpu2006-pinpoints-w100M-d30M-m10
SNIPER_DIR=/home/gciotto/sniper

echo $PROJECT_DIR

function runPintoolForBenchmark {
		
	mkdir ${PROJECT_DIR}/${1} &> /dev/null	

	benchmark="${1##*.}"

	for INPUT in ${PINBALL_DIR}/*${benchmark}*/ ; do

		echo ${INPUT}

		for pinball in $(find ${INPUT} -type f -name "*${benchmark}*.address"); do
		
			pinball_prefix=${pinball%.address}
			pinball_name=$(basename ${pinball_prefix})
			echo ${pinball_name}

			mkdir  ${PROJECT_DIR}/${1}/${pinball_name} &> /dev/null

			${SNIPER_DIR}/run-sniper --pinballs=${INPUT}${pinball_name} -d  ${PROJECT_DIR}/${1}/${pinball_name} -n 2 --roi
	
		done
	done

}

#benchmarks=( 400.perlbench 401.bzip2 403.gcc 429.mcf )
benchmarks=( 403.gcc )

# Iterates only directories, which are the benchmarks
for d in ${benchmarks[@]}; do
	echo $d
	runPintoolForBenchmark "${d}"
done
