#!/bin/bash

# Author: Gustavo CIOTTO PINTON RA 117136
# Computer Architecture II MO601B

PROJECT_DIR=/home/gciotto/MO601-117136/project4/src

PINBALLS=w100M_d30M
#PINBALLS=w0_d1B

SNIPER_DIR=/home/gciotto/sniper
SNIPER_CFG=article

SUFFIX_GENERAL=30_11
SUFFIX_4KB=4KB_article_pinballs_${PINBALLS}_${SUFFIX_GENERAL}
SUFFIX_4MB=4MB_article_pinballs_${PINBALLS}_${SUFFIX_GENERAL}
SUFFIX_IDEAL=ideal_article_pinballs_${PINBALLS}_${SUFFIX_GENERAL}

if [ "${PINBALLS}" == "w100M_d30M" ] ; then
  
    PINBALL_DIR=/home/gciotto/INTcpu2006-pinpoints-w100M-d30M-m10 # 100M warmup, 30M detailed mode pinballs 
    SNIPER_FLAGS="--roi-script -s roi-icount:0:100000000:30000000"
    PINBALL_SUBDIR=""

elif [ "${PINBALLS}" == "w0_d1B" ] ; then

    PINBALL_DIR=/home/gciotto/cpu2006_pinballs # 1B detailed region pinballs
    SNIPER_FLAGS=""
    PINBALL_SUBDIR=pinball_short.pp/
fi

echo $PROJECT_DIR

function runPintoolForBenchmark {
		
    benchmark="${1##*.}"

    echo $benchmark

    mkdir ${PROJECT_DIR}/${SUFFIX_4KB} &> /dev/null
    mkdir ${PROJECT_DIR}/${SUFFIX_4MB} &> /dev/null
    mkdir ${PROJECT_DIR}/${SUFFIX_IDEAL} &> /dev/null

    mkdir ${PROJECT_DIR}/${SUFFIX_4KB}/${1} &> /dev/null
    mkdir ${PROJECT_DIR}/${SUFFIX_4MB}/${1} &> /dev/null
    mkdir ${PROJECT_DIR}/${SUFFIX_IDEAL}/${1} &> /dev/null

    for INPUT in ${PINBALL_DIR}/*${benchmark}*/ ; do

    INPUT=${INPUT::-1}
    echo ${INPUT}

    DEST_PATH_4KB=${PROJECT_DIR}/${SUFFIX_4KB}/${1}/${INPUT##*/}
    DEST_PATH_4MB=${PROJECT_DIR}/${SUFFIX_4MB}/${1}/${INPUT##*/}
    DEST_PATH_IDEAL=${PROJECT_DIR}/${SUFFIX_IDEAL}/${1}/${INPUT##*/}

    echo ${DEST_PATH_4KB}
    echo ${DEST_PATH_4MB}

    mkdir ${DEST_PATH_4KB} &> /dev/null
    mkdir ${DEST_PATH_4MB} &> /dev/null
    mkdir ${DEST_PATH_IDEAL} &> /dev/null

    for pinball in $(find "${INPUT}/${PINBALL_SUBDIR}" -type f -name "*.address"); do
      
		pinball_prefix=${pinball%.address}
		pinball_name=$(basename ${pinball_prefix})
		echo ${pinball_name}

        mkdir ${DEST_PATH_4KB}/${pinball_name} &> /dev/null
        mkdir ${DEST_PATH_4MB}/${pinball_name} &> /dev/null
        mkdir ${DEST_PATH_IDEAL}/${pinball_name} &> /dev/null

       	echo "Processing ${pinball_name} 4KB... "
        ${SNIPER_DIR}/run-sniper  --pinballs=${pinball_prefix} -c ${SNIPER_CFG}_4KB ${SNIPER_FLAGS} -d ${DEST_PATH_4KB}/${pinball_name} > ${DEST_PATH_4KB}/${pinball_name}/sim.output
        cat  ${DEST_PATH_4KB}/${pinball_name}/sim.output

        echo "Processing ${pinball_name} 4MB... "
        ${SNIPER_DIR}/run-sniper  --pinballs=${pinball_prefix} -c ${SNIPER_CFG}_4MB ${SNIPER_FLAGS} -d ${DEST_PATH_4MB}/${pinball_name} > ${DEST_PATH_4MB}/${pinball_name}/sim.output
        cat  ${DEST_PATH_4MB}/${pinball_name}/sim.output

        echo "Processing ${pinball_name} IDEAL... "
        ${SNIPER_DIR}/run-sniper  --pinballs=${pinball_prefix} -c ideal-${SNIPER_CFG} ${SNIPER_FLAGS} -d ${DEST_PATH_IDEAL}/${pinball_name} > ${DEST_PATH_IDEAL}/${pinball_name}/sim.output
        cat  ${DEST_PATH_IDEAL}/${pinball_name}/sim.output

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
