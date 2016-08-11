#! /usr/bin/env python
import os
from credo.modelrun import ModelRun, SimParams
from credo.modelsuite import ModelSuite, StgXMLVariant
import credo.modelsuite as msuite
import credo.jobrunner
from credo.analysis import modelplots
import credo.reporting.standardReports as sReps
from credo.reporting import getGenerators

elRes=16
stoptime=600.0
mRun = ModelRun("RayTay-basic", "RayleighTaylorBenchmark.xml",
    simParams=SimParams(stoptime=stoptime, nsteps=-1, dumpevery=3))
mRun.paramOverrides={"elementResI":elRes, "elementResJ":elRes}

mSuite = ModelSuite("output/raytay-suite-%dx%d-%d_t" % (elRes, elRes, stoptime),
    templateMRun=mRun)
gravRange = [0.7 + x * 0.1 for x in range(4)]
mSuite.addVariant("gravity", StgXMLVariant("gravity", gravRange))
ampRange = [0.02, 0.04, 0.07]
mSuite.addVariant("initPerturbation",
    StgXMLVariant("components.lightLayerShape.amplitude", ampRange))

mSuite.generateRuns()

def reportResults(mSuite, mResults):
    indicesIter = msuite.getVariantIndicesIter(mSuite.modelVariants,
        mSuite.iterGen)
    varNameDicts = msuite.getVariantNameDicts(mSuite.modelVariants, indicesIter)
    for resI, mRes in enumerate(mResults):
        print "Post-process result %d: with" % resI,
        print ", ".join(["%s=%g" % item for item in varNameDicts[resI].iteritems()])
        mRes.readFrequentOutput()
        mRes.freqOutput.plotOverTime('Vrms', depName='Time', 
            path=mRes.outputPath)
        maxVal, maxTimeStep = mRes.freqOutput.getMax('Vrms')
        maxTime = mRes.freqOutput.getValueAtStep('Time', maxTimeStep)
        print "\tMaximum value of Vrms was %f, at time %.2f" % (maxVal, maxTime)
    modelplots.plotOverAllRuns(mResults, 'Vrms', depName='Time',
        path=mSuite.outputPathBase)
    mSuite.analysisImages = [
        'Vrms-multiRunTimeSeries.png']
    mSuite.modelImagesToDisplay = [[] for runI in range(len(mSuite.runs))]
    for runI, mRun in enumerate(mSuite.runs):
        mRes = mResults[runI]
        fStep = mRes.freqOutput.finalStep()
        fTime = mRes.freqOutput.getValueAtStep('Time', fStep)
        dEvery = mRun.simParams.dumpevery
        lastImgStep = fStep / dEvery * dEvery
        vrmsMax, vrmsPeakStep = mRes.freqOutput.getMax('Vrms')
        vrmsMaxTime = mRes.freqOutput.getValueAtStep('Time', vrmsPeakStep)
        vrmsPeakImgStep = mRun.simParams.nearestDumpStep(fStep, vrmsPeakStep)
        vrmsPeakImgTime = mRes.freqOutput.getValueAtStep('Time',
            vrmsPeakImgStep)
        mSuite.modelImagesToDisplay[runI] = [
            (0, "initial"),
            (vrmsPeakImgStep, "t=%f, near VRMS peak %g (at t=%g)" %\
                (vrmsPeakImgTime, vrmsMax, vrmsMaxTime)),
            (mRun.simParams.nearestDumpStep(fStep, fStep*.5), "1/2"),
            (mRun.simParams.nearestDumpStep(fStep, fStep*.75), "3/4"),
            (lastImgStep, "final state (t=%f)" % fTime)]

    for rGen in getGenerators(["RST", "ReportLab"], mSuite.outputPathBase):
        sReps.makeSuiteReport(mSuite, mResults, rGen,
            os.path.join(mSuite.outputPathBase, "%s-report.%s" %\
                ("RayTaySuite-examples", rGen.stdExt)), imgPerRow=3)

if __name__ == "__main__":
    postProcFromExisting = True
    if postProcFromExisting == False:
        jobRunner = credo.jobrunner.defaultRunner()
        mResults = jobRunner.runSuite(mSuite)
    else:
        mResults = mSuite.readResultsFromPath(mSuite.runs[0].basePath)
    reportResults(mSuite, mResults)
