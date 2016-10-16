#!/bin/bash

# This script runs 10 SPEC CPU2006 benchmarks with PIN tool in order
# to evaluate memory access.
#
# Possible inputs:
#	- int : executes only SPECINT benchmarks
#	- fp : executes only SPECFP benchmarks
#	- set 1, 2 pr 3 : executes 1/3 of the benchmarks
#	- all : executes all 10 benchmarks
#	- benchmark : executes that benchmark
#	- toy : runs only toy benchmark
# Author: Gustavo CIOTTO PINTON RA 117136
# Computer Architecture II MO601B

SPEC_DIR=/home/gciotto/SPEC2006
PIN_DIR=/home/gciotto/pinplay-drdebug-pldi2016-3.0-pin-3.0-76991-gcc-linux
TOOL_NAME=page_simulator_tool
export PIN_ROOT=${PIN_DIR}
export PROJECT_DIR=${PWD}
PINBALL_DIR=${PROJECT_DIR}/cpu2006_pinballs
SUFFIX=16_10

echo $PROJECT_DIR

function runPintoolForBenchmark {
		
	mkdir ${PROJECT_DIR}/${1} &> /dev/null	

	benchmark="${1##*.}"

	for pinball in $(find ${PROJECT_DIR}/*/cpu2006-${benchmark}-ref-1.pp/ -type f -name "*${benchmark}*.address"); do
		
		pinball_prefix=${pinball%.address}
		pinball_name=$(basename ${pinball_prefix})
		echo ${pinball_name}

		$PIN_ROOT/pin -xyzzy -reserve_memory ${pinball_prefix}.address -t ${PROJECT_DIR}/obj-intel64/${TOOL_NAME}.so -o ${PROJECT_DIR}/${1}/${pinball_name}.log_ref_${SUFFIX} -replay -replay:basename ${pinball_prefix} -- $PIN_ROOT/extras/pinplay/bin/intel64/nullapp
	
	done

}

# Build page_simulator_tool pintool, according to the pin manual page. 
echo "Compiling 'page_simulator_tool.cpp'"
(cd ${PROJECT_DIR} && make dir obj-intel64/${TOOL_NAME}.so)

if [ "$1" = "int" ]; then

	benchmarks=( 400.perlbench 401.bzip2 403.gcc 429.mcf )

elif [ "$1" = "fp" ]; then

	benchmarks=( 410.bwaves 434.zeusmp 436.cactusADM 459.GemsFDTD 481.wrf 433.milc )

elif [ "$1" = "all" ]; then

	benchmarks=( 410.bwaves 429.mcf 434.zeusmp 436.cactusADM 459.GemsFDTD 400.perlbench 401.bzip2 481.wrf 433.milc 403.gcc )

elif [ "$1" = "toy" ]; then

	benchmarks=( )

elif [ "$2" = "1" ]; then

	benchmarks=( 410.bwaves 429.mcf 434.zeusmp 436.cactusADM )

elif [ "$2" = "2" ]; then

	benchmarks=( 459.GemsFDTD 400.perlbench 401.bzip2 )

elif [ "$2" = "3" ]; then

	benchmarks=( 481.wrf 433.milc 403.gcc )

else
	
	benchmarks=( $1 )

fi


# Iterates only directories, which are the benchmarks
for d in ${benchmarks[@]}; do
	echo $d
	runPintoolForBenchmark "${d}"
done

# run toy benchmark 
mkdir ${PROJECT_DIR}/toy &> /dev/null
echo "Compiling 'toy-benchmark.cpp'"
g++ -O2 -o ${PROJECT_DIR}/toy_b ${PROJECT_DIR}/toy-benchmark.cpp

echo "Running 'toy-benchmark.cpp' with pin"
$PIN_ROOT/pin -t ${PROJECT_DIR}/obj-intel64/${TOOL_NAME}.so -o  ${PROJECT_DIR}/toy/toy.log_${SUFFIX} -- ${PROJECT_DIR}/toy_b

# systemctl suspend

