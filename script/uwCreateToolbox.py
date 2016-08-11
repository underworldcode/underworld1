#!/usr/bin/env python

import os, sys, errno
import getopt
from shutil import copytree, copymode, Error
import fnmatch

def createToolbox( name, path, verbose ):
   if( verbose ):
      print "Using toolbox name \"" + name + "\""

   # Sort out the template dir
   basename = os.path.dirname( sys.argv[0] )
   if( basename == '' ):
      basename = '.'
   template_dir = os.path.join(basename, "ToolboxTemplate")
   if( verbose ):
      print "Using template files from: " + template_dir

   # Sort out the output dir
   if( path == None ):
      path = os.getcwd()
   if( verbose ):
      print "Creating toolbox in directory \"" + path + "\""
   toolbox_dir = os.path.join(path, name)

   try:
      copytree( template_dir, toolbox_dir )
   except Error:
      print "\n\n\ Some unknown error occurred.\n\n"
      raise
   except OSError as e:
      if e.errno == errno.EEXIST:
         print "Unable to create the toolbox directory structure - toolbox name already in use in target output path"
         sys.exit( 2 )
      else:
         raise

   # first, rename all *JAMBox* directories to required name
   matches = []
   for root, dirnames, filenames in os.walk(toolbox_dir):
      for dirs in fnmatch.filter(dirnames, '*JAMBox*'):
         matches.append( [os.path.join(root, dirs), os.path.join(root,dirs.replace("JAMBox",name)) ] )
   # traverse in reverse to ensure child directories are renamed before parents
   for matcholdname, matchnewname in reversed(matches):
      os.rename( matcholdname, matchnewname  )

   # now, rename all *JAMBox* files 
   matches = []
   for root, dirnames, filenames in os.walk(toolbox_dir):
      for files in fnmatch.filter(filenames, '*JAMBox*'):
         matches.append(os.path.join(root, files))
   for match in matches:
      os.rename( match, match.replace("JAMBox", name) )


   # now, sed style replace of *JAMBox* within files
   matches = []
   for root, dirnames, filenames in os.walk(toolbox_dir):
      for files in filenames:
         filenameold = os.path.join(root,files)
         filenamenew = filenameold + '.tmp'
         fileold = open( filenameold, 'r' )
         filenew = open( filenamenew, 'w' )
         for line in fileold:
            filenew.write( line.replace("JAMBox",name) )
         fileold.close()
         filenew.close()
         copymode( filenameold, filenamenew )
         # remove old file
         os.remove( os.path.join(root,files) )
         # rename new file to old file
         os.rename( os.path.join(root,files) + '.tmp', os.path.join(root,files) )


def usage():
   #      12345678901234567890123456789012345678901234567890123456789012345678901234567890
   print "The Underworld toolbox creator tool"
   print "Copyright (c) Monash University, 2012"
   print ""
   print "Creates a new Underworld toolbox project (directory structure, build system, "
   print "minimum files."
   print ""
   print "--help (-e)                            Prints out this help information."
   print "--verbose (-v)                         Prints more information, where available."
   print "--name=name (-n name)                  Specify the name of the toolbox."
   print "--output_path=dir (-o dir)             Specify the parent directory in which to"
   print "                                        create the toolbox."
   print ""
   print "The toolbox name must be provided."
   print ""


def main():
   # Parse options...
   try:
      opts, args = getopt.getopt(sys.argv[1:], "hn:o:v", ["help", "name=", "output_path=", "verbose"])
   except getopt.GetoptError, err:
      # print help information and exit:
      print str( err ) # will print something like "option -a not recognized"
      usage()
      sys.exit( 2 )
   name = None
   path = None
   verbose = False
   for o, a in opts:
      if o in ("-v", "--verbose"):
         verbose = True
      elif o in ("-h", "--help"):
         usage()
         sys.exit()
      elif o in ("-n", "--name"):
         name = a
      elif o in ("-o", "--output_path"):
         path = a
      else:
         assert False, "unhandled option"

   # Check for valid combinations of options...
   if( name == None ):
      print "Error:  A toolbox name must be given.  Run 'uwCreateToolbox.py --help' for options."
      sys.exit( 2 )

   while True:
      try:
         name.index("JAMBox")
      except ValueError:
         break
      else:
         print "Error:  The name 'JAMBox' is a reserved keyword.  Please select another name."
         sys.exit( 2 )

   # Do something!
   createToolbox( name, path, verbose )

   #print "\nYour Toolbox has been created.\n\nYou can build it automatically by moving it to your Underworld root directory." \
   print "\nYour Toolbox has been created.  You may like to move it to another location\n" \
         "\nYou can build it (in any location) by setting the UW_DIR environment variable to point to your Underworld build directory.\n"

if __name__ == "__main__":
   main()

