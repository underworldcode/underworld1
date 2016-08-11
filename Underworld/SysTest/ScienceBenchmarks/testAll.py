#!/usr/bin/env python

from credo.systest import *
from credo.systest.systestrunner import getSuitesFromModules

def suite():
    allSuite = SysTestSuite("Underworld", "SciBench")
    # Load all the known benchmarks in this dir
    #modNames = ['credo_rayTayBenchmark',
    #    'credo_thermalConvDimBenchmark', 'credo_thermalConvNonDimBenchmark',
    #    'credo_thermalScalingSuite']
    #suites = getSuitesFromModules(modNames)
    import credo_rayTayBenchmark
    import credo_thermalConvDimBenchmark
    import credo_thermalConvNonDimBenchmark
    import credo_thermalScalingSuite
    import credo_orthotropicBenchmark
    # Don't import orthotropic for now - until hdf5 usage resolved
    mods = [credo_rayTayBenchmark, credo_thermalConvDimBenchmark,
        credo_thermalConvNonDimBenchmark, credo_thermalScalingSuite,
        credo_orthotropicBenchmark]
    suites = [mod.suite() for mod in mods]
    allSuite.addSubSuites(suites)
    return allSuite

if __name__ == "__main__":
    testRunner = SysTestRunner()
    testRunner.runSuite(suite(), postProcFromExisting=False)
