#! /usr/bin/env python
from credo.modelrun import ModelRun, SimParams
import credo.jobrunner

mRun = ModelRun("RayTay-basicBenchmark", "RayleighTaylorBenchmark.xml",
    "output/raytay-scibench-credo-basic",
    simParams=SimParams(stoptime=20.0),
    paramOverrides={"gravity":1})
    #solverOpts="myOpts.opt")

def postProc(mRun, mResult):
    mResult.readFrequentOutput()
    mResult.freqOutput.plotOverTime('Vrms', depName='Time', show=True,
        path=mResult.outputPath)

    maxVal, maxTimeStep = mResult.freqOutput.getMax('Vrms')
    maxTime = mResult.freqOutput.getValueAtStep('Time', maxTimeStep)
    print "Maximum value of Vrms was %f, at time %d" % (maxVal, maxTime)

if __name__ == "__main__":
    jobRunner = credo.jobrunner.defaultRunner()
    mResult = jobRunner.runModel(mRun)
    postProc(mRun, mResult)

