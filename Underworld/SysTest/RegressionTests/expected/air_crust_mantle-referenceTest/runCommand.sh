#!/bin/sh
cd /home/julian/code/lukeVisit/working_uw/Underworld/InputFiles/melt_model
mpiexec -np 1 /home/julian/code/lukeVisit/working_uw/build/bin/StGermain air_crust_mantle.xml /home/julian/code/lukeVisit/working_uw/Underworld/InputFiles/melt_model/expected/air_crust_mantle-referenceTest/credo-analysis.xml  -options_file ./serialSolver.opts
