#!/usr/bin/python

## This python file is designed to extract out plugin information from .c files and collate them
## into a html page for users. This is NOT designed for developers. It will not extract out 
## comments about the functions...yet.
import createHTMLDocuments, createDocument
import sys, os.path, os, string, subprocess, shutil
from createDocument import createDocuments
from createHTMLDocuments import createHTMLDocuments
from createDocs import createListDictionary
from createDocs import findVersion
from createDocs import findProjectDirectories

#DESIGN NOTE: This file will assume the following about c file plugins:
# 1. All the main information needed on the code is in the C file or accompanying XML file.
# 2. There MAYBE (eventually WILL BE) an XML file that gives a working example
#    of including the plugin
# 3. It will have the following partial line: "return PluginsManager_Submit("
# 4. It MAY (eventually WILL) have a /** \plugins **/ style comment that will
#    give the main user info about the plugin.
# Points 2. and 4. have yet to be incorporated into the codebase, but they will be.

## creates the listDictionary for createPlugins file.
def createListDictionary(directoryPath, docPath, docDataPath, arg1, arg2):
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

    dictionary['pluginsSubDir'] = 'Plugins'
    dictionary['pluginsSubDirPath'] = os.path.join(dictionary['docPath'], dictionary['pluginsSubDir'])

    # Define stylesheet data to input
    dictionary['stylesheetList'] = [os.path.join(dictionary['docDataPath'], 'print.css'),os.path.join(dictionary['docDataPath'], 'menu.css'),os.path.join(dictionary['docDataPath'], 'codexStylesheet.css')]
    # Define javascript to input
    dictionary['script'] =[os.path.join(dictionary['docDataPath'], 'menuscript.js')] 
    #Define blurb
    dictionary['documentBlurb'] ="This is a list of the available plugins."
    # Define picture directory
    dictionary['imagePath'] = os.path.join(dictionary['docDataPath'], 'images')

    #Define output picture directory
    dictionary['pictureDirectory'] = 'images/'

    dictionary['arg1'] = arg1
    dictionary['arg2'] = arg2

    if (dictionary['arg1'] == ""):
        dictionary['extWeb'] = [['http://www.underworldproject.org','Underworld Home Page']]
    else:
        dictionary['extWeb'] = arg1
    
    if (dictionary['arg2'] == ""):
        dictionary['email'] = 'underworld-users@vpac.org'
    else:
        dictionary['email'] = arg2
    #TODO: Write a plugins intro blurb. Until then, this remains blank.
    dictionary['indexFileName'] = ""
    
    return dictionary

class PluginProject():
    def __init__(self,  name, path):
        self.name = name
        self.pluginsList = []
        self.path = path
    ## searches all c files, calls functions to check it is a plugin. If so, adds it to the pluginsList
    ## for the given project
    def searchProjects(self):
    
        for root, dirs, files in os.walk(self.path):
            # find filenames that END in ".c" 
            for filename in files:
                myLength = len(filename) - 2
                stringNumber = string.rfind(filename, '.c')
                filePath = os.path.join(root, filename)
                if (stringNumber == myLength):
    
                # Open file and check if it is a plugin.
                    if os.path.isfile(filePath):
                        pluginFile = open(filePath, "r")
	        	    #print filePath
                        text = []
	                text = (pluginFile).readlines()
                        if self.checkIfIsPlugin(text) == True:
                            # create plugin dictionary.
                            pluginDic = self.extractPlugin(filename, filePath)
                            (self.pluginsList).append(pluginDic)

    ## extracts out plugin information and adds it to the dictionary for that plugin.
    def extractPlugin(self, filename, filePath):
        #each plugin will have a set of dictionary entries:
        # name, location, XML, blurb.
        plugin = {}
        plugin['name'] = string.rstrip(filename, '.c')
        plugin['location'] = filePath
        #TODO: for XML, will check if there is a file: "name.xml" and extract out code 
        plugin['XML'] = ''
        #TODO: for blurb, will look for /** \plugin **/ style comment to extract out data.
        plugin['description'] = ''    
  
        return plugin    

    ## checks the contents of a file for line/s that would be present if the file was
    ## a plugin. Not robust at present.
    def checkIfIsPlugin(self, text):
        # search for the following line section in a .c file.
        textLine = "return PluginsManager_Submit("
        flagValue = False
            # check for plugins line
        for line in text:
            if (string.find(line, textLine) != -1):
                #This has the line and is probably a plugin.
                #TODO: This does not allow for extra spaces in the string, which is very likely.
                #       but it will do for the present.
                flagValue = True
    
        return flagValue


## This is the main function.
def createPlugins(dictionary):

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
        project = PluginProject(projectName, path)
        #project.assignMetas()
        project.searchProjects()
        projectList.append(project)
        projectList.sort()



    # Now create the html pages for the projects.
    print "Now creating HTML documents in directory: " + os.path.realpath(dictionary['pluginsSubDirPath'])

    htmlDocuments = createPluginHTMLDocuments("Plugins Webpage", dictionary['projectNumber'], dictionary['documentBlurb'],  projectList, dictionary['pluginsSubDirPath'],  dictionary['stylesheetList'],  dictionary['extWeb'],  dictionary['email'], dictionary['script'],  dictionary['pictureDirectory'],  dictionary['indexFileName'],  "Google", "")
    htmlDocuments.createHTMLPages()
    # These two aren't necc. for this file, as the images are already in the right place.    
    htmlDocuments.copyPictures(dictionary['imagePath'])
    htmlDocuments.copyStylesheets()

## specific help statement for createPlugins.
def printHelpStatement(dictionary):
         print "Help for createPlugins.py. This file is designed to be standalone."
         print "To run, type ./createPlugins.py \n"
         print "To run with input options: "
         print "./createPlugins.py  INPUTDIR OUTPUTDIR DOCDATAPATH WEBPAGES EMAIL\n"
         print "Defaults:"
         print "INPUTDIR: Name of base input directory.  Default: '../../', Current: "+ dictionary['directoryPath']
         print "OUTPUTDIR: Name of directory to which will contain plugins output files. Default: '../../doc/', Current: "+ dictionary['docPath']
         print "DOCDATAPATH: Name of path to StGermain/doc/ directory, which has necessary files for the createPlugins to run. Default: "+dictionary['directoryPath']+"/StGermain/doc/', Current: "+ dictionary['docDataPath']
         print "WEBPAGES: [['http://www.underworldproject.org','Underworld Home Page']]"
         print "EMAIL: 'underworld-users@vpac.org'\n"
         print "************************************\n"
         print "Other defaults: (Not all necessary) \n"
         print dictionary.items()

class createPluginHTMLDocuments(createHTMLDocuments):
    def __init__(self, documentName, documentVersion, documentBlurb,  projectList,  documentPath, styleSheetList,  externalWebpages,  contactEmail, scripts,  pictureDirectory,  indexFileName,  searchBarType,  searchPath):
        createHTMLDocuments.__init__(self, documentName, documentVersion, documentBlurb,  projectList,  documentPath, styleSheetList,  externalWebpages,  contactEmail, scripts,  pictureDirectory,  indexFileName,  searchBarType,  searchPath)
        #createDocuments.__init__(self, documentName,  projectList, documentPath)
        #print self.projectList

    def createMain(self,  count):
        text = ''
        #create all meta entries for current Project page
        project =self.projectList[count]

        #Sort meta names into an ordered list
        print "Creating Plugin entries for project: " + str(project.name) 

        names = []
        for plugin in project.pluginsList:
            names.append(plugin['name'])
        names.sort()
        for name in names:
            text += self.createPluginEntry(name, count)
        return text

    def createPluginEntry(self, name, count):
        text =""
        #create individual meta entry
        project = self.projectList[count]

        #Select Plugin
        for plugin in project.pluginsList:
            if plugin['name'] == name:
            #Add plugin data to file
                text +='<h3 align="center">\n'
                
                text +='<a name="'+unicode(plugin['name'])+'">'+unicode(plugin['name'])+'</a>\n'
                text += '</h3>\n<br/>\n'
                if plugin.has_key('location'):
                    text += self.addSimpleComponentInfo('Location', unicode(plugin['location']))
                if plugin.has_key('description'):
                    text += self.addSimpleComponentInfo('Description', self.checkTextForEquations((plugin['description']).encode('ascii','xmlcharrefreplace'),unicode(plugin['name'])+'Description' ) )
                if plugin.has_key('XML'):
                #next comes XML Example, if any
                    text +=self.addExampleInfo('Example',  (plugin['XML']).encode('ascii','xmlcharrefreplace'))           
        return unicode(text)        
    def createSidebar(self,  count):
        project = self.projectList[count]
        text = '<div id="'+self.div.sidebar+'">\n' 
        text +=       '<a name="'+project.name+'">\n'
        text +=        '<h2>'+project.name+'</h2>\n </a>\n' 
        text +=       '<h3> '+self.name+' Menu </h3>\n'
        text += '<ul class="menu">\n'
        names = []
        for plugin in project.pluginsList:
            names.append(plugin['name'])
        names.sort()
        text += self.sidebarSort(names, project.name)                    
        text +='<li><a href="./'+project.name+'.html#top">Back to top</a>\n</li>'
        text += '</ul>\n'                        
        text += '</div>\n'
        return text

if __name__=='__main__':
    values = sys.argv
    for i in range(7):
        if len(values)< 7:
            values.append("")
    #print values     
    #Define input directory (relative to ~/stgUnderworldE/ )

    # set directoryPath correctly, even if help function is invoked.
    if ((values[1] == "help") or (values[1] == '-h') or (values[1] =='--help') or (values[1] == 'h')):
        directoryPath = os.path.realpath('../../')
    elif values[1] != '':
        directoryPath = os.path.realpath(values[1])
    else:
        directoryPath = os.path.realpath('../../')

    # Create the output directory. This is relative to the ./mainProj/config page.(relative to ~/stgUnderworldE/ )
    if values[2] != "":
        docPath = os.path.realpath(values[2])
    else:
        docPath = os.path.join(directoryPath, 'doc/')

    # create the main dictionary which holds the path information.
    mainDictionary = createListDictionary(directoryPath, docPath, values[3], values[4], values[5])
    #print mainDictionary.items() 

    # Set up help print statement
    if ((values[1] == "help") or (values[1] == '-h') or (values[1] =='--help') or (values[1] == 'h')):
        printHelpStatement(mainDictionary)

    else:
        #check the directory now exists.
        createDocument.checkOutputPath(docPath)

        #find version of the code using mercurial
        findVersion(mainDictionary)

        # call main function
        createPlugins(mainDictionary)

