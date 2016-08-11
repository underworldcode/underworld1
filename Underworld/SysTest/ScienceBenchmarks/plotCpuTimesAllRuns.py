#! /usr/bin/env python

import os, sys
import math
from credo.modelresult import ModelResult
import plotCpuTimes

def plotAllRuns(outDir):
    scalingDirs = [os.path.join(outDir, sub) for \
        sub in os.walk(outDir).next()[1]]
    scalingDirs.sort()
    nDirs = len(scalingDirs)

    cpuTimes = []
    for sd in scalingDirs:
        mRes = ModelResult(os.path.split(sd)[-1], sd)
        mRes.readFrequentOutput()
        fo = mRes.freqOutput
        cpuTimes.append(plotCpuTimes.getPerStepCPU_Times(fo))

    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt

    fig = plt.figure(figsize=(8, 4.0 + 1.0*len(cpuTimes)))
    fig.text(0.5, 0.95, "%s: CPU time/timestep" % outDir,
        horizontalalignment='center')
    plt.subplots_adjust(hspace=0.4)
    maxTSteps = max(map(len,cpuTimes))
    subplots = []
    for ii, cpuTimeList in enumerate(cpuTimes):
        ax = plt.subplot(nDirs, 1, ii+1)
        subplots.append(ax)
        plt.title(os.path.split(scalingDirs[ii])[-1])
        plt.plot(cpuTimeList)
        plt.ylim(0, math.ceil(max(cpuTimeList)*10)/10.0)
        plt.xlim(0, maxTSteps)
        majLocator = matplotlib.ticker.MaxNLocator(3)
        ax.yaxis.set_major_locator(majLocator)

    for ii in range(nDirs-1):
        plt.setp(subplots[ii].get_xticklabels(), visible=False)

    subplots[-1].set_xlabel("timestep")
    subplots[nDirs/2].set_ylabel("Step Time (sec)")
    plt.show()
    plt.savefig(os.path.join(outDir, "cpuTimePerStep.png"))

if __name__ == "__main__":    
    try:
        outDir = sys.argv[1]
    except:
        print "Error: Please pass in output directory of model suite."
        raise

    plotAllRuns(outDir)
