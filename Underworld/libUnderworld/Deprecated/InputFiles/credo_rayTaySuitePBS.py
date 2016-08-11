#! /usr/bin/env python
"""This simply runs the rayTaySuite, but using PBS"""

import credo_rayTaySuite as rayTaySuite
import credo.jobrunner.pbsjobrunner as pbsrunner

if __name__ == "__main__":
    mSuite = rayTaySuite.mSuite
    for mRun in mSuite.runs:
        mRun.jobParams['pollInterval'] = 60
        mRun.jobParams['PBS'] = {}
        mRun.jobParams['PBS']['sourcefiles'] = ['/usr/local/Modules/default/init/bash']
        mRun.jobParams['PBS']['modules'] = ['underworld/dev']
    mode = "postproc"
    if mode == "submit":
        jobRunner = pbsrunner.PBSJobRunner()
        jobMetaInfos = jobRunner.submitSuite(mSuite)
    elif mode == "collect":
        # TODO: have to read the jobMetaInfos from somewhere ...
        #  perhaps Handles read from file?
        mResults = jobRunner.blockSuite(mSuite, jobMetaInfos) 
    else:
        mResults = mSuite.readResultsFromPath(mSuite.runs[0].basePath)
        rayTaySuite.reportResults(mSuite, mResults)
        import credo.reporting.reportLab as rlRep
        rlRep.makeSuiteReport(mSuite,
            os.path.join(mSuite.outputPathBase, "%s-report.pdf" %\
                "RayTay_Suite"), imgPerRow=3)
