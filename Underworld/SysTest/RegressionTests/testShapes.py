#!/usr/bin/env python

from credo.systest import *

testSuite = SysTestSuite("Underworld", "RegressionTests")

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

# Set to a high time, as extension test can be quite slow on older procs
testSuite.setAllTimeouts(minutes=60)

def suite():
    return testSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(testSuite)
