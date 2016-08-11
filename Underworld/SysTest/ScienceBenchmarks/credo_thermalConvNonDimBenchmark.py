#! /usr/bin/env python
# Non-dimensional Thermal Convection with Ra benchmark

import os, operator, itertools
from credo.systest import *
from credo.modelrun import ModelRun, SimParams
from credo.modelsuite import StgXMLVariant
import credo.modelsuite as msuite
import credo.jobrunner
import credo.io.stgfreq as stgfreq
import thermalConvPostProc
from credo.analysis import modelplots
import credo.reporting.standardReports as sReps
from credo.reporting import getGenerators

testSuite = SysTestSuite("Underworld", "SciBench-ThermalConv-NonDim")
sciBTest = SciBenchmarkTest("ThermalConvNonDimBenchmark")
sciBTest.description = """Tests that Underworld's Thermal Convection capabilty
 accords well with the known Blankenbach (1989) results (non-dimensional)"""
testSuite.sysTests.append(sciBTest)
## Configuration parameters for overall benchmark
RaRange = [1.0e4, 1.0e5, 1.0e6]
elRes = 32
## Configure the model and suite to use for this benchmark
mRun = ModelRun("ThermalConvection_NonDim",
    "ThermalConvectionBenchmark_NonDim.xml",
    simParams=SimParams(dumpevery=100, nsteps=int(2e6)))
    
# Set to resolution needed to pass expected ranges instead of the default 12x12
# Running a suite of models for this Benchmark for 3 Rayleigh Numbers:
# 10^4, 10^5, 10^6
mRun.paramOverrides['elementResI'] = elRes
mRun.paramOverrides['elementResJ'] = elRes
mRun.paramOverrides["components.temperatureContours.interval"] = 0.1
sciBTest.mSuite.templateMRun=mRun
sciBTest.mSuite.addVariant("Ra", StgXMLVariant("Ra", RaRange))
sciBTest.mSuite.addVariant("gravity", StgXMLVariant("gravity", RaRange))
#Generate the actual suite runs
sciBTest.mSuite.subOutputPathGenFunc = msuite.getSubdir_TextParamVals
sciBTest.mSuite.generateRuns(iterGen=itertools.izip)

## Configure tests to apply
# These are the ranges of Vrms and Nusselt Numbers from the Blankenbach
#  paper (1989)
# For Ra = 10^4, 10^5, 10^6m, from Blankenbach paper at Res 18x18
BBRa = RaRange
BBVrmsMin = [40.79, 180.339114, 777.409478]
BBVrmsMax = [43.316, 193.378, 836.55]
BBNusseltMin = [4.565, 8.52298, 17.952049]
BBNusseltMax = [4.864, 10.417, 21.075]
# Actual max and mins used for testing based on Blankenbach, but are a bit
#  looser currently - need to investigate/refine further.
minVrms = [42.0, 193.0, 834.3]
maxVrms = [44.0, 199, 880.8]
minNusselt = [4.8, 10.0, 20.8]
maxNusselt = [5.0, 10.7, 22.3]

## Now set up test components
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
        BBVrmsMin, BBVrmsMax, BBNusseltMin, BBNusseltMax,
        {"UW Actual":vrmsActuals}, {"UW Actual":nusseltActuals},
        os.path.join(sciBTest.outputPathBase, "VrmsAndNusseltValues.png"))
    observables = {'Vrms':vrmsActuals, 'Vrms Passed':vrmsResults,
        'Nusselt':nusseltActuals, 'Nusselt Passed':nusseltResults,
        'nSteps':nSteps}
    msuite.writeInputsOutputsToCSV(sciBTest.mSuite, observables,
        "thermalNonDimBMResults.csv")
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
    for runI, mRun in enumerate(sciBTest.mSuite.runs):
        simParams = mRun.getSimParams()
        finalStep = nSteps[runI]
        sciBTest.mSuite.modelImagesToDisplay[runI] = [
            (0, ""),
            (100, ""),
            (simParams.nearestDumpStep(finalStep, finalStep/5), ""),
            (simParams.nearestDumpStep(finalStep, finalStep), "")]
    for rGen in getGenerators(["RST", "ReportLab"], sciBTest.outputPathBase):
        sReps.makeSciBenchReport(sciBTest, mResults, rGen,
            os.path.join(sciBTest.outputPathBase, "%s-report.%s" %\
                (sciBTest.testName, rGen.stdExt)), imgPerRow=4)

sciBTest.setCustomReporting(customReport_VRMS_Nusselt)

def suite():
    return testSuite

if __name__ == "__main__":
    postProcFromExisting = True
    jobRunner = credo.jobrunner.defaultRunner()
    testResult, mResults = sciBTest.runTest(jobRunner, postProcFromExisting,
        createReports=True)
