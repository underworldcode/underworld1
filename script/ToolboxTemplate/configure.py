#!/usr/bin/env python
import sys, subprocess, shutil, os

dir = os.environ.get('UW_DIR',None)
if dir is None:
	print 'UW_DIR environment variable not set! this must point to your Underworld *build* directory.'
	sys.exit()

# pull the config directory from the StGermain build
subp = subprocess.Popen(
	'hg clone https://www.underworldproject.org/hg/SConfigure config ', shell=True
)
subp.wait()

sconsBin = os.path.join('config', 'scons', 'scons.py')

subp = subprocess.Popen(
	sconsBin + ' --config=force -f SConfigure ' + 
	'--build-dir=${UW_DIR} ' +
	'--pcu-dir=${UW_DIR} ' +
	'--stgermain-dir=${UW_DIR} ' +
	'--stgdomain-dir=${UW_DIR} ' +
	'--stgfem-dir=${UW_DIR} ' +
	'--picellerator-dir=${UW_DIR} ' +
	'--underworld-dir=${UW_DIR} ' + ' '.join(sys.argv[1:]), shell=True
)
subp.wait()
