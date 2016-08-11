#!/usr/bin/env python

from credo.systest import *

testSuite = SysTestSuite("Underworld", "RegressionTests")

basicRheoModels = ["Anisotropic.xml", "Arrhenius.xml", "ArrheniusPIC.xml", 
    "DepthDependentViscosity.xml",
    "Extension.xml", "FrankKamenetskii.xml", "InternalHeating.xml",
    "MobileLid.xml", "MultiThermalDiffusivity.xml", 
    "MultiComponent.xml", "NonNewtonian.xml", "NonNewtonianPicard.xml",
    "Woidt78_model_a.xml"]

for modelXML in basicRheoModels:
    for np in [1, 2]:
        testSuite.addStdTest(ReferenceTest, modelXML, runSteps=10, nproc=np)

# Example of check VelocityMagnitudeField
testSuite.addStdTest(ReferenceTest, "PoiseuilleFlow.xml", nproc=2, runSteps=0, fieldsToTest=["VelocityMagnitudeField","PressureField"])

testSuite.addStdTest(ReferenceTest, "CylinderRiseThermal.xml",
    runSteps=10, nproc=1)

for np in [1, 2]:
    testSuite.addStdTest(AnalyticTest, "testSLAdvectionDiffusion.xml", nproc=np, defFieldTol=0.001)

anRheoModels = ["NonNewtonianShear.xml", "Trubitsyn2006Isoviscous.xml"]
for modelXML in anRheoModels:
    for np in [1, 2, 4, 8]:
        testSuite.addStdTest(AnalyticTest, modelXML, nproc=np)

for np in [1, 2]:
    testSuite.addStdTest(AnalyticTest, "testSLAdvectionDiffusion.xml", nproc=np)


# TODO: move these to a separate "Benchmarks" list?
for np in [1, 2]:
    testSuite.addStdTest(ReferenceTest, ["RayleighTaylorBenchmark.xml"],
        runSteps=10, nproc=np)
for np in [1, 2]:
    testSuite.addStdTest(ReferenceTest, ["ThermoChemBenchmark.xml"],
        runSteps=10, nproc=np)

# add HDF5 condition function test
try:
	config_file = open("../../../config.cfg")
except IOError:
	print "config.cfg can't be opened, Did you build the code already?"
else:
	for line in config_file:
		if "HAVE_HDF5" in line:			
			for np in [1,2]:
				testSuite.addStdTest(ReferenceTest, "HDF5CondFuncExample.xml",
					nproc=np, expPathPrefix="expected", runSteps=0, fieldsToTest=["VelocityField"])
			break

# shapes tests
for np in [1,2]:
   testSuite.addStdTest(ReferenceTest, "Example_RBF_BelowHeightField.xml",
      nproc=np, expPathPrefix="expected", runSteps=0, fieldsToTest=["MaterialIndexField"])
   testSuite.addStdTest(ReferenceTest, "Example_RBF_BetweenHeightField.xml",
      nproc=np, expPathPrefix="expected", runSteps=0, fieldsToTest=["MaterialIndexField"])
   testSuite.addStdTest(ReferenceTest, "Example_RBF_FieldValueShape2D.xml",
      nproc=np, expPathPrefix="expected", runSteps=0, fieldsToTest=["MaterialIndexField"])
   testSuite.addStdTest(ReferenceTest, "Example_RBF_FieldValueShape3D.xml",
      nproc=np, expPathPrefix="expected", runSteps=0, fieldsToTest=["MaterialIndexField"])

testSuite.addStdTest(RestartTest, "Arrhenius.xml")
testSuite.addStdTest(RestartTest, "DepthDependentViscosity.xml")
testSuite.addStdTest(RestartTest, "Extension.xml")
testSuite.addStdTest(RestartTest, "NonNewtonian.xml")
testSuite.addStdTest(RestartTest, "RayleighTaylorBenchmark.xml")
testSuite.addStdTest(RestartTest, "Woidt78_model_a.xml")

# add melt model tests

mySolverOpts = 'melt_model/serialSolver.opts'
paramOpts = {"plugins[7].FeVariableHDF5Filename":"./melt_model/tempIC/TemperatureField.00700.h5", "plugins[7].MeshHDF5Filename":"./melt_model/tempIC/Mesh.linearMesh.00700.h5"}

testSuite.addStdTest( ReferenceTest, 'melt_model/air_crust_mantle.xml', nproc=1, paramOverrides=paramOpts, runSteps=1, defFieldTol=7e-4,expPathPrefix="expected", solverOpts=mySolverOpts )
 # now override the name because credo can't handle the paramOpts to create a valud testName
testSuite.sysTests[-1].testName='air_crust_mantle-referenceTest'


# Set to a high time, as extension test can be quite slow on older procs
testSuite.setAllTimeouts(minutes=60)

def suite():
    return testSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(testSuite)
