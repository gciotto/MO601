#!/bin/bash

# This script runs all SPEC CPU2006 benchmarks with PIN tool in order
# to count how many instructions are executed.
# Each benchmark is run with the following command
# 'runspec --config=project1.cfg --iterations=1 --size=base --tune=base --noreportable benchmark',
# where project1.cfg configures the tune parameter as 'base' and output_root as the directory
# of this project.
#
# Author: Gustavo CIOTTO PINTON RA 117136
# Computer Architecture II MO601B

SPEC_DIR=/home/gciotto/SPEC2006
export PIN_DIR=/home/gciotto/pin-3.0-76991-gcc-linux
PROJECT_DIR=${PWD}
PIN_TOOL=inscount2

echo $PROJECT_DIR

function runPintoolForBenchmark {
		
	mkdir ${PROJECT_DIR}/${1}	
	
	runspec --config=project1c --size=ref --iterations=1 --tune=base --noreportable ${1}
	
	mv ${PROJECT_DIR}/${PIN_TOOL}.tmp.log ${PROJECT_DIR}/${1}/${PIN_TOOL}_.log_ref_2
	
}

# Source shrc file. To do it, we need to be in the same directory as the file
cd ${SPEC_DIR}
source shrc

# Build inscount_tls pintool, according to the pin manual page. 'inscount2' presents the best performance.
echo "Compiling 'inscount2.cpp'"
( cd ${PIN_DIR}/source/tools/ManualExamples && make dir obj-intel64/${PIN_TOOL}.so )

# Iterates only directories, which are the benchmarks
for d in ${SPEC_DIR}/benchspec/CPU2006/*/ ; do
	
	benchmarks=${d##*2006/}
	runPintoolForBenchmark "${benchmarks::-1}"
done


