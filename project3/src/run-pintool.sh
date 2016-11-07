#!/bin/bash

# Author: Gustavo CIOTTO PINTON RA 117136
# Computer Architecture II MO601B

PROJECT_DIR=${PWD}
PINBALL_DIR=/home/gciotto/INTcpu2006-pinpoints-w100M-d30M-m10
SNIPER_DIR=/home/gciotto/sniper
PIN_ROOT=/home/gciotto/pinplay-drdebug-pldi2016-3.0-pin-3.0-76991-gcc-linux

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

			CMD="$PIN_ROOT/pin -xyzzy -reserve_memory ${pinball_prefix}.address -t ${PROJECT_DIR}/obj-intel64/${TOOL_NAME}.so -o ${PROJECT_DIR}/${1}/${pinball_name}.log_ref_${SUFFIX} -replay -replay:basename ${pinball_prefix} -- $PIN_ROOT/extras/pinplay/bin/intel64/nullapp"

			${SNIPER_DIR}/run-sniper -- ${CMD}
	
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

elif [ "$1" = "i1" ]; then

	benchmarks=( 400.perlbench 401.bzip2 403.gcc 429.mcf 445.gobmk 456.hmmer )

elif [ "$1" = "i2" ]; then

	benchmarks=( 458.sjeng 462.libquantum 464.h264ref 471.omnetpp 473.astar 483.xalancbmk )

elif [ "$1" = "fp1" ]; then

	benchmarks=( 410.bwaves 416.gamess 433.milc 434.zeusmp 436.cactusADM 437.leslie3d 444.namd 447.dealII )

elif [ "$1" = "fp2" ]; then

	benchmarks = ( 450.soplex 453.povray 454.calculix 459.GemsFDTD 465.tonto 470.lbm 481.wrf 482.sphinx3 )

else
	
	benchmarks=( $1 )

fi

# Iterates only directories, which are the benchmarks
for d in ${benchmarks[@]}; do
	echo $d
	runPintoolForBenchmark "${d}"
done
