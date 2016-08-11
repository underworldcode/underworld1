#!/usr/bin/env python

from credo.systest import *

testSuite = SysTestSuite("Underworld", "RegressionTests-lowres")

lowResOpts = {"elementResI":10, "elementResJ":10, "elementResK":10}

models = ["Anisotropic.xml", "Arrhenius.xml", "ArrheniusPIC.xml", 
    "DepthDependentViscosity.xml",
    "Extension.xml", "FrankKamenetskii.xml", "InternalHeating.xml",
    "MobileLid.xml", "MultiThermalDiffusivity.xml", 
    "MultiComponent.xml", "NonNewtonian.xml", "NonNewtonianPicard.xml",
    "Nightmare.xml", "RayleighTaylorBenchmark.xml",
    "ThermoChemBenchmark.xml"]

for modelXML in models:
    for np in [1, 2]:
        testSuite.addStdTest(ReferenceTest, modelXML,
            nproc=np, expPathPrefix="lowres_expected",
            nameSuffix="lowres",
            paramOverrides=lowResOpts, runSteps=5)

# Example of check VelocityMagnitudeField
testSuite.addStdTest(ReferenceTest, "PoiseuilleFlow.xml", nproc=1, runSteps=0, fieldsToTest=["VelocityMagnitudeField","PressureField"])


for np in [1, 2]:
    testSuite.addStdTest(AnalyticTest, "testSLAdvectionDiffusion.xml", nproc=np, defFieldTol=0.001, paramOverrides=lowResOpts)

# Parallel version of cylinder rise thermal failing (was carried over
# from prev test system)
# TODO: this needs to be checked out in future.
testSuite.addStdTest(ReferenceTest, "CylinderRiseThermal.xml",
    nproc=1, expPathPrefix="lowres_expected",
    nameSuffix="lowres",
    paramOverrides=lowResOpts, runSteps=5)

# use different resolutions for Woidt78_model_a.xml
lowResOpts = {"elementResI":10, "elementResJ":25, "elementResK":10}

for np in [1,2]:
   testSuite.addStdTest(ReferenceTest, "Woidt78_model_a.xml",
         nproc=np, expPathPrefix="lowres_expected",
         nameSuffix="lowres",
         paramOverrides=lowResOpts, runSteps=5)

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


testSuite.setAllTimeouts(minutes=30)

def suite():
    return testSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(testSuite)
