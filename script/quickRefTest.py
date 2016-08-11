#! /usr/bin/env python
import getopt
import sys, os
import credo
import credo.systest


def exampleMSG():
# function for printing example usage
   print "\n\tEXAMPLE USAGE\n"
   print "\t./quickRefTest.py -c myInput.xml\t --- generates expected files, 1 proc run of myInput.xml"
   print "\t./quickRefTest.py myInput.xml\t\t --- tests against expected files of myInput.xml using 1 proc & 4 procs"
   print "\n\tMultiple input files can be given as arguments" 
   print "\n"
   sys.exit(1)

# parameter list for scriptable commands
paramOpts = None #["elementResI":15, "particlePerCell":10]

if __name__ == "__main__":

### SECTION: Parse input
   if len(sys.argv) < 2:
      exampleMSG()

   try:
      opts, args = getopt.gnu_getopt(sys.argv[1:], "hc", ["help","create"])
   except getopt.GetoptError:          
      exampleMSG()
      
 
   if len(args) < 1:
      exampleMSG()

   fileNames = args # grab the first argument (not command line option)
   for fileName in fileNames:
      if not os.path.isfile(fileName):
	 print "Error: Can't find file", fileName
	 sys.exit(1)

   generate=False
   for opt, arg in opts:
      if opt == "-c":
	 generate=True

### END SECTION: Parse input

# Define model
   mRun = credo.systest.referenceTest.ReferenceTest(fileNames, 
      "theReferenceTest",
      paramOverrides=paramOpts,
      runSteps=1,
      basePath=os.getcwd(),
      expPathPrefix="expected") 

# generate expected input and exit
   if generate: 
      mRun.regenerateFixture(credo.jobrunner.defaultRunner());
      sys.exit(0)

# setup testing suite, add tests (1&4proc) and execute using a job runner
   mSuite = credo.systest.SysTestSuite("projName", "suiteName")
   mSuite.addStdTest( credo.systest.ReferenceTest, fileName, paramOverrides=paramOpts, runSteps=1, defFieldTol=1e-7,expPathPrefix="expected" ) 
   mSuite.addStdTest( credo.systest.ReferenceTest, fileName, nproc=4, paramOverrides=paramOpts, runSteps=1, defFieldTol=1e-7,expPathPrefix="expected" ) 
   jobRunner = credo.systest.SysTestRunner()
   mResult = jobRunner.runSuite(mSuite) 
