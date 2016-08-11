#!/usr/bin/env python

import xml.sax
import stgFlattenDtd
import pprint
import sys

"""

usage:
./parseStgFlattenDtd.py flatten.xml components.context.Type=test maxTimeSteps=3

"""
def main( argv = None ):
    if argv == None:
        argv = sys.argv
    filename = argv[1]

    # Read the file into a string
    xml_file = file( filename )
    xml_lines = xml_file.readlines()
    xml_text = ""
    for l in xml_lines:
            xml_text += str(l)

    #dtdDict = stgFlattenDtd.readXML( xml_text )
    #pprint.pprint( dtdDict )

    h = stgFlattenDtd.getXmlHandler( xml_text )
    for arg in sys.argv[2:]:
        h.modify_param( arg )
    print h.writeXml()

if __name__=='__main__':
    main()

