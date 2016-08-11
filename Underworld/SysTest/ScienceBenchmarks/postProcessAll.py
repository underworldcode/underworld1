#!/usr/bin/env python
"""A small utility script to post-process all of the CREDO benchmark suites
in the current directory (matching the wildcard pattern credo_*.pdf)."""

import glob, fnmatch
import credo.systest
from credo.systest.systestrunner import getSuitesFromModules

credoFiles = glob.glob("credo_*.py")
credoFiles.sort()
credoModNames = [cf.rstrip(".py") for cf in credoFiles]
tSuites = getSuitesFromModules(credoModNames)

sRunner = credo.systest.SysTestRunner()
sRunner.runSuites(tSuites, postProcFromExisting=True, createReports=True)

print "PDF Reports generated are:"
for ts in tSuites:
    pdfReps = []
    for sysTest in ts.sysTests:
        reps = fnmatch.filter(sysTest.generatedReports, '*.pdf')
        pdfReps.extend(reps)
    for rep in pdfReps:
        print "%s" % rep
