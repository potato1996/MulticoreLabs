# Lab1 Baseline Version(Version 0)

This is the baseline parallel(multithread) version of TSM.

This version simply divide the work as even as possible, 
use the same algorithm as serial version, and finally
gather up the result.

Since different threads do NOT share the shortest path they
got, thus the "user time" would be larger than serial version.

Shared Var      | NONE

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
