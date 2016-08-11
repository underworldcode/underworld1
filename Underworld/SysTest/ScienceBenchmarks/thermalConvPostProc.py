#!/usr/bin/env python
# Set of utility funcs useful for different thermal benchmarks

def plotResultsVsBlankenbach(Ras, BBVrmsMin, BBVrmsMax,
        BBNusseltMin, BBNusseltMax,
        vrmsResDict, nusseltResDict,
        outputFilename):
    """Plot graphs of VRMS and Nusselt actual results versus reference ones
    (found in Blankenbach paper).

    `vrmsResDict` and `nusseltResDict` are dicts mapping "name of set" :
    list of results per run. E.g.:
    vrmsActuals = {'UW_16x16':[34.2, 45.8]}
    Multiple entries in the dictionary will all be plotted.
    """    
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    plt.figure(1)
    plt.subplot(211)
    plt.title('VRMS values vs Ra Number')
    vmin = plt.plot(Ras, BBVrmsMin, 'bo-', label='Blankenbach 1989 (min)')
    vmax = plt.plot(Ras, BBVrmsMax, 'ro-', label='Blankenbach 1989 (max)')
    for setName, vrmsResults in vrmsResDict.iteritems():
        vactual = plt.plot(Ras, vrmsResults, 'ko-', label=setName)
    plt.ylabel('Vrms')
    plt.xlabel('Ra Number')
    plt.xscale('log')
    plt.grid(True)
    plt.legend(loc='best', prop={'size':'small'})
    plt.draw()

    plt.subplot(212)
    plt.title('Nusselt Values vs Ra Number')
    nmin = plt.plot(Ras, BBNusseltMin, 'bo-', label='Blankenbach 1989 (min)')
    nmax = plt.plot(Ras, BBNusseltMax, 'ro-', label='Blankenbach 1989 (max)')
    for setName, nusseltResults in nusseltResDict.iteritems():
        vactual = plt.plot(Ras, nusseltResults, 'ko-', label=setName)
    plt.ylabel('Nusselt Number')
    plt.xlabel('Ra Number')
    plt.xscale('log')
    plt.grid(True)
    plt.legend(loc='best', prop={'size':'small'})
    plt.subplots_adjust(hspace=0.4)
    plt.draw()
    plt.savefig(outputFilename,
        dpi=None, facecolor='w', edgecolor='w',
        orientation='portrait', papertype=None, format=None,
        transparent=False)
