#!/usr/bin/env python
import sys, subprocess, shutil, os

dir = os.environ.get('STG_DIR',None)
if dir is None:
	print 'STG_DIR environment variable not set! this must point to your StGermain directory.'
	sys.exit()

# pull the config directory from the StGermain build
subp = subprocess.Popen(
	'hg clone ${STG_DIR}/config ', shell=True
)
subp.wait()
subp = subprocess.Popen(
	'scons/scons.py --config=force -f SConfigure ' + 
	'--build-dir=${STG_DIR}/build ' +
	'--pcu-dir=${STG_DIR}/build ' +
	'--stgermain-dir=${STG_DIR}/build ' +
	'--stgdomain-dir=${STG_DIR}/build ' +
	'--stgfem-dir=${STG_DIR}/build ' +
	'--picellerator-dir=${STG_DIR}/build ' +
	'--underworld-dir=${STG_DIR}/build ' + 
	'--pderework-dir=${STG_DIR}/build ' + ' '.join(sys.argv[1:]), shell=True
)
subp.wait()
