#!/usr/bin/perl -w
#
use strict;

##### INCLUDED FILES ######
require "commonTestFunctions.pl";

##### SUBROUTINES #####
sub runTests;


##### GLOBAL VARS #####
our $testReport = "[Bitten] purpose=test current numerical fields against previously checkpoint fields of this model\n";
our $cvgFileName = "";
# Set the default tolerance. At the moment the calls to read command line and input files are within
# runTests, so the tolerance has to be passed call-by-reference and set within them.
our $tolerance = 3e-2;
our $helpStr = "To run checkpoint tests:

./checkpointTest.pl <xmlFile> [ OPTIONS ]

where OPTIONS:
   -optionsFile <fileName>   : where <fileName> is the options file. Command line agruments in StGermain format.
   -c                        : will \"create\" checkpointed data only. By default this flag in not set and the script only checks against previous checkpointed data.  
   -n <#>                    : the timestep checkpoint writing (if -c is defined) or checkpoint testing will occur on. By default this is timestep 10.  
   -np <#>                   : the number of processors to run. (This value will overwrite the number of preocessors given in the optionsFile  
   -serial                   : will execute test without any mpi binary prefix. (Overwrites \"-np\" option).
   -useCM                    : enables the test model to use a Contant Mesh
   -tol <#>                  : The tolerance to use in testing (default is $tolerance)
   -h                        : this help message

EXAMPLE:
  ./checkpointTest.pl testVelicSolS.xml -optionsFile OFile.dat
		(Runs with option file OFile.dat and checks against the expected file)
	
";

######  MAIN PROGRAM    ######

# 1) Run the xml 
$cvgFileName = &runTests( $tolerance );

# 2) Check against expected, checkpoint files 
exit &testConvergence( $cvgFileName, $tolerance );

######  END MAIN  ######


sub runTests {
	my $res;
	my $command;
	my $createTest=0; #boolean to create an expected file, defaut 0

	# read commandline args
	my $arg;
	my $ii = 0;
	my $xmlFile = " ";
	my $optFile = " ";
	my $numberOfTimeSteps = 10; # testing Timestep is 10 by default
	my @procs = (1,1,1,1);
	my @commandLines = ""; #("--elementResI=32 --elementResJ=32 " );
	my $outputPath = " ";
	my $nProcs = -1;
	my $isSerial = 0;
	my $mpiBin = $ENV{'UNDERWORLD_MPI'};
	my $useCM = 0;
												
	#Set the tolerance call by reference
	&readCommandLineOptions( \@ARGV, $helpStr, $xmlFile, $optFile, $createTest, $numberOfTimeSteps, $outputPath, $nProcs, $isSerial, $useCM, $_[0] );

	if( $xmlFile eq " " ) { die "\n\n### ERROR ###\nNo xml file specified, stopped" ; }
	if( !(-e $xmlFile) ) { die "\n\n### ERROR ###\nCannot find input file: $xmlFile, stopped" ; }

	# check if options file is given, otherwise run default
	if( $optFile ne " " ) {
		if( !(-e $optFile) ) { die "\\n### ERROR ###\nnCannot find run options file $optFile, stopped"; }

		# read in run options file
		&readOptionsFile( $optFile, \@procs, \@commandLines );
		print "\nUsing options file $optFile, specifed options are:\n-n $procs[0] "; foreach (@commandLines) { print "$_ "; }
	}

	# if commandline option np is valid use it
	if( $nProcs > 0 ) { $procs[0] = $nProcs; }
	if( $isSerial ) { $procs[0] = 1; }

	my $exec = "udw"; # executable name
	my $stdout;
	my $stderr;

	# create strings for 1) creating checkpoint data & 2) testing against checkpointed data
	my $xmlSegmentCreateTest = "<StGermainData xmlns=\"http://www.vpac.org/StGermain/XML_IO_Handler/Jun2003\">
		<param name=\"checkpointEvery\" mergeType=\"replace\">10</param>
		<param name=\"outputPath\" mergeType=\"replace\">./expected/$xmlFile</param>
		<param name=\"maxTimeSteps\" mergeType=\"replace\">$numberOfTimeSteps</param>
		<param name=\"dumpEvery\" mergeType=\"replace\">0</param>
</StGermainData>";
# 	my $xmlSegmentToTest = "<StGermainData xmlns=\"http://www.vpac.org/StGermain/XML_IO_Handler/Jun2003\">
# 	<struct name=\"components\" mergeType=\"merge\">
# 		<struct name=\"tester\">
# 			<param name=\"Type\">FieldTest</param>
# 		</struct>	
# 	</struct>

# 	<param name=\"outputPath\" mergeType=\"replace\">./output/$xmlFile</param>
# 	<param name=\"checkpointEvery\" mergeType=\"replace\">0</param>
# 	<param name=\"dumpEvery\" mergeType=\"replace\">0</param>
# 	<param name=\"maxTimeSteps\" mergeType=\"replace\">$numberOfTimeSteps</param>
# 	<struct name=\"pluginData\" mergeType=\"replace\">
# 		<list name=\"NumericFields\">
# 			<param>VelocityField</param> <param>0</param>
# 			<param>PressureField</param> <param>1</param>
# 			<param>TemperatureField</param> <param>2</param>
# 		</list> 
# 		<param name=\"IntegrationSwarm\">gaussSwarm</param>
# 		<param name=\"ConstantMesh\">constantMesh</param>
# 		<param name=\"testTimestep\">$numberOfTimeSteps</param>
# 		<param name=\"ElementMesh\">linearMesh</param>
# 		<param name=\"normaliseByAnalyticSolution\">True</param>
# 		<param name=\"context\">context</param>
# 		<param name=\"appendToAnalysisFile\">True</param>
# 		<!-- reference soln stuff -->
# 		<param name=\"useReferenceSolutionFromFile\">true</param>
# 		<param name=\"referenceSolutionFilePath\">./expected/$xmlFile</param>
# 		<list name=\"ReferenceFields\">
# 			<param>VelocityField</param>
# 			<param>PressureField</param>
# 			<param>TemperatureField</param>
# 		</list> 
# 	</struct> 
# </StGermainData>";

	my $xmlSegmentToTest = "<StGermainData xmlns=\"http://www.vpac.org/StGermain/XML_IO_Handler/Jun2003\">
	<param name=\"outputPath\" mergeType=\"replace\">./output/$xmlFile</param>
</StGermainData>";

	# Need to check for an executable
	if( !(-e "./../../../build/bin/StGermain" ) ) {
		die "\n\n### ERROR ###\nCan't find ./../../../build/bin/StGermain - the executable which runs the test, stopped";
	}

	if( $createTest ) {
		print "\n--- Creating checkpoint files for $xmlFile at timestep $numberOfTimeSteps---\n";
	} else {
		print "\n--- Testing the $xmlFile ---\n";
	}

	# is the symbolic link there, if not create it
	if( !(-e $exec) ) {
		$command = "ln -s ../../../build/bin/StGermain $exec";
		print "\n$command\n\n";
		&executeCommandline( $command );
	}	

	# check if there's a log dir
	if( !(-e "log/") ) {
		$command = "mkdir log";
		&executeCommandline( $command );
	}

	# declare stdout and stderr files, in log dir.
	$stdout = "log/$xmlFile"."_runs.stdout";
	$stderr = "log/$xmlFile"."_runs.stderr";

	# remove old log file, if it exists
	if( -e "$stdout" ) {
		$command = "rm $stdout";
		&executeCommandline( $command );
	}

	# remove old cvg file, if it exists 
	if( scalar (glob "*.cvg") ) { 
		$command = "rm *.cvg"; 
		&executeCommandline( $command );
	} 

	# create help.xml for setting up test
	if( $createTest ) {
		$command = "echo \'$xmlSegmentCreateTest\' > help.xml ";
	} else {
		$command = "echo \'$xmlSegmentToTest\' > help.xml ";
	}
	&executeCommandline($command);

	# run test case
	$command = "mpiexec -n $procs[0] ./$exec $xmlFile help.xml $commandLines[0] --pluginData.appendToAnalysisFile=True >$stdout";
	$command .= " 2>$stderr";
	print "$command";
	&executeCommandline( $command );

	# check error stream for error result
	open( ERROR, "<$stderr" );
	my $line;
	foreach $line (<ERROR>) {
		if( $line =~ m/[E|e]rror/ ) {
			close(ERROR); 
			die ("\n\n### ERROR ###\nError in runtime: see $stderr or $stdout - stopped" ); 
		}
	}

	# if no error close file and delete it
	close(ERROR); 
	$command = "rm $stderr"; &executeCommandline($command);

	# removing help.xml
	$command = "rm help.xml";
	print "\n$command\n"; &executeCommandline($command);

	# removing softlink
	$command = "rm $exec";
	print "$command\n"; &executeCommandline($command);

	print "--- Finished  ---\n\n";

	# if we're only creating checkpoint file, end program here
	if( $createTest ) { exit(0); }

	$testReport .= "[Bitten] proc=$procs[0]\n";

	#search for resolution to report
	my $resx;
	my $resy;
	my $resz;
	open( FLATOUTPUT, "./output/$xmlFile/input.xml" )
		or die ("\n\n### ERROR ###\n\t\tCouldn't open output file, ./output/$xmlFile/input.xml " );

	my $resolution;
	foreach $line (<FLATOUTPUT>) {
		if( $line =~ m/\"elementResI\">(\d+)</ ) { $resx = $1; }
		elsif( $line =~ m/\"elementResJ\">(\d+)</ ) { $resy = $1; }
		elsif( $line =~ m/\"elementResK\">(\d+)</ ) { $resz = $1; }
	}
	close( FLATOUTPUT );

	#get the total CPU time from plugin
	my @labels;
	my $label;
	my $totalTime;
	my $freqOutput = "./output/$xmlFile/FrequentOutput.dat";

	if( !(-e $freqOutput) ) {
		die("\n\n### ERROR ###\nCouldn't open $freqOutput");
	}
	$ii = 0;
	$command = "head -n 1 $freqOutput";

	@labels = split( /\s+/, &executeCommandline( $command ) );
	foreach $label (@labels) {
		if( $label =~ m/CPUTime/ ) { last; }
		$ii++;
	}
	$command = "tail -n 1 $freqOutput";
	@labels = split( /\s+/, &executeCommandline( $command ) );
	$totalTime = $labels[$ii-1];
	$testReport .= "[Bitten] time=$totalTime\n";

	#append to report string
	$testReport .= "[Bitten] resx=$resx\n";
	$testReport .= "[Bitten] resy=$resy\n";
	if ( defined $resz ) { $testReport .= "[Bitten] resz=$resz\n"; }
	else { $testReport .= "[Bitten] resz=0\n"; }

	# return convergence file name
	$command = "ls *\.cvg 2>/dev/null";
	my $cvg = &executeCommandline($command);
	chomp( $cvg );
	return $cvg;
}


