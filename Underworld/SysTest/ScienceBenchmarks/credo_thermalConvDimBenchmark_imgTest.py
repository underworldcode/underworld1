#! /usr/bin/env python
# Dimensional Thermal Convection with Buoyancy force benchmark
# This is just to quickly generate some images.

import os, copy
import credo_thermalConvDimBenchmark
import credo.jobrunner

#Nproc is key param for images
nproc = 2
tSuffix = "-imgTest-nproc_%d" % (nproc)

testSuite = copy.deepcopy(credo_thermalConvDimBenchmark.suite())
testSuite.suiteName += tSuffix
sciBTest = testSuite.sysTests[0]
sciBTest.testName += tSuffix
#Update all output paths ... should be better auto way to do this
oldBasePath = sciBTest.outputPathBase
sciBTest.outputPathBase += tSuffix
sciBTest.mSuite.outputPathBase = sciBTest.outputPathBase
for mRun in sciBTest.mSuite.runs:
    relSub = os.path.relpath(mRun.outputPath, oldBasePath)
    mRun.outputPath = os.path.join(sciBTest.outputPathBase, relSub)

#Now reduce timesteps
for mRun in sciBTest.mSuite.runs:
    mRun.simParams.nsteps = 3
    mRun.simParams.dumpevery = 1
    mRun.jobParams['nproc'] = nproc

#Scram the custom reporting - no images
# Remove out the middle steps of images
# TODO: code below won't work, as we now do this in one step ...
#for imgInfo in sciBTest.mSuite.modelImagesToDisplay:
#    del(imgInfo[1])
sciBTest.customReporting = None

if __name__ == "__main__":
    postProcFromExisting = True
    jobRunner = credo.jobrunner.defaultRunner()
    testResult, mResults = sciBTest.runTest(jobRunner, postProcFromExisting,
        createReports=True)
