#!/usr/bin/perl -w
#
use strict;

sub makeMGOptionsArray;
sub runTests;
sub testConvergence;
sub generateConvergence;
sub testNumbersAgainstExpected;
sub readParamFile;
sub testOutputsAgainstEachOther;
sub postProcessFunc;
our $progHelp = "To run MG convergence tests:

./runAndTestMG.pl <xmlFile> [ OPTIONS ]

where OPTIONS:
  -myOwnOptFiles <specifiedMGOptFile> : where <specifiedMGOptFile> is a .dat file with the list of the options files you want to use and this will call a separate 
										subroutine to include your options files in an array by reading the .dat file and storing the lines with .opt in an array.  
										See options/exampleOptionFile.opt for a list of all the possible options you can set and exampleOptFilesMG.dat for an 
										example of how to list the option files you want to use.
										
  -paramFile <fileName>				  : where <fileName> is the param file where both resolution and mgLevels are set. By default serial runs of resolution 64sq, 
										128sq with mgLevels of 4 and 5 are done- REMEMBER TO SET the mgLevels parameter in the command line.  See OFileMG2D.dat.  A
										recommended method for setting mgLevels is to find the number of levels where the coarsest grid is about 8x8 and the number
										of levels should be calculated as if you are running the job on 1 proc.  For example for a 64sq job, the mgLevels is set to
										4 regardless of how many procs the job is on (the MG solver takes care to ensure that the coarsest grid on each proc is as 
										close to 8x8 as possible).
   
  -nonAnalytic <refFilePath>		  : where <refFilePath> is the high resolution reference solution file path where the high resolution solution field variable is 
										compared against the run time solutions instead of the analytic solution.  IF THIS IS SET, YOU MUST SET THE TEST TIMESTEP 
										variable.  See pluginForMGTest.xml and the pluginData struct for how to set out the numeric and reference fields for
										comparison.
  
  -testTimestep <testTimstep>		  : The timestep at which you want your high resolution reference solution and your numeric solution compared at.									
  
  -againstExpected <expfileName>         : will check the generated numbers against a file you you specify, assuming the file is in the 'expected/MG/' dir.  It can be a frequent output file or a 
										convergence file. By default a check will not occur.
  
  -againstEachOther					  : will compare the generated convergence results and frequent output results for each option file (assumes these results are 
										generated in the log dir). Finds the quickest inner, outer and if non-linear, non-linear iterations, the least number of inner, 
										outer and non-linear iterations and the best convergence rates for each field.  The log dir files will be saved with the 
										name of the particular option file concatinated onto the end of the file name, for user reference.  For example: 
										log/Underworld_ExtensionFMM.xml-analysis.frequentout-MG-opt_1.opt.  The best option files for each field variable (convergence 
										or speed) will be printed to screen.
										
-postProcess						  : will generate: 1. A table based on run parameters 
													   2. A table based on option file parameters 
													   3. A table/s based on frequent output variables relating to the ksp solvers etc from the option files
													   4. A table/s based on convergence variables relating to the ksp solvers etc from the option files
													   5. A graph/s plotting the frequent output vars that vary for each run
													   6. A graph/s plotting the convergence fields that vary for each run

EXAMPLES: 1 analytic, 1 nonanalytic

1. ./runAndTestMG.pl testVelicSolCx.xml -myOwnOptFiles exampleOptFilesMG.dat -againstEachOther -postProcess

Runs with user specified option files, NO param file OFile.dat, NO check against a specified expected file although the generated convergence and timing variables for
each option file specified, are compared against each other and graphs and tables are outputted in .png and .txt format.

2. UNCOMMENT OUT PLUGIN DATA STRUCT IN pluginForMGTest.xml 
   ./runAndTestMG.pl ExtensionFMM.xml -paramFile OFileMG2D.dat -nonAnalytic ./output/HRRefSoln_opt_2 -testTimestep 1      

Runs with default option files, a user specified resolution and mgLevel via a param file OFileMG2D.dat, NO check against a specified expected file 
and the timestep the high resolution and low resolution field variables are compared at is 1.

These scripts measure the convergence rates in numerical fields generated in Underworld either against analytic solutions supplied by Mirko Velic 
OR a fine resolution grid that was run earlier.
The error of FEM fields should decrease with higher resolution - depending on the choice of element type that represents a given field.
These scripts run error measures on given field at several resolutions and then process how the error converges.

THESE SCRIPTS REQUIRE EXPERIMENTAL TO BE BUILT\n\n\n";

######  Main program    ######
# 
# For each option file: Run the same xml at the 2 different resolutions
#    these will generate error data

#check if myOwnOptFiles is set and then run a subroutine to store the file names in an array	
print "\nIf running this on serial PC please start mpd before running.  If running this with non analytic reference solution UNCOMMENT COMPONENT STRUCT IN pluginForMGTest.xml\n";

my $useOwnOptFiles = 0;
my $numberOfOptFiles;
my $specifiedMGoptfiles = "";
my @optionsfiles = ("options/opt_1.opt", "options/opt_2.opt", "options/opt_3.opt", "options/opt_4.opt", "options/opt_5.opt", "options/opt_6.opt"); 
my $arg;
my $ii = 0;
my $xmlFile;
my $runAgainstEachOther = 0;
my $postProcess = 0;
my @command;
my $optionsfile;
my $index;

foreach $arg (@ARGV) {
	if( $arg =~ m/\-myOwnOptFiles/ ) {
		#need to get just the filename
		$specifiedMGoptfiles = $ARGV[$ii+1];
		$useOwnOptFiles = 1;
		
	}
	if( $arg =~ m/\-againstEachOther/ ) {
			$runAgainstEachOther = 1;
	}
	if( $arg =~ m/\-postProcess/ ) {
			$postProcess = 1;
	}
	if( $arg =~ m/.*\.xml$/ ) { $xmlFile = $arg; }
	
	$ii++;
}
if($useOwnOptFiles == 0){
	$numberOfOptFiles = 6;
}
if($useOwnOptFiles == 1){
	
	$numberOfOptFiles = &makeMGOptionsArray($specifiedMGoptfiles,\@optionsfiles );
	
	if($numberOfOptFiles <= 0){
		die "mgOptions array was not made, can not continue\n";
	}
	
}
my $run_I;
my $testsToRun;

for ($run_I = 0; $run_I < $numberOfOptFiles; $run_I++){
		$optionsfile = $optionsfiles[$run_I];
		$testsToRun = &runTests( $run_I, $numberOfOptFiles, $optionsfile, \@command );
		
		$index = $testsToRun*$run_I + $testsToRun - 1;
		
		# 3) Use a linear regression algorithm on the data generated
		#    and check the results for good behavior and acceptable convergence	
		&testConvergence( $optionsfiles[$run_I], &generateConvergence() );
		
}
	
if( $runAgainstEachOther == 1 ) {
	
		&testOutputsAgainstEachOther($xmlFile, $numberOfOptFiles, $testsToRun, \@optionsfiles);
}	
if( $postProcess == 1 ) {
		print "call postprocess\n";
		&postProcessFunc($xmlFile, $numberOfOptFiles, $testsToRun, \@optionsfiles, \@command);
}	

######  End main program ######
sub makeMGOptionsArray {
	my ( $specifiedMGoptfiles, $optionsfiles ) = @_;
	my $line_I = 0;
	my $line;
	
	if( $specifiedMGoptfiles eq " " ) {
		warn "\nNo run options file specifed using commandline arg '-myOwnOptFiles xxx'".
				 "\nUsing default options files";
		return -1;
	}	
	if( !(-e $specifiedMGoptfiles) ) { 
		die "\nCannot find run param file $specifiedMGoptfiles, stopped"; 
		return 0;
	} else{	
			open MGoptfiles, "<$specifiedMGoptfiles" || die "Can't open param file $specifiedMGoptfiles, stopped" ;
			foreach $line (<MGoptfiles>) {
			# only process lines that end with .opt
				if( $line =~ m/.*\.opt$/  ) {
					#take out the last character in the line
					chomp $line;
					$optionsfiles->[$line_I] = $line;
					print "$optionsfiles->[$line_I]\n";
					$line_I++;
			} else { next; }
		}
		print "There are $line_I options files\n";
		return $line_I;
	}
	
}
sub runTests {
	my $res;
	my $comm;
	my $makearray;
	my ( $run_I, $numberOfOptFiles, $optionsfile, $command_ref) = @_;	# read
	my $arg;
	my $ii = 0;
	my $i = 0;
	my $xmlFile = " ";
	my $paramFile = " ";
	my $refFilePath = " ";
	my $testTimestep = 0;
	my $testsToRun = 2;
	my @procs = (1,1);
	my @mgLevels = (2,3); #8x8 coarsest grid
	my @commandLines = ("--elementResI=32 --elementResJ=16 ",
					  "--elementResI=64 --elementResJ=32 ");
					  							
	# check if xml exists and param file is specified
	foreach $arg (@ARGV) {
		if( $arg =~ m/.*\.xml$/ ) { $xmlFile = $arg; }
		elsif( $arg =~ m/\-paramFile/ ) { $paramFile = $ARGV[$ii+1]; }
		elsif( $arg =~ m/\-nonAnalytic/ ) { $refFilePath = $ARGV[$ii+1]; }
		elsif( $arg =~ m/\-testTimestep/ ) { $testTimestep = $ARGV[$ii+1]; }
		elsif( $arg =~ m/^\-h$/ ) { print $progHelp; exit }
		elsif( $arg =~ m/^\-\-help$/ ) { print $progHelp; exit }
		$ii++;
	}
	if( $xmlFile eq " " ) { die "\nNo xml file specified, stopped" ; }
	if( ($refFilePath ne " ") && ($testTimestep == 0) ) { die "\nNo timestep specified, stopped" ; }
	if( !(-e $xmlFile) ) { die "\nCannot find input file: $xmlFile, stopped" ; }

	# check if options file is given, otherwise run default
	if( $paramFile eq " " ) {
		warn "\nNo run param file specifed using commandline arg '-paramFile xxx'".
				 "\nUsing default serial mode, with commandLines:\n"; foreach (@commandLines) { print "$_\n"; }
		
		
	}	else {
		if( !(-e $paramFile) ) { die "\nCannot find run param file $paramFile, stopped"; }
		# read in run options file
		$testsToRun = &readParamFile( $paramFile, \@procs, \@commandLines );
		
	}


# do checks on executables and log files

	my $exec = "Underworld";
	my $stdout;
	my $stderr;
	# Need to check for an executable
	if( (-e "../../../build/bin/Underworld" eq 0 ) ) {
		die "\nCan't find ../../../build/bin/Underworld - the executable which runs the test, stopped";
	}
	print "\n--- Testing the convergence of $xmlFile ---\n";
	# is the symbolic link there, if not create it
	if( !(-e $exec) ) {
		$comm = "ln -s ../../../build/bin/Underworld $exec";
		print "\n$comm\n\n";
		`$comm`;
	}	
	# check if there's a log dir
	if( !(-e "log/") ) {
		$comm = "mkdir log";
		`$comm`;
	}
	# declare stdout and stderr files, in log dir.
	$stdout = "log/$xmlFile"."_runs.stdout";
	$stderr = "log/$xmlFile"."_runs.stderr";

	# remove old log file, if it exists
	if( -e "$stdout" ) {
		$comm = "rm $stdout";
		`$comm`;
	}
	# remove old cvg file, if it exists for the FIRST run:
	if($run_I == 0){
		$comm = "ls *\.cvg 2>/dev/null";
		my $cvg = `$comm`;
		chomp( $cvg );
		if( $cvg ne "" ) { $comm = "rm $cvg"; `$comm`;} 
	
		# remove old log file, if it exists
		$comm = "l 2>/dev/null";
		my $log = `$comm`;
		chomp( $log );
		if( $log ne "" ) { $comm = "rm $log"; `$comm`;} 
	}
# commence running 
    my $run2_I = 0;
	my $comm_index = 0;
	for( $run2_I = 0 ; $run2_I < $testsToRun ; $run2_I++ ) {
		$comm_index = $testsToRun*$run_I + $run2_I;
		print "command index is $comm_index\n";
# run test case
#add the options file to the command lines
		if($refFilePath eq " "){
# Analytic solution run
			if($paramFile eq " "){
				# use default mgLevels
				print "run1 $run_I, run2 $run2_I\n";
				$comm = "mpirun -np $procs[$run2_I] ./$exec $xmlFile --include StgFEM/MultigridForRegular.xml --include pluginForMGTest.xml --mgLevels=$mgLevels[$run2_I] $commandLines[$run2_I] -options_file $optionsfile --pluginData.appendToAnalysisFile=True --pluginData.FrequentOutputReadFileName=./output/" . $xmlFile . "_MG/FrequentOutput.dat --pluginData.FrequentOutputDumpFileName=Underworld_" . $xmlFile . "-analysis.frequentout --outputPath=./output/" . $xmlFile . "_MG >$stdout";
				$command_ref->[$comm_index] = "mpirun -np $procs[$run2_I] --mgLevels=$mgLevels[$run2_I] $commandLines[$run2_I] -options_file $optionsfile";
			}
			else{
				$comm = "mpirun -np $procs[$run2_I] ./$exec $xmlFile --include StgFEM/MultigridForRegular.xml --include pluginForMGTest.xml $commandLines[$run2_I] -options_file $optionsfile --pluginData.appendToAnalysisFile=True --pluginData.FrequentOutputReadFileName=./output/" . $xmlFile . "_MG/FrequentOutput.dat --pluginData.FrequentOutputDumpFileName=Underworld_" . $xmlFile . "-analysis.frequentout --outputPath=./output/" . $xmlFile . "_MG >$stdout";
				$command_ref->[$comm_index] = "mpirun -np $procs[$run2_I] $commandLines[$run2_I] -options_file $optionsfile";
			}
		}
		else{
# High Res solution run	
			if($paramFile eq " "){
				# use default mgLevels
				$comm = "mpirun -np $procs[$run2_I] ./$exec $xmlFile --include StgFEM/MultigridForRegular.xml --include pluginForMGTest.xml --mgLevels=$mgLevels[$run2_I] $commandLines[$run2_I] -options_file $optionsfile --pluginData.appendToAnalysisFile=True --pluginData.useHighResReferenceSolutionFromFile=True --pluginData.testTimestep=$testTimestep --pluginData.referenceSolutionFilePath=$refFilePath --pluginData.FrequentOutputReadFileName=./output/" . $xmlFile . "_MG/FrequentOutput.dat --pluginData.FrequentOutputDumpFileName=Underworld_" . $xmlFile . "-analysis.frequentout --outputPath=./output/" . $xmlFile . "_MG >$stdout";
				$command_ref->[$comm_index] = "mpirun -np $procs[$run2_I] --mgLevels=$mgLevels[$run2_I] $commandLines[$run2_I] -options_file $optionsfile";
			}
			else{
				$comm = "mpirun -np $procs[$run2_I] ./$exec $xmlFile --include StgFEM/MultigridForRegular.xml --include pluginForMGTest.xml $commandLines[$run2_I] -options_file $optionsfile --pluginData.appendToAnalysisFile=True --pluginData.useHighResReferenceSolutionFromFile=True --pluginData.testTimestep=$testTimestep --pluginData.referenceSolutionFilePath=$refFilePath --pluginData.FrequentOutputReadFileName=./output/" . $xmlFile . "_MG/FrequentOutput.dat --pluginData.FrequentOutputDumpFileName=Underworld_" . $xmlFile . "-analysis.frequentout --outputPath=./output/" . $xmlFile . "_MG >$stdout";
				$command_ref->[$comm_index] = "mpirun -np $procs[$run2_I] $commandLines[$run2_I] -options_file $optionsfile";
			}
		}
		print "$comm";
		$comm .= " 2>$stderr";
		`$comm`;
		
# check error stream for error result
		open( ERROR, "<$stderr" );
		my $line;
		foreach $line (<ERROR>) {
			if( $line =~ m/[E|e]rror/ ) {
				close(ERROR); 
#	$comm = "rm $stderr"; `$comm`;
				die ("\n\tError: see $stderr or $stdout - stopped" ); 
			}
		}
		close(ERROR); 
		$comm = "rm $stderr"; `$comm`;
		
		print " ....finished\n\n";
		
	}
	# removing softlink
	$comm = "rm $exec";
	print "$comm\n";
	`$comm`;
	print "--- Finished convergence runs ---\n\n";
	
	
	return $testsToRun;
}

sub readParamFile {
	my ( $paramFile, $procs, $commandLines ) = @_;
	my $line;
	# $line_I represents the number of tests to run
	my $line_I = 0;
	# open options file
	open PFILE, "<$paramFile" || die "Can't open param file $paramFile, stopped" ;
	foreach $line (<PFILE>) {
		chomp $line;
		# only process lines that start with np we are assuming the mgLevel param is set here
		if( $line =~ m/^np\s+(\d+)\s+(.*)/ ) {
			$procs->[$line_I] = $1;
			$commandLines->[$line_I] = $2;
			$line_I++;
		} else { next; }
	}
	return $line_I;
}

sub testNumbersAgainstExpected {
	my $datFile1 = $_[0];
	my $datFile2 = $_[1];
	my $expectedFile = $_[2];
	my $line;
	my $ii;
	my $needLabels = 1;
	my $jj;
	my @input1_1;
	my @input1_2;
	my @input1 = (@input1_1,@input1_2 );
	my @input2_1;
	my @input2_2;
	my @input2 = (@input2_1,@input2_2 );
	my @keysd;
	my @keyse;
	my @expected1;
	my @expected2;
	my @expected = (@expected1,@expected2 );
	my @array;
	
	#print "expectedFile is $expectedFile and datFile1 is $datFile1 and datFile2 is $datFile2\n"; 
# first gets the scalar input from the expected file:
# gets results from the expected convergence file and stores the input
	$jj=0;
	
	open EXPECTED, "<$expectedFile" || die "Can't open param file $expectedFile, stopped";
	foreach $line (<EXPECTED>) {
		if( $line =~ m/^\-\-\-\-\-\-\-\-\-/ ) {
# exit input if ------ is detected
			last;
		} 
		if( ($needLabels == 1) && $line=~ /#Res/ ) {
				@keyse = split(/\s+/, $line );
				$needLabels = 0;
		}
		if( $line =~ m/^\s*\D/ ) {
# disregard lines starting with non-Digits
			next;
		} 
		if( $line =~ m/^\s*\d/ ) {
			# split the line into vars
				@array = split(/\s+/, $line);
				
				for($ii = 0; $ii < scalar(@keyse); $ii++){
					$expected[$jj][$ii] = $array[$ii]; 
					#print "$array[$ii] $expected[$jj][$ii]\n";
				}
				$jj++;	
		}		
	}
	close(EXPECTED);
	
	$needLabels = 1;
	$jj = 0;
# then finds out if the expected file is a convergence file or a frequentoutput file:
	if($expectedFile =~ /convergence-rates/){
# opens the convergence rate file generated in the log dir and stores the input
		open( INPUT, "<$datFile1");
		print "\n--- Testing the convergence output against the expected file $expectedFile ---\n\n";
		
# get results from inputfile
		foreach $line (<INPUT>) {
			if( $line =~ m/^\-\-\-\-\-\-\-\-\-/ ) {
# exit input if ------ is detected
				last;
			} 
			if( ($needLabels == 1) && $line=~m/^#Res\s+(.*)/ ) {
				@keysd = split(/\s+/, $line );
				$needLabels = 0;
			}
			if( $line =~ m/^\s*\D/ ) {
# disregard lines starting with non-Digits
				next;
			} 
			if( $line =~ m/^\s*\d/ ) {
				# split the line into vars
				@array = split(/\s+/, $line);
				
				for($ii = 0; $ii < scalar(@keyse); $ii++){
					$input1[$jj][$ii] = $array[$ii]; 
					#print "$array[$ii] $input1[$jj][$ii]\n";
				}
				$jj++;	
			}
			
		}
		close(INPUT);
		
		
		if( scalar(@keysd) != scalar(@keyse) ) { 
			die "Error: The expected file \n\"$expectedFile\"\n".
			"has a different number of results than the test file \n\"$datFile1\"\n".
			"so the results testing can't be done.\n".
			"Remove argument '-againstExpected' to not check the resulting errors agains the expected file.\n"; 
		}
		
		my $error;
		my $r_error;
		my $r_tol = 0.05;
		my $numlines = $jj;
		print "relative tolerance is set to $r_tol%\n";
		for( $jj = 0 ; $jj < $numlines ; $jj++ ) {
			for( $ii = 1 ; $ii < scalar(@keyse) ; $ii++ ) {
				$error = abs( $input1[$jj][$ii] - $expected[$jj][$ii] );
				$r_error = 100 * ($error / $expected[$jj][$ii]);
				if( $r_error > $r_tol ) {
					die "Results of $keyse[$jj-1] differs by $r_error%\n";
				} 
# This is to verbose be could be useful to understand error
#else {
# print "Results of $keys[$jj-1] are $r_error\n";
# }
			}
		}
		print "All values within tolerance\n";
		print "\n-- Finished testing the output against the expected convergence file ---\n";
	}
	
	$jj = 0;
	$ii = 0;
	$needLabels = 1;
	if($expectedFile =~ /frequentout/){
		print "\n--- Testing the frequentoutput against the expected file $expectedFile ---\n\n";
		open( INPUT, "<$datFile2");
		
		
# get results from inputfile
		foreach $line (<INPUT>) {
			if( $line =~ m/^\-\-\-\-\-\-\-\-\-/ ) {
# exit input if ------ is detected
				last;
			} 
			if( ($needLabels == 1) && $line=~ /#Res/ ) {
				@keysd = split(/\s+/, $line );
				$needLabels = 0;
			}
			if( $line =~ m/^\s*\D/ ) {
# disregard lines starting with non-Digits
				next;
			} 
			if( $line =~ m/^\s*\d/ ) {
				
				# split the line into vars
				@array = split(/\s+/, $line);
				for($ii = 0; $ii < scalar(@keyse); $ii++){
					$input2[$jj][$ii] = $array[$ii]; 
					#print "$array[$ii] $input2[$jj][$ii]\n";
				}
				$jj++;	
			}
			
		}
		close(INPUT);
		
	
		
		if( scalar(@keysd) != scalar(@keyse) ) { 
			die "Error: The expected file \n\"$expectedFile\"\n".
			"has a different number of results than the test file \n\"$datFile2\"\n".
			"so the results testing can't be done.\n".
			"Remove argument '-againstExpected' to not check the resulting errors agains the expected file.\n"; 
		}
		
		
		my $error;
		my $r_error;
#might need to fix this up for the frequent outputs- r_tol should be less?
		my $r_tol = 10.0;
		my $numlines = $jj;
		print "relative tolerance is set to $r_tol%\n";
		for( $jj = 0 ; $jj < $numlines ; $jj++ ) {
			for( $ii = 1 ; $ii < scalar(@keyse) ; $ii++ ) {
				$error = abs( $input2[$jj][$ii] - $expected[$jj][$ii] );
				if($error == 0.0){
					$r_error = 0.0;
				}
				else{
					$r_error = 100 * ($error / $expected[$jj][$ii]);
				}
				if( $r_error > $r_tol ) {
					die "Results of $keyse[$jj-1] differs by $r_error%\n";
				} 
# This is to verbose be could be useful to understand error
#else {
# print "Results of $keys[$jj-1] are $r_error\n";
# }
			}
		}
		print "All values within tolerance\n";
		print "\n-- Finished testing the output against the expected frequentoutputs file ---\n";
	}

	
}

sub testOutputsAgainstEachOther {
	my ($xmlFile, $numOptFiles, @optionsFiles) = @_; 
	my @convFile;
	my @outputFile;
	my @filename;
	my $numvars;
	my $line;
	my $ii = 0;
	my $needLabels = 1;
	my $jj = 0;
	my $kk = 0;
	my @inputc1;
	my @inputc2;
	my @inputc = (@inputc1,@inputc2 );
	my @inputv1;
	my @inputv2;
	my @inputv3;
	my @inputv = (@inputv1,@inputv2,@inputv3 );
	my @keysc;
	my @keysv;
	my @expected;
	my @minIts1;
	my @minIts2;
	my @minIts = (@minIts1,@minIts2);
	my @bestconv;
	my @bestfreq1;
	my @bestfreq2;
	my @bestfreq = (@bestfreq1, @bestfreq2);
	my @maxconv;
	my $optionFile;
	my @file;
	
	# get the names of the covergence files and frequentOutput Files using the first entry in the optionsfiles array
	@file = split ('/',$optionsfiles[0]); 
	$optionFile = $file[-1];
	
	my $command1 = "ls log/*frequentout*$optionFile 2>/dev/null";
	#print "this is the command $command1\n";
	my $freqoutFile = `$command1`;
	my $command2 = "ls log/*analysis.cvg-convergence-rates*$optionFile 2>/dev/null";
	my $convergenceFile = `$command2`;

	print "$convergenceFile, $freqoutFile\n";
	

	# get the file name of the options file WITHOUT the directory:

	
	#then cut out the optionsfiles[0] part:
	$freqoutFile = substr($freqoutFile, 0, -(length($optionFile) + 2));
	$convergenceFile = substr($convergenceFile, 0, -(length($optionFile) + 2));
	
	
	my $bestFreqFile = "log/BestTimingAndIts_$xmlFile";
	my $bestConvFile = "log/BestConvergence_$xmlFile";
	#print "$freqoutFile, $convergenceFile\n";
	open( CONVFILE, ">$bestConvFile" ) || die "Can't open '$bestConvFile': $!\n";
	open( OUTFILE, ">$bestFreqFile" ) || die "Can't open '$bestFreqFile': $!\n";
	
	print "Output files are $bestConvFile and $bestFreqFile\n";
	
				
	my @array;
	 for($ii = 0; $ii < $numOptFiles; $ii++){
		@file = split ('/',$optionsfiles[$ii]); 
		$optionFile = $file[-1];
		 $convFile[$ii] = "$convergenceFile-$optionFile";
		 $outputFile[$ii] = "$freqoutFile-$optionFile"; 
	 }
	 
	 for ($kk = 0; $kk < $numOptFiles; $kk++){
		 open INPUT, "<$convFile[$kk]" || die "Can't open file $convFile[$kk], stopped";
		 print "\n--- Comparing the convergence output for file $convFile[$kk] ---\n\n";
		
# get results from inputfile
		 foreach $line (<INPUT>) {
			 
			 if( ($needLabels == 1) && $line=~ /#Res/ ) {
				 #print "$line\n";
				 chomp $line;
				 @keysc = split (/ /, $line);
				 shift(@keysc);
				 if($kk == 0){
					$numvars = scalar(@keysc);
					#print "$numvars\n";
				 }
				  # first check if the number of variables of the next file matches the number of variables of the first file 
				if($kk > 0){
					if(scalar(@keysc) != $numvars){
						die "number of variables incompatible with 1st conv file can not compare\n";
					}
				}
			 }
			if($line =~ /cvg/){
				# get rid of the cvg. rate from the beginning of the line:
				$line = substr($line, 12);
				chomp $line;
				#print "$line\n";
				# split the line into vars
				@array = split(/\s+/, $line);
				#print "$array[0] $array[1] $array[2] $array[3] $numvars\n";
				for($ii = 0; $ii < $numvars; $ii++){
					$inputc[$kk][$ii] = $array[$ii]; 
					#print "$array[$ii] $inputc[$kk][$ii]\n";
				}
			}
		 }
		 close(INPUT);
		 
	 }
	 $ii = 0;
	 $jj = 0;
	 my $i = 0;
	 my $j = 0;
	 my $res = 0;
	 my $beenhere = 0;
	 my @freqoutindex;
	 my $needlabels = 1;
	 for ($kk = 0; $kk < $numOptFiles; $kk++){
		 open INPUT, "<$outputFile[$kk]" || die "Can't open file $outputFile[$kk], stopped";
		 print "\n--- Testing the frequent output vars for file $outputFile[$kk] ---\n\n";
		 #reset res here
		 $res = 0;
		# get results from inputfile
		 foreach $line (<INPUT>) {
			 if( ($needLabels == 1) && $line=~m/^#Res#\s+(.*)/ ) {
				 @keysv = split(/\s+/, $line );
				 if($kk == 0){
					 $numvars = scalar(@keysv);
					 if($i == 0){
# want to get the index for anything to do with Its
						 for($jj = 0; $jj < scalar(@keysv); $jj++){
							 if (	
									($keysv[$jj] =~ /Its/)
									||
									($keysv[$jj] =~ /CPU/)
									){
								 $freqoutindex[$j] = $jj;
								 $j++;
							 }
						 }
					 }
					 #find out how many resolutions
					$i++;
				 }
				 if($kk > 0){
					 if($beenhere == 0){
						$res = 0;
						$beenhere = 1;
					 }
					 if(scalar(@keysv) != $numvars){
						 die "number of variables incompatible with 1st freqout file can not compare\n";
					 }
				 }
				 
			 }
			 if( $line =~ m/^\s*\D/ ) {
                # disregard lines starting with non-Digits
				 next;
			 } 
			 if( $line =~ m/^\s*\d/ ) {
				 chomp $line;
				 @array = split(/\s+/, $line);
				 for($ii = 0; $ii < $numvars; $ii++){
					#where i stands for each res
					$inputv[$kk][$res][$ii] = $array[$ii];
					#print "$kk $res $ii $inputv[$kk][$res][$ii]\n";
				 }
				 $res++;
			 }
		 } 
		 close(INPUT);
		 
	 }
	my $maxval = $j - 1;
	my @commentc;
	my @commentk;
	my @equalcountc;
	#now want to find min convergence and the smallest CPU times and number of its for inner outer and in some cases non linear 
	 #1st start with conv:
	 
	 # for each variable
	 for($ii = 0; $ii < scalar(@keysc); $ii++){
		$commentc[$ii] = "";
		# for each file
		$maxconv[$ii] = $inputc[0][$ii];
		$bestconv[$ii] = 0;
		# reset equalcount;
		$equalcountc[$ii] = 0;
		for($kk = 0; $kk < $numOptFiles; $kk++){
			#print "$inputc[$kk][$ii]\n";
			if($inputc[$kk][$ii] > $maxconv[$ii]){
				$maxconv[$ii] = $inputc[$kk][$ii];
				$bestconv[$ii] = $kk;
			}
			elsif(($inputc[$kk][$ii] = $maxconv[$ii])){
				$commentk[$ii] = $kk;
				if($commentc[$ii] ne ""){
					$commentc[$ii] = join "", $commentc[$ii], " and ", $optionsfiles[$kk]; 
				}
				else{
					$commentc[$ii] = join "", $commentc[$ii], $optionsfiles[$kk]; 
				}
				$equalcountc[$ii]++;
			}
		}	
		# if the maxconv still equals the first entry and the first entry is not greater than the 2nd, then set max conv to 0
		if(
			($maxconv[$ii] == $inputc[0][$ii])
			&&
			($inputc[0][$ii] == $inputc[1][$ii])
		){
			$maxconv[$ii] = 0.0;
			$bestconv[$ii] = 0;
		}
	 }
	 
	 # print best conv
	for($ii = 0; $ii < scalar(@keysc); $ii++){
		if($maxconv[$ii] == 0.0){
			 print CONVFILE  "No best convergence found for $keysc[$ii], $inputc[0][$ii]"; 
			if(($commentc[$ii] ne "") && ($equalcountc[$ii] > 1)){
				 print CONVFILE  "convergence for these option files: $commentc[$ii] are all equal \n";
			}
		}
		else{
			 print CONVFILE  "Best conv for $keysc[$ii] is $optionsfiles[$bestconv[$ii]]\n";
			if(($commentc[$ii] ne "") && ($equalcountc[$ii] > 1)){
				 print CONVFILE  "and convergence for these option files: $commentc[$ii] are equal $inputc[$commentk[$ii]][$ii]\n\n";
			}

		}
	}
	 print CONVFILE "\n\n";
#2nd freqouts:
	my $numRes = $i-1;
	my $k_prev = 0;
	my @commentr;
	my @commentv;
	my @commentf = (@commentr, @commentv);
	my @equalcountr;
	my @equalcountv;
	my @equalcountf = (@equalcountr, @equalcountv);
	 #print "how many res is $i and maxval is $maxval\n";
# for each res
	 for($jj = 0; $jj < $numRes; $jj++){
		# reset the j var:
		$j = 0;
		
		# for each variable:
		  
		 for($ii = 0; $ii < scalar(@keysv); $ii++){  
				$commentf[$jj][$ii] = "";
				# set the minIts for each variable at each res:
				$minIts[$jj][$ii] = $inputv[0][$jj][$ii];
				# set the bestfreq to the first k
				$bestfreq[$jj][$ii] = 0;
				# reset equalcount for each res:
				$equalcountf[$jj][$ii] = 0;
				
				# for each file
				# reset the k_prev			
				$k_prev = 0;
				for($kk = 0; $kk < $numOptFiles; $kk++){
			      #print "maxval is $maxval kk is $kk j is $j and ii is $ii freqout[$j] is $freqoutindex[$j] and numOptFiles is $numOptFiles\n";
# if at the right index in the input:
				 if(
						($ii == $freqoutindex[$j])
						&&
						($j < $maxval)
					){
					 #print "kk is $kk j $j freqout[j] = ii $ii jj $jj kk $kk k_prev is $k_prev minIts $minIts[$jj][$ii] inputv[kk][jj][ii] $inputv[$kk][$jj][$ii]\n";
					 if($inputv[$kk][$jj][$ii] < $minIts[$jj][$ii]){
						 $minIts[$jj][$ii] = $inputv[$kk][$jj][$ii];
						 $bestfreq[$jj][$ii] = $kk;
						 #print "keysv[$ii] minimum is $minIts[$jj][$ii] for res $jj and options file is $optionsfiles[$kk]\n";
					 }
					 elsif(($inputv[$kk][$jj][$ii] == $minIts[$jj][$ii])){
						if($commentf[$jj][$ii] ne ""){
							$commentf[$jj][$ii] = join "", $commentf[$jj][$ii], " and ", $optionsfiles[$kk];
						}
						else{
							$commentf[$jj][$ii] = join "", $commentf[$jj][$ii], $optionsfiles[$kk];
						} 
						$equalcountf[$jj][$ii]++;
					}

					if($k_prev != $kk){
							$k_prev = $kk;
					}	
					# increment $j if $ii = $freqoutindex[$j]
					if(
						($ii == $freqoutindex[$j])
						&&
						($kk == $numOptFiles -1) 
					){
						$j++;
					}
				 }
			 }
# if the minIts still equals the first entry and the first entry is not less than the 2nd, then set minIts to 0 and bestfreq also to 0
			 if(
				($minIts[$jj][$ii] == $inputv[0][$jj][$ii])
				&&
				($inputv[0][$jj][$ii] == $inputv[1][$jj][$ii])
				){
				 $minIts[$jj][$ii] = 0.0;
				 $bestfreq[$jj][$ii] = -1;
			 }
		 }	
		 
	 }
	 
	 
# print best frequent output for each res:
	for($jj = 0; $jj < $numRes; $jj++){
		$j = 0;
		for($ii = 0; $ii < scalar(@keysv); $ii++){
			if(
				($ii == $freqoutindex[$j])
				&&
				($j < $maxval)
			){
				if($bestfreq[$jj][$ii] != -1){
					 print OUTFILE "Best $keysv[$ii] for res $inputv[0][$jj][0] is $optionsfiles[$bestfreq[$jj][$ii]]\n";
					if(($commentf[$jj][$ii] ne "") && ($equalcountf[$jj][$ii] > 1)){
						 print OUTFILE "and for $keysv[$ii] these option files: $commentf[$jj][$ii] are equal\n\n";
					}

				}
				else{
					 print OUTFILE "No best $keysv[$ii] for res $inputv[0][$jj][0], ";
					if(($commentf[$jj][$ii] ne "") && ($equalcountf[$jj][$ii] > 1)){
						 print OUTFILE "for $keysv[$ii] these option files: $commentf[$jj][$ii] are equal\n";
					}
				}
				$j++;
			}
			
		}
		 print OUTFILE "\n\n";
	}
	 print OUTFILE "\n\n";
}


sub generateConvergence {
	# check if the cvg can be found
	my $command1 = "ls *\.cvg 2>/dev/null";
	my $command2 = "ls *\.frequentout 2>/dev/null";
	my $command;
	my $datFile;
	my $expectedFile;
	my $cvgFile = `$command1`;
	my $freqoutFile = `$command2`;
	chomp $cvgFile;
	chomp $freqoutFile;
	print "cvgFile is $cvgFile and freqoutFile is $freqoutFile\n";
	if( $cvgFile eq "" ) { die "There is no cvg file found analyse\n" ; }
	if( $freqoutFile eq "" ) { die "There is no frequentoutput file found analyse\n" ; }
	# run David A. May's convergence tester, cheers Mayhem
	$command = "./generate-convergence-rates.pl -errorfile $cvgFile";
	`$command`;

	$datFile = "$cvgFile\-convergence\-rates.dat";
	if( !(-e $datFile) ) { die "Can't find the convergence rate file: $datFile\n"; }
	#print "frequout file is $freqoutFile\n";
	# search ARGV for againstExpected flag
	my $arg;
	my $runAgainstExpected = 0;
	
	my $ii = 0;
	foreach $arg (@ARGV) {
		if( $arg =~ m/\-againstExpected/ ) { 
				
				$expectedFile = $ARGV[$ii+1];
				$runAgainstExpected = 1;
		}
		$ii++;
	}


	if( $runAgainstExpected ) {
	if( -e "./expected/MG/$expectedFile" ) { 
			&testNumbersAgainstExpected($datFile, $freqoutFile, "./expected/MG/$expectedFile");
		}	else { 
			die "Can't find the expected file: ./expected/$expectedFile\n". 
			"Consider adding argument '-convergeOnly' to the executable to not check the resulting errors agains the expected file.\n"; 
		}
	}
	
	# remove the old file
	$command = "rm $cvgFile";
	`$command`;
	# don't need to do this as it has been moved
	# remove the old freqoutFile:
	#$command = "rm $freqoutFile";
	#`$command`;

	return $datFile;
}


sub testConvergence {
	my ($optionsFile, $datFile) = @_;
	my $command;
	my @keys;
	my @cvgRates;
	my @correlations;
	my $needLabels = 1;
	my $line;
	 #test convergence numbers
	open(INPUT, "<$datFile") || die "Can't open the expected file $datFile\n" ; 
	while ($line = <INPUT>) {
		chomp $line;
    # parse for convergence rates
		if ( $line =~ m/^cvg\.\srate\s+(.*)/ ) {
			@cvgRates = split (/\s+/, $1 );
		}
    # parse for correlation results
		elsif( $line =~ m/^corr\.\s+(.*)/ ) {
			@correlations = split (/\s+/, $1 );
		}
    # parse for variable labels
		elsif( ($needLabels == 1) && $line=~m/^#Res\s+(.*)/ ) {
			@keys = split(/\s+/, $1 );
			$needLabels = 0;
		}
	}

  # now check results and report
	my $ii;
	my $jj;
	my $length;
	my $status;
	my $result = "Pass";
	my $report;
	my $padding;
	print "--- Results of convergence ---\n\n";
	for( $ii = 0 ; $ii < scalar(@correlations) ; $ii++ ) {
		$status = "good";
		$report = "";
		$padding = "";

		$length = 25-length($keys[$ii]); 
		for( $jj = 0 ; $jj < $length ; $jj++ ) {
			$padding .= " ";
		}

		print "$keys[$ii] $padding- convergence rate = $cvgRates[$ii] - correlation $correlations[$ii] .... ";
		if( $correlations[$ii] < 0.99 ) {
			$status = "BAD!!!";
			$result = "Fail";
			$report = $report."\thas a correlation of $correlations[$ii], below the accepted threshold of 0.99\n";
		}
		if( $keys[$ii] =~ m/^Velocity/ ) {
			if( $cvgRates[$ii] < 1.6 ) {
				$status = "BAD!!!";
				$result = "Fail";
				$report = $report."\thas a convergence rate of $cvgRates[$ii], theoretically velocity fields should converge at a rate ~ 2\n";
			}
		} elsif ($keys[$ii] =~ m/Pressure/ ) {
			if( $cvgRates[$ii] < 0.9 ) {
				$status = "BAD!!!";
				$result = "Fail";
				$report = $report."\thas a convergence rate of $cvgRates[$ii], theoretically pressure fields should converge at a rate ~ 1\n";
			}
		} elsif ($keys[$ii] =~ m/StrainRate/ ) {
				if( $cvgRates[$ii] < 0.85 ) {
					$status = "BAD!!!";
					$result = "Fail";
					$report = $report."\thas a convergence rate of $cvgRates[$ii], theoretically pressure fields should converge at a rate ~ 1\n";
				}
		}
		print "$status\n$report";
	}

	close( INPUT );
	my @option;
	# get the file name of the options file WITHOUT the directory:
	# @LIST = split(/PATTERN/, STRING, LIMIT);
	@option = split ('/',$optionsFile); 
	$optionsFile = $option[-1];	
	#print "$datFile is to be moved\n";
	$command = "mv $datFile log/$datFile-MG-$optionsFile";
	`$command`;
	
	#mv the freqout file as well:
	$command = "ls *\.frequentout 2>/dev/null";
	my $freqoutFile = `$command`;
	# get the file name of the options file WITHOUT the directory:
	# @LIST = split(/PATTERN/, STRING, LIMIT);
	@option = split ('/',$optionsFile); 
	$optionsFile = $option[-1];	
	chomp $freqoutFile;
	#print "$freqoutFile is to be moved\n";
	$command = "mv $freqoutFile log/$freqoutFile-MG-$optionsFile";
	`$command`;

	print "\nResult = $result\n";
}


sub postProcessFunc {
	my ($xmlFile, $numberOfOptFiles, $testsToRun, $optionsfiles_ref, $command_ref ) = @_;	# read
	my $runFile = $xmlFile;
	my @param;
	my @param2;
	my @np;
	my @mgLevels;
	my @resI;
	my @resJ;
	my @resK;
	my $resKinc;
	my $i;
	my $j;
	my $maxp;
	my $line;
	my $parFile;
	my $counter;
	my $total = $numberOfOptFiles*$testsToRun;
	
	$j = 0;
	$resKinc = "no";
	
	print "$numberOfOptFiles, $testsToRun, $total\n";
	
	
	
#first store the params for each run in an array:
	for ($counter = 0; $counter < $total; $counter++){
		$line = $command_ref->[$counter];	
		#print "$line\n";
		@param = split(/ /, $line);
		$maxp = scalar(@param);
		for ($i = 0; $i < $maxp; $i++){
			if($param[$i] =~ m/np/){
				$np[$j] = $param[$i+1];
			}
			if($param[$i] =~ m/mgLevels/){
				@param2 = split(/=/, $param[$i]);
#only 2 parts to the split
				$mgLevels[$j] = $param2[1];
			}
			if($param[$i] =~ m/elementResI/){
				@param2 = split(/=/, $param[$i]);
#only 2 parts to the split
				$resI[$j] = $param2[1];
			}
			if($param[$i] =~ m/elementResJ/){
				@param2 = split(/=/, $param[$i]);
#only 2 parts to the split
				$resJ[$j] = $param2[1];
			}
			if($param[$i] =~ m/elementResK/){
				$resKinc = "yes";
				@param2 = split(/=/, $param[$i]);
#only 2 parts to the split
				$resK[$j] = $param2[1];
			}
		}
		$j++;
		
	}	
	
	
	
	
	my $maxruns = scalar(@np);
	print "maxruns $maxruns\n";
	my $maxparams;
	
	if($resKinc eq "yes"){
		$maxparams = 5;
	}
	else{
		$maxparams = 4;
	}
	
	my @ptablecol;
	my @ptablerow;
	my @ptable = (@ptablerow, @ptablecol);
	
	$ptable[0][0] = "$runFile";
	
	print "maxparams $maxparams\n";
	
#then create the table:
	$i = 1;
	my $jprev = 1;
#row is $j
	
	for($j = 0; $j < $maxruns; $j++){
#column is i
		if($jprev != $j){
			$i = 1;
			$jprev = $j;
			$ptable[$j+1][0] = "run_$j";
		}
#np
		if(($i == 1) && scalar(@np) == $maxruns){
			$ptable[0][$i] = "np";
			$ptable[$j+1][$i] = $np[$j];
			
			$i++;
		}
		else{
			$i++;
		}
#mgLevels
		if($i == 2 && scalar(@mgLevels) == $maxruns){
			$ptable[0][$i] = "mgLevels";
			$ptable[$j+1][$i] = $mgLevels[$j];
			$i++;
		}
		else{
			$i++;
		}
#resI
		if($i == 3 && scalar(@resI) == $maxruns){
			$ptable[0][$i] = "resI";
			$ptable[$j+1][$i] = $resI[$j];
			$i++;
		}
		else{
			$i++;
		}
#resJ
		if($i == 4 && scalar(@resJ) == $maxruns){
			$ptable[0][$i] = "resJ";
			$ptable[$j+1][$i] = $resJ[$j];
			$i++;
		}
		else{
			$i++;
		}
#resK
		if($i == 5 && scalar(@resK) == $maxruns){
			$ptable[$i][0] = "resK";
			$ptable[$j+1][$i] = $resK[$j];
			$i++;
		}
		else{
			$i++;
		}
	}
	
#need option file array
	
#maxoptfiles = option file array length
	
# read thru the option files:
	
# first find the commonalities in the option files
	
# then find the differences for each ksp in the option files
	my @rtol;
	my @ksp_type;
	my @pc_type;
	my @pc_sub_type;
	my @smoothup;
	my @smoothdown;
	my @smooths_min;
	my @smooths_max;
	my @smooths_to_start;
	my @smoothing_accel;
	my @smoothing_inc;
	my @reduction;
	my $maxoptfiles;
	my $Ofile;
	my $a11;
	

	
	$maxoptfiles = $numberOfOptFiles;
	
	for($i = 0; $i < $maxoptfiles; $i++){
		open $Ofile, "<$optionsfiles_ref->[$i]" || die "Can't open param file $optionsfiles_ref->[$i], stopped" ;
		foreach $line (<$Ofile>){
# if ksp label contains A11 store in 2D A11 option array
			if( ($line =~ m/\-A11/) && !($line =~ m/\#/) ) {
				chomp($line);
				$a11 = 'yes';
#if rtol
				if( $line =~ m/\_rtol/ ){
					@param2 = split(/ /, $line);
#only 2 parts to the split
					$rtol[$i] = $param2[1]; 
				}
#if ksp_type
				if( $line =~ m/\_ksp_type/ ){
					@param2 = split(/ /, $line);
#only 2 parts to the split
					$ksp_type[$i] = $param2[1];
				}
#if pc_type
				if( $line =~ m/\_pc_type/ ){
					@param2 = split(/ /, $line);
#only 2 parts to the split
					$pc_type[$i] = $param2[1];
				}
#if pc_sub_type
				if( $line =~ m/\_pc_sub_type/ ){
					@param2 = split(/ /, $line);
#only 2 parts to the split
					$pc_sub_type[$i] = $param2[1];
				}
#if smoothup
				if( $line =~ m/\_smoothup/ ){
					@param2 = split(/ /, $line);
#only 2 parts to the split
					$smoothup[$i] = $param2[1];
				}
#if smoothdown
				if( $line =~ m/\_smoothdown/ ){
					@param2 = split(/ /, $line);
#only 2 parts to the split
					$smoothdown[$i] = $param2[1];
				}
#if accelerating:
#if smooths_min
				if( $line =~ m/\_smooths_min/ ){
					@param2 = split(/ /, $line);
#only 2 parts to the split
					$smooths_min[$i] = $param2[1];
				}
#if smooths_max
				if( $line =~ m/\_smooths_max/ ){
					@param2 = split(/ /, $line);
#only 2 parts to the split
					$smooths_max[$i] = $param2[1];
				}
#if smooths_to_start
				if( $line =~ m/\_smooths_to_start/ ){
					@param2 = split(/ /, $line);
#only 2 parts to the split
					$smooths_to_start[$i] = $param2[1];
				}
#if smoothing_acceleration 
				if( $line =~ m/\_smoothing_acceleration/ ){
					@param2 = split(/ /, $line);
#only 2 parts to the split
					$smoothing_accel[$i] = $param2[1];
				}
#if smoothing_increment
				if( $line =~ m/\_smoothing_inc/ ){
					@param2 = split(/ /, $line);
#only 2 parts to the split
					$smoothing_inc[$i] = $param2[1];
				}
#if target_cycles_10fold_reduction
				if( $line =~ m/\_10fold_/ ){
					@param2 = split(/ /, $line);
#only 2 parts to the split
					$reduction[$i] = $param2[1];
				}
			}
		}
		close($Ofile);
	}
	
	
	my @optFileNameArray;
	for($i = 0; $i < $maxoptfiles; $i++){
		$line = $optionsfiles_ref->[$i];
		chomp($line);
		@param2 = split(/\//, $line);
		$optFileNameArray[$i] = $param2[1];
	}



	my @otablecol;
	my @otablerow;
	my @otable = (@otablerow, @otablecol);
	my $maxoptfileparams;
	#then create the table:
	$otable[0][0] = "$runFile";
	$i = 1;
	$jprev = 1;
	print "maxoptfiles is $maxoptfiles\n";
	for($j = 0; $j < $maxoptfiles; $j++){
	#column is i
		if($jprev != $j){
			$i = 1;
			$jprev = $j;
			$otable[$j+1][0] = "$optFileNameArray[$j]";
		}
	#rtol
		if(($i == 1) && ($rtol[$j])){
			$otable[0][$i] = "rtol";
			$otable[$j+1][$i] = $rtol[$j];
			$i++;
		}
		else{
			$otable[0][$i] = "rtol";
			$otable[$j+1][$i] = "NA";
			$i++;
		}
	#ksp_type
		if(($i == 2) && ($ksp_type[$j])){
			$otable[0][$i] = "ksp_type";
			$otable[$j+1][$i] = $ksp_type[$j];
			$i++;
		}
		else{
			$otable[0][$i] = "ksp_type";
			$otable[$j+1][$i] = "NA";
			$i++;
		}
	#pc_type
		if(($i == 3) && ($pc_type[$j])){
			$otable[0][$i] = "pc_type";
			$otable[$j+1][$i] = $pc_type[$j];
			$i++;
		}
		else{
			$otable[0][$i] = "pc_type";
			$otable[$j+1][$i] = "NA";
			$i++;
		}
	#pc_sub_type
		if(($i == 4) && ($pc_sub_type[$j])){
			$otable[0][$i] = "pc_sub_type";
			$otable[$j+1][$i] = $pc_sub_type[$j];
			$i++;
		}
		else{
			$otable[0][$i] = "pc_sub_type";
			$otable[$j+1][$i] = "NA";
			$i++;
		}
	#smoothup
		if(($i == 5) && ($smoothup[$j])){
			$otable[0][$i] = "smoothup/min,max,start";
			$otable[$j+1][$i] = $smoothup[$j];
			$i++;
		}
	#accelerating
		elsif(($i == 5) && ($smooths_min[$j])){
			$otable[$j+1][$i] = "$smooths_min[$j],$smooths_max[$j],$smooths_to_start[$j]";
			$i++;
		}
		else{
			$otable[$i][0] = "smoothup";
			$otable[$j+1][$i] = "NA";
			$i++;
		}
	#smoothdown
		if(($i == 6) && ($smoothdown[$j])){
			$otable[0][$i] = "smoothdown/accel,inc,red";
			$otable[$j+1][$i] = $smoothdown[$j];
			$i++;
		}
	#accelerating
		elsif($i == 6 && ($smooths_min[$j])){
			$otable[$j+1][$i] = "$smoothing_accel[$j],$smoothing_inc[$j],$reduction[$j]";
			$i++;
		}
		else{
			$otable[0][$i] = "smoothdown";
			$otable[$j+1][$i] = "NA";
		}
	}

	$maxoptfileparams = $i;

	my $outputFile = "log/graph_and_table_file_MG.txt";
	my $outFile;

	open $outFile, ">$outputFile" || die "Can't open specified option file $outputFile, stopped" ;
	#print param table to file
	for($i = 0; $i < ($testsToRun + 1); $i++){
		for($j = 0; $j < ($maxparams + 1); $j++){
			print $outFile "$ptable[$i][$j]	";
		}
		print $outFile "\n";
	}

	print $outFile "\n\n";
	print "maxoptfileparams $maxoptfileparams\n";


	#print option table to file
	for($i = 0; $i <= $maxoptfiles; $i++){
		for($j = 0; $j < $maxoptfileparams; $j++){
			print $outFile "$otable[$i][$j]	";
		}
		print $outFile "\n";
	}

	print $outFile "\n\n";

	print "finished param and optfile table\n";

	# if ksp = A11, conv field var = vel1field and vel2field, timing var = innerCPUTime, its var = inner its.
	#convfieldarray
	# then make a table of the frequent output vars that are affected by the options for that ksp

	# then make a table of the convergence vars that are affected by the options for that ksp
	my @convtablecol;
	my @convtablerow;
	my @convtable = (@convtablerow, @convtablecol);
	my @freqouttablecol1;
	my @freqouttablerow1;
	my @freqouttable1 = (@freqouttablerow1, @freqouttablecol1);
	my @freqouttablecol2;
	my @freqouttablerow2;
	my @freqouttable2 = (@freqouttablerow2, @freqouttablecol2);
	my $velfieldmax;

	$convtable[0][0] = "Convergence";
	$convtable[1][0] = "velocity Field 1";
	$convtable[2][0] = "velocity Field 2";
	
	$velfieldmax = 2;
	
	my $logcFile;
	my $logfFile;
	my $iprev = -1;
	
	#now find the cvg file names in the log dir
	my $command;
	my $cvgname;
	my @cvgarray;
	my $length;
	my $cvgfile;
	my $freqname;
	my @freqarray;
	my $freqfile;
	
	$command = "ls log/*cvg*MG-$optFileNameArray[0]";
	$cvgname = `$command`;
	
	
	
	@cvgarray = split(/-/, $cvgname);
	$length = scalar(@cvgarray);
	
	for($i = 0; $i < ($length - 1); $i++){
		if($i == 0){
			$cvgfile = $cvgarray[$i];
		}
		else{
			$cvgfile = $cvgfile . "-" . $cvgarray[$i];
		}
	}
	
	#print "$cvgfile\n";
	
	$command = "ls log/*frequentout*MG-$optFileNameArray[0]";
	$freqname = `$command`;
	
	
	@freqarray = split(/-/, $freqname);
	$length = scalar(@freqarray);
	
	for($i = 0; $i < ($length - 1); $i++){
		if($i == 0){
			$freqfile = $freqarray[$i];
		}
		else{
			$freqfile = $freqfile . "-" . $freqarray[$i];
		}
	}
	
	#print "$freqfile\n";
	
	for($i = 0; $i < $maxoptfiles; $i++){
		#print "$cvgfile-$optFileNameArray[$i]\n";
		open $logcFile, "<$cvgfile-$optFileNameArray[$i]" || die "Can't open specified file $cvgfile-$optFileNameArray[$i], stopped";
		if($iprev != $i){
			$j = 0;
			$iprev = $i;
			$convtable[0][$i + 1] = "$optFileNameArray[$i]";
		}
		foreach $line (<$logcFile>){
			if($line =~ m/cvg/){
				chomp($line);
				@param2 = split(" ", $line);
				#print "$param2[0], $param2[1], $param2[2], $param2[3], $param2[4]\n";
				$convtable[1][$i + 1] = $param2[2];
				$convtable[2][$i + 1] = $param2[3];
				#print "$convtable[1][$i+1] and $convtable[2][$i+1]\n";
			}
		}
		close($logcFile);
	}




	$freqouttable1[0][0] = "AvgCPUtime4innerits";

	$freqouttable2[0][0] = "Avg#ofinnerits";

	for($i = 0; $i < $testsToRun; $i++){
		$freqouttable1[$i+1][0] = "run_$i";
		$freqouttable2[$i+1][0] = "run_$i";
	}

	$j = 0;
	my $k = 0;
	my $innerCpu;
	my $innerIts;

	for($i = 0; $i < $maxoptfiles; $i++){
		open $logfFile, "<$freqfile-$optFileNameArray[$i]" || die "Can't open specified file log/Underworld_testVelicSolCx_Mine.xml-analysis.frequentout-MG-$optFileNameArray[$i], stopped" ;
		foreach $line (<$logfFile>){
			if($line =~ /#Res#/){
				@param2 = split(" ", $line);
				for($j = 0; $j < scalar(@param2); $j++){
					if($param2[$j] =~ /AvgCPUInner/){
						$innerCpu = $j;
						#print "innerCpu is $innerCpu\n";
					} 
					if($param2[$j] =~ /AvgInIts/){
						$k = $j;
						$innerIts = $k;
						#print "innerIts is $innerIts\n";
					} 
				}
			}
		}
		close($logfFile);
	}

	$j = 0;
	$iprev = -1;

	for($i = 0; $i < $maxoptfiles; $i++){
		open $logfFile, "<$freqfile-$optFileNameArray[$i]" || die "Can't open specified file log/Underworld_testVelicSolCx_Mine.xml-analysis.frequentout-MG-$optFileNameArray[$i], stopped" ;
		if($iprev != $i){
			$j = 0;
			$iprev = $i;
			$freqouttable1[0][$i + 1] = "$optFileNameArray[$i]";
			$freqouttable2[0][$i + 1] = "$optFileNameArray[$i]";
		}
		foreach $line (<$logfFile>){
			if($line ne "\n"){
				@param2 = split(" ", $line);
				#print "param2[0] is $param2[0]\n";
				if(($param2[0] ne "#Res#")){
					#print "j is $j\n";
					$freqouttable1[$j+1][$i + 1] = $param2[$innerCpu];
					$freqouttable2[$j+1][$i + 1] = $param2[$innerIts];
					#print "freqouttable $j+1, $i+1 $freqouttable1[$j+1][$i+1] $freqouttable2[$j+1][$i+1]\n";
					$j++;
				}
			}
		}
		close($logfFile);
	}

	#if lable = scr

	# if ksp = scr, conv field var = pressure, timing var = outerCPUTime, its var = outer its


	for($i = 0; $i <= $testsToRun; $i++){
		for($j = 0; $j <= $maxoptfiles; $j++){
			print $outFile "$freqouttable1[$i][$j]	";
		}
		print $outFile "\n";
	}
	print $outFile "\n\n";

	for($i = 0; $i <= $testsToRun; $i++){
		for($j = 0; $j <= $maxoptfiles; $j++){
			print $outFile "$freqouttable2[$i][$j]	";
		}
		print $outFile "\n";
	}

	print $outFile "\n\n";
	
	for($i = 0; $i <= $velfieldmax; $i++){
		for($j = 0; $j <= $maxoptfiles; $j++){
			print $outFile "$convtable[$i][$j]	";
		}
		print $outFile "\n";
	}

	print $outFile "\n\n";

	close($outFile);

	# run graph plotter that plots the Avg CPU time for inner Its:
	my	$command = "python post_process_graphs.py";
	`$command`;

	# plot the graph of freq outputs and plot the graph of conv. vars by calling the script :plotarrays.py

}
