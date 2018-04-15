#!/bin/bash
#Author: Dayou Du (dayoudu@nyu.edu)

PROGRAMPATH=$1
REFPROGRAMPATH=$2
GENERATEPROGRAMPATH=$3

CURR_PATH=`pwd`
INPUT_FOLDER=${CURR_PATH}/input
PROGOUT_FOLDER=${CURR_PATH}/out
REFOUT_FOLDER=${CURR_PATH}/refout

#program parameters
PROC_NUMS="1 2 10 20 40"
UNKNOWN_NUMS="10 100 1000 10000 100000"

#testing parameters
TEST_NUM=10

#load mpi module
module load mpi/openmpi-x86_64

echo "PROGRAM=<$PROGRAMPATH> REFPROGRAM=<$REFPROGRAMPATH> GENPROGRAM=<$GENERATEPROGRAMPATH>"

#clean up
rm -rf ${INPUT_FOLDER}
rm -rf ${PROGOUT_FOLDER}
rm -rf ${REFOUT_FOLDER}

mkdir ${INPUT_FOLDER}
mkdir ${PROGOUT_FOLDER}
mkdir ${REFOUT_FOLDER}

#generate inputs
cd ${INPUT_FOLDER}

for n in ${UNKNOWN_NUMS}; do
	${GENERATEPROGRAMPATH} ${n} 0.001
done

#generate refout

cd ${REFOUT_FOLDER}

for n in ${UNKNOWN_NUMS}; do
	${REFPROGRAMPATH} ${INPUT_FOLDER}/${n}.txt
done

#start testing

cd ${PROGOUT_FOLDER}

TESTS="`seq 1 ${TEST_NUM}`"

for p in ${PROC_NUMS}; do
	for n in ${UNKNOWN_NUMS}; do
		for t in ${TESTS}; do
			echo "NUM_PROCS = ${p}  NUM_UNKNOWNS = ${n}"
			time mpirun -n ${p} ${PROGRAMPATH} ${INPUT_FOLDER}/${n}.txt
		done
	done
	#check results
	for n in ${UNKNOWN_NUMS}; do
		diff ${REFOUT_FOLDER}/${n}.sol ${PROGOUT_FOLDER}/${n}.sol
	done
done
