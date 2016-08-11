#!/usr/bin/env python
import credo.jobrunner
from credo.modelrun import ModelRun
from credo.analysis.fields import FieldComparisonOp, FieldComparisonList

mRun = ModelRun("CosineHillRotate", "CosineHillRotateBC.xml", "output/rotateAnalysis" )
mRun.writeInfoXML()

jobRunner = credo.jobrunner.defaultRunner()
mRes = jobRunner.runModel(mRun)
mRes.writeRecordXML()

#-----------------------------
# Post-process
fieldComps = FieldComparisonList()
# Should there be an interface to get from result too?
fieldComps.readFromStgXML(mRun.modelInputFiles, mRun.basePath)
fieldComps.postRun(mRun, mRun.basePath)
fCompResults = fieldComps.getAllResults(mRes)

for res in fCompResults:
    print "Error for field '%s' was %s" % (res.fieldName, res.dofErrors)
    res.plotOverTime(show=True, path=mRes.outputPath)

mRes.readFrequentOutput()
mRes.freqOutput.plotOverTime('Time', show=True, path=mRes.outputPath)

maxTime, maxTimeStep = mRes.freqOutput.getMax('Time')
print "Maximum value of time was %f, at step %d" % (maxTime, maxTimeStep)
