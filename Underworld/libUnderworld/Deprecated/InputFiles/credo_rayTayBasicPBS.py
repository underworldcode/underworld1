#! /usr/bin/env python
from datetime import timedelta
from credo.modelrun import ModelRun, SimParams
from credo.modelrun import JobParams
import credo.jobrunner.pbsjobrunner

mRun = ModelRun("RayTay-basicBenchmark", "RayleighTaylorBenchmark.xml",
    "output/raytay-scibench-credo-basic-pbs",
    simParams=SimParams(stoptime=20.0),
    paramOverrides={"gravity":1},
    inputFilePath="Underworld/InputFiles")
    #solverOpts="myOpts.opt")
mRun.jobParams = JobParams(nproc=1, maxRunTime=timedelta(minutes=10), pollInterval = 50) 
#jobName MUST be the SAME as the model name for the time being
mRun.jobParams['PBS'] = {'jobNameLine':'#PBS -N RayTay-basicBenchmark'}
#edit this list to include modules req'd on your chosen machine.
mRun.jobParams['PBS']['sourcefiles'] = ['/usr/local/Modules/default/init/bash']
mRun.jobParams['PBS']['modules'] = ['underworld/dev']
mRun.writeInfoXML()
jobRunner = credo.jobrunner.pbsjobrunner.PBSJobRunner()
mResult = jobRunner.runModel(mRun)
mResult.writeRecordXML()

#-----------------------------

mResult.readFrequentOutput()
# This plot doesn't work perhaps due to non interactive mode?
#mResult.freqOutput.plotOverTime('Vrms', depName='Time', show=True, path=mResult.outputPath)

maxVal, maxTimeStep = mResult.freqOutput.getMax('Vrms')
maxTime = mResult.freqOutput.getValueAtStep('Time', maxTimeStep)
print "Maximum value of Vrms was %f, at time %d" % (maxVal, maxTime)

