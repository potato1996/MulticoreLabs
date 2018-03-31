#!/bin/bash
#Author: Dayou Du (dayoudu@nyu.edu)

PROGRAMPATH=$1
CITYNUM=$2
INPUTFILE=$3

echo "PROGRAM=<$PROGRAMPATH> CITINUM=<$CITYNUM> INPUTFILE=<$INPUTFILE>"

INS="`seq 1 ${CITYNUM}`"
TESTNUM="`seq 1 20`"

for t in ${INS}; do
	for n in ${TESTNUM}; do
		echo "$PROGRAMPATH $CITYNUM $t $INPUTFILE"
		time ${PROGRAMPATH} ${CITYNUM} ${t} ${INPUTFILE}
	done
done
