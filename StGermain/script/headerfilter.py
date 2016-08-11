#!/usr/bin/env python
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
from createDocument import Meta

 
## Base input class designed to act as a doxygen filter to
# change Underworld headers to a C++ format to be parsed
# correctly  into the doxygen html pages, preserving
# inheritance and class
class Input():
    def __init__(self, filename):
		self.filename = str(filename)
                self.metaFilename = ""
		self.file=""
		self.output=""
		self.functionName = ""
		self.parentName = ""
		self.variablesString = ""
		self.lines = []
		self.addedText=""
   ## A check for meta file existence
    def checkMetaFile(self):
        # check for meta file
        
        filename = os.path.realpath(self.filename)
        nameList = string.rsplit(str(filename), ".")        
        self.metaFilename = nameList[0] + ".meta"
        if os.path.isfile(self.metaFilename):
            return True

        else:
            return False

    ## Add any meta file data to comments at top of function  
    def addMetaFile(self):
        #open meta file
        if os.path.isfile(self.metaFilename):
            #parse out meta information using Steve's scripts
            fileData =open(self.metaFilename)
            metaText = fileData.read()

            metaFile = Meta(metaText)
            metaFile.createMetaDictionaryDtd()


            # put into C friendly format at top of function
            # allows for dictionaries embedded in lists in the dictionary
            metaText = "\n/** \\addtogroup Components */ \n/**"
            # Need to extract out the Project name from the meta file description.
            # This will be added to the ingroup Name
            # TODO: The main problem with this process is that one file in the
            # code MUST have a \addtogroup or \defgroup command for this to work.
            # But I CAN'T add it here as this program has no memory of how often it's 
            # been called so I cannot be sure I only add the \addtogroup once.
            # For some reason it doesn't like adding it too many times.  
            metaText += " \\ingroup Components \n  "
            description = (metaFile.dictionary).pop('Description')
            # description goes at top of Doxygen file with 'brief' statement
            desc2 = description.replace("$", " \\f$ ")
            metaText += "\\brief "+desc2+".  <ul> "
            # Now cycle through dictionary and parse out all entries into a list
            for key, val in (metaFile.dictionary).iteritems():
                # If a dictionary entry is a list, parse it out correctly (all dictionaries are embedded in lists)
                if (isinstance(val, list) == True) and len(val) > 0 :
                    if (len(val) > 0): 
                        metaText +=  "<li> <b> "+ key +" </b> : \\n <table> "
                        # Table for dictionary embedded in the dictionary
                        #Basic key search lifted from createHTMLDocuments. It makes sure
                        # all possible table titles and entries are included.
                        keyList = []
                        
                        for item in val:
                            if (isinstance(item, dict) == True):    
                                keyListTemp = item.keys()
                                for newK in keyListTemp :                                    
                                    if( keyList.count(newK) == 0 ):
                                        keyList.append(newK)
                        keyList.sort()
                        
                        if len(keyList) > 0 :
                            metaText+= " <tr> \n "
                            for key2 in keyList:
                                metaText += "<th> "+ key2 + "</th>"
                            metaText += "  </tr> \n "
                        for item in val:
                            if (isinstance(item, dict) == True) :
                                metaText += "<tr>"
                                if len(keyList) > 0 :
                                    for key2 in keyList :
                                        val2 = item.get(key2, "")
                                        if len(val2) > 0:
                                            newVal2 =val2.replace("$", " \\f$ ")
                                        else:
                                            newVal2 = val2
                                        metaText+= " <td> "+ newVal2 +" </td> "                            
                            else:
                                metaText+= "<td> " +str(item) + " </td> "
                            metaText += " </tr>"
                        metaText += " </table> \n </li>"
                    else:
                        metaText += ""
                # Parse out values if they are just a dictionary
                # This is unlikely to be used.                         
                elif (isinstance(val, dict) == True):
                    if len(val) > 0 :
                        metaText += "<ul> \n "
                        for key2, val2 in val.iteritems():
                            metaText+= " <li> " +key2 + ": "+ val2 +" </li> "
                        metaText += "</ul> \n "
                    else:
                        metaText += ""   
                # For example, puts it in verbatim tags to display correctly  
                elif (key == "Example") :
                    metaText+= "<li><b> "+key + "</b>: "  +" \\verbatim "+ val +" \\endverbatim " +" </li> \n " 
                # puts LaTeX equations into Doxygen recognisable form.
                elif (key == "Equation") and (len(val) > 0) :
                    newVal = val
                    while (newVal.count("$") > 0):
                        newVal = (newVal.replace("$", "\\f[", 1)).replace("$", "\\f]", 1)
                    metaText += "<li><b> "+key + "</b>: "  + newVal +" </li> \n "
                else:
                    metaText+= "<li><b> "+key + "</b>: " + str(val) +" </li> \n "
            metaText += "</ul> */\n "
            # Now find location to write meta-data and include it. 
            # finding the initial location of the struct for this file
            if self.parentName != "":
			inheritanceText = " : "+self.parentName
            else:
                        inheritanceText = ""
	    searchText = "struct "+ self.functionName + inheritanceText +"\n {\n "
            stringNum = (self.output).find(searchText)

            # Now to create new string
            outText = ""
            stringEnd = len(self.output)
            # add in text before stringNum
            if (stringNum >=0):
                for i in range(stringNum-1):
                    outText +=self.output[i]
                # add in meta text
                outText += metaText.encode('ascii','xmlcharrefreplace')
                # add in rest of file
                for i in range(stringNum, stringEnd-1):
                    outText += self.output[i]

                # save to self.output
                self.output = outText
            else:
                outText =  metaText.encode('ascii','xmlcharrefreplace') + self.output

    ## Convert any "TODO's" into "\todos"
    def convertTodos(self, text):
        #search text for Todos
        self.output = string.replace(text, "TODO", "\\todo")

    ## This function converts the header information into a C++ format.
    def convertHeaders(self,  text) :

        stringStart = -1
        stringEnd = -1
        lineCount = 0
        functionNames = []
        parentNames = []
        goodFunctionNames = []
        goodParentNames = []
        functionLineCount = []
        functionCounter = 0
        for line in text:
			# find the function Name of the macro:
			# Find line that starts with: "#define __" and ends with " \"
			stringStart = (str(line)).find("#define __" )
                        functionName = ""
                        parentName = ""

			if stringStart >= 0:

				stringEnd =(str(line)).find("\\")
				if stringEnd >=0:
					functionName = ((line[stringStart+10: stringEnd]).lstrip()).rstrip()

					stringNextStart = -1
					stringNextEnd = -1
					stringTemp = ""
					# find the parentName if one exists:
					# Find line that starts with "__" and ends with "\\"
					for i in range(1,15):
					    if (lineCount + i) < (len(text)-1):
					        stringNextStart = (str(text[lineCount +i])).find("__")




					    else:
					        stringNextStart = (str(text[len(text)-1])).find("__")

                                                
					    if stringNextStart >=0:

                                                if (lineCount + i) < (len(text)-1): 
						    stringNextEnd =(str(text[lineCount+i])).find("\\")

                                                else:
                                                    stringNextEnd = (str(text[len(text)-1])).find("\\")


						if stringNextEnd >= 0:
                                                    if (lineCount + i) < (len(text)-1):
							parentName = ((text[lineCount +i][stringNextStart + 2:stringNextEnd]).lstrip()).rstrip()
                                                        


                                                        functionNames.append(functionName)
                                                        parentNames.append(parentName)
                                                      
                          	                        functionLineCount.append(lineCount)
                                                        functionCounter = functionCounter+1
                                                        break
                                                        
                                                    else:
                                                        parentName = ((text[len(text)-1][stringNextStart + 2:stringNextEnd]).lstrip()).rstrip()

                                                        functionNames.append(functionName)
                                                        parentNames.append(parentName)
					                functionLineCount.append(lineCount)
                                                        functionCounter = functionCounter+1
                                                        break
                                                   
					    stringNextStart = -1
					    stringNextEnd = -1


                                        
                                        
			stringStart = -1
			stringEnd = -1
			stringMiddle = -1

                        
			#Find line that creates the inherited struct:
			# Find line that looks like "	struct ",self.functionName," { __",self.FunctionName," };"
                        functionCounter = 0


                        for functionName in functionNames:
			    myString = ""
			    myString = "struct "
                            myMiddleString = functionName
			    myNextString = "__"+functionName
			    stringStart = (str(line)).find(myString )
                            
			    if stringStart >=0:

                                stringMiddle = (str(line)).find(myMiddleString)
				stringEnd = (str(line)).find(myNextString)
				if stringEnd >=0:

                                    # have to make this adaptable to multifunction possiblilities ...ouch.
                                    goodFunctionNames.append(functionName)

                                    goodParentNames.append(parentNames[functionCounter])
                                    #Extract out lineCount beginning for section

                                    (self.lines).append(functionLineCount[functionCounter])
                                    # add lineCount end for section
                                    (self.lines).append(lineCount)

			        elif stringMiddle >=0:
                                #check next line for remainder of code, just in case it didn't fit on one line.
                                    stringEnd = (str(text[lineCount+1])).find(myNextString)
                                    if stringEnd >=0:
                                         goodFunctionNames.append(functionName)
                                         goodParentNames.append(parentNames[functionCounter])

                                         (self.lines).append(functionLineCount[functionCounter])
                                         (self.lines).append(lineCount+1)

                                            
                            functionCounter = functionCounter +1			
			lineCount = lineCount + 1

                   		

        # Now we have to check for #ifdef and #else statements within the code structure.
        # If length self.lines > 2, then that means there maybe multiple definitions of the
        # class, or multiple classes defined within the file... the second would be harder to manage.
        
        if len(self.lines) > 2:

            endLines = []
            #check line before self.lines beginning for #ifdef or #else statements
	    # self.lines comes in [start, stop] pairs
            for i in range (len(self.lines)/2):

              # arbitrary range to check no of lines before #define line
              inLoop = False
              for k in range(1,4):
                lineNum = self.lines[2*i] -k
                if lineNum < 0:
                    lineNum = 0

                if ((text[lineNum]).find("#ifdef") ) or ( (text[lineNum]).find("#else") ) :
                     inLoop = True

                     break
                
              # It is in an #ifdef loop. 
              #so have to redefine the lines to be removed.
              #Look for #else or #end
              if inLoop == True:

                 for lineNum in range(self.lines[2*i], len(text) ):     
                     if ((text[lineNum]).find("#else") >=0 ) or ( (text[lineNum]).find("#end") >=0 ) :

                          endLines.append(self.lines[2*i+1])
                          self.lines[2*i +1] = lineNum -1
                          break



        # Now, if there is text to replace:
        goodFuncCount = 0

        for functionName in goodFunctionNames:
            # dodgy set here, to let meta stuff work.
            self.functionName = functionName

            if functionName != "":
			# replace all text between lines [a, b] with new struct structure:
			inheritanceText = ""
			# if no parent function, add all inbetween text
                        parentName = goodParentNames[goodFuncCount]
                        self.parentName = goodParentNames[goodFuncCount]
			if parentName != "":
				inheritanceText = " : "+parentName
			self.addedText = "struct "+ functionName + inheritanceText +"\n {\n "
			lineNum = -1

			if operator.mod(len(self.lines),2) == 0:

				for lineNum in range(self.lines[goodFuncCount*2]+1, self.lines[goodFuncCount*2 + 1]+1):
					if parentName == "":
                                            if (text[lineNum].find(functionName) < 0) and (text[lineNum].find("struct ") < 0) :
						self.addedText += text[lineNum].split("\\")[0] + "\n "

					else:
                                            if (text[lineNum].find(functionName) < 0) and (text[lineNum].find("struct ") < 0) :
						# add inbetween text removing old parent function line
						myText = -1
						myText = text[lineNum].find(parentName)
						if (myText < 0):
							self.addedText += text[lineNum].split("\\")[0] + "\n "
                                     

				# replace old text section with new 'addedText'

                                if goodFuncCount == 0 :

			            for lines in range(0, self.lines[goodFuncCount*2]):

                                       # make sure old struct functionname line not included.
                                        if len(self.lines) >2 :
                                            if lines != endLines[goodFuncCount]:
					        self.output += text[lines]

                                        else:

                                                self.output += text[lines]

                                else:
                                    for lines in range(self.lines[((goodFuncCount-1) * 2)+1]+1, self.lines[goodFuncCount*2]):


                                       # make sure old struct functionname line not included.
                                        if len(self.lines) >2 :
                                            if lines != endLines[goodFuncCount]:
					        self.output += text[lines]
                                        else:
                                            if lines != self.lines[len(self.lines)-1]:
                                                self.output += text[lines]


			        self.output += self.addedText


                        #Add in public: statement just in case.
                                self.output += "public: \n" 


                
            goodFuncCount = goodFuncCount + 1
  		


                
        # if no alterations are needed, then write output to screen as is
        totalFunctionName = ""	
        for functionName in goodFunctionNames:
            totalFunctionName = totalFunctionName + functionName

        if totalFunctionName == "":
	    for lines in range(0, len(text)):
	        self.output += text[lines] 
            self.output+="  "
        endLineNum = -1
        # Find last #endif statement and insert bracket before it.
        if goodFunctionNames != []:
          if goodFunctionNames[0] != "" :

            for lines in range(len(text)-1, self.lines[len(self.lines) -1], -1):
		if ((text[lines]).find("#endif") >= 0):
                    endLineNum = lines
                    break

            for lines in range(self.lines[len(self.lines)-1]+1, len(text) ):
                 if (lines == endLineNum):
	             self.output +=  "};\n" + text[lines]
		 else:
                     valueBool = False
                     if len(self.lines) > 2:
                         for value in range(0, len(endLines)):
                             if lines == endLines[value]:
                                  valueBool = True
                     else:
                        
                         if lines == self.lines[1]:

                             valueBool = True
                     if valueBool == False:
		         self.output += text[lines]



    ## main function to tie all class functions together in a run
    def main(self):  	
		# Read in files

		self.file = open(self.filename, "r")
		
		text = []
		text = (self.file).readlines()

		self.convertHeaders(text)
                self.convertTodos(self.output)
                if (self.checkMetaFile() == True):       
                    self.addMetaFile()

		if self.output != "":
			print "/* *****************************************************************/"
			print "/* THIS FILE HAS BEEN ALTERED TO LOOK LIKE C++ FOR DOXYGEN PARSING */"
			print "/* IT IS NOT THE REAL SOURCE CODE. REFER TO CHECKOUTS OF CODE FOR  */"
			print "/* ACCURATE C CODE DETAILS.                                        */"
			print "/* *****************************************************************/"	
			print self.output
		else:
			print self.file
		

    
## code to run as a standalone from terminal
if __name__ == "__main__":
	# script to run function:
	# sys.argv[1] is the filename to parse.
	values = sys.argv
        if len(values)>1:
	    inputValues = Input(sys.argv[1])

	    inputValues.main()
        else:
            print "NO INPUT FILE!!!!!!!!!!!"            

