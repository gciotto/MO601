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
PIN_DIR=/home/gciotto/pin-3.0-76991-gcc-linux
TOOL_NAME=page_simulator_tool
export PIN_ROOT=${PIN_DIR}
PROJECT_DIR=${PWD}

echo $PROJECT_DIR

function runPintoolForBenchmark {
		
	mkdir ${PROJECT_DIR}/${1}	

	runspec --config=project2 --size=test --tune=base --iterations=1 --noreportable ${1}

	mv ${PROJECT_DIR}/${TOOL_NAME}.tmp.log ${PROJECT_DIR}/${1}/${TOOL_NAME}.${1}.log_test_29_09

}

# Source shrc file. To do it, we need to be in the same directory as the file
cd ${SPEC_DIR}
source shrc

# Build inscount1 pintool, according to the pin manual page. 'inscount1' presents better performance
# than 'inscount0' because it considers inserting the analysis callback function after each trace instead of
# every intruction.
echo "Compiling 'page_simulator_tool.cpp'"
(cd ${PROJECT_DIR} && make dir obj-intel64/${TOOL_NAME}.so)


benchmarks=( 410.bwaves 429.mcf 434.zeusmp 436.cactusADM 459.GemsFDTD 400.perlbench 401.bzip2 403.gcc 481.wrf 433.milc )

# Iterates only directories, which are the benchmarks
for d in ${benchmarks[@]}; do
	echo $d
	runPintoolForBenchmark "${d}"
done

