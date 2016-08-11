#! /usr/bin/env python
import os
from credo.modelrun import ModelRun, SimParams
from credo.modelsuite import ModelSuite, StgXMLVariant
import credo.modelsuite as msuite
import credo.jobrunner
from credo.analysis import modelplots

import credo_rayTaySuite

elRes=32
stoptime=2500.0
mRun = ModelRun("RayTay-basic", "RayleighTaylorBenchmark.xml",
    simParams=SimParams(stoptime=stoptime, nsteps=-1, dumpevery=3))
mRun.paramOverrides={"elementResI":elRes, "elementResJ":elRes}
mSuite = ModelSuite("output/raytay-suite-simple-%dx%d-%d_t" %\
    (elRes, elRes, stoptime),
    templateMRun=mRun)
ampRange = [0.02 + x * 0.02 for x in range(10)]
mSuite.addVariant("initPerturbation",
    StgXMLVariant("components.lightLayerShape.amplitude", ampRange))

mSuite.subOutputPathGenFunc = msuite.getSubdir_RunIndexAndText
mSuite.generateRuns()

if __name__ == "__main__":
    postProcFromExisting = False
    if postProcFromExisting == False:
        jobRunner = credo.jobrunner.defaultRunner()
        mResults = jobRunner.runSuite(mSuite)
    else:
        mResults = mSuite.readResultsFromPath(mSuite.runs[0].basePath)
    credo_rayTaySuite.reportResults(mSuite, mResults)
    import credo.reporting.reportLab as rlRep
    rlRep.makeSuiteReport(mSuite, 
        os.path.join(mSuite.outputPathBase, "%s-report.pdf" %\
            "RayTay_Suite"), imgPerRow=5)
