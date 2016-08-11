#!/usr/bin/env python

from credo.systest import *

testSuite = SysTestSuite("Underworld", "PerformanceTests-rep") 

resSet2D = [(16,16), (32,32), (64,64), (128,128)]

velicModels = [
    ["testVelicSolA.xml", "REP/testVelicSolA-REP.xml"],
    ["testVelicSolB.xml", "REP/testVelicSolB-REP.xml"], 
    ["testVelicSolCx.xml", "REP/testVelicSolCx-REP.xml"],
    ["testVelicSolKz.xml", "REP/testVelicSolKz-REP.xml"],
    ["testVelicSolS.xml", "REP/testVelicSolS-REP.xml" ] ]

# REP Version of tests
repOpts = {"components.constitutiveMatrix.storeConstitutiveMatrix":0,
    "components.recoveredStressField.useStoredConstitutive":0}
for modelXML in velicModels:
    testSuite.addStdTest(AnalyticMultiResTest, modelXML,
        resSet=resSet2D, paramOverrides=repOpts, nameSuffix="rep")

testSuite.setAllTimeouts(minutes=30)

def suite():
    return testSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(testSuite)
