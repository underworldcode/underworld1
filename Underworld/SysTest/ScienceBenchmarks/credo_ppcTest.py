#! /usr/bin/env python
# PPC benchmark
# CAN ONLY BE RUN IN SERIAL!
 
import operator
import os
import itertools
from credo.modelrun import ModelRun, SimParams
from credo.modelsuite import ModelSuite, StgXMLVariant
import credo.modelsuite as msuite
import credo.jobrunner
import credo.reporting.standardReports as sReps
from credo.reporting import getGenerators
from thermalConvPostProc import *

mRun = ModelRun("PPCBenchmark", "PPCTest/ExtensionPPC.xml",
    "output/PPCBenchmark",
    simParams=SimParams(stoptime=0.625),
	nproc=1)
	
# Set to resolution needed to pass expected ranges
mRun.paramOverrides['elementResI'] = 64
mRun.paramOverrides['elementResJ'] = 32
	
def testPPC(mRun, mResult):
    #------------------------------------
    # Tests whether the PPC scaling works
    # accurately by comparing the analytic
    # length of extension (ie 10km) with
    # the actual final length
    # The final length is 2.625 given by:
    # Actual length of extension * scaling coefficient:
    # 1.0e3*6.25e-5 = 0.625 which is 2.625 total 
    #------------------------------------


    # The numerical solutions should be within this range:

    maxXCoord = 2.64
    minXCoord = 2.62

    PPCPass = 'no'

    #----------------------------------------
    mResult.readFrequentOutput()

    maxTime, maxTimeStep = mResult.freqOutput.getMax('Time')
    TimeStep = maxTimeStep - 1
    Time = mResult.freqOutput.getValueAtStep('Time', TimeStep)

    # Get values at right-most node ie in this instance node 64 which is at line 65:

    finalTS = "%s" % (TimeStep)
    filename = "output/PPCBenchmark/MaterialAtVelocityNodes_CPU0_" \
        + finalTS + ".dat"
    f1 = open(filename, 'r')
    array1 = f1.readlines()
    line1 = array1[65]
    line1 = line1.lstrip('[')
    line1 = line1.rstrip(']\n')
    print line1, line1.split('	')  # test
    array2 = [float(x) for x in line1.split('	')]
    xcoord = array2[2]
        
    print "At time %g farthest xcoord is %g\n" %\
            (Time, xcoord)

    if xcoord > minXCoord and xcoord < maxXCoord: 
            PPCPass = 'yes'	
            print "xcoord check passed within %f and %f\n"	%\
            (minXCoord, maxXCoord)
    else:
            print 'xcoord check FAILED'

if __name__ == "__main__":
    jobRunner = credo.jobrunner.defaultRunner()
    mResult = jobRunner.runModel(mRun)
    testPPC(mRun, mResult)
