#!/bin/bash

# Author: Gustavo CIOTTO PINTON RA 117136
# Computer Architecture II MO601B

PROJECT_DIR=/home/gciotto/MO601-117136/project3/src
PINBALL_DIR=/home/gciotto/cpu2006_pinballs
SNIPER_DIR=/home/gciotto/sniper
SNIPER_CFG=article
#SNIPER_FLAGS="--roi-script -s roi-icount:0:100000000:30000000"
SNIPER_FLAGS=""

SUFFIX_IDEAL=ideal_article_pinballs_w0_d1B
SUFFIX_NORMAL=article_pinballs_w0_d1B

echo $PROJECT_DIR

function runPintoolForBenchmark {
		
	benchmark="${1##*.}"

    echo $benchmark

    mkdir ${PROJECT_DIR}/${SUFFIX_IDEAL} &> /dev/null
    mkdir ${PROJECT_DIR}/${SUFFIX_NORMAL} &> /dev/null

    mkdir ${PROJECT_DIR}/${SUFFIX_IDEAL}/${1} &> /dev/null
    mkdir ${PROJECT_DIR}/${SUFFIX_NORMAL}/${1} &> /dev/null

	for INPUT in ${PINBALL_DIR}/*${benchmark}*/ ; do

    INPUT=${INPUT::-1}
	echo ${INPUT}

	for pinball in $(find ${INPUT}/pinball_short.pp/ -type f -name "*.address"); do
      
            DEST_PATH_IDEAL=${PROJECT_DIR}/${SUFFIX_IDEAL}/${1}/${INPUT##*/}
            DEST_PATH_NORMAL=${PROJECT_DIR}/${SUFFIX_NORMAL}/${1}/${INPUT##*/}

            echo ${DEST_PATH_IDEAL}
            echo ${DEST_PATH_NORMAL}

            mkdir ${DEST_PATH_IDEAL} &> /dev/null
            mkdir ${DEST_PATH_NORMAL} &> /dev/null

			pinball_prefix=${pinball%.address}
			pinball_name=$(basename ${pinball_prefix})
			echo ${pinball_name}

			mkdir  ${PROJECT_DIR}/${1}_${SUFFIX}/${pinball_name} &> /dev/null

       		echo -n "Processing ${pinball_name} IDEAL... "
			${SNIPER_DIR}/run-sniper  --pinballs=${pinball_prefix} -c ideal-${SNIPER_CFG} ${SNIPER_FLAGS} -d ${DEST_PATH_IDEAL} > ${DEST_PATH_IDEAL}/sim.output
            cat  ${DEST_PATH_IDEAL}/sim.output

       		echo -n "Processing ${pinball_name} NORMAL... "
			${SNIPER_DIR}/run-sniper  --pinballs=${pinball_prefix} -c ${SNIPER_CFG} ${SNIPER_FLAGS} -d ${DEST_PATH_NORMAL} > ${DEST_PATH_NORMAL}/sim.output
            cat  ${DEST_PATH_NORMAL}/sim.output

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
