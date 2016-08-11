#!/usr/bin/python

## \file 
## Code to create Doxygen on it's own. It has more options on where/what and how to create them.

import createDocs
import createHTMLDocuments, createDocument
import sys, os.path, os, string, subprocess, shutil
from createDocument import createDocuments
from createDocs import createListDictionary
from createDocs import createDoxygen
from createDocs import findVersion

## Function for specific help statement for running Doxygen ONLY
def printHelpStatement(dictionary):

         print "Help for createDoxygen.py. This file is designed to be standalone."
         print "To run, type ./createDoxygen.py \n"
         print "To run with input options: "
         print "./createDoxygen.py DOXYGENINPUTDIR DOXYGENOUTPUTDIR DOCDATAPATH DOXYGENLONG DOXYGENCONFIGFILE PLUGINDIRECTORIES \n"
         print "Defaults:"
         print "DOXYGENINPUTDIR: Name of directory for code base. Default: '../../', Current: "+ dictionary['directoryPath']
         print "DOXYGENOUTPUTDIR: Name of directory to which Doxygen will output it's files. Default: '../../doc/', Current: "+ dictionary['docPath']
         print "DOCDATAPATH: Name of path to StGermain/doc/ directory, which has necessary file for Doxygen run. Default: "+dictionary['directoryPath']+"/StGermain/doc/', Current: "+ dictionary['docDataPath']
         print "DOXYGENLONG: Turns On/Off The long image-intense doxygen configuration. Options: 'Yes' or 'No'. Default: 'No'., Current: "+ dictionary['doxyToggle']
         print "DOXYGENCONFIGFILE: Name of Doxygen Configuration file.  Default: DoxyfileLite, Current: "+dictionary['configFile']
         print "PLUGINDIRECTORIES: Dictionary of Toolboxes and directories. This is for scons use and should be left blank. Default: ''"
         print "*******WARNING********"
         print "ALL data in "+dictionary['docPath']+ " will be overwritten or removed!"
         print "**********************" 
         print "Other defaults: (Not all necessary) \n"
         #print dictionary.items()


if __name__=='__main__':
    values = sys.argv
    for i in range(8):
        if len(values)< 8:
            values.append("")

    #Define input directory (relative to ~/stgUnderworldE/ )
    if (values[1] != "") and not( (values[1] == "help") or (values[1] == '-h') or (values[1] =='--help') or (values[1] == 'h' ) ):
        directoryPath = os.path.realpath(values[1])        
    else:
        directoryPath = os.path.realpath('../../')
    print directoryPath
    # Create the output directory. This is relative to the ./mainProj/config page.(relative to ~/stgUnderworldE/ )
    if values[2] != "":
        docPath = os.path.realpath(values[2])
    else:
        docPath = os.path.join(directoryPath, 'doc/')
    print docPath

    # createDictionary
    mainDictionary = createListDictionary("","", "", values[4], directoryPath, docPath, values[3], values[5], values[6])

    # Set up help print statement
    if ((values[1] == "help") or (values[1] == '-h') or (values[1] =='--help') or (values[1] == 'h')):
        printHelpStatement(mainDictionary)

    else:
        #check the directory now exists.
        createDocument.checkOutputPath(docPath)

        findVersion(mainDictionary)
        
        createDoxygen(mainDictionary)



