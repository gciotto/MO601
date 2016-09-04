#!/bin/bash

PIN_ROOT=/home/gciotto/pin-3.0-76991-gcc-linux
PROJECT_DIR=/home/gciotto/workspace/MO601-117136/Projeto-1/item-d
TOOL_ROOT=${PROJECT_DIR}/obj-intel64/most-used-ins.so

${PIN_ROOT}/pin -t ${TOOL_ROOT} -o ${PROJECT_DIR}/most-used-ins.tmp.log -- $1

