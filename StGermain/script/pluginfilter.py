#!/usr/bin/env python

## \file
## This file is designed to be called by doxygen and outputs to standard output.
## It finds the plugins in the code and adds the appropriate group ID so Doxygen
## will put it on the appropriate Module page.
import os, platform
import getopt
import glob
import os.path
import shutil
import string, math, operator
import sys
import token
import tokenize
import string
from createDocs import findProjectDirectories

## Class to hold all information for parsing in a C file and:
##    * Checking if it's a plugin
##    * If so, adding a line to make sure the file is added to plugins page.
class Input():
    def __init__(self, filename, baseDirectory, pluginsDirectories):
		self.filename = os.path.realpath(str(filename))
                self.baseDirectory = os.path.realpath(str(baseDirectory))
		self.file=""
		self.output=""
                self.addedText = "" 
                if pluginsDirectories == "":
                    self.pluginsDirectories = {}
                else:
                    self.pluginsDirectories = eval(pluginsDirectories)
                self.textLine = "return PluginsManager_Submit("
                self.textLineSure = "PluginsManager_Submit("
                self.line = -1
                self.pluginName = ""
                self.textLine2 = ""
                self.project = ""
                self.nameLine = -1
  
    def findPluginName(self, text):

        lineList = string.split(text[self.line], ',')
        #print lineList[1]
        self.textLine2 = "const Type "+ string.strip(lineList[1]) + " ="
        for count in range(len(text)):

            flagTotal = self.testTwoLines(text[count], self.textLine2)
            if flagTotal ==True:
                lineList = string.split(text[count], '=')
                if string.rstrip(string.lstrip(lineList[1])) == "" or string.lstrip(string.rstrip(lineList[1])) == "\\n":
                    #check next line
                    if count < len(text) -1:
                        newLineList = string.split(text[count+1], '"')
                        self.pluginName = newLineList[1]
                        self.nameLine = count
                else:
                    newValue = string.split(lineList[1], '"')
                    self.pluginName = newValue[1] 
                    self.nameLine = count
                      
        
    def checkDirectories(self):
        dirToCheck = os.path.realpath(os.path.dirname(self.filename))

        for toolbox, directories in (self.pluginsDirectories).iteritems():
            for directory in directories:
                fullPath = os.path.realpath( self.baseDirectory + "/" + str(toolbox) + "/" + str(directory) )
                if fullPath == dirToCheck:
                    self.project = toolbox
                    return True

        return False

    def readFile(self):
        self.file = open(self.filename, "r")

        text = []
        text = (self.file).readlines()
        self.file.close()
        return text
    ## main function to tie all class functions together in a run
    def main(self):
        text = []  	
	# Read in files
        if len(self.pluginsDirectories) > 0 :
            #check directories to see if file in a plugin
            if self.checkDirectories() == True:
                text = self.readFile()
                if self.checkIfIs(text, self.textLineSure) == True:
                    self.findPluginName(text)
                    # add appropriate line to file.
                    self.addPluginText(text)
                #else:
                #    self.addPluginText(text)
        else:
            text = self.readFile()
            if (self.checkIfIs(text, self.textLine) == True):
                self.findPluginName(text)
                self.project = self.findPluginProject()   
                self.addPluginText(text)

        if self.output != "":
                print "/* *****************************************************************/"
                print "/* THIS FILE HAS BEEN ALTERED TO LOOK LIKE C++ FOR DOXYGEN PARSING */"
                print "/* IT IS NOT THE REAL SOURCE CODE. REFER TO CHECKOUTS OF CODE FOR  */"
                print "/* ACCURATE C CODE DETAILS.                                        */"
                print "/* *****************************************************************/"	
                print self.output
        else:
                lineTotal = ""
                for line in text:
                    lineTotal += line
                print lineTotal
		

    def checkIfIs(self, text, textLine):
        # search for the following line section in a .c file.
        # This is a partial line for the PluginsMamager Register function, 
        # which all plugins have to have. Only this section is used
        # as the full line varies depending on the plugins name. It seems to work OK.

        flagValue = False

        # check for plugins line (more robust version 
        #-- allows for different size and location of whitespace)
        lineNum = 0
        for line in text:
            # split lines by whitespace into lists. 
            # if lineList has all values that appear in textLineList it's a plugin!
            flagTotal = self.testTwoLines(line, textLine)

            # set the returning flag.                
            if flagTotal == True:       
                flagValue = True
                self.line = lineNum
            lineNum = lineNum + 1
        return flagValue
    ## find the project the plugin belongs to.
    def findPluginProject(self):
        
        subDirectoryList =  string.split(string.split(self.filename, self.baseDirectory)[1], "/")
        projectList = findProjectDirectories(self.baseDirectory)
        
        for directory in subDirectoryList:
            for project in projectList:
                if directory == project:
                    return project
        return ""
    def addPluginText(self, text):
        #This line may be a problem if submit line is first in file -- highly unlikely
        if self.line < 0: 
            insertLine = self.nameLine
        else:
            insertLine = self.line
        
        if insertLine > 0:
            rangeNum = insertLine -1
        else:
            rangeNum = 0

            #Add in content before plugin submit line
        for lineNum in range(rangeNum):
            self.output += text[lineNum]
        # add in text to identify file as a plugin
        self.addedText = "/** \\file \n \\ingroup " +self.project +"_Plugins \n**/ \n" + "/** \n* \\addtogroup " +self.project +"_Plugins \n**/\n"
        self.output += self.addedText
            
        #Add in text that shows the plugins name as called by the XML:
        pluginText = "/** \\file \n \\brief Plugin Name: '<b>" + self.pluginName + "</b>'  **/\n" 
        self.output += pluginText
        for lineNum in range(rangeNum, len(text)):
            self.output += text[lineNum]

    #Testif lineB appears completely in lineA.
    def testTwoLines(self, lineA, lineB):
        lineAList = string.split(lineA)
        lineBList = string.split(lineB)
        subFlagValues = []
        for bValue in lineBList:
            subFlagValue = False
            for aValue in lineAList:
                if (string.find(aValue, bValue) != -1):
                    subFlagValue = True
            subFlagValues.append(subFlagValue)

            # if ALL the subflags are true, then the line has been found.
        flagTotal = True
        for flag in subFlagValues:
            flagTotal = flagTotal and flag
             
        return flagTotal

## code to run as a standalone from terminal
if __name__ == "__main__":
	# script to run function:
	# sys.argv[1] is the filename to parse.
	values = sys.argv
        if len(values)>1:
	    inputValues = Input(sys.argv[2], sys.argv[1], "")

	    inputValues.main()
        else:
            print "NO INPUT FILE!!!!!!!!!!!"            

