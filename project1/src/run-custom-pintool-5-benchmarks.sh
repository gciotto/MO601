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
export PIN_ROOT=${PIN_DIR}
PROJECT_DIR=${PWD}

echo $PROJECT_DIR

function runPintoolForBenchmark {
		
	mkdir ${PROJECT_DIR}/${1}	

	runspec --config=project1d --size=ref --tune=base --iterations=1 --noreportable ${1}

	mv ${PROJECT_DIR}/most-used-ins.tmp.log ${PROJECT_DIR}/${1}/most-used-ins.${1}.log_ref

}

# Source shrc file. To do it, we need to be in the same directory as the file
cd ${SPEC_DIR}
source shrc

# Build my pintool, according to the pin manual page.
echo "Compiling 'most-used-ins.cpp'"
(cd ${PROJECT_DIR} && make dir obj-intel64/most-used-ins.so)


benchmarks=( perlbench bzip2 gcc mcf gobmk)

# Iterates the array containing the benchmarks selected for analysis
for d in ${benchmarks[@]}; do
	echo $d
	runPintoolForBenchmark "${d}"
done

