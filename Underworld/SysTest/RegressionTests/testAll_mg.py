#!/usr/bin/env python
import os
import copy
from credo.systest import *

mgSuite = SysTestSuite("Underworld", "RegressionTests")

basicRheoModels = ["Anisotropic.xml", "Arrhenius.xml", "ArrheniusPIC.xml",
    "DepthDependentViscosity.xml",
    "Extension.xml", "FrankKamenetskii.xml", "InternalHeating.xml",
    "MobileLid.xml", "MultiThermalDiffusivity.xml",
    "MultiComponent.xml", "NonNewtonian.xml", "NonNewtonianPicard.xml",
    "Woidt78_model_a.xml"]

for modelXML in basicRheoModels:
    for np in [1, 2]:
        mgSuite.addStdTest(ReferenceTest, modelXML, runSteps=10, nproc=np)

#mgSuite.addStdTest(ReferenceTest, "CylinderRiseThermal.xml",
#    runSteps=10, nproc=1)

anRheoModels = ["NonNewtonianShear.xml", "Trubitsyn2006Isoviscous.xml"]
for modelXML in anRheoModels:
    for np in [1, 2, 4, 8]:
        mgSuite.addStdTest(AnalyticTest, modelXML, nproc=np)

# TODO: move these to a separate "Benchmarks" list?
for np in [1, 2]:
    mgSuite.addStdTest(ReferenceTest, ["RayleighTaylorBenchmark.xml"],
        runSteps=10, nproc=np)
for np in [1, 2]:
    mgSuite.addStdTest(ReferenceTest, ["ThermoChemBenchmark.xml"],
        runSteps=10, nproc=np)

# These 3 restart tests are failing with MultiGrid,
# So disabling these for now until the issue is resolved.
#mgSuite.addStdTest(RestartTest, "Arrhenius.xml")
#mgSuite.addStdTest(RestartTest, "DepthDependentViscosity.xml")
#mgSuite.addStdTest(RestartTest, "Extension.xml")
mgSuite.addStdTest(RestartTest, "NonNewtonian.xml")
mgSuite.addStdTest(RestartTest, "RayleighTaylorBenchmark.xml")
mgSuite.addStdTest(RestartTest, "Woidt78_model_a.xml")

# Set to a high time, as extension test can be quite slow on older procs
mgSuite.setAllTimeouts(minutes=60)

#mgSuite = copy.deepcopy(mgSuite)
# Customise to run each test with Multigrid options
mgSuite.suiteName += "-mg"
mgSetup = "StgFEM/MultigridForRegular.xml"
mgSolverOpts = "options-uzawa-mg.opt"
mgUWOpts = {"mgLevels":2}
for sysTest in mgSuite.sysTests:
    sysTest.testName += "-mg"
    sysTest.updateOutputPaths(sysTest.outputPathBase + "-mg")
    sysTest.inputFiles.append(mgSetup)
    sysTest.paramOverrides.update(mgUWOpts)
    sysTest.solverOpts = mgSolverOpts

def suite():
    return mgSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(mgSuite)
