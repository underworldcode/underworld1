#!/usr/bin/env python

from credo.systest import *

testSuite = SysTestSuite("StgFEM", "PerformanceTests")
resSet1 = [(10,10), (20,20), (30,30), (40,40)]
paramSet = {"sinusoidalLidWavenumber":1}
for np in [1, 2, 4]:
    testSuite.addStdTest(AnalyticMultiResTest, ["AnalyticSinusoid.xml"],
        nproc=np, resSet=resSet1, paramOverrides=paramSet)

testSuite.setAllTimeouts(minutes=10)

def suite():
    return testSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(testSuite)
