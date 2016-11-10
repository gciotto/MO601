#!/bin/bash

# Author: Gustavo CIOTTO PINTON RA 117136
# Computer Architecture II MO601B

PROJECT_DIR=/home/gciotto/MO601-117136/project3/src
PINBALL_DIR=/home/gciotto/cpu2006_pinballs
SNIPER_DIR=/home/gciotto/sniper
#PIN_ROOT=/home/gciotto/pinplay-drdebug-pldi2016-3.0-pin-3.0-76991-gcc-linux

SUFFIX=1B

echo $PROJECT_DIR

function runPintoolForBenchmark {
		
	benchmark="${1##*.}"

    echo $benchmark

    mkdir ${PROJECT_DIR}/${1}_${SUFFIX} &> /dev/null

	for INPUT in ${PINBALL_DIR}/*${benchmark}*/ ; do

        INPUT=${INPUT::-1}
		echo ${INPUT}

		for pinball in $(find ${INPUT}/pinball_short.pp -type f -name "*.address"); do
      
            DEST_PATH=${PROJECT_DIR}/${1}_${SUFFIX}/${INPUT##*/}

            echo ${DEST_PATH}

        	mkdir ${DEST_PATH} &> /dev/null

			pinball_prefix=${pinball%.address}
			pinball_name=$(basename ${pinball_prefix})
			echo ${pinball_name}

			# mkdir  ${PROJECT_DIR}/${1}_${SUFFIX}/${pinball_name} &> /dev/null

       		echo -n "Processing ${pinball_name}... "

			CMD="${PIN_ROOT}pin -xyzzy -reserve_memory ${pinball_prefix}.address -t ${PROJECT_DIR}/obj-intel64/warmup.so  -replay -replay:basename ${pinball_prefix} -- $PIN_ROOT/extras/pinplay/bin/intel64/nullapp"

#			echo $CMD

#			python ${PROJECT_DIR}/pywrapper.py ${CMD}
#			${SNIPER_DIR}/run-sniper --roi -d ${PROJECT_DIR}/${1}/${pinball_name} -c gainestown  -- python ${PROJECT_DIR}/pywrapper.py "${CMD}"

			${SNIPER_DIR}/run-sniper  --pinballs=${pinball_prefix} -c gainestown -d ${DEST_PATH} > ${DEST_PATH}/sim.output
            cat  ${DEST_PATH}/sim.output

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

	benchmarks=( 450.soplex 453.povray 454.calculix 459.GemsFDTD 465.tonto 470.lbm 481.wrf 482.sphinx3 )

else

	benchmarks=( $1 )

fi

# Iterates only directories, which are the benchmarks
for d in ${benchmarks[@]}; do
	echo ${d}
	runPintoolForBenchmark "${d}"
done
