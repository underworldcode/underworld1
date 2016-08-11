import sys, os, subprocess, platform

EnsureSConsVersion(0, 98)

# Colours
colours = {}
colours['cyan']   = '\033[96m'
colours['purple'] = '\033[95m'
colours['blue']   = '\033[94m'
colours['green']  = '\033[92m'
colours['yellow'] = '\033[93m'
colours['red']    = '\033[91m'
colours['end']    = '\033[0m'

# If the output is not a terminal, remove the colours.
if not sys.stdout.isatty():
   for key, value in colours.iteritems():
      colours[key] = ''

compile_source_message = '%sCompiling %s==> %s$SOURCE%s' % \
   (colours['blue'], colours['purple'], colours['yellow'], colours['end'])

link_program_message = '%sLinking Program %s==> %s$TARGET%s' % \
   (colours['cyan'], colours['purple'], colours['yellow'], colours['end'])

link_library_message = '%sLinking Library %s==> %s$TARGET%s' % \
   (colours['cyan'], colours['purple'], colours['yellow'], colours['end'])

ranlib_library_message = '%sRanlib Library %s==> %s$TARGET%s' % \
   (colours['cyan'], colours['purple'], colours['yellow'], colours['end'])

install_message = '%sInstalling %s==> %s$TARGET%s'% \
   (colours['green'], colours['purple'], colours['yellow'], colours['end']) 

# CUSTOMISE THE ENVIRONMENT HERE.
env = Environment(ENV=os.environ,
                  tools=['default', 'pcutest', 'stg', 'dist', 'doc'],
                  toolpath=['StGermain/pcu/script', 'StGermain/script', 'script'])

# Check if scons is launched with detail flag.
detail = ARGUMENTS.get('detail', 0)
Help("""
SCons Build Options:
    Type: './scons.py detail=1' to build Underworld showing full detail non-coloured stdout" 
""" )

# If detail flag is not set, let's use our own build strings.
if not int(detail):
    env['CXXCOMSTR'] = compile_source_message
    env['CCCOMSTR'] = compile_source_message
    env['SHCCCOMSTR'] = compile_source_message
    env['SHCXXCOMSTR'] = compile_source_message
    env['ARCOMSTR'] = link_library_message
    env['RANLIBCOMSTR'] = ranlib_library_message
    env['SHLINKCOMSTR'] = link_library_message
    env['LINKCOMSTR'] = link_program_message
    env['INSTALLSTR'] = install_message

# Load CREDO, the system testing tool
env.Tool('credosystest', toolpath=['credo/scons'])

# Needed for Darwin.
env['_abspath'] = lambda x: File(x).abspath

# Ludicrous-speed!
env.Decider("MD5-timestamp")

# Load configuration.
values = {}
if(not os.path.isfile('config.cfg')):
   print 'Error: \'config.cfg\' file not found.  The configuration script (configure.py) must be run successfully before you can build (via scons.py).'
   sys.exit()

execfile("config.cfg", globals(), values)
env._dict.update(values)

# Set LIBPATH and RPATH for libraries
if env['prefix'] == env.GetLaunchDir():
   env['RPATH'] += [os.path.join(env['prefix'], env['build_dir'], "lib")]
   env['LIBPATH'] = [os.path.join(env['prefix'], env['build_dir'], "lib")] + env['LIBPATH']
else:
   env['RPATH'] += [os.path.join(env['prefix'], "lib")]
   env['LIBPATH'] = [os.path.join(env['prefix'], "lib")] + env['LIBPATH']

# Check if we're using 64bit.
if platform.architecture()[0] == '64bit':
    env.AppendUnique(CPPDEFINES=[('SYSTEM_SIZEOF_LONG', 8)])

# Need to manipulate the build directory to keep SCons happy. Because of SCons' target
# rules we need to make the build directory a default target.
env["build_dir"] = os.path.join(env.GetLaunchDir(), env["build_dir"])
env["prefix"] = os.path.join(env.GetLaunchDir(), env["prefix"])
env["INST_BUILD_DIR"] = env["build_dir"]
env["INST_PREFIX"] = env["prefix"]
env.Default(env["build_dir"])

# Add the build directory's include path.
env.AppendUnique(CPPPATH=env['build_dir'] + '/include')

# Need to define the extension for shared libraries as well
# as the library directory.
ext = env['ESCAPE']('"' + env['SHLIBSUFFIX'][1:] + '"')
lib_dir = env['ESCAPE']('"' + os.path.abspath(env['build_dir']) + '/lib' + '"')
env.AppendUnique(CPPDEFINES=[('MODULE_EXT', ext), ('LIB_DIR', lib_dir)])

# Include the library path.
env.AppendUnique(LIBPATH=env['build_dir'] + '/lib')
env.AppendUnique(RPATH=env.Dir(env['build_dir'] + '/lib').abspath)

# If we have no shared libraries, include a pre-processor definition to
# prevent modules from trying to load dynamically.
if not env['shared_libs']:
    env.AppendUnique(CPPDEFINES=['NOSHARED'])

# Detect the subrepos available to be used for extracting revision ids
# and paths for compilation defines later on.
repos = {}
paths = ['.']
while paths:
    path = paths.pop()
    if os.path.realpath(path) in repos:
        continue
    for root, dirs, files in os.walk(path):
        for d in dirs[:]:
            repos[os.path.realpath(root)] = root
repos = repos.values()
repos.sort()

from string import upper
import re
for r in repos:
    if r == '.':
        r = 'ROOT'
    repo_name = upper(re.sub('\W', '', r))
    env.AppendUnique(IDDEFINES=[(repo_name, env["ESCAPE"]('"UNKNOWN"'), \
                     env["ESCAPE"]('"UNKNOWN"'), env["ESCAPE"]('"UNKNOWN"'))])

# Need to insert some 'HAVE_*' definitions based on what packages we
# found during configuration.
if 'HAVE_HDF5' in env['CPPDEFINES']:
    env.AppendUnique(CPPDEFINES=["READ_HDF5", "WRITE_HDF5"])

# If we were given a prefix other than the default, tell StGermain where to
# find XML include files.
if env['prefix'] != env.GetLaunchDir():
    env.AppendUnique(CPPDEFINES=[('STG_INCLUDE_PATH', env['ESCAPE']('"' + env['prefix'] + '/lib/StGermain"'))])

# Make sure 'install' has a proper target.
env.Alias("install", env["prefix"])

# INSERT TARGETS HERE.

# Make a copy of the config.log and config.cfg in the build_dir
dir = Dir(env['build_dir']).abspath
if not os.path.exists(dir):
   os.makedirs(dir)

env.Install(env['build_dir'], 'config.log')
env.Install(env['build_dir'], 'config.cfg')
env.Install(os.path.join(env['build_dir'],'share/StGermain/scripts'), Glob('script/*.py*'))
if env['prefix'] != env.GetLaunchDir():
   env.Install(env['prefix'], 'config.log')
   env.Install(env['prefix'], 'config.cfg')
   env.Install(os.path.join(env['prefix'],'share/StGermain/scripts'), Glob('script/*.py*'))

if env['with_viewer']:
   if not os.path.exists(dir+'/gLucifer/Viewer'):
      os.makedirs(dir+'/gLucifer/Viewer')
   Execute(Delete(File(env['build_dir']+'/gLucifer/Viewer/offscreen.cfg')))
   Execute(Copy(File(env['build_dir']+'/gLucifer/Viewer/output.cfg'), File('output.cfg')))
   Execute(Copy(File(env['build_dir']+'/gLucifer/Viewer/viewer.cfg'), File('viewer.cfg')))
   Execute(Copy(File(env['build_dir']+'/gLucifer/Viewer/offscreen.cfg'), File('offscreen.cfg')))

Export('env')

SConscript('StGermain/SConscript',
           variant_dir=env['build_dir'] + '/StGermain',
           duplicate=0)
env.Prepend(LIBS=['pcu'])
env.Prepend(LIBS=['StGermain'])

SConscript('StgDomain/SConscript',
           variant_dir=env['build_dir'] + '/StgDomain',
           duplicate=0)
env.Prepend(LIBS=['StgDomain'])

SConscript('StgFEM/SConscript',
           variant_dir=env['build_dir'] + '/StgFEM',
           duplicate=0)
env.Prepend(LIBS=['StgFEM'])

SConscript('PICellerator/SConscript',
           variant_dir=env['build_dir'] + '/PICellerator',
           duplicate=0)
env.Prepend(LIBS=['PICellerator'])

SConscript('Underworld/SConscript',
           variant_dir=env['build_dir'] + '/Underworld',
           duplicate=0)
env.Prepend(LIBS=['Underworld'])

if env['with_experimental']:
    SConscript('Experimental/SConscript',
               variant_dir=env['build_dir'] + '/Experimental',
               duplicate=0)
    env.Prepend(LIBS=['Experimental'])

if env['with_spmodel']:
    SConscript('SPModel/SConscript',
               variant_dir=env['build_dir'] + '/SPModel',
               duplicate=0)
    env.Prepend(LIBS=['SPModel'])

if env['with_glucifer']:
    SConscript('gLucifer/SConscript',
               variant_dir=env['build_dir'] + '/gLucifer',
               duplicate=0)
    env.Prepend(LIBS=['gLucifer'])

if env['with_viewer']:
    SConscript('gLucifer/Viewer/SConscript',
               variant_dir=env['build_dir'] + '/gLucifer/Viewer',
               duplicate=0)

if env['with_pderework']:
    SConscript('PDERework/SConscript',
               variant_dir=env['build_dir'] + '/PDERework',
               duplicate=0)
    env.Prepend(LIBS=['PDERework'])

if env['with_geothermal']:
    SConscript('Geothermal/SConscript',
               variant_dir=env['build_dir'] + '/Geothermal',
               duplicate=0)
    env.Prepend(LIBS=['Geothermal'])

if env['with_groundwater']:
    SConscript('Groundwater/SConscript',
               variant_dir=env['build_dir'] + '/Groundwater',
               duplicate=0)
    env.Prepend(LIBS=['Groundwater'])

if env['with_gocad']:
    SConscript('GocadToolbox/SConscript',
               variant_dir=env['build_dir'] + '/GocadToolbox',
               duplicate=0)
    env.Prepend(LIBS=['GocadToolbox'])

if env['with_importers']:
    SConscript('ImportersToolbox/SConscript',
               variant_dir=env['build_dir'] + '/ImportersToolbox',
               duplicate=0)
    env.Prepend(LIBS=['ImportersToolbox'])

if env['with_solvers']:
    SConscript('Solvers/SConscript',
               variant_dir=env['build_dir'] + '/Solvers',
               duplicate=0)
    env.Prepend(LIBS=['Solvers'])

if env['with_spherical']:
    SConscript('Spherical/SConscript',
               variant_dir=env['build_dir'] + '/Spherical',
               duplicate=0)
    env.Prepend(LIBS=['Spherical'])

if env['with_viscoelastic']:
    SConscript('Viscoelastic/SConscript',
               variant_dir=env['build_dir'] + '/Viscoelastic',
               duplicate=0)
    env.Prepend(LIBS=['Viscoelastic'])



#
# Build static version of StGermain.
#

if env['static_libs']:
    env.Program('bin/StGermain',
                ['StGermain/src/main.c',
                 File(env['build_dir'] + '/StGermain/stg_static_modules.c').abspath])

# Adding in documentation.
#env.Alias("doc", None, env.Action(File("StGermain/script/createDocs.py").abspath))
#env.AlwaysBuild("doc")

# Dump package config.
filename = env['build_dir'] + '/lib/pkgconfig/stgermain.pc'
# env.write_pkgconfig(filename, 'StGermain', 'The StGermain Framework')

env.Dist("underworld-%s"%env.GetOption("dist_version"),
         ["configure.py", "SConstruct", "config", "script", "StGermain",
          "StgDomain", "StgFEM", "PICellerator", "Underworld",
          "Experimental", "gLucifer"])

# NB: help() printout about testing that used to be here moved to credo/scons.

    
#Run the functions to create the Alias commands for the documentation of stgUnderworld. 


env.Alias("doc-codex", None, env.Action(env.AddCodexSuite()))
env.Alias("doc", None, env.Action(env.AddAllSuite()))
env.Alias("doc-doxygen", None, env.Action(env.AddDoxygenSuite()))
env.Alias("doc-doxygenlite", None, env.Action(env.AddDoxygenLiteSuite()))


#AlwaysBuild("doc")
#AlwaysBuild("doc-codex")
#AlwaysBuild("doc-doxygen")
#AlwaysBuild("doc-doxygenlite")


# TODO: test targets and master suite runners currently go below,
# see CREDO tool.
# Ideally if could do all target stuff properly, this should go back in the
#  CREDO tool.
Import('LOWRES_SUITES')
Import('INTEGRATION_SUITES')
Import('VISUALISATION_SUITES')
Import('CONVERGENCE_SUITES')
Import('SCIBENCH_SUITES')
lowresSuiteRun = env.RunSuites( 
    Dir(os.path.join(env['TEST_OUTPUT_PATH'], env["CHECK_LOWRES_TARGET"])),
    LOWRES_SUITES)
env.AlwaysBuild(lowresSuiteRun)
env.Alias(env["CHECK_LOWRES_TARGET"], lowresSuiteRun)
intSuiteRun = env.RunSuites(
    Dir(os.path.join(env['TEST_OUTPUT_PATH'], env["CHECK_INTEGRATION_TARGET"])),
    INTEGRATION_SUITES)
env.AlwaysBuild(intSuiteRun)
env.Alias(env["CHECK_INTEGRATION_TARGET"], intSuiteRun)
vizSuiteRun = env.RunSuites(
    Dir(os.path.join(env['TEST_OUTPUT_PATH'], env["CHECK_VISUALISATION_TARGET"])),
    VISUALISATION_SUITES)
env.AlwaysBuild(vizSuiteRun)
env.Alias(env["CHECK_VISUALISATION_TARGET"], vizSuiteRun)
cvgSuiteRun = env.RunSuites(
    Dir(os.path.join(env['TEST_OUTPUT_PATH'], env["CHECK_CONVERGENCE_TARGET"])),
    CONVERGENCE_SUITES)
env.AlwaysBuild(cvgSuiteRun)
env.Alias(env["CHECK_CONVERGENCE_TARGET"], cvgSuiteRun)
scibenchSuiteRun = env.RunSuites( 
    Dir(os.path.join(env['TEST_OUTPUT_PATH'], env["CHECK_SCIBENCH_TARGET"])),
    SCIBENCH_SUITES)
env.AlwaysBuild(scibenchSuiteRun)
env.Alias(env["CHECK_SCIBENCH_TARGET"], scibenchSuiteRun)
# Run the lowres checks as part of default and complete
env.Alias(env['CHECK_DEFAULT_TARGET'], env['CHECK_LOWRES_TARGET'])
env.Alias(env['CHECK_COMPLETE_TARGET'], env['CHECK_LOWRES_TARGET'])
# For the others, just add to the complete target
env.Alias(env['CHECK_COMPLETE_TARGET'], env['CHECK_INTEGRATION_TARGET'])
env.Alias(env['CHECK_COMPLETE_TARGET'], env['CHECK_CONVERGENCE_TARGET'])
#Don't run scibench as part of 'complete', since could run for 
# -very- long time.
#env.Alias(env['CHECK_COMPLETE_TARGET'], env['CHECK_SCIBENCH_TARGET'])
