#! /usr/bin/env python
import os, copy
import csv
import itertools
import credo.modelsuite as msuite
from credo.modelrun import ModelRun, SimParams
from credo.modelsuite import ModelSuite, JobParamVariant
import credo.jobrunner
import credo.analysis.modelplots as modelplots

elResSweep = [32, 64, 96, 128, 196, 256]#, 64, 128]#256
defParams = SimParams(nsteps=5)
mRun = ModelRun("RayTayOverturn", "RayleighTaylorBenchmark.xml",
    simParams=defParams)

mSuites = []
for elRes in elResSweep:
    mRun.paramOverrides["elementResI"] = elRes
    mRun.paramOverrides["elementResJ"] = elRes
    outPathBase = os.path.join('output','perfScalingTest', 'res%04d' % elRes)
    if not os.path.exists(outPathBase):
        os.makedirs(outPathBase)

    mSuite = ModelSuite(outPathBase, templateMRun=mRun)
    mSuite.elRes = elRes
    mSuite.procRange = [1, 2, 4, 8]#, 4]#, 8]#, 2, 4]
    mSuite.addVariant("nproc", JobParamVariant("nproc", mSuite.procRange))
    mSuite.subOutputPathGenFunc = msuite.getSubdir_RunIndexAndText
    mSuite.generateRuns(iterGen=itertools.izip)
    mSuites.append(mSuite)

#-----------------------------

def reporting(mSuite, mResults):
    print "With element res %d" % mSuite.elRes
    speedups = modelplots.getSpeedups(mSuite.runs, mResults)
    print "Speedups were: %s" % speedups
    csvName = os.path.join(mSuite.outputPathBase, "runPerfInfo.csv")
    csvFile = open(csvName, "wb")
    wtr = csv.writer(csvFile)
    wtr.writerow(["Run", "nproc", "walltime (sec)", "max mem (MB)", "speedup"])
    for runI, mRes in enumerate(mResults):
        nproc = mSuite.runs[runI].jobParams["nproc"]
        #perfInfo = mRes.jobMetaInfo.defaultPerformance()
        perfInfo = mRes.jobMetaInfo.performance['UnixTimeCmd']
        wallTime = perfInfo['walltime']
        maxMemKb = perfInfo['maxMem_KB']
        maxMemMb = float(maxMemKb)/1000.0
        #timePerStep = wallTime / 
        wtr.writerow([runI, nproc, wallTime, maxMemMb, speedups[runI]])
    csvFile.close()
    print "Wrote run results to %s" % (csvName)
    modelplots.plotSpeedups(mSuite.runs, mResults, path=mSuite.outputPathBase)

if __name__ == "__main__":
    postProcFromExisting = False
    for mSuite in mSuites:
        if postProcFromExisting:
            mResults = mSuite.readResultsFromPath(mSuite.runs[0].basePath)
        else:
            jobRunner = credo.jobrunner.defaultRunner()
            mResults = jobRunner.runSuite(mSuite)
        reporting(mSuite, mResults)
