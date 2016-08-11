#!/usr/bin/env python

from credo.systest import *

testSuite = SysTestSuite("Underworld", "PerformanceTests-default")
resSet2D = [(16,16), (32,32), (64,64), (128,128)]
resSet3D = [(4,4,4), (8,8,8), (16,16,16), (32,32,32)]

velicModels = ["testVelicSolA.xml", "testVelicSolB.xml", 
    "testVelicSolCx.xml", "testVelicSolKz.xml", "testVelicSolS.xml"]

# Regular tests
for modelXML in velicModels:
    for np in [1, 2]:
        testSuite.addStdTest(AnalyticMultiResTest, modelXML, resSet=resSet2D,
            nproc=np)

for np in [1, 2]:
    testSuite.addStdTest(AnalyticMultiResTest, 
        "testDepthDependentViscosity3D_Exponential.xml", resSet=resSet3D,
        nproc=np)

testSuite.setAllTimeouts(minutes=30)

def suite():
    # We want to test the SPR and REP versions, as well as these defaults
    import sprTestAll
    import repTestAll
    allSuite = SysTestSuite("Underworld", "PerformanceTests")
    allSuite.addSubSuites([testSuite, sprTestAll.suite(), repTestAll.suite()])
    return allSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(suite())
