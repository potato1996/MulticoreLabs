# Lab1 Version1

This is parallel(multithread) version1 of TSM.

This version shared `minDis`-the shortest path they got,
but donot unroll the first two loops.

Since different threads DO share the shortest path they
got, thus the "user time" would be close to serial version.
But accessing the shared variable without race condition
brings overheads.

Shared Var      | minDis_s
Loop Unrolling  | NO

## Prerequistites

g++-6.2.0

(with openmp support)

## How to Compile

type `make`

## How to Run

ptsm <num-cities> <num-threads> <input-file-path>

## Options

We have three MACRO options defined in `Common.h`

MAXCITIES  | The maximum city numbers (default 10, as given in problem description)

DEBUG      | If set, check the result correctness and print messages

USE_MULTI_THREAD | If set, use openmp version. Otherwise use serial version.
