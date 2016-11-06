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

            echo -n "Processing ${pinball_name}... "

			${SNIPER_DIR}/run-sniper --pinballs=${INPUT}${pinball_name} -d  ${PROJECT_DIR}/${1}/${pinball_name} -c gainestown > ${PROJECT_DIR}/${1}/${pinball_name}/sniper_cmd.out
	
            echo "Done"

		done
	done

}

if [ "$1" = "int" ]; then

	benchmarks=( 400.perlbench 401.bzip2 403.gcc 429.mcf 445.gobmk 456.hmmer 458.sjeng 462.libquantum 464.h264ref 471.omnetpp 473.astar 483.xalancbmk )

elif [ "$1" = "fp" ]; then

	benchmarks=( 410.bwaves 416.gamess 433.milc 434.zeusmp 436.cactusADM 437.leslie3d 444.namd 447.dealII 450.soplex 453.povray 454.calculix 459.GemsFDTD 465.tonto 470.lbm 481.wrf 482.sphinx3 )

elif [ "$1" = "all" ]; then

	benchmarks=( 400.perlbench 401.bzip2 403.gcc 429.mcf 445.gobmk 456.hmmer 458.sjeng 462.libquantum 464.h264ref 471.omnetpp 473.astar 483.xalancbmk 410.bwaves 416.gamess 433.milc 434.zeusmp 436.cactusADM 437.leslie3d 444.namd 447.dealII 450.soplex 453.povray 454.calculix 459.GemsFDTD 465.tonto 470.lbm 481.wrf 482.sphinx3 )

else
	
	benchmarks=( $1 )

fi

# Iterates only directories, which are the benchmarks
for d in ${benchmarks[@]}; do
	echo $d
	runPintoolForBenchmark "${d}"
done
