#! /usr/bin/env python
import os, copy
import csv
import itertools
from credo.modelrun import ModelRun, SimParams
import credo.modelsuite as msuite
from credo.modelsuite import ModelSuite, StgXMLVariant
import credo.jobrunner
import credo.analysis.modelplots as modelplots

outPathBase = os.path.join('output','resScalingTest')
if not os.path.exists(outPathBase):
    os.makedirs(outPathBase)

defParams = SimParams(nsteps=2)
mRun = ModelRun("RayTayOverturn", "RayleighTaylorBenchmark.xml",
    simParams=defParams)

mSuite = ModelSuite(outPathBase, templateMRun=mRun)
mSuite.resRange = [8, 16, 32, 64, 96, 128, 150, 180, 256]
mSuite.addVariant("elResI", StgXMLVariant("elementResI", mSuite.resRange))
mSuite.addVariant("elResJ", StgXMLVariant("elementResJ", mSuite.resRange))
# TODO: would be nice to have a simpler subpath modifier
mSuite.subOutputPathGenFunc = msuite.getSubdir_RunIndexAndText
mSuite.generateRuns(iterGen=itertools.izip)

#-----------------------------

def reporting(mSuite, mResults):
    timePerEls = modelplots.getTimePerEls(mSuite.runs, mResults)
    print "Time per els were: %s" % timePerEls
    csvName = os.path.join(outPathBase, "runPerfInfo.csv")
    csvFile = open(csvName, "wb")
    wtr = csv.writer(csvFile)
    wtr.writerow(["Run", "res(els)", "walltime (sec)", "max mem (MB)", 
        "Time(sec)/element"])
    for runI, mRes in enumerate(stdResults):
        res = mSuite.runs[runI].paramOverrides["elementResI"]
        #TODO: once "default" set up, use that to get these.
        #perfInfo = mRes.jobMetaInfo.defaultPerformance()
        perfInfo = mRes.jobMetaInfo.performance['UnixTimeCmd']
        wallTime = perfInfo['walltime']
        maxMemKb = perfInfo['maxMem_KB']
        maxMemMb = float(maxMemKb)/1000.0
        #timePerStep = wallTime / 
        wtr.writerow([runI, res, wallTime, maxMemMb, timePerEls[runI]])
    csvFile.close()
    print "Wrote run results to %s" % (csvName)
    modelplots.plotTimePerEls(mSuite.runs, mResults, path=mSuite.outputPathBase)
    modelplots.plotWalltimesByRuns(mSuite.runs, mResults,
        path=mSuite.outputPathBase)

if __name__ == "__main__":
    postProcFromExisting = False
    if postProcFromExisting:
        stdResults = mSuite.readResultsFromPath(mSuite.runs[0].basePath)
    else:
        jobRunner = credo.jobrunner.defaultRunner()
        stdResults = jobRunner.runSuite(mSuite)
    reporting(mSuite, stdResults)
