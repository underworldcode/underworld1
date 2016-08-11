#! /bin/bash


if [ "$1" = "-h"  -o "$1" = "--help" ]    
then
	
	cat <<'HELP_MESSAGE'

UnderworldBashFunctions.sh

   A collection of useful workflow tools written to work with Bash ( just in case that's all you
   happen to have in your armoury - for example within a queuing system environment )
   You should source this file at the top of the scripts in which the functions are used.
	
HELP_MESSAGE
	
# Note, at the end of this file there is
# some more code to call the functions
# and display their help messages	

# I think there is probably a clever way
# for the script to parse itself and print
# out everything between the HELP_MESSAGE tags
# but that is somewhat beyond my level of skill
	
fi



underworld_looping_with_restart() {

	ULP_UWRUN=$1
	ULP_CHECKPOINTREADDIR=$2
	ULP_OUTPUTDIR=$3
	ULP_TARGETTIMESTEPS=$4
	ULP_INCTIMESTEPS=$5
	ULP_MAXTRIES=$6
	
	
if [ "$#" -ne "6" -o "$1" = "-h"  -o "$1" = "--help" ]
	then
	cat <<'HELP_MESSAGE'

underworld_looping_with_restart()

   A function to continually restart U/W until some total number of timesteps is reached. 
   This is to account for random crashes due to bugs and memory leaks etc.

   usage: underworld_looping_with_restart CMD OLDCHECKPOINTDIR OUTPUTDIR TIMESTEPS SUBSTEPS RETRIES
          ALL six arguments are required  
    
   * CMD -              is the complete executable to launch the UW job 
                          ( but note that it needs to accept further arguments on the command line,
                            so no re-direction is permitted )
   * OLDCHECKPOINTDIR - is the location of checkpoints to restart from. 
                          ( Currently the last checkpoint in the directory is selected - this may change)	
   * OUTPUTDIR -        is the directory where this script will dump files and all subsequent checkpoints	
   * TIMESTEPS -        the total number of timesteps to be run	
   * SUBSTEPS  -        the number of timesteps before forcing a restart
   * RETRIES   -        the number of loops before this function gives up 
   
   NOTE - the function also returns early if no checkpoints are created. It does not check
          to see if the script suddenly stops making headway after creating one or more checkpoints
	
HELP_MESSAGE
	
	printf "You called: underworld_looping_with_restart $* \n\n"	
				
	return 1
fi	
	
	
	ULP_CHECKPOINTWRITEDIR=${ULP_OUTPUTDIR}/Checkpoints
	ULP_OUTPUTFILE=${ULP_OUTPUTDIR}/output.txt 
	ULP_ERRORFILE=${ULP_OUTPUTDIR}/errormessages.txt
	ULP_LASTCHECKPOINTDIR=`ls $ULP_CHECKPOINTREADDIR | tail -1` >& /dev/null
	ULP_LASTCHECKPOINTSTEP=`echo ${ULP_LASTCHECKPOINTDIR##*.} | sed -e 's/^0\+//'`  
	
	mkdir $ULP_OUTPUTDIR      >& /dev/null 
	mkdir $ULP_CHECKPOINTWRITEDIR  >& /dev/null

	if [ -z $ULP_LASTCHECKPOINTSTEP ]
		then
			ULP_LASTCHECKPOINTSTEP=0
			printf "No previous checkpoints in %s\n"  $ULP_CHECKPOINTREADDIR
		else
			printf "Previously calculated checkpoint - %s \n" $ULP_LASTCHECKPOINTSTEP
	fi
	
	ULP_LASTTIMESTEP=$ULP_LASTCHECKPOINTSTEP
	
	let ULP_COUNTER=0
	while [ "$ULP_LASTTIMESTEP" -lt "$ULP_TARGETTIMESTEPS" ]; do

		printf "Run # %d from timestep %s using checkpoints in %s \n" $ULP_COUNTER $ULP_LASTCHECKPOINTSTEP $ULP_CHECKPOINTREADDIR
		printf "You can monitor the output of this job in %s.%s \n" $ULP_OUTPUTFILE $ULP_LASTCHECKPOINTSTEP 

		eval $ULP_UWRUN \
		    --checkpointReadPath=$ULP_CHECKPOINTREADDIR --checkpointWritePath=$ULP_CHECKPOINTWRITEDIR \
		    --outputPath=$ULP_OUTPUTDIR \
			--restartTimestep=$ULP_LASTCHECKPOINTSTEP \
			--maxTimeSteps=$ULP_INCTIMESTEPS \
			< /dev/null 1>"$ULP_OUTPUTFILE.$ULP_LASTCHECKPOINTSTEP" 2>"$ULP_ERRORFILE.$ULP_LASTCHECKPOINTSTEP"  

		ULP_CHECKPOINTREADDIR=$ULP_CHECKPOINTWRITEDIR

		# See how many steps we actually computed	

		ULP_LASTLINE=(`tail -1 $ULP_OUTPUTDIR/FrequentOutput.dat`)
		ULP_LASTTIMESTEP=${ULP_LASTLINE[0]}
		printf "Previously calculated timestep - %s \n" $ULP_LASTTIMESTEP
		
		ULP_LASTCHECKPOINTDIR=`ls $ULP_CHECKPOINTREADDIR | tail -1`
		ULP_LASTCHECKPOINTSTEP=`echo ${ULP_LASTCHECKPOINTDIR##*.} | sed -e 's/^0\+//'`

		# If there have been no checkpoints saved then we can't really carry on
		if [ -z "$ULP_LASTCHECKPOINTSTEP" ]  
			then
			ULP_LASTCHECKPOINTSTEP="0"
			printf "Error - failed to create any new checkpoint information in this iteration\n"
			break  
		fi
		
		# If a process was interupted before any timestepping was done, force the value back
		if [ "$ULP_LASTTIMESTEP" = "#" ]
			then
			echo "Forcing timestep value"
			ULP_LASTTIMESTEP=$ULP_LASTCHECKPOINTSTEP
		fi

		let ULP_COUNTER=ULP_COUNTER+1

		if [ "$ULP_MAXTRIES" -lt "$ULP_COUNTER" ]
			then
			break
		fi

	done

	printf "Completed %s timesteps (of %s) in %s restarts\n" $ULP_LASTTIMESTEP $ULP_TARGETTIMESTEPS $ULP_COUNTER
	
   return 0
}



# ==========================================
# If the script is called with --help or -h
# ==========================================


if [ "$1" = "-h"  -o "$1" = "--help" ]    
	then
	underworld_looping_with_restart "--help"
fi


 

