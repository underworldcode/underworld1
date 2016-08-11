#!/usr/bin/env python

from credo.systest import *

testSuite = SysTestSuite("Underworld", "PerformanceTests-spr")

resSet2D = [(16,16), (32,32), (64,64), (128,128)]

velicModels = [
    ["testVelicSolA.xml", "SPR/testVelicSolA-SPR.xml"],
    ["testVelicSolB.xml", "SPR/testVelicSolB-SPR.xml" ], 
    ["testVelicSolCx.xml", "SPR/testVelicSolCx-SPR.xml" ],
    ["testVelicSolKz.xml", "SPR/testVelicSolKz-SPR.xml" ],
    ["testVelicSolS.xml", "SPR/testVelicSolS-SPR.xml"] ]

# SPR Version of tests
sprOpts = {"components.constitutiveMatrix.storeConstitutiveMatrix":0}
for modelXML in velicModels:
    testSuite.addStdTest(AnalyticMultiResTest, modelXML,
        resSet=resSet2D, paramOverrides=sprOpts, nameSuffix="spr")

testSuite.setAllTimeouts(minutes=30)

def suite():
    return testSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(testSuite)
