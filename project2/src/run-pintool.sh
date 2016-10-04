#!/bin/bash

# This script runs all SPEC CPU2006 benchmarks with PIN tool in order
# to count how many instructions are executed.
# Each benchmark is run with the following command
# 'runspec --config=project1.cfg --iterations=1 --size=test --noreportable benchmark',
# where project1.cfg configures the tune parameter as 'base' and output_root as the directory
# of this project.
#
# Author: Gustavo CIOTTO PINTON RA 117136
# Computer Architecture II MO601B

SPEC_DIR=/home/gciotto/SPEC2006
PIN_DIR=/home/gciotto/pinplay-drdebug-pldi2016-3.0-pin-3.0-76991-gcc-linux
TOOL_NAME=page_simulator_tool
export PIN_ROOT=${PIN_DIR}
export PROJECT_DIR=${PWD}
PINBALL_DIR=${PROJECT_DIR}/cpu2006-pinballs-pinplay-1.1

echo $PROJECT_DIR

function runPintoolForBenchmark {
		
	mkdir ${PROJECT_DIR}/${1}	

	benchmark="${1##*.}"

	$PIN_ROOT/pin -xyzzy -reserve_memory ${PINBALL_DIR}/cpu2006-${benchmark}-ref-1/pinball.address -t ${PROJECT_DIR}/obj-intel64/${TOOL_NAME}.so -o ${PROJECT_DIR}/${1}/${1}.log_ref_03_10 -replay -replay:basename ${PINBALL_DIR}/cpu2006-${benchmark}-ref-1/pinball -- $PIN_ROOT/extras/pinplay/bin/intel64/nullapp

}

# Source shrc file. To do it, we need to be in the same directory as the file
cd ${SPEC_DIR}
source shrc

# Build inscount1 pintool, according to the pin manual page. 'inscount1' presents better performance
# than 'inscount0' because it considers inserting the analysis callback function after each trace instead of
# every intruction.
echo "Compiling 'page_simulator_tool.cpp'"
(cd ${PROJECT_DIR} && make dir obj-intel64/${TOOL_NAME}.so)

echo $1 = $2
if [ "$2" -eq "1" ]; then

	benchmarks=( 410.bwaves 429.mcf 434.zeusmp 436.cactusADM )

elif [ "$2" -eq "2" ]; then

	benchmarks=( 459.GemsFDTD 400.perlbench 401.bzip2 )

elif [ "$2" -eq "3" ]; then

	benchmarks=( 481.wrf 433.milc )

else
	
	benchmarks=( 410.bwaves 429.mcf 434.zeusmp 436.cactusADM 459.GemsFDTD 400.perlbench 401.bzip2 481.wrf 433.milc )

fi

# Iterates only directories, which are the benchmarks
for d in ${benchmarks[@]}; do
	echo $d
	runPintoolForBenchmark "${d}"
done

