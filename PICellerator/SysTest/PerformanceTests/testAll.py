#!/usr/bin/env python

from credo.systest import *

testSuite = SysTestSuite("PICellerator", "PerformanceTests")

resSet1 = [(10,10),(20,20),(30,30),(40,40)]
for np in [1, 2, 4]:
    testSuite.addStdTest(AnalyticMultiResTest, ["BuoyancyExample.xml"],
        nproc=np, resSet=resSet1)

testSuite.setAllTimeouts(minutes=20)

def suite():
    return testSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(testSuite)
