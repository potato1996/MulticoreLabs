# Lab1 Version2

This is parallel(multithread) version2 of TSM.

This version shared `minDis`-the shortest path they got.
Also unroll the first two loops, which distribute the 
workloads more evenly

Since different threads DO share the shortest path they
got, thus the "user time" would be close to serial version.
But accessing the shared variable without race condition
brings overheads.

Shared Var      | minDis_s

Loop Unrolling  | YES

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
