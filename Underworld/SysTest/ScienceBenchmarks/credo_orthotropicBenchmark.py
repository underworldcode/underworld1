#! /usr/bin/env python
import os
import itertools
from credo.systest import *
from credo.modelrun import ModelRun, SimParams
from credo.modelsuite import StgXMLVariant
import credo.modelsuite as msuite
import credo.jobrunner
from fieldValWithinRangeTC import FieldValWithinRangeTC
import ortho_analytic
import credo.reporting.standardReports as sReps
from credo.reporting import getGenerators

testSuite = SysTestSuite("Underworld", "SciBench-Orthotropic")
sciBTest = SciBenchmarkTest("OrthotropicRheologyBenchmark")
testSuite.sysTests.append(sciBTest)

mRun = ModelRun("OrthotropicRheologyBenchmark", "Orthotropic45UnderVelBCs.xml",
    nproc=2)
sciBTest.mSuite.templateMRun = mRun

# Running a suite of models for this Benchmark for 3 Orthotropic 
# Constitutive matrices:
C_h11 = 1000
C_h22 = 1000
#C_h33 will be varied
C_h12 = 0
C_h13 = 0
C_h23 = 0

Ch33Range = [50.0, 10.0, 1.0]
expStress = []
for C_h33 in Ch33Range:
    s = ortho_analytic.calc_stress(C_h11, C_h22, C_h33, C_h12, C_h13, C_h23)
    expStress.append(s)

sciBTest.mSuite.addVariant("C33",
    StgXMLVariant("components.anisoVisc.C33", Ch33Range))
sciBTest.mSuite.subOutputPathGenFunc = msuite.getSubdir_RunIndexAndText
iterStrategy = itertools.izip
sciBTest.mSuite.generateRuns(iterGen=iterStrategy)

# Calculate expected solutions ... rearrange expStress into lists of min
#  and max stress components per run
fCompMap = {"XX":0, "YY":1, "XY":2}
minStress = {}
maxStress = {}
for comp, compI in fCompMap.iteritems():
    minStress[comp] = [0 for ii in range(len(expStress))]
    maxStress[comp] = [0 for ii in range(len(expStress))]
    for runI in range(len(expStress)):
        minStress[comp][runI] = expStress[runI][compI] - .1
        maxStress[comp][runI] = expStress[runI][compI] + .1
#Hand-adjust for last run X-Y
minStress['XY'][2] = expStress[2][fCompMap['XY']] - .8
maxStress['XY'][2] = expStress[2][fCompMap['XY']] + .8

#print "minStress = %s" % minStress
#print "maxStress = %s" % maxStress

# Add DeviatoricStress and Stress tests
sciBTest.setupEmptyTestCompsList()
for runI, mRun in enumerate(sciBTest.mSuite.runs):    
    # Get element_I:- ideally would be done using some func that can
    #  calc these based on known field geometry
	# Get values at central position ie 0.25, 0.35 which is at line 1811:
    elI = 1810
    for fName in ["recoveredDeviatoricStress", "Stress"]:
        for fComp, fCompI in fCompMap.iteritems():
            sciBTest.addTestComp(runI, "%s-%s" % (fName, fComp),
                FieldValWithinRangeTC(fName, elI, fCompI, 1,
                    (minStress[fComp][runI], maxStress[fComp][runI])))

def customReporting(sciBTest, mResults):
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    stress = {}
    recDeviatoricStress = {}
    recDeviatoricStressTCs = {}
    recDeviatoricStressRes = {}
    stressTCs = {}
    stressRes = {}
    for fComp in ["XX", "XY", "YY"]:
        recDeviatoricStressTCs[fComp], recDeviatoricStressRes[fComp] = sciBTest.getTCRes(\
            "recoveredDeviatoricStress-%s" % fComp)
        stressTCs[fComp], stressRes[fComp] = sciBTest.getTCRes(\
            "Stress-%s" % fComp)
        recDeviatoricStress[fComp] = [tc.actualVal for tc in recDeviatoricStressTCs[fComp]]
        stress[fComp] = [tc.actualVal for tc in stressTCs[fComp]]

    inIter = msuite.getVariantIndicesIter(sciBTest.mSuite.modelVariants,
        iterStrategy)
    varDicts = msuite.getVariantNameDicts(sciBTest.mSuite.modelVariants,
        inIter)
    for resI, mRes in enumerate(mResults):
        print "Post-process result %d" % (resI),
        print "with variants applied of:"
        print varDicts[resI]
        
        print "Value of recDeviatoricStress(%s) is (%s)\n stress(%s) is (%s)." %\
            (", ".join(fCompMap),
                ", ".join(["%g" % recDeviatoricStress[c][resI] for c in fCompMap]),
                ", ".join(fCompMap),
                ", ".join(["%g" % stress[c][resI] for c in fCompMap]))
    # TO DO:
    #plotDeviatoricStressVsAnalytic(maxStressXX, maxStressYY, maxStressXY,
    #	minStressXX, minStressYY, minStressXY,
    #    {"DeviatoricStress_XX":recDeviatoricStress['XX']}, {"DeviatoricStress_YY":recDeviatoricStress['YY']},
    #    {"DeviatoricStress_XY":recDeviatoricStress['XY']},
    #	{"Stress_XX":stressXX}, {"Stress_YY":stressYY}, {"Stress_XY":stressXY})

    #plt.savefig(os.path.join(mSuite.outputPathBase,
    #    "RecoveredDeviatoricStressAndStressValues.png"), 
    #       dpi=None, facecolor='w', edgecolor='w',
     #      orientation='portrait', papertype=None, format=None,
      #    transparent=False)	  
            
    # Save to a CSV file.
    observables = {'recovered DeviatoricStress XX': recDeviatoricStress['XX'],
        'recovered DeviatoricStress XX Passed': recDeviatoricStressRes['XX'],
        'recovered DeviatoricStress YY': recDeviatoricStress['YY'],
        'recovered DeviatoricStress YY Passed': recDeviatoricStressRes['YY'],
        'recovered DeviatoricStress XY': recDeviatoricStress['XY'],
        'recovered DeviatoricStress XY Passed': recDeviatoricStressRes['XY'],
        'stress XX': stress['XX'],
        'stress XX Passed': stressRes['XX'],
        'stress YY': stress['YY'], 
        'stress YY Passed': stressRes['YY'],
        'stress XY': stress['XY'],
        'stress XY Passed': stressRes['XY']}
    msuite.writeInputsOutputsToCSV(sciBTest.mSuite, observables,
        "OrthotropicTestStressValues.csv")
    sciBTest.mSuite.analysisImages = None    
    sciBTest.mSuite.modelImagesToDisplay = None
    for rGen in getGenerators(["RST", "ReportLab"], sciBTest.outputPathBase):
        sReps.makeSciBenchReport(sciBTest, mResults, rGen,
            os.path.join(sciBTest.outputPathBase, "%s-report.%s" %\
                (sciBTest.testName, rGen.stdExt)))

sciBTest.setCustomReporting(customReporting)

def suite():
    return testSuite

if __name__ == "__main__":
    postProcFromExisting = True
    jobRunner = credo.jobrunner.defaultRunner()
    testResult, mResults = sciBTest.runTest(jobRunner, postProcFromExisting,
        createReports=True)
