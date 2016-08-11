#!/usr/bin/env python

## \file
## createDocs is designed to be run through "scons doc" from the base directory.
## It will create Codex and Doxygen webpages in appropriate default directories.

import createHTMLDocuments, createDocument
import subprocess
import stat
import sys, os.path, os, string, subprocess, shutil, subprocess
from createDocument import createDocuments
from createDocument import Project
from createDocument import Meta
from createHTMLDocuments import createHTMLDocuments
from createHTMLDocuments import DivIds

##This which function is taken from:
##http://stackoverflow.com/questions/377017/test-if-executable-exists-in-python
def which(program):
    def is_exe(fpath):
        return os.path.exists(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None




## find a list of projects in the base directory, and remove obvious non-project listings
def findProjectDirectories(mainDirectory):
    directory = os.path.realpath(mainDirectory)
    # find a list of projects in the base directory
    projectNames = os.listdir(directory)
    # remove items from list that are not projects:
    # known examples: config, DOC, Doc, doc
    directoryValues = ['config', 'DOC', 'doc', 'Doc', '.hg', 'DOXYGEN', 'build', 'script', '.sconf_temp', 'scons', 'Doxygen', 'doxygen', 'credo', 'testLogs', 'BuildRecipes']
    projectTempList = projectNames
    for value in directoryValues:
        for project in projectTempList:
            if (string.count(project, value) > 0):
                if projectNames.count(project) > 0:
                    projectNames.remove(project)

    # Make sure only directories are included
    projList = []
    for value in projectNames:
        #print value, os.path.isdir(str(mainDirectory + '/'+value))
        if (os.path.isdir(str(mainDirectory + '/'+value)) == True):
            projList.append(value)
    #print "Creating pages for projects: ", projList
    return projList

## Create a dictionary that contains all data needed to create Codex and Doxygen pages
def createListDictionary(arg1, arg2, arg3, arg4, directoryPath, docPath, docDataPath, arg5, pluginsDirectories):
    dictionary = {}
    # Add items to dictionary

    dictionary['directoryPath'] = os.path.realpath(directoryPath)

    dictionary['docPath'] = os.path.realpath(docPath)
    if docDataPath == "":
        dictionary['docDataPath'] = os.path.realpath(os.path.join(directoryPath,'StGermain/doc/'))
    else:
        dictionary['docDataPath'] = os.path.realpath(docDataPath)

    if docDataPath != "":
        dictionary['docScriptPath'] = os.path.join(string.rstrip(os.path.realpath(dictionary['docDataPath']), "doc/"), "script/")
    else:
        dictionary['docScriptPath'] = os.path.realpath(os.path.join(directoryPath,'StGermain/script/'))

    # Add pluginsList dictionary
    if pluginsDirectories != "":
        dictionary['pluginsDirectories'] = eval(pluginsDirectories)
    else:
        dictionary['pluginsDirectories'] = {}
    # find base Directory from docDataPath
    dictionary['baseDirectory'] = (string.split(dictionary['docDataPath'], "StGermain/doc"))[0]
    # Define Codex values
    # Codex Subdir
    dictionary['codexSubDir'] = 'Codex'
    dictionary['codexSubDirPath'] = os.path.join(dictionary['docPath'], dictionary['codexSubDir'])    
    # Define stylesheet data to input
    dictionary['stylesheetList'] = [os.path.join(dictionary['docDataPath'], 'print.css'),os.path.join(dictionary['docDataPath'], 'menu.css'),os.path.join(dictionary['docDataPath'], 'codexStylesheet.css')]

    # Define javascript to input
    dictionary['script'] =[os.path.join(dictionary['docDataPath'], 'menuscript.js')] 
    #Define blurb
    dictionary['documentBlurb'] ="This is a list of the available components."
    
    # Define picture directory
    dictionary['imagePath'] = os.path.join(dictionary['docDataPath'], 'images')

    #Define output picture directory
    dictionary['pictureDirectory'] = 'images/'

    dictionary['arg1'] = arg1
    dictionary['arg2'] = arg2
    dictionary['arg3'] = arg3
    dictionary['arg4'] = string.capitalize(arg4)
    dictionary['arg5'] = arg5
    # Get data read in from command line
        
    if (dictionary['arg1'] == ""):
        dictionary['metaFlag'] = 'dtd'
    elif ((dictionary['arg1'] == "help") or (dictionary['arg1'] == '-h') or (dictionary['arg1'] =='--help') or (dictionary['arg1'] == 'h')):
        dictionary['metaFlag'] = 'dtd'
    else:
        dictionary['metaFlag'] = arg1

    if (dictionary['arg2'] == ""):
        dictionary['extWeb'] = [['http://www.underworldproject.org','Underworld Home Page']]
    else:
        dictionary['extWeb'] = arg2
    
    if (dictionary['arg3'] == ""):
        dictionary['email'] = 'underworld-users@vpac.org'
    else:
        dictionary['email'] = arg3

    #Sets which Doxygen config file to use. Yes
    if (dictionary['arg4'] == ""):
        dictionary['doxyToggle'] = 'Yes'
    else:
        dictionary['doxyToggle'] = dictionary['arg4']

    #doxytoggle and arg5 flags work together to decide which config file doxygen will use.
    # Arg4 is an on/off switch flag. If blank, it will assume it is off.
    # Arg4 toggles between the 2 default names for the doxygen config file: DoxyfileLite and Doxyfile.
    # They are virtually identical, except that DoxyfileLite had DOT turned OFF.
    # If arg5 is not blank, the code will use the user-specified doxygen config file, no matter what the 
    # toggle setting.
    
    if ((dictionary['doxyToggle'] == "No") and (dictionary['arg5'] == "")):
        dictionary['configFile'] = 'DoxyfileLite'
    elif ((dictionary['doxyToggle'] == "Yes") and (dictionary['arg5'] == "")):
        dictionary['configFile'] = 'Doxyfile'
    elif (dictionary['arg5'] != ""):
        dictionary['configFile'] = str(arg5)
    else:
        dictionary['configFile'] = 'DoxyfileLite'
        
        #Define location of content for index html page
    if (dictionary['metaFlag'] == 'dtd'):
        dictionary['indexFileName'] = os.path.join(dictionary['docDataPath'], 'IndexContent-Dtd.html')
    elif (dictionary['metaFlag'] == 'xsd'):
        dictionary['indexFileName'] = os.path.join(dictionary['docDataPath'], 'IndexContent-Xsd.html')
    else:
        dictionary['indexFileName'] = ""

    # Doxygen values
    dictionary['doxygenSubDir'] = 'Doxygen'
    dictionary['doxygenSubDirPath'] = os.path.join(dictionary['docPath'], dictionary['doxygenSubDir'])
    dictionary['configPath'] = os.path.join(dictionary['docDataPath'], dictionary['configFile'])
    dictionary['headerFilterPath'] = dictionary['docScriptPath']+"/headerfilter.py"
    dictionary['pluginFilterPath'] = dictionary['docScriptPath']+"/pluginfilter.py"
    dictionary['tempPythonFilePath'] = dictionary['docScriptPath'] + "/tmpfilter.py"
    dictionary['SConscriptFilename'] = 'SConscript'
    # All the following values are contained within 'docDataPath':
    dictionary['htmlHeader'] = "header.html"
    dictionary['htmlFooter'] =  "footer.html"
    dictionary['htmlStylesheet'] =  "customdoxygen.css"
    dictionary['htmlImagesPath'] = "doxyimage/"

    dictionary['configNew'] =  "Doxyfile.new"
    dictionary['htmlDir'] = "html/"


    return dictionary

## Print help statement appropriate for 'createDocs.py'
def printHelpStatement(dictionary):
         print "Help for createDocs.py. This file is designed to be run with scons doc."
         print "Use createHTMLDocs.py for the standalone version. "
         print "To run, type ./createDocs.py \n"
         print "To run with input options: "
         print "./createDocs.py METATYPE WEBPAGES EMAIL DOXYGENLONG DOXYGENCONFIGFILE PLUGINDIRECTORIES\n"
         print "Defaults:"
         print "METATYPE: Changes output metatype: 'dtd' or 'xsd'. Default: 'dtd'. Current: "+dictionary['metaFlag']
         print "WEBPAGES: List of webpages for top of codex page. Default: [['http://www.underworldproject.org','Underworld Home Page']]. Current: " +str(dictionary['extWeb'])
         print "EMAIL: Email address for page footers in html. Default: 'underworld-users@vpac.org'  Current: " +dictionary['email']
         print "DOXYGENLONG: Turns On/Off the long image-intense doxygen configuration and limit's it to plugin directories only. Options: 'Yes' or 'No'. Default: 'Yes'., Current: "+ "'"+dictionary['doxyToggle'] +"'"
         print "DOXYGENCONFIGFILE: Name of Doxygen Configuration file.  Default: Doxyfile, Current: "+dictionary['configFile']
         print "PLUGINDIRECTORIES: Dictionary of Toolboxes and directories. This is for scons use and should be left blank. Default: ''"

         print "*******WARNING********"
         print "ALL data in "+dictionary['docPath']+ " will be overwritten!"
         print "**********************" 
         print "Other defaults: \n"
         #print dictionary.items()

## This function extracts out the mercurial branch to use as the "version number" in both Doxygen and the codex. It uses 
##the findProjectDirectories function to select projects to version check. If there is no mercurial on the system or it 
##can't be accessed it will use the default version value - "Bleeding Edge"
def findVersion(dictionary):

    
    #create a command to run mercurial
    mercurialCommand = ' hg' + ' branch'
    StGermainPath = string.rstrip(os.path.realpath(dictionary['docDataPath']), "doc/")
    #I'm going to need to check the branch in the base directory, and in the main program sub-functions.
    #I'm going to get it for the base dir and StGermain at present.
    directoriesToCheck = [] 
    projectNames = findProjectDirectories(dictionary['directoryPath'])
    for project in projectNames:
        projectDir = os.path.join(dictionary['directoryPath'] + '/' + project)
        if os.path.exists(projectDir):
            directoriesToCheck.append(projectDir)

    currentDir = os.getcwd()
    returnValue = []
    # Get the branch ID for each directory that needs to be checked using an external call to mercurial.
    for directory in directoriesToCheck:
        if os.access(directory, os.F_OK) == True:
          os.chdir(directory)

          p = subprocess.Popen(mercurialCommand, shell=True, stdout=subprocess.PIPE , stderr=subprocess.PIPE)

          output, errors = p.communicate()
          if errors == '':
              returnValue.append(output.rstrip('\n'))
          else:
              print "Warning - error in extracting version from codebase in directory: "
              print str(directory) +". Using default value."
          

    #If only one of them is 'default', use the other one.
    branchID = 'default'
    branches = []
    for value in returnValue:
        if value != 'default':
            # Make sure a branch value is not already in the list of branches. 
            valueDoubled = False
            for branch in branches:
                if value == branch:
                    valueDoubled = True
            if valueDoubled == False:
                branches.append(value)

    # If There is only 1 branch name (excluding default listing), use this as the branch ID
    if len(branches) == 1:
       branchID = branches[0]

    # If there are multiple (non-default) branch names, print a warning and append them
    # all together as the branch name of the project.
    if len(branches) > 1:            
        branchID = ""
        for value in branches:
            print "*******************************************************************"
            print " WARNING: Multiple Branch ID's - Check whether your source code is correct"
            print "*******************************************************************"
            branchID += str(value) + " and "

    if branchID == 'default':
        dictionary['projectNumber'] = 'Bleeding Edge'
    else:
        dictionary['projectNumber'] = branchID

    print 'Project Version Number = ' + dictionary['projectNumber']

## Main file to run component codex creator with defaults built in
def createCodex(dictionary):

    #Now create the projects.
    
    projectList = []

    projectNames = findProjectDirectories(dictionary['directoryPath'])

    for projectName in projectNames:
        # check project name in current path already
        if (string.count(dictionary['directoryPath'],  str("/"+projectName)) > 0) :
           path =  os.path.realpath(dictionary['directoryPath']) + "/"
        # check if adding projectName
        else:
           path = os.path.realpath(dictionary['directoryPath']) + "/"+ str(projectName) + "/"
        print "Creating data for " + projectName
        project =Project(projectName, path,  dictionary['metaFlag'])
        project.assignMetas()
        projectList.append(project)
        projectList.sort()
        

    #now create the HTML documents
    print "Now creating HTML documents in directory: " + os.path.realpath(dictionary['codexSubDirPath'])
    
    htmlDocuments = createHTMLDocuments("Component Codex", dictionary['projectNumber'], dictionary['documentBlurb'],  projectList, dictionary['codexSubDirPath'],  dictionary['stylesheetList'],  dictionary['extWeb'],  dictionary['email'], dictionary['script'],  dictionary['pictureDirectory'],  dictionary['indexFileName'],  "Google", "")

    htmlDocuments.createHTMLPages()
    # These two aren't necc. for this file, as the images are already in the right place.    
    htmlDocuments.copyPictures(dictionary['imagePath'])
    htmlDocuments.copyStylesheets()

## create the doxygen web pages
def createDoxygen(dictionary):

    #check doxygen exists
    doxygenExec = which("doxygen")

    #Create directory to write Doxygen to
    if os.path.isdir(dictionary['doxygenSubDirPath']) == False:
        print "Path for Doxygen output does not exist. Creating directory: " + dictionary['doxygenSubDirPath'] 
        os.mkdir(dictionary['doxygenSubDirPath'])
    #Check whether Doxyfile exists in dictionary[docPath]

    if os.path.isfile(dictionary['configPath'])==True:
        print "Config file, "+dictionary['configPath']+ " exists."
    elif os.path.isfile(dictionary['configPath'])==False:
        print "Config file " + str(dictionary['configPath']) + " does not exist."
        print "Creating Doxygen config file, name: " + dictionary['configFile']

    # Modify doxygen entries to work with current directory system

    # Open Doxyfile
    doxyfile = open(dictionary['configPath'])   
    doxyfileString = doxyfile.readlines()
    doxyfile.close()
    newDoxyfileString = ""
    for line in doxyfileString:
        newLine = ""
        
        # Find the "INPUT" field and add the root project dir to it
        if (line.find("INPUT=") != -1) or ((line.find("INPUT ") != -1) and (line.find("=") != -1)):
            if ((dictionary['doxyToggle'] == "No") and (dictionary['arg5'] == "")):
                #Only do plugins directories, not all source code (only works if plugindDirectories is specified).

                if len(dictionary['pluginsDirectories']) > 0:
                    newLine = "INPUT = "
                    for key in dictionary['pluginsDirectories'].iterkeys():
                        
                        for value in (dictionary['pluginsDirectories'])[key]:
                            newLine += " " +dictionary['baseDirectory']+ '/'+key+'/'+ value + " \ \n"
                    # add in script directory for mainpage.
                    newLine += " " + dictionary['docScriptPath']
                else:
                    newLine = "INPUT = "+ dictionary['directoryPath']
                print "Found INPUT"
            else:
                newLine = "INPUT = "+ dictionary['directoryPath']
                print "Found INPUT"
            print newLine
        elif (line.find("STRIP_FROM_PATH") != -1)and (line.find("=") != -1):
            newLine = "STRIP_FROM_PATH = "+ dictionary['directoryPath']
            print "Found STRIP_FROM_PATH"
            print newLine
        elif ((line.find("PROJECT_NUMBER") != -1) and (line.find("=") != -1)):
            newLine = "PROJECT_NUMBER = "+ dictionary['projectNumber']
            print "Found PROJECT_NUMBER"
            print newLine
        elif ((line.find("OUTPUT_DIRECTORY") != -1) and (line.find("=") != -1)):
            newLine = "OUTPUT_DIRECTORY = " + dictionary['docPath'] +"/"+dictionary['doxygenSubDir']+'/'
            print "Found OUTPUT_DIRECTORY"
            print newLine
        elif ((line.find("FILTER_PATTERNS") != -1) and (line.find("=") != -1)):
            newLine = "FILTER_PATTERNS = *.h="+dictionary['headerFilterPath'] + " *.c="+dictionary['tempPythonFilePath']
            print "Found FILTER_PATTERNS: "
            print newLine
        elif ((line.find("HTML_STYLESHEET") != -1) and (line.find("=") != -1)):
            newLine = "HTML_STYLESHEET = "+dictionary['docDataPath']+"/"+dictionary['htmlStylesheet']
            print "Found HTML_STYLESHEET: "
            print newLine
        elif ((line.find("HTML_FOOTER") != -1) and (line.find("=") != -1)):
            newLine = "HTML_FOOTER = "+dictionary['docDataPath']+"/"+dictionary['htmlFooter']
            print "Found HTML_FOOTER: "
            print newLine
        elif ((line.find("HTML_HEADER") != -1) and (line.find("=") != -1)):
            newLine = "HTML_HEADER = "+dictionary['docDataPath']+"/"+dictionary['htmlHeader']
            print "Found HTML_HEADER: "
            print newLine
        elif ((line.find("WARN_LOGFILE") != -1) and (line.find("=") != -1)):
            newLine = "WARN_LOGFILE = "+dictionary['docPath']+"/"+dictionary['doxygenSubDir']+ '/warnings.log'
            print "Found WARN_LOGFILE: "
            print newLine
        else:
            newLine = line
 
        newDoxyfileString = newDoxyfileString + newLine

    
    #write new doxyfile
    
    doxyfileNew = open(os.path.join(dictionary['docDataPath'],dictionary['configNew']), 'w')
    doxyfileNew.write(newDoxyfileString)
    doxyfileNew.close()
    print " Made temp file: Doxyfile.new"
    
    createTempFile(dictionary)
    print "created temp file: " + dictionary['tempPythonFilePath']
    # copy pics to new directory
    copyPics(os.path.join(dictionary['docDataPath'],dictionary['htmlImagesPath']), os.path.join(dictionary['doxygenSubDirPath'],dictionary['htmlDir'])) 
    
    # Copy stylesheets in base docdatapath to doxygen output dir
    copyPics(os.path.join(dictionary['docDataPath'], "*.css"), os.path.join(dictionary['doxygenSubDirPath'],dictionary['htmlDir']))
    #run doxygen ( this command checks PATH for doxygen.
    print "Creating Doxygen documentation for " + str(dictionary['configPath'])
    print " using modified file: "+str(os.path.join(dictionary['docDataPath'],dictionary['configNew'])) 
    print "Output located in " + dictionary['doxygenSubDirPath'] 
 
    print "Now running doxygen..."

    pid =subprocess.call([doxygenExec, str(os.path.join(dictionary['docDataPath'],dictionary['configNew']))])

    # copy pics to new directory
    copyPics(os.path.join(dictionary['docDataPath'],dictionary['htmlImagesPath']), os.path.join(dictionary['doxygenSubDirPath'],dictionary['htmlDir'])) 
    print "Removing temp files..."
    #remove temporary Doxyfile
    if os.path.join(dictionary['docDataPath'],dictionary['configNew']):
        os.remove(str(os.path.join(dictionary['docDataPath'],dictionary['configNew'])))
    #remove temporary python file
    if os.path.exists(dictionary['tempPythonFilePath']):
        os.remove(str(dictionary['tempPythonFilePath']))
    print "Finished creating Doxygen documentation"

## Basic copy function for directory contents. Is not recursive.
def copyPics(pathFrom, pathTo):
    print "Copying content of directory of pictures :" + os.path.realpath(pathFrom) + "/* to directory: " + os.path.realpath(pathTo) 
    createDocument.checkOutputPath(pathTo)
    # Copy everything there to the moment
    os.system('cp '+ os.path.realpath(pathFrom) + "/* "+os.path.realpath(pathTo) )

## Function to create a temporary python file to pass in other useful variables to a Doxygen filter
def createTempFile(dictionary):    
    tmpText = "#!/usr/bin/env python \n"
    tmpText +="import pluginfilter \n"
    tmpText += "from pluginfilter import Input  \n"
    tmpText += "import sys  \n"
    
    tmpText += 'if __name__ == "__main__": \n'

    tmpText +="    values = sys.argv \n"
    tmpText +="    if len(values)>1: \n"
    tmpText +="        inputValues = Input(sys.argv[1], '"+dictionary['baseDirectory']+"', "+'"'+ str(dictionary['pluginsDirectories'])+'" )\n'
    tmpText +="        inputValues.main()\n"
    tmpText +="    else: \n"
    tmpText +="        print 'NO INPUT FILE!!!!!!!!!!!' \n"
    # write text to temp python file
    if os.path.isfile(dictionary['tempPythonFilePath']):
        os.remove(dictionary['tempPythonFilePath'])
    tmpFile = open(dictionary['tempPythonFilePath'], 'w')
    tmpFile.write(tmpText)
    tmpFile.close()
   #make sure it's executable
    os.chmod(dictionary['tempPythonFilePath'], stat.S_IRWXG | stat.S_IRWXU)
    print "********************************************"
    print os.access(dictionary['tempPythonFilePath'], os.X_OK)

## This function is designed to be called by the scons file doc.py. 
# It is designed to have a pluginsDirectories dictionary attached which was
# created in scons and is a list of all the directories that contain plugins for
# each toolbox.
    
if __name__=='__main__':
    values = sys.argv
    for i in range(7):
        if len(values)< 7:
            values.append("")
    
    #Define input directory (relative to ~/stgUnderworldE/ )
    directoryPath = os.path.realpath('.')

    # Create the output directory. This is relative to the ./mainProj/config page.(relative to ~/stgUnderworldE/ )
    docPath = os.path.realpath('./doc/')
    
    # createDictionary
    mainDictionary = createListDictionary(values[1],values[2], values[3], values[4], directoryPath, docPath, "", values[5], values[6])

    # Set up help print statement
    if ((mainDictionary['arg1'] == "help") or (mainDictionary['arg1'] == '-h') or (mainDictionary['arg1'] =='--help') or (mainDictionary['arg1'] == 'h')):
        printHelpStatement(mainDictionary)

    else:
        #check the directory now exists.
        createDocument.checkOutputPath(docPath)
        findVersion(mainDictionary)
        createCodex(mainDictionary)
        createDoxygen(mainDictionary)


