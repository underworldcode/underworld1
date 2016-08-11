#!/usr/bin/env python

from credo.systest import *
testSuite = SysTestSuite("PICellerator", "RegressionTests")

for np in [1,2,4,8]:
    testSuite.addStdTest(AnalyticTest, "AnalyticShearXZ.xml", nproc=np)

testSuite.setAllTimeouts(minutes=10)    

def suite():
    return testSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(testSuite)
