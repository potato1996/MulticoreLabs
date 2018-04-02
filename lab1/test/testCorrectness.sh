#!/bin/bash
#Author: Dayou Du (dayoudu@nyu.edu)

CITYNUM=$1
TESTNUM=$2

INS="`seq 1 ${CITYNUM}`"
TEST="`seq 1 ${TESTNUM}`"

for t in ${INS}; do
	for n in ${TEST}; do
		echo "./tsm $t"
		./tsm $t
		echo "./tsmoptimal $t cities$t.txt"
		./tsmoptimal $t cities$t.txt
		./runTest.sh ../src/version0/ptsm $t cities$t.txt 3
		./runTest.sh ../src/version1/ptsm $t cities$t.txt 3
		./runTest.sh ../src/version2/ptsm $t cities$t.txt 3
	done
done

