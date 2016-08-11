#! /usr/bin/env python

# Rayleigh-taylor benchmark - lowres

import os
import copy
from credo.systest import *
import credo_rayTayBenchmark

from credo.analysis import modelplots
import credo.io.stgfreq as stgfreq
import credo.reporting.standardReports as sReps
from credo.reporting import getGenerators

elRes=16
tSuffix = "-lowres"
testSuite = copy.deepcopy(credo_rayTayBenchmark.suite())
testSuite.suiteName += tSuffix
for sysTest in testSuite.sysTests:
    sysTest.testName += tSuffix
    #sysTest.updateOutputPaths(sysTest.outputPathBase + tSuffix)
    oldBasePath = sysTest.outputPathBase
    sysTest.outputPathBase += tSuffix
    sysTest.mSuite.outputPathBase = sysTest.outputPathBase
    for mRun in sysTest.mSuite.runs:
        relSub = os.path.relpath(mRun.outputPath, oldBasePath)
        mRun.outputPath = os.path.join(sysTest.outputPathBase, relSub)
    sysTest.mSuite.runs[0].paramOverrides.update(
        {"elementResI":elRes, "elementResJ":elRes})
    # slacken output test requirements
    sysTest.testComps[0]['VRMS of first diapir'].tRange = (200, 260)
    # Add another for verification
    sysTest.testComps[0]['VRMS at t=1.0'] = OutputWithinRangeTC("Vrms",
        stgfreq.closestToSimTime, allowedRange=(0.0011,0.0012),
        opDict={'targTime':180.0})
    sysTest.customReporting = None

def customReporting(sciBTest, mResults):
    # This gets called immediately post the models being run.
    # First set up overall analysis images
    modelplots.plotOverAllRuns(mResults, 'Vrms', depName='Time',
        path=sciBTest.outputPathBase)
    sciBTest.mSuite.analysisImages = ["Vrms-multiRunTimeSeries.png"]
    # Now specific per-run images
    fStep = mResults[0].freqOutput.finalStep()
    dEvery = sciBTest.mSuite.runs[0].simParams.dumpevery
    lastImgStep = fStep / dEvery * dEvery
    vrmsPeakTime = sciBTest.testComps[0]['VRMS of first diapir'].actualTime
    vrmsPeakStep = mResults[0].freqOutput.getClosest('Time', vrmsPeakTime)[1]
    vrmsPeakImgStep = int(round(vrmsPeakStep / float(dEvery))) * dEvery
    vrmsPeakImgStep = min([vrmsPeakImgStep, lastImgStep])
    # Create an empty list of images to display
    sciBTest.mSuite.modelImagesToDisplay = [[] for runI in \
        range(len(sciBTest.mSuite.runs))]
    # Choose which model timestep images to display:- note that here we're
    #  programmatically choosing to show the Peak VRMS timestep.
    sciBTest.mSuite.modelImagesToDisplay[0] = [
        (10, "initial state"),
        (vrmsPeakImgStep, "near first VRMS peak at t=%f" % vrmsPeakTime),
        (lastImgStep, "")]
    # Here we just ask the CREDO reporting API to get Report Generators for
    # PDF (ReportLab) and RST (Restructured Text) output, and create a 
    # standard science benchmark report.
    for rGen in getGenerators(["RST", "ReportLab"], sciBTest.outputPathBase):
        sReps.makeSciBenchReport(sciBTest, mResults, rGen,
            os.path.join(sciBTest.outputPathBase, "%s-report.%s" %\
                (sciBTest.testName, rGen.stdExt)), imgPerRow=2)

testSuite.sysTests[0].setCustomReporting(customReporting)

def suite():
    return testSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(testSuite)
