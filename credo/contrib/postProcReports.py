#!/usr/bin/env python

"""This is a work-in-progress example to show modification of an existing
suite to run a report and generate images.

Example created:- 2011/04/12, PatrickSunter"""

import os
import credo.jobrunner
import credo.reporting as rep
import credo.reporting.standardReports as sReps

import testAll_lowres

ts = testAll_lowres.suite()

sysTest = ts.sysTests[0]

# TODO: really should post-proc from existing, but this isn't working perfectly
# yet for basic sys tests.
jobRunner = credo.jobrunner.defaultRunner()
sysTest.setupTest()
testResult, mResults = sysTest.runTest(jobRunner,
        postProcFromExisting=False, createReports=True)

# Dodgy test report configuration
sysTest.mSuite.analysisImages = []
sysTest.mSuite.modelImagesToDisplay = None

for rGen in rep.getGenerators(["RST", "ReportLab"], sysTest.outputPathBase):
    sReps.makeSciBenchReport(sysTest, mResults, rGen,
        os.path.join(sysTest.outputPathBase, "%s-report.%s" %\
            (sysTest.testName, rGen.stdExt)), imgPerRow=2)
