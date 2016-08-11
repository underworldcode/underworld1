#!/bin/sh
cd /home/julian/code/fresh_uw/Underworld/SysTest/RegressionTests
mpiexec -np 1 /home/julian/code/fresh_uw/build/bin/StGermain PoiseuilleFlow.xml /home/julian/code/fresh_uw/Underworld/SysTest/RegressionTests/expected/PoiseuilleFlow-referenceTest/credo-analysis.xml 
