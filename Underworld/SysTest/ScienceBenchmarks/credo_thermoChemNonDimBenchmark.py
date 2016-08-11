#! /usr/bin/env python
"""Dimensionless Thermal Chemical Convection with ThermoChem Buoyancy force"""

import os
import operator
import itertools
from credo.systest import *
from credo.modelrun import ModelRun, SimParams
from credo.modelsuite import StgXMLVariant
import credo.modelsuite as msuite
import credo.jobrunner
from credo.io.stgfreq import maxOp, closestToSimTime
from credo.analysis import modelplots
import thermalConvPostProc
import credo.reporting.standardReports as sReps
from credo.reporting import getGenerators

testSuite = SysTestSuite("Underworld", "SciBench-ThermoChem-Dim")
sciBTest = SciBenchmarkTest("ThermoChemBenchmark")
sciBTest.description = """Tests that Underworld's Thermal Convection capabilty
 accords well with the known Van Keken (1989) 2nd Thermo-Chem benchmark."""
testSuite.sysTests.append(sciBTest)
## Configuration Model and suite to use for this benchmark
stopTime = 0.035
mRun = ModelRun("ThermoChemConvectionBenchmark", 
    "ThermoChemBenchmark.xml",
    simParams=SimParams(dumpEvery=100, nsteps=-2, stopTime=stopTime),
    nproc=4)

#Visualisation customisation.
sciBTest.mSuite.templateMRun=mRun

# setup varying resolution runs, and also vary number of particles per cell
set20 = [64, 96, 128]
set100 = [64, 96, 128]
elResX = set20 + set100
elResY = [resX//2 for resX in elResX]
pPerCell = [20] * len(set20) + [100] * len(set100)
sciBTest.mSuite.addVariant("elementResI",
    StgXMLVariant("elementResI", elResX))
sciBTest.mSuite.addVariant("elementResJ",
    StgXMLVariant("elementResJ", elResY))
sciBTest.mSuite.addVariant("pPerCell",
    StgXMLVariant("particlesPerCell", pPerCell))
# how we want the output sub directories named
sciBTest.mSuite.subOutputPathGenFunc = msuite.getSubdir_TextParamVals
# controls how the variants list is iterated through
sciBTest.mSuite.generateRuns(iterGen=itertools.izip)

## Configure tests to apply
# These are VRMS ranges from the relevant paper

sciBTest.setupEmptyTestCompsList()
for runI, mRun in enumerate(sciBTest.mSuite.runs):
    sciBTest.addTestComp(runI, "VRMS Max",
        OutputWithinRangeTC("Vrms", maxOp, (545, 555), 
            tRange=[0.00418058, 0.0040483]) )
    sciBTest.addTestComp(runI, "VRMs 2xU values, 1st U",
        OutputWithinRangeTC("Vrms", closestToSimTime, (245,255),
            opDict={'targTime':0.0088}, tRange=[0.00866561,0.00893891]))
    sciBTest.addTestComp(runI, "VRMs 2xU values, 2nd U",
        OutputWithinRangeTC("Vrms", closestToSimTime, (239,249),
            opDict={'targTime':0.0108}, tRange=[0.0106418,0.0109572]))
    sciBTest.addTestComp(runI, "Early Entrainment",
        OutputWithinRangeTC("Entrainment", closestToSimTime, (0.005,0.02),
            opDict={'targTime':0.01}))
    sciBTest.addTestComp(runI, "Mid Entrainment",
        OutputWithinRangeTC("Entrainment", closestToSimTime, (0.06,0.1),
            opDict={'targTime':0.02}))

def customReport_VRMS(sciBTest, mResults):
    for mRes in mResults: mRes.readFrequentOutput()
    # Some custom output generation and analysis
    fSteps = [mResult.freqOutput.finalStep() for mResult in mResults]
    vrmsTCs, vrmsResults = sciBTest.getTCRes("VRMS Max")
    vrmsActuals = [tc.actualVal for tc in vrmsTCs]
    # Now specific per-run images
    dEvery = sciBTest.mSuite.runs[0].simParams.dumpevery
    lastImgSteps = [fStep / dEvery * dEvery for fStep in fSteps]
    # find the timestep
    vrmsPeakTimes = [tc.actualTime for tc in vrmsTCs]
    vrmsPeakSteps = [mRes.freqOutput.getClosest('Time', peakTime)[1] for mRes, peakTime in\
            zip(mResults, vrmsPeakTimes) ] 
    # TODO: useful if below values available on modelResults automatically.
    # Plotting
    modelplots.plotOverAllRuns(mResults, 'Vrms', depName='Time',
        path=sciBTest.outputPathBase)
    modelplots.plotOverAllRuns(mResults, 'Entrainment',
        path=sciBTest.outputPathBase, depName='Time')
    #TODO: modularise the below
    import plotCpuTimesAllRuns as plotCpus
    plotCpus.plotAllRuns(sciBTest.outputPathBase)
    sciBTest.mSuite.analysisImages = [
        'Vrms-multiRunTimeSeries.png',
        'Entrainment-multiRunTimeSeries.png',
        'cpuTimePerStep.png']
    sciBTest.mSuite.modelImagesToDisplay = [[] for runI in \
        range(len(sciBTest.mSuite.runs))]
    lastImgSteps = []
    vrmsPeakImgSteps = []
    for runI, mRun in enumerate(sciBTest.mSuite.runs):
        finalStep = fSteps[runI]
        simParams = mRun.getSimParams()
        lastImgSteps.append(simParams.nearestDumpStep(finalStep, finalStep))
        vrmsPeakImgSteps.append(simParams.nearestDumpStep(vrmsPeakSteps[runI], 
            finalStep))
    for resI, mResult in enumerate(mResults):
        simParams = sciBTest.mSuite.runs[resI].getSimParams()
        qtrStep = simParams.nearestDumpStep(fSteps[resI]//4, fSteps[resI])
        halfStep = simParams.nearestDumpStep(fSteps[resI]//2, fSteps[resI])
        qtrTime = mResult.freqOutput.getValueAtStep("Time", qtrStep)
        halfTime = mResult.freqOutput.getValueAtStep("Time", halfStep)
        sciBTest.mSuite.modelImagesToDisplay[resI] = [
            (0, ""),
            (vrmsPeakImgSteps[resI], "VRMS Peak, t=%f" % vrmsPeakTimes[resI]),
            (qtrStep, "t=%f" % qtrTime),
            (halfStep, "t=%f" % halfTime),
            (lastImgSteps[resI], "Final, t=%f" % (stopTime))]
    for rGen in getGenerators(["RST", "ReportLab"], sciBTest.outputPathBase):
        sReps.makeSciBenchReport(sciBTest, mResults, rGen,
            os.path.join(sciBTest.outputPathBase, "%s-report.%s" %\
                (sciBTest.testName, rGen.stdExt)), imgPerRow=3)    

sciBTest.setCustomReporting(customReport_VRMS)

def suite():
    return testSuite

if __name__ == "__main__":
    postProcFromExisting = True
    jobRunner = credo.jobrunner.defaultRunner()
    testResult, mResults = sciBTest.runTest(jobRunner, postProcFromExisting,
        createReports=True)
