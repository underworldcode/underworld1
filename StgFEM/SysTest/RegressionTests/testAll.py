#!/usr/bin/env python

from credo.systest import *

testSuite = SysTestSuite("StgFEM", "RegressionTests")

analyticModels = ["CosineHillRotateBC.xml", "CosineHillRotateBC-DualMesh.xml",
    "HomogeneousNaturalBCs.xml", "HomogeneousNaturalBCs-DualMesh.xml",
    "SteadyState1D-x.xml", "SteadyState1D-y.xml",
    "AnalyticSimpleShear.xml"]

for modelXML in analyticModels:
    for nproc in [1, 2, 4]:
        testSuite.addStdTest(AnalyticTest, modelXML, nproc=nproc)

ss0_5Opts = {"defaultDiffusivity":0.5, "A":0.1}
for nproc in [1, 2, 4]:
    testSuite.addStdTest(AnalyticTest, ["SteadyState1D-x.xml"],
        nproc=nproc, paramOverrides=ss0_5Opts)

asOpts = {"sinusoidalLidWavenumber":1}
for nproc in [1, 2, 4]:
    testSuite.addStdTest(AnalyticTest, ["AnalyticSinusoid.xml"],
        nproc=nproc, paramOverrides=asOpts)

for nproc in [1, 2, 4, 8]:
    testSuite.addStdTest(AnalyticTest, ["TempDiffusion.xml"],
        nproc=nproc)

testSuite.addStdTest(HighResReferenceTest, ["TempDiffusionHR.xml"],
    fieldsToTest=["TemperatureField"], highResRatio=4)

for nproc in [1,2]:
    testSuite.addStdTest(RestartTest, ["Multigrid.xml"],
        solverOpts='options-uzawa-mg.opt', nproc=nproc)
    testSuite.addStdTest(ReferenceTest, ["Multigrid.xml"],
        solverOpts='options-uzawa-mg.opt', nproc=nproc)
    testSuite.addStdTest(HighResReferenceTest, ["Multigrid.xml"],
        highResRatio=2, fieldTols={"VelocityField":0.2, "PressureField":0.5},
        solverOpts='options-uzawa-mg.opt', nproc=nproc)

testSuite.setAllTimeouts(minutes=10)

def suite():
    return testSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(testSuite)
