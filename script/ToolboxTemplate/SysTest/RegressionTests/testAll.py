#!/usr/bin/env python

from credo.systest import *

testSuite = SysTestSuite("JAMBox", "RegressionTests")

basicRheoModels = ["SomeModel.xml", "SomeOtherModel.xml", "etcModel.xml"]

for modelXML in basicRheoModels:
    for np in [1, 2]:
        testSuite.addStdTest(ReferenceTest, modelXML, runSteps=10, nproc=np)

# Set to a high time, as extension test can be quite slow on older procs
testSuite.setAllTimeouts(minutes=60)

def suite():
    return testSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(testSuite)
