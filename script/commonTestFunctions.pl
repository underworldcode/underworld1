# Common testing functions used by the various testing scripts.

#Pass in the order @ARGV, then helpStr, then input/output list containing: xmlFile[2], optFile[3], createTest[4], numberOfTimeSteps[5], outputPath[6], nProcs[7], isSerial[8], useCM[9], tol[10]
sub readCommandLineOptions {
	my @argList = @{$_[0]};
	my @helpStr = $_[1];
	my $arg;
	my $ii = 0;
	

	# check if xml exists and options file is specified
	for( $ii = 0 ; $ii < scalar(@argList) ; $ii++ ) {
		$arg = $argList[$ii];
		if( $arg =~ m/.*\.xml$/ ) { $_[2] = $arg; }
		elsif( $arg =~ m/\-optionsFile/ ) { $_[3] = $argList[$ii+1]; $ii++; }
		elsif( $arg =~ m/^\-h/ ) { print $helpStr; exit }
		elsif( $arg =~ m/^\-\-help/ ) { print $helpStr; exit }
		elsif( $arg =~ m/^\-c/ ) { $_[4]=1; }
		elsif( $arg =~ m/^\-n$/ ) { $_[5]=$argList[$ii+1]; $ii++; }
		elsif( $arg =~ m/^\-D/ ) { $_[6]=$argList[$ii+1]; $ii++; }
		elsif( $arg =~ m/^\-np/ ) { $_[7]=$argList[$ii+1]; $ii++; }
		elsif( $arg =~ m/^\-serial/ ) { $_[8]=1; }
		elsif( $arg =~ m/^\-useCM/ ) { $_[9]=1; }
		elsif( $arg =~ m/^\-tol/ ) { $_[10]=$argList[$ii+1]; $ii++ }
	}

	print "\n--- Specified command-line options:---\n";
	print "xmlFile = $_[2], optionsFile = $_[3], createTest=$_[4],\n";
	print "numberOfTimeSteps=$_[5], outputPath=$_[6], nProcs = $_[7], isSerial = $_[8],\n";
	print "useCM = $_[9], tol= $_[10]\n";
}

sub readOptionsFile {
	my ( $optFile, $procs, $commandLines ) = @_;
	my $line;
	# $line_I represents the number of tests to run
	my $line_I = 0;
	# open options file
	open OPTFILE, "<$optFile" || die "Can't open options file $optFile, stopped" ;
	foreach $line (<OPTFILE>) {
		chomp $line;
		# only process lines that start with np
		if( $line =~ m/^np\s+(\d+)(\s*$|\s+(.*))/ ) {
			$procs->[$line_I] = $1;
			$commandLines->[$line_I] = $2;
			$line_I++;
		} else { next; }
	}
	return $line_I;
}

sub testConvergence {
 	my $datFile = $_[0]; 
	my @keys;
	my $tolerance = $_[1]; 
	my @errors;
	my $line;
	my $nKeys;
	my $nErrs;
	my $report;
	my $result;
	my $command;
	my $ii;
	# test convergence numbers
	open(INPUT, "<$datFile") || die "Can't open the expected file $datFile\n" ; 
	while ($line = <INPUT>) {
		chomp $line;
		if ( $line =~ m/^\#Res\s.*/ ) {
   		# parse for variable labels
			@keys = split (/\s+/, $line );
		}
		else {
			@errors = split(/\s+/, $line );
		}
	}

	# ensure the number of keys and error measures agree
	$nKeys = @keys;
	$nErrs = @errors;
	
	if( $nKeys != $nErrs ) { die "The number of keys against the number of errors in file $datFile don't agreed\n"; }

	$result = "Pass";
	$report = "";

	$testReport .= "[Bitten] tolerance=$tolerance\n";

	# go through all  errors and check if they're within tolerance
	for( $ii = 1 ; $ii < $nKeys ; $ii++ ) {
		if( abs($errors[$ii]) > $tolerance ) {
			$result = "Fail";
			$report .= "***BAD NEWS*** ... $keys[$ii] differs by more than " . $tolerance*100 . "\% tolerance from expected file, error is $errors[$ii]\n";
			$testReport .= "[Bitten] error in $keys[$ii]=$errors[$ii]\n";
		} else {
			$report .= "pass ... $keys[$ii] within a ". $tolerance*100 ."\% relative tolerance from expected file\n";
			$testReport .= "[Bitten] error in $keys[$ii]=$errors[$ii]\n";
		}
	}

	close( INPUT );

	print "\n$report";
	print "\nResult = $result\n";

	$testReport .= "[Bitten] status=$result\n";

	# remove the used data file
	$command = "rm $datFile";
	&executeCommandline($command);

	open ( JERICO_FILE, "+>.jericoFile" );
	print JERICO_FILE "$testReport\n";
	close( JERICO_FILE );

	if( $result eq "Pass" ) {
		exit(0);
	} else {
		exit(1);
	}
}


sub executeCommandline {
# pass in single string to execute on the command
	my $command = $_[0];

  my $output = qx{$command};            # that's the new shell's $$
	my $exitStatus = $? >> 8;

	# check the exit status of the command
	if( $exitStatus ne 0 ) { die "\n\n### ERROR ###\nCouldn't execute the command\n$command\n\n"; }

	return $output;
}
1;
