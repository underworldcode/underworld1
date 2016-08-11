#!/bin/sh
cd /home/jmansour/work/code/stgUnderworld_vanilla/Underworld/SysTest/RegressionTests
mpiexec -np 2 /home/jmansour/work/code/stgUnderworld_vanilla/build/bin/StGermain Nightmare.xml /home/jmansour/work/code/stgUnderworld_vanilla/Underworld/SysTest/RegressionTests/lowres_expected/Nightmare-referenceTest/credo-analysis.xml --elementResI=10 --elementResJ=10 --elementResK=10
