#!/usr/bin/env python

import unittest, os, os.path, shutil
import sys
#Find the directory one up so program can use these python programs.
sys.path.append( os.path.join( os.getcwd(), '..' ) )
import createDocs, createDocument

##\file 
## This file tests the functions in createDocs.py using the python unittest framework.

class TestSequenceFunctions(unittest.TestCase):

    def setUp(self):
        #Setup variables to be input into the List Directory
        self.curdir = os.getcwd()
        arg1 = ''
        arg2 = ''
        arg3 = ''
        arg4 = ''
        directoryPath = os.path.realpath(os.path.join(self.curdir,'../'))
        docPath = os.path.realpath(os.path.join(self.curdir,'./testresult'))
        docDataPath = os.path.realpath(os.path.join(self.curdir, '../../doc'))
        arg5 = ''
        pluginsDirectories = ""
        self.values = [arg1, arg2, arg3, arg4, directoryPath, docPath, docDataPath, arg5, pluginsDirectories ]


    def test_createListDictionary(self):

        # Create a dictionary (basic values)
        basicDict = createDocs.createListDictionary(self.values[0], self.values[1], self.values[2],self.values[3],self.values[4],self.values[5],self.values[6],self.values[7],self.values[8])
        #check whether entries passed in correctly.
        self.assertEqual(basicDict['arg1'], self.values[0])
        self.assertEqual(basicDict['arg2'], self.values[1])
        self.assertEqual(basicDict['arg3'], self.values[2])
        self.assertEqual(basicDict['arg4'], self.values[3])
        self.assertEqual(basicDict['directoryPath'], self.values[4])
        self.assertEqual(basicDict['docPath'], self.values[5])
        self.assertEqual(basicDict['docDataPath'], self.values[6])
        self.assertEqual(basicDict['arg5'], self.values[7])      
        # test for a few set default values and some directories. Not exhaustive.
        self.assertEqual(basicDict['metaFlag'], 'dtd')
        self.assertEqual(basicDict['extWeb'], [['http://www.underworldproject.org','Underworld Home Page']])
        self.assertEqual(basicDict['email'], 'underworld-users@vpac.org')
        self.assertEqual(basicDict['doxyToggle'], 'Yes')
        self.assertEqual(basicDict['configFile'], 'Doxyfile')
        self.assertTrue(os.path.isfile( basicDict['configPath']))
        #create dictionaries to use all toggle options. and get output.
        liteDict = createDocs.createListDictionary(self.values[0], self.values[1], self.values[2],'No',self.values[4],self.values[5],self.values[6],self.values[7],self.values[8]) 
        self.assertEqual(liteDict['doxyToggle'], 'No')
        self.assertTrue(os.path.isfile( liteDict['configPath']))

        print "Tested createListDictionary"
        
    def test_findProjectDirectories(self):

        directoryPath = os.path.realpath('../../../')
        projectList = createDocs.findProjectDirectories(directoryPath)

        # Make it applicable for UW with and without Experimental toolbox.
        if projectList.count('Experimental') > 0 :
            testList = ['StGermain', 'StgDomain', 'StgFEM', 'PICellerator', 'Underworld', 'gLucifer', 'Experimental']
        else:
            testList = ['StGermain', 'StgDomain', 'StgFEM', 'PICellerator', 'Underworld', 'gLucifer']
         #Test to make sure list contains everything up to UW. This test is going to fail for non-UW projects
        self.assertEqual(sorted(projectList), sorted(testList))

        print "Tested findProjectDirectories"
    def test_printHelpStatement(self):
        pass

    def test_findVersion(self):
        #Deliberately set directory to 1 higher than stg Dir. This should produce a 'fail' in most cases.(we want 
        # it to 'fail' so it uses the default value. Want this test to work on all versions of UW.)
        directoryPath = os.path.realpath('../../../../')
        #This test only tests this function. Not if hg is there. 
        basicDict = createDocs.createListDictionary(self.values[0], self.values[1], self.values[2],self.values[3],directoryPath,self.values[5],self.values[6],self.values[7],self.values[8])
        #redirecting output of function so it doesn't display to stout
        saveout = sys.stdout  
        fsock = open('test_findVersion.log', 'w') 
        sys.stdout = fsock  
        #run function
        createDocs.findVersion(basicDict)
        sys.stdout = saveout    
        fsock.close() 
        #should go back to stout now ...
        # check the version.
        self.assertEqual(basicDict['projectNumber'], 'Bleeding Edge')
        print "Tested findVersion"

    def test_copyPics(self):
        # redirect output
        saveout = sys.stdout  
        fsock = open('test_copyPics.log', 'w') 
        sys.stdout = fsock
        # copy dir contents          
        createDocs.copyPics(os.path.realpath('.'),self.values[5])
        sys.stdout = saveout    
        fsock.close() 
        #should go back to stout now ...
        #check if it copied stuff.
        self.assertTrue(os.path.isfile(os.path.join(self.values[5], 'testCreateDocs.py')))
        print "Tested copyPics"

    def test_createCodex(self):
        #This test will be longish. Have to create a full codex.
        # Will fail for non-UW versions.
        directoryPath = os.path.realpath('../../../') 
        basicDict = createDocs.createListDictionary(self.values[0], self.values[1], self.values[2],self.values[3],directoryPath,self.values[5],self.values[6],self.values[7],self.values[8])
        #redirecting output of function so it doesn't display to stout
        saveout = sys.stdout  
        fsock = open('test_createCodex.log', 'w') 
        sys.stdout = fsock  
        #create Codex
        createDocument.checkOutputPath(self.values[5])
        createDocs.findVersion(basicDict)
        createDocs.createCodex(basicDict)
        sys.stdout = saveout    
        fsock.close() 
        #should go back to stout now ...
        #check for existence of html files that should have been created.
        self.assertTrue(os.path.isfile(os.path.join(basicDict['codexSubDirPath'], 'index.html')))
        self.assertTrue(os.path.isfile(os.path.join(basicDict['codexSubDirPath'], 'StGermain.html')))
        self.assertTrue(os.path.isfile(os.path.join(basicDict['codexSubDirPath'], 'StgDomain.html')))
        self.assertTrue(os.path.isfile(os.path.join(basicDict['codexSubDirPath'], 'StgFEM.html')))
        self.assertTrue(os.path.isfile(os.path.join(basicDict['codexSubDirPath'], 'PICellerator.html')))
        self.assertTrue(os.path.isfile(os.path.join(basicDict['codexSubDirPath'], 'Underworld.html')))
        self.assertTrue(os.path.isfile(os.path.join(basicDict['codexSubDirPath'], 'gLucifer.html')))

        print "Tested createCodex"

    def test_createDoxygen(self):
        #create dictionary
        basicDict = createDocs.createListDictionary(self.values[0], self.values[1], self.values[2],self.values[3],self.values[4],self.values[5],self.values[6],self.values[7],self.values[8])
        print "Output from test test_createDoxygen. Please ignore"
        #print self.values
        print "%%%%%%%%%%%%%%%%%%%%%%%%%"
        #redirecting output of function so it doesn't display to stout. Needs more work.
        saveout = sys.stdout  
        fsock = open('test_createDoxygen.log', 'w') 
        sys.stdout = fsock
        #run createDoxygen
        createDocument.checkOutputPath(self.values[5])
        createDocs.findVersion(basicDict)
        createDocs.createDoxygen(basicDict)
        sys.stdout = saveout    
        fsock.close() 
        #should go back to stout now ...
        print "%%%%%%%%%%%%%%%%%%%%%%%%%"
        #check for main index.html page in standard location. If it's there, Doxygen worked. Maybe not perfectly,
        # but it's there and running.
        self.assertTrue(os.path.isfile(os.path.join(basicDict['doxygenSubDirPath'], "html/index.html")))

        print "Tested Doxygen"


    def test_createTempFile(self):
        #create dictionary
        basicDict = createDocs.createListDictionary(self.values[0], self.values[1], self.values[2],self.values[3],self.values[4],self.values[5],self.values[6],self.values[7],self.values[8])
        basicDict['tempPythonFilePath'] = os.path.realpath(self.values[5] + "/tmpfilter.py")
        #redirecting output of function so it doesn't display to stout. Needs more work.
        saveout = sys.stdout  
        fsock = open('test_createTempFile.log', 'w') 
        sys.stdout = fsock

        #run creation file
        createDocument.checkOutputPath(self.values[5])
        createDocs.createTempFile(basicDict)

        sys.stdout = saveout    
        fsock.close() 
        #should go back to stout now ...
        #test for existence. and that it runs.
        self.assertTrue(os.path.isfile(basicDict['tempPythonFilePath']))

        print "Tested createTempFile"

    def tearDown(self):
        # Change directory back to saved dir at start.
        os.chdir(self.curdir)

        # Remove log files
        if os.path.isfile('test_createDoxygen.log'):
            os.remove('test_createDoxygen.log')

        if os.path.isfile('test_copyPics.log'):
            os.remove('test_copyPics.log')

        if os.path.isfile('test_createCodex.log'):
            os.remove('test_createCodex.log')

        if os.path.isfile('test_findVersion.log'):
            os.remove('test_findVersion.log')

        if os.path.isfile('test_createTempFile.log'):
            os.remove('test_createTempFile.log')

        # remove output of tests sub-directory
        if os.path.isdir(self.values[5]):
            shutil.rmtree(self.values[5])
        pass
if __name__ == '__main__':
    unittest.main()

