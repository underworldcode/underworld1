#!/usr/bin/env python
import os, operator, itertools
from credo.systest import *
from credo.modelrun import ModelRun, SimParams
from credo.modelsuite import StgXMLVariant
import credo.modelsuite as msuite
import credo.jobrunner
import credo.io.stgfreq as stgfreq
import credo.reporting.standardReports as sReps
from credo.reporting import getGenerators

testSuite = SysTestSuite("Underworld", "SciBench-ThermalConv-Scaling")
sciBTest = SciBenchmarkTest("ThermalConvScalingBenchmark")
sciBTest.description = """Tests Underworld can perform thermal convection
  with key input parameters scaled across a wide range of values. See
  the file scalingSuite_MathsExplanation.[tex|pdf] for a full explanation."""
testSuite.sysTests.append(sciBTest)
## Configure the model and suite to use for this benchmark
mRun = ModelRun("ThermalConvection_Scaling", 
    "ThermalConvectionBenchmark_Dim.xml",
    simParams=SimParams(nsteps=2000, dumpevery=100))
mRun.paramOverrides["steadySteps"] = 100
mRun.paramOverrides["tolerance"] = 0.0005
sciBTest.mSuite.templateMRun=mRun
# A set of scaled parameters we will iterate through in a series of runs.
mSuite = sciBTest.mSuite
# Parameters: boxSize, visc, alpha, gravity, diffusion, B.C. temp
paramSets = [
    (1.0e8, 2.5e19, 2.5e-5, 10,     1.0,    1000),
    (1.0,   2.5e19, 2.5e-5, 10,     1.0e-6, 1.0e21),
    (1.0e6, 1.0e24, 1.0,    10,     1.0e-6, 1000),
    (1.0e6, 2.5e19, 2.5e-4, 1,      1.0e-6, 1000),
    (1.0e6, 2.5e19, 2.5e-5, 10,     1.0e-6, 1000),
    (1.0e6, 1.0,    2.5e-5, 10,     2.5e+13,1000),
    (1.0e6, 2.5e19, 2.5e-5, 1.0e4,  1.0e-6, 1)
    ]
# The last run has very high gravity, which would require an incompressible
#  formulation

BoxSize = map(operator.itemgetter(0), paramSets)
viscRange = map(operator.itemgetter(1), paramSets)
alphaRange = map(operator.itemgetter(2), paramSets)
gravityRange = map(operator.itemgetter(3), paramSets)
diffRange = map(operator.itemgetter(4), paramSets)
BCtempRange = map(operator.itemgetter(5), paramSets)

mSuite.addVariant("X", StgXMLVariant("maxX", BoxSize))
mSuite.addVariant("Y", StgXMLVariant("maxY", BoxSize))
mSuite.addVariant("viscosity", StgXMLVariant("components.viscosity.eta0",
    viscRange))
mSuite.addVariant("alpha", StgXMLVariant("components.material.alpha",
    alphaRange))
mSuite.addVariant("gravity", StgXMLVariant("gravity", gravityRange))
mSuite.addVariant("diffusion", StgXMLVariant("defaultDiffusivity", diffRange))
#This is a derived parameter based on the BCtempRange
ICpertAmpRange = [bcMax * 0.1 for bcMax in BCtempRange]
mSuite.addVariant("BCcoord",
    StgXMLVariant("SinusoidalTempIC_TopLayerCoord", BoxSize))
mSuite.addVariant("BCtempBot",
    StgXMLVariant("SinusoidalTempIC_BottomLayerBC", BCtempRange))
mSuite.addVariant("ICpertAmp",
    StgXMLVariant("SinusoidalTempIC_PerturbationAmplitude", ICpertAmpRange))
mSuite.addVariant("tempBCval",
    StgXMLVariant("temperatureBCs.vcList[0].variables[0].value", BCtempRange))
sciBTest.mSuite.generateRuns(iterGen=itertools.izip)
#Visualisation customisation - don't set as a 'variant' since it's a derived
 # parameter, just adjust directly.
for mRun, tempRange in zip(mSuite.runs, BCtempRange):
    tInt = tempRange/10.0
    mRun.paramOverrides["components.temperatureContours.interval"] = tInt

## Configure tests to apply
# Need this scaling factor array to do the Vrms within Range check:
vrmsScalingFactors = [1.0e-8, 1.0e-6, 1.0e-12, 1.0e-12, 1.0e-12,
    2.5e7, 1.0e-12]
sciBTest.setupEmptyTestCompsList()
for runI, mRun in enumerate(sciBTest.mSuite.runs):
    vrmsMin = 42.0*vrmsScalingFactors[runI]
    vrmsMax = 45.0*vrmsScalingFactors[runI]	
    if mSuite.modelVariants['gravity'].paramRange[runI] < 10.1:
        sciBTest.addTestComp(runI, "Scaled VRMS",
            OutputWithinRangeTC("Vrms", stgfreq.lastOp, (vrmsMin, vrmsMax)))
    else:
        mSuite.runDescrips[runI] += " (Not applying test for runs with"\
            " Gravity > 10, as this requires a compressible formulation.)"

def customReporting(sciBTest, mResults):    
    #Plotting/CSV writing
    for mRes in mResults: mRes.readFrequentOutput()
    vrmsTCs, vrmsResults = sciBTest.getTCRes("Scaled VRMS")
    recSteps = [mRes.freqOutput.finalStep() for mRes in mResults]
    vrmsActuals = [mRes.freqOutput.getValueAtStep("Nusselt", ns) \
        for mRes, ns in zip(mResults, recSteps)]
    nusseltActuals = [mRes.freqOutput.getValueAtStep("Nusselt", ns) \
        for mRes, ns in zip(mResults, recSteps)]
    for mRes in mResults:    
        mRes.freqOutput.plotOverTime('Vrms', depName='Time', show=False,
            path=mRes.outputPath)
        mRes.freqOutput.plotOverTime('Nusselt', depName='Time', show=False,
            path=mRes.outputPath)
    observables = {'Vrms':vrmsActuals, 'Vrms Pass':vrmsResults,
        'Nusselt':nusseltActuals, 'nSteps':recSteps,
        'ScalingFac':vrmsScalingFactors}
    msuite.writeInputsOutputsToCSV(sciBTest.mSuite, observables,
        "scalingResults.csv")
    # Actually for this benchmark, we want to show the VRMS and Nusselt
    #  images generated in _each run_
    sciBTest.mSuite.analysisImages = None
    #sciBTest.mSuite.analysisImages = [
    #    'VrmsAndNusseltValues.png',
    #    'Nusselt-multiRunTimeSeries.png',
    #    'Vrms-multiRunTimeSeries.png']
    sciBTest.mSuite.modelImagesToDisplay = [[] for runI in \
        range(len(sciBTest.mSuite.runs))]
    lastImgSteps = []
    for finalStep, mRun in zip(recSteps, sciBTest.mSuite.runs):
        simParams = mRun.getSimParams()
        lastImgSteps.append(simParams.nearestDumpStep(finalStep, finalStep))
    for runI in range(len(mResults)):
        sciBTest.mSuite.modelImagesToDisplay[runI] = [
            (0, "initial"),
            (lastImgSteps[runI], "final")]
    for rGen in getGenerators(["RST", "ReportLab"], sciBTest.outputPathBase):
        sReps.makeSciBenchReport(sciBTest, mResults, rGen,
            os.path.join(sciBTest.outputPathBase, "%s-report.%s" %\
                (sciBTest.testName, rGen.stdExt)), imgPerRow=2)

sciBTest.setCustomReporting(customReporting)

def suite():
    return testSuite

if __name__ == "__main__":
    postProcFromExisting = True
    jobRunner = credo.jobrunner.defaultRunner()
    testResult, mResults = sciBTest.runTest(jobRunner, postProcFromExisting,
        createReports=True)
