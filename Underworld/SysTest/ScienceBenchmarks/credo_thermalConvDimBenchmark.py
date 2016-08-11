#! /usr/bin/env python
"""Dimensional Thermal Convection with Buoyancy force benchmark"""

import os
import operator
import itertools
from credo.systest import *
from credo.modelrun import ModelRun, SimParams
from credo.modelsuite import StgXMLVariant
import credo.modelsuite as msuite
import credo.jobrunner
import credo.io.stgfreq as stgfreq
from credo.analysis import modelplots
import thermalConvPostProc
import credo.reporting.standardReports as sReps
from credo.reporting import getGenerators

testSuite = SysTestSuite("Underworld", "SciBench-ThermalConv-Dim")
sciBTest = SciBenchmarkTest("ThermalConvDimBenchmark")
sciBTest.description = """Tests that Underworld's Thermal Convection capabilty
 accords well with the known Blankenbach (1989) results (dimensional)"""
testSuite.sysTests.append(sciBTest)
# Running a suite of models for this Benchmark for 3 Rayleigh Numbers:
# 10^4, 10^5, 10^6- scaling factor is 10^-12 for vrms and 10-3 for nusselt 
RaRange = [1.0e6, 1.0e5, 1.0e4] 
## Configuration Model and suite to use for this benchmark
mRun = ModelRun("ThermalConvectionDimensionalBenchmark", 
    "ThermalConvectionBenchmark_Dim.xml",
    simParams=SimParams(dumpEvery=100, nsteps=5000),
    nproc=2)
mRun.paramOverrides["steadySteps"] = 100
mRun.paramOverrides["tolerance"] = 0.0005
#Visualisation customisation.
mRun.paramOverrides["components.temperatureContours.interval"] = 100
sciBTest.mSuite.templateMRun=mRun
viscRange = [2.5e17, 2.5e18, 2.5e19]
sciBTest.mSuite.addVariant("viscosity",
    StgXMLVariant("components.viscosity.eta0", viscRange))
sciBTest.mSuite.subOutputPathGenFunc = msuite.getSubdir_TextParamVals
sciBTest.mSuite.generateRuns(iterGen=itertools.izip)
## Configure tests to apply
# These are the ranges of Vrms and Nusselt Numbers from the Blankenbach
# paper (1989)
# For Ra = 10^4, 10^5, 10^6m, rough ranges from Blankenbach paper
# TODO: opposite direction to other test
BBRa = [1.0e6, 1.0e5, 1.0e4] 
BBVrmsMin = [7.774094e-10, 1.803391e-10, 4.079e-11]
BBVrmsMax = [8.3655e-10, 1.93378e-10, 4.3316e-11]
BBNusseltMin = [1.7952049e-2, 8.52298e-3, 4.565e-3]
BBNusseltMax = [2.1075e-2, 1.0417e-2, 4.864e-3]

# These are some slightly more relaxed results compared to Blankenback
#  Paper to test against.
minVrms = [8.3e-10, 1.92e-10, 4.2e-11]
maxVrms = [8.5e-10, 1.96e-10, 4.3e-11]
# the Ra 10^6 nusselt value is really different from the paper
minNusselt = [1.55e-2, 9.5e-3, 4.7e-3]
maxNusselt = [1.80e-2, 1.02e-2, 4.8e-3]
sciBTest.setupEmptyTestCompsList()
for runI, mRun in enumerate(sciBTest.mSuite.runs):
    sciBTest.addTestComp(runI, "VRMS vs Blankenbach",
        OutputWithinRangeTC("Vrms", stgfreq.lastOp,
            (minVrms[runI], maxVrms[runI])))
    sciBTest.addTestComp(runI, "Nusselt vs Blankenbach",
        OutputWithinRangeTC("Nusselt", stgfreq.lastOp,
            (minNusselt[runI], maxNusselt[runI])))

def customReport_VRMS_Nusselt(sciBTest, mResults):
    # Some custom output generation and analysis
    vrmsTCs, vrmsResults = sciBTest.getTCRes("VRMS vs Blankenbach")
    nusseltTCs, nusseltResults = sciBTest.getTCRes("Nusselt vs Blankenbach")
    vrmsActuals = [tc.actualVal for tc in vrmsTCs]
    nusseltActuals = [tc.actualVal for tc in nusseltTCs]
    # TODO: useful if below values available on modelResults automatically.
    for mRes in mResults: mRes.readFrequentOutput()
    nSteps = [mRes.freqOutput.finalStep() for mRes in mResults]
    # Plotting / CSV writing
    thermalConvPostProc.plotResultsVsBlankenbach(BBRa,
        BBVrmsMin, BBVrmsMax,
        BBNusseltMin, BBNusseltMax,
        {"UW Actual":vrmsActuals}, {"UW Actual":nusseltActuals},
        os.path.join(sciBTest.outputPathBase, "VrmsAndNusseltValues.png"))
    observables = {'Vrms':vrmsActuals, 'Vrms Passed':vrmsResults,
        'Nusselt':nusseltActuals, 'Nusselt Passed':nusseltResults,
        'nSteps':nSteps}
    msuite.writeInputsOutputsToCSV(sciBTest.mSuite, observables,
        "thermalDimBMResults.csv")
    modelplots.plotOverAllRuns(mResults, 'Nusselt',
        path=sciBTest.outputPathBase)   
    modelplots.plotOverAllRuns(mResults, 'Vrms',
        path=sciBTest.outputPathBase)   
    #TODO: modularise the below
    import plotCpuTimesAllRuns as plotCpus
    plotCpus.plotAllRuns(sciBTest.outputPathBase)
    sciBTest.mSuite.analysisImages = [
        'VrmsAndNusseltValues.png',
        'Nusselt-multiRunTimeSeries.png',
        'Vrms-multiRunTimeSeries.png',
        'cpuTimePerStep.png']
    sciBTest.mSuite.modelImagesToDisplay = [[] for runI in \
        range(len(sciBTest.mSuite.runs))]
    lastImgSteps = []
    for finalStep, mRun in zip(nSteps, sciBTest.mSuite.runs):
        simParams = mRun.getSimParams()
        lastImgSteps.append(simParams.nearestDumpStep(finalStep, finalStep))
    sciBTest.mSuite.modelImagesToDisplay[0] = [
        (0, ""),
        (700, ""),
        (lastImgSteps[0], "")]
    sciBTest.mSuite.modelImagesToDisplay[1] = [
        (0, ""),
        (800, ""),
        (lastImgSteps[1], "")]
    sciBTest.mSuite.modelImagesToDisplay[2] = [
        (0, ""),
        (400, ""),
        (lastImgSteps[2], "")]
    for rGen in getGenerators(["RST", "ReportLab"], sciBTest.outputPathBase):
        sReps.makeSciBenchReport(sciBTest, mResults, rGen,
            os.path.join(sciBTest.outputPathBase, "%s-report.%s" %\
                (sciBTest.testName, rGen.stdExt)), imgPerRow=3)    

sciBTest.setCustomReporting(customReport_VRMS_Nusselt)

def suite():
    return testSuite

if __name__ == "__main__":
    postProcFromExisting = True
    jobRunner = credo.jobrunner.defaultRunner()
    testResult, mResults = sciBTest.runTest(jobRunner, postProcFromExisting,
        createReports=True)
