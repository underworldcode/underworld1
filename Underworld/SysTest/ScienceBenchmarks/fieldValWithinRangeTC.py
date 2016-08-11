""""Custom TC to check a field value is within a given range at a given point.

.. note:: not adding to CREDO yet until we resolve best way to handle HDF5
   checkpoint access."""
import os
from xml.etree import ElementTree as etree
from credo.systest.api import SingleRunTestComponent, CREDO_PASS, CREDO_FAIL

def getFieldValue(mRes, fieldName, elementI, tStep):
    """Function to get value from a HDF5 field, presuming h5utils is installed,
    and using potentially space-intensive method of converting binary to 
    txt first."""
    cpFName = os.path.join(mRes.outputPath, "%sField.%.5d.h5" %\
        (fieldName, tStep))
    txtFieldFName = os.path.join(mRes.outputPath, "%s.dat" % fieldName)
    cmd = "h5totxt %s >> %s" % (cpFName, txtFieldFName)
    os.system(cmd)
    f2 = open(txtFieldFName, 'r')
    array2 = f2.readlines()
    line2 = array2[elementI]
    line2 = line2.lstrip('[')
    line2 = line2.rstrip(']\n')
    print line2, line2.split(',')  # test
    fValue = [float(x) for x in line2.split(',')]
    os.unlink(txtFieldFName)
    return fValue

class FieldValWithinRangeTC(SingleRunTestComponent):
    """Test component to check value of a field component in a given range."""
    def __init__(self, fieldName, elementI, fCompI, tStep, 
            allowedRange):
        SingleRunTestComponent.__init__(self, "outputWithinRange")
        self.fieldName = fieldName
        self.fCompI = fCompI
        self.elementI = elementI
        self.tStep = tStep
        self.allowedRange = allowedRange
        self.actualVal = None
        self.withinRange = None
   
    def _writeXMLCustomSpec(self, specNode):
        etree.SubElement(specNode, 'fieldName', value=self.fieldName)
        etree.SubElement(specNode, 'tStep', value=str(self.tStep))
        etree.SubElement(specNode, 'fCompI', value=str(self.fCompI))
        etree.SubElement(specNode, 'elementI', value=str(self.elementI))
        etree.SubElement(specNode, 'allowedRange-min',
            value=str(self.allowedRange[0]))
        etree.SubElement(specNode, 'allowedRange-max',
            value=str(self.allowedRange[1]))
    
    def attachOps(self, modelRun):
        """Implements base class
        :meth:`credo.systest.api.SingleRunTestComponent.attachOps`."""
        #TODO: enforce that required field is loaded up in XML as one to be
        # checkpointed.
        # Enforce that tStep is one that will be created as per dump step
        pass
    
    def check(self, mResult):
        """Implements base class
        :meth:`credo.systest.api.SingleRunTestComponent.check`."""
        fNodeVal = getFieldValue(mResult, self.fieldName, self.elementI,
            self.tStep)
        self.actualVal = fNodeVal[self.fCompI]
        self.withinRange = (self.allowedRange[0] <= self.actualVal \
            <= self.allowedRange[1])
        if not self.withinRange:
            resTxt = "not within"
            overallResult = False
        else:
            resTxt = "within"
            overallResult = True
        statusMsg = "Model field '%s', element %d, comp %d value %g"\
            " %s required range (%g,%g) at tstep %d."\
            % ((self.fieldName, self.elementI, self.fCompI, self.actualVal,
                resTxt,) + self.allowedRange + (self.tStep,))
        print statusMsg
        self._setStatus(overallResult, statusMsg)
        return overallResult            
        
    def _writeXMLCustomResult(self, resNode, resultsSet):
        avNode = etree.SubElement(resNode, 'actualValue')
        avNode.text = "%6e" % self.actualVal
        wrNode = etree.SubElement(resNode, 'withinRange')
        wrNode.text = str(self.withinRange)
