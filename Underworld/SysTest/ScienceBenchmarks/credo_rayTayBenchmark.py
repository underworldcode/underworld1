#! /usr/bin/env python
# Rayleigh-taylor benchmark

import os
from credo.systest import *
from credo.modelrun import ModelRun, SimParams
import credo.jobrunner
from credo.analysis import modelplots
import credo.io.stgfreq as stgfreq
import credo.reporting.standardReports as sReps
from credo.reporting import getGenerators

testSuite = SysTestSuite("Underworld", "SciBench-RayTay")

# Test Configuration shorthand vars
elRes = 128

sciBTest = SciBenchmarkTest("RayleighTaylor-VanKekenBenchmark")
sciBTest.description = """This models the evolution of a sinusoidal
    Rayleigh-Taylor instability. It has a lower density fluid on the bottom
    of the box, and a higher density fluid above. The results are 
    benchmarked against P.E. van Keken et al, 'A comparison of methods for
    the modelling of thermochemical convection', <i>JGR</i>, 1997."""

testSuite.sysTests.append(sciBTest)
#Configure suite
mRun = ModelRun("RayleighTaylorOverturn",
    "RayleighTaylorBenchmark.xml",
    simParams=SimParams(dumpevery=10, cpevery=50, stoptime=300.0, nsteps=-2),
    paramOverrides={"elementResI":elRes, "elementResJ":elRes})
sciBTest.mSuite.addRun(mRun, "Ray tay benchmark single run at res %s^3" % elRes)
#Configure test components
sciBTest.setupEmptyTestCompsList()
sciBTest.addTestComp(0, "VRMS of first diapir",
    OutputWithinRangeTC("Vrms", stgfreq.maxOp, 
        allowedRange=(2.8e-3, 3.2e-3), tRange=(200,220)))

def customReporting(sciBTest, mResults):
    # This gets called immediately post the models being run.
    # First set up overall analysis images
    modelplots.plotOverAllRuns(mResults, 'Vrms', depName='Time',
        path=sciBTest.outputPathBase)
    import plotCpuTimesAllRuns as plotCpus
    plotCpus.plotAllRuns(sciBTest.outputPathBase)
    sciBTest.mSuite.analysisImages = ["Vrms-multiRunTimeSeries.png",
        'cpuTimePerStep.png']
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
        (120, ""),
        (vrmsPeakImgStep, "near first VRMS peak at t=%f" % vrmsPeakTime),
        (lastImgStep, "")]
    # Here we just ask the CREDO reporting API to get Report Generators for
    # PDF (ReportLab) and RST (Restructured Text) output, and create a 
    # standard science benchmark report.
    for rGen in getGenerators(["RST", "ReportLab"], sciBTest.outputPathBase):
        sReps.makeSciBenchReport(sciBTest, mResults, rGen,
            os.path.join(sciBTest.outputPathBase, "%s-report.%s" %\
                (sciBTest.testName, rGen.stdExt)), imgPerRow=2)

sciBTest.setCustomReporting(customReporting)

def suite():
    return testSuite

if __name__ == "__main__":
    jobRunner = credo.jobrunner.defaultRunner()
    testResult, mResults = sciBTest.runTest(jobRunner,
        postProcFromExisting=True, createReports=True)
