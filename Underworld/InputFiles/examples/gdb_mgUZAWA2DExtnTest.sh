#! /bin/bash

#
#   Solve using Uzawa + MG
#
#   Uzawa Solver has two ksps
#   1: the preconditioner ksp with prefix -Uzawa_pcSolver_
#   2: the velocity ksp with prefix       -Uzawa_velSolver_
#   When Multigrid is used the MG functions replace the velocity ksp
#   and set it's prefix to -A11       ( line 308 in /StgFEM/SLE/SystemSetup/src/PETScMGSolver.c )
#   The preconditioner ksp is unchanged.
#
export UWPATH="../../.."
export UWEXEC="gdb --args $UWPATH/build/bin/Underworld" 

OUT="outputMGUZAWA_gdb"
mkdir $OUT >& /dev/null
$UWEXEC $UWPATH/Underworld/InputFiles/+LithosphereSandbox/Sandbox2D.xml  $UWPATH/Underworld/InputFiles/Sandbox2DOverRide.xml \
    $UWPATH/StgFEM/Apps/StgFEM_Components/MultigridForRegular.xml \
    -options_file $UWPATH/Underworld/InputFiles/options/options-uzawa-mg.opt \
  		--outputPath="./$OUT" \
  		--components.stokesEqn.isNonLinear=True \
  		--components.box.startY=0.0 \
  		--components.box.endY=1.0 \
  		--components.weakZone.startY=0.0 --components.weakZone.endY=0.05 --components.substrateViscosity.eta0=0.1 \
  		--saveDataEvery=1 --checkpointEvery=2 --checkpointWritePath="./$OUT/Checkpoints" --checkpointAppendStep=1 \
  		--Cohesion=50 --Cohesion2=50 --FrictionCoeff=0.25 --FrictionCoeff2=0.25 \
   		--minYieldCriterion=1.0 --minViscosity=1.0 \
                --nonLinearTolerance=0.001 \
  		--elementResI=64 --elementResJ=32 \
                --mgLevels=5 \
  		--maxTimeSteps=100
