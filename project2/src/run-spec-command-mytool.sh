#!/bin/bash

PIN_ROOT=/home/gciotto/pin-3.0-76991-gcc-linux
PROJECT_DIR=/home/gciotto/MO601-117136/project2/src
TOOL_NAME=page_simulator_tool
TOOL_ROOT=${PROJECT_DIR}/obj-intel64/${TOOL_NAME}.so

${PIN_ROOT}/pin -t ${TOOL_ROOT} -o ${PROJECT_DIR}/${TOOL_NAME}.tmp.log -- $1

