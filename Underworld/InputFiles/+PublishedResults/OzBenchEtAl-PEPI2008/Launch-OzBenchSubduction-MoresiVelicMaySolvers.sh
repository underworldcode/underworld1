#! /bin/bash

# This variable needs to be set to point to the Underworld build you are using

export UWPATH=$HOME/+Codes/stgUnderworldE-local-stable  # Usually a symbolic link to most recent build !

# These variables are defined from the environment above and do not need to be changed

export UW_DIR=$UWPATH/build
export UWEXEC=$UW_DIR/bin/Underworld
export GLUCIFER=$UWPATH/build/bin/gLucifer

# These variables define the location of the OzbenchSubduction.xml file you are using ($PROJECTPATH)
# and the directory where the output is destined to be stored ($OUTPUTPATH)
# and the name of the subdirectory for this run (note, you can over-ride parameters on the
# command line and therefore run multiple experiments without changing the XML files	)

PROJECTPATH=$UWPATH/Underworld/InputFiles/+PublishedResults/OzBenchEtAl-PEPI2008
OUTPUTPATH=$HOME/+Simulations/OzBenchSubductionRuns+MVSolve
EXPTNAME=OzBench3000kmWide

OUTPUTDIR=$OUTPUTPATH/$EXPTNAME 
INPUTDIR=$OUTPUTDIR

if [ ! -d $OUTPUTDIR ]; then 
	mkdir -p $OUTPUTDIR; 
fi
	
mkdir $OUTPUTDIR/Checkpoints >& /dev/null

OUTPUTFILE=${OUTPUTDIR}/output.txt
ERRORFILE=${OUTPUTDIR}/errormessages.txt 

openmpirun -np 8 $UWEXEC \
                  $PROJECTPATH/OzbenchSubduction.xml \
                  $PROJECTPATH/OzBenchSubduction-LargePerturbation.xml \
                  $UWPATH/Solvers/InputFiles/AugLagStokesSLE-GtMG.xml \
                  $UWPATH/Solvers/InputFiles/VelocityMassMatrixSLE.xml \
                  $UWPATH/Solvers/InputFiles/kspinterface.xml \
                  $UWPATH/Solvers/InputFiles/MultigridForRegularSCR.xml \
    -options_file $UWPATH/Solvers/examples/options-scr-mg-accelerating.opt \
      --mgLevels=5 \
      --nonLinearMaxIterations=25 \
      -mg_accelerating_smoothing_view 0 \
      -uzawastyle 0 \
      -scrPCKSP_ksp_view \
  	  -ksp_type bsscr \
      -pc_type none   \
      -ksp_k2_type GG  \
      -augmented_lagrangian 1 \
      -no_scale \
      --penaltyNumber=1000.0 \
      -Q22_pc_type uwscale \
  		-build_const_pressure_nullspace \
      -scr_ksp_type fgmres \
      -Xscr_ksp_view \
      -scr_ksp_monitor \
  		-scr_ksp_rtol 1.0e-3 \
  		-A11_ksp_rtol 1.0e-4 \
      -A11_ksp_type fgmres \
      -A11_ksp_converged_reason \
      -XA11_ksp_view \
      -XA11_ksp_monitor \
      -backsolveA11_ksp_type fgmres \
      -backsolveA11_ksp_rtol 1.0e-4 \
	--maxTimeSteps=2500 \
	--outputPath=$OUTPUTDIR \
	--dumpEvery=5 \
	--checkpointEvery=10 \
	--saveEvery=5 \
	--checkpointReadPath=$OUTPUTDIR/Checkpoints \
	--checkpointWritePath=$OUTPUTDIR/Checkpoints \
	--checkpointAppendStep=1 \
	--elementResI=96 \
	--elementResJ=48 \
	--elementResK=96 \
	--mgLevels=4 \
  --minX=-1.8 \
  --minY=0.0 \
  --minZ=0.0 \
  --maxX=4.2 \
  --maxY=1.0 \
  --maxZ=6.0 \
  --Xcomponents.vonMisesUpper.cohesion=0.04 \
  --Xcomponents.upperSlabShape.startY=0.95 \
	--XslabStartZ=0.0 \
	--XslabEndZ=3.0 \
	--XrestartTimestep=100 > $OUTPUTFILE

 
