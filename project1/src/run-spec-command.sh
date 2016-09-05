#!/bin/bash

PIN_ROOT=/home/gciotto/pin-3.0-76991-gcc-linux
PIN_TOOL=inscount2
PROJECT_DIR=/home/gciotto/workspace/MO601-117136/Projeto-1/item-c
TOOL_ROOT=${PIN_ROOT}/source/tools/ManualExamples/obj-intel64/${PIN_TOOL}.so

${PIN_ROOT}/pin -t ${TOOL_ROOT} -o ${PROJECT_DIR}/${PIN_TOOL}.tmp.log -- $1

