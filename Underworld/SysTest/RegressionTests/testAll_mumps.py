#!/usr/bin/env python
import os
import copy
import re
from credo.systest import *
from testAll import testSuite

# Check PETSc version to know which options file to use.
petsc_major_version = 2
# Let's try opening the config.cfg file.
try:
    config_file = open("../../../config.cfg")
except IOError:
    print "config.cfg can't be opened, Did you build the code already?"
else:
    # Create our regular expression to get the CPPPATH from the config.cfg.
    cpppath_re = re.compile(r'^\s*CPPPATH\s*=\s*\[(?P<paths>.*)\]\s*$')
    # Read the config file line by line and check it againts our re.
    for line1 in config_file:
        match1 = cpppath_re.match(line1)
        if match1:
            paths = match1.group('paths').split(', ')
            # For each paths found, check if we can find petscversion.h
            for path in paths:
                path += '/petscversion.h'
                path = path.replace('\'', '')
                # Once we've found petscversion.h, extract the major version from the file.
                if os.path.exists(path):
                    try:
                        version_file = open(path)
                    except IOError:
                        print "petscversion.h can't be opened."
                    else:
                        # Create our regular expression to get PETSc's major version.
                        version_re = re.compile(\
                            r'^#define\s*PETSC_VERSION_MAJOR\s*(?P<version>\d*)$')
                        for line2 in version_file:
                            match2 = version_re.match(line2)
                            # Assign the major version.
                            if match2:
                                petsc_major_version = int(match2.group('version'))
                        version_file.close()
    config_file.close()

mumpsSuite = copy.deepcopy(testSuite)
# Customise to run each test with Multigrid options
mumpsSuite.suiteName += "-mumps"
# Depending on the PETSc version, use the right options file.
if petsc_major_version == 2:
    mumpsSolverOpts = "uzawa-mumps-2.3.opt"
elif petsc_major_version == 3:
    mumpsSolverOpts = "uzawa-mumps-3.0.opt"
for sysTest in mumpsSuite.sysTests:
    sysTest.testName += "-mumps"
    sysTest.updateOutputPaths(sysTest.outputPathBase + "-mumps")
    sysTest.solverOpts = mumpsSolverOpts

def suite():
    return mumpsSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(mumpsSuite)
