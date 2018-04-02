#!/bin/bash
#Author: Dayou Du (dayoudu@nyu.edu)

PROGRAMPATH=$1
CITYNUM=$2
INPUTFILE=$3
TESTNUM=$4

echo "PROGRAM=<$PROGRAMPATH> CITINUM=<$CITYNUM> INPUTFILE=<$INPUTFILE>"

INS="`seq 1 ${CITYNUM}`"
TEST="`seq 1 ${TESTNUM}`"

for t in ${INS}; do
	for n in ${TEST}; do
		echo "$PROGRAMPATH $CITYNUM $t $INPUTFILE"
		time ${PROGRAMPATH} ${CITYNUM} ${t} ${INPUTFILE}
	done
done
