#!/usr/bin/env python

import copy, os
from credo.systest import *
import testAll

subDir = "qTest"
tSuffix = "-qTest"
def suite():
    lSuite = copy.deepcopy(testAll.suite())
    lSuite.suiteName += tSuffix
    # Delete the orthotropic test from the suite to run
    del(lSuite.subSuites[-1])
    for tSuite in lSuite.subSuites:
        tSuite.suiteName += tSuffix
        for sysTest in tSuite.sysTests:
            oldBasePath = sysTest.outputPathBase
            sysTest.outputPathBase += tSuffix
            for mRun in sysTest.mSuite.runs:
                relSub = os.path.relpath(mRun.outputPath, oldBasePath)
                mRun.outputPath = os.path.join(sysTest.outputPathBase, relSub)
                # Hack it down to minimal steps
                sp = mRun.getSimParams()
                sp.nsteps = min(sp.nsteps, 2)
                mRun.simParams = sp
                mRun.paramOverrides.update({"elementResI":8, "elementResJ":8})
    return lSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(suite(), postProcFromExisting=False)
