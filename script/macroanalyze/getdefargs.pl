#!/usr/bin/perl

use Term::ANSIColor;
use Data::Dumper;

$dir=`pwd`;
#$dir =~ /((\/\w+)+\/stgUnderworld\w*).*/;
#$UWdir = $1;
chop $dir;
$LIST="$dir"."/script/macroanalyze";

unshift(@INC, $LIST);

our $paren;
our $cparen;
our $getdefinesall;
our $reg;
require 'doomstr.pl';
require 'doominc.pl';
our $dir;
our $UWdir;
our $filename;
our $pc;
our $hugestr;
our $defargsfile;
our $withrepofile;
our %getparent;
our %getdefarg;


#################################################################################################################################
#$dir=`pwd`; #print "$dir\n";
#$dir =~ /((?:\/\w+)+\/stgUnderworld\w+)(.*)/;
#$UWdir = $1;


$filename="$ARGV[0]";
open FILE, "$filename"  or die "can't find file: $!\n";
$hugestr='';

while (<FILE>){

    $hugestr .= "$_";

}
$pc = "$dir\/"."parentchildhashtable.txt";
$defargsfile = "$dir\/"."defargs.txt";

## populate the hash tables from file: the files may or may not exist
open FILE, "$pc";
while (<FILE>){
    /(\w+)\s(\w+)/;
    $getparent{ $1 } = $2;
}
close FILE;
open FILE, "$defargsfile";
while (<FILE>){
    /(\w+)\s(.*)/;
    $getdefarg{ $1 } = $2;
}
close FILE;

#################################################################################################################################
#################################################################################################################################
#################################################################################################################################
#################################################################################################################################
#### find all functions that are of the form ?X_?New ############################################################################
@functions=$hugestr=~/\w+(?:\**\s+|\s+\**|\s+\**\s+)(_\w+New\w*\s*\([^{}]+$cparen)/g; ## Only looking at _X_?New functions now i.e. starting with _
#########      is of form / (?: | | | )( ) / 2nd pair of brackets is the match  #################################################
#################################################################################################################################
# get fname fbody and fargs and fargslist from the function defn ################################################################
#######print "filename is $filename\n";  ########################################################################################
$i=0; $goforth = 1;# $numparents = 0;
foreach (@functions){
    ##print "$_ :::::\n\n";
    $fname[$i]=&getfname($_);
    $fbody[$i]=&getfbodyverbatim($_);
    ## get parent functions
    $subfuncs[$i]=[$fbody[$i]=~/\W(_(?:\w+_*)+_New[^()]*$paren)/g]; ## must have a _ in front of name i.e. they should because they are private constructors
    $fbodyclean[$i]=&getfbody($_);
    $subfuncsclean[$i]=[$fbodyclean[$i]=~/\W(_(?:\w+_*)+_New[^()]*$paren)/g];
    @pfunc=@{$subfuncsclean[$i]};
    # $#pfunc should be either 0 or -1; else there are too many parent functions in a child function
    if( $#pfunc > 0 ){ 
	open FILE, ">>log.txt";   print FILE "Function $fname[$i] has too many _New functions in file $filename\n"; close FILE;
	$goforth = 0 ; ## stop processing
	exit;
    }
    if($goforth == 1){
	$cclass[$i]=&getclass($fname[$i]); #list of potential child class names  	#print "Child class is $cclass[$i] num pfunc = $#pfunc\n";
	if($#pfunc == -1){ $pclass[$i] = $cclass[$i];} # this will remove it when we do AminusB test
	foreach (@pfunc){
	    s/\n//g;	    #print ">>>>>>>>>>$_<<<<<<<<<<<<\n";
	    $pclass[$i] = &getclass($_); #list of potential parent class names
	    s/[ \t]+/ /g;	    #print "Class of $_\n is pclass >>$pclass[$i]<<\n";	    #if($cclass[$i] ne $pclass[$i]){ $numparents++;}
	}
    }
    $i++;
}# foreach (@functions)
@pclasssub = &getYnotinX(\@pclass,\@cclass);
if($#pclasssub > 0) { open FILE, ">>log.txt";   print FILE "More than one child parent relation found in file $filename\n"; close FILE; }
# If we are here we have a 1-1 or 1-0 relation between child and parent functions
# i.e. no child function X has more than 1 parent function Y and may have none
foreach $Yclass (@pclasssub){
    #print "Y $Yclass mm\n";
    $i=0;  $j=0;
    foreach $Xclass (@pclass){
	#print "X $Xclass nn\n";
	if($Xclass eq $Yclass){
	    $pclass[$i] = NULL;
	    $cclasssub[$j] = $cclass[$i];
	    $Xfunc[$j] = $functions[$i]; ## functions in their original form
	    $Xname[$j] = $fname[$i];
	    $Xbody[$j] = &getfbody($functions[$i]);
	    $Xargs[$j] = &getfargs($functions[$i]);
	    $XargsVerbatim[$j] = &getfargsverbatim($functions[$i]);
	    $XbodyVerbatim[$j] = &getfbodyverbatim($functions[$i]);
	    $Yfunc[$j] = $subfuncs[$i][0];## functions in their original form
	    $Yname[$j] = &getfname($Yfunc[$j]);
	    $Yargs[$j] = &getfargs($Yfunc[$j]);
	    #print "Yargs = $Yargs[$j]\n";
	    $YargsVerbatim[$j] = &getfargsverbatim($Yfunc[$j]);
	    $j++;
	    #print "Found a class $cclass\n";
	}
	$i++;
    }
}
@Yclass = @pclasssub;
@Xclass = @cclasssub;

$i=0;
$reponame=$ARGV[0];
$reponame =~ /\.\/(\w+)\/.*/;
$reponame=$1;
open FILE, ">>$pc";
open WREP, ">>$withrepofile";
$i=0;
foreach (@Xname){
    if( !exists $getparent{ $_ }  ){
	print FILE "$_ $Yname[$i]\n";
	print WREP "$_ $Yname[$i] $reponame\n";
	$getparent{ $_ } = $Yname[$i];
    }
    $i++;
}
close FILE;
close WREP;
### At this point we have all the child parent functions and their classes
### Usually the above arrays have only one member each
#################################################################################################################################
#################################################################################################################################
### Now check for macros
$i=0;
# cool it looks like ALL macros as args end in 'ARGS'
# Lets just worry about the DEFARGS part i.e. the args to X
# because we will construct PASSARGS from the DEFARGS anyway
foreach $args (@Xargs){
    @Xargset = split(/,/,$args);
    #print Dumper(@Xargset);
    $Xhasmacro[$i]=0;
    $Xisplugin[$i]=0;
    if( $#Xargset == 0 ){
	#print "Only has one argument\n";
	if( ($args !~ /\w+\W+\w+/) && ($args !~ /void/) ){
	    #print color 'blue'; print "Is a macro $args in $filename\n"; print color 'reset';
	    if($args !~ /ARGS/){
		open FILE, ">>log.txt";
		print FILE "Looks like macro but doesn't have ARGS in name $Xname[$i] $args in $filename\n";
		close FILE;
	    }
	    $Xhasmacro[$i] =1;
	}
	else{
	    if($args =~ /ARGS/){ 
		open FILE, ">>log.txt"; print FILE "has 2 parts to args but has ARGS in it $Xname[$i] $args $filename\n"; close FILE; }
	    #print "Is a plugin $Xname[$i] $args   $filename\n";
	    $Xisplugin[$i]=1;
	}
    }
    $i++;
}

#Now we know who has macros and who are probably plugins
## Process the functions that have macros now and build the defarg hash table
$i=0;
foreach $Xfuncname (@Xname){
    ###########################################################################################################
    if($Xhasmacro[$i]){
	$Xargs[$i] =~ /(\w+)/;
	$macname = $1;
	$propermacname = "$Xclass[$i]"."_DEFARGS";
	print ">>>>>>>>>$propermacname<<<<<<<<<<<< >>>>>>>>>>>>>>>$macname<<<<<<<<<<<<<<<<\n";
	$headerfile=$filename;   $headerfile =~ s/\.c/\.h/; # get the headerfile 
        ###########################################################################################################
   	if( -e  $headerfile){
	    open FILE, "$headerfile";   $headerstr='';
	    while (<FILE>){ $headerstr .= $_; }
	    close FILE;
	    $getmacrodefine=&createdefinestr($macname);
	    $headerstr =~ /$getmacrodefine/g;
	    $hashdefine = $1;
	    $hashdefine =~ s/\s+/ /g; # clean it up a bit and make it fit on one line
	    #print "$hashdefine\n";
            ###########################################################################################################
	    if($hashdefine eq ''){ 
		print "Could not find definition for $macname. Try again by trying proper macro name\n";
		# Try what the name should be
		$macname=$propermacname;
		$getmacrodefine=&createdefinestr($macname);
		$headerstr =~ /$getmacrodefine/g;
		$hashdefine = $1;
		$hashdefine =~ s/\s+/ /g; # clean it up a bit and make it fit on one line
		#print "$hashdefine\n";
		if($hashdefine eq ''){ 
		    #print "Could not find definition for $macname. Try again by remangling macro name to proper but with ARGS instead of DEFARGS\n";
		    # Try what it might be
		    $macname = "$Xclass[$i]"."_ARGS";
		    $getmacrodefine=&createdefinestr($macname);
		    $headerstr =~ /$getmacrodefine/g;
		    $hashdefine = $1;
		    $hashdefine =~ s/\s+/ /g; # clean it up a bit and make it fit on one line
		    #print "$hashdefine\n";
		    if($hashdefine eq ''){ 
			open FILE, ">>log.txt";	
			print FILE "Could not find definition for $macname used by $Xname[$i] in $filename. We can assume is equivalent to proper macro that doesn't exist yet!\n"; 
			close FILE;
			# OK...if a macroname was used but not defined in a function and was not found
			# then it must be defined elsewhere but can be taken to be the same as the proper macro name because it works
			if( !exists $getdefarg{$propermacname} ) {
			    $Xargs[$i] =~ /(\w+)/;  $macname = $1; # get original macro name from the Xargs again
			    open FILE, ">>$defargsfile"; print FILE "$propermacname $macname \#define $propermacname $macname\n"; close FILE;
			    # this one won't appear in the properdefargs file because we cannot define it in the proper way yet
			}
		    }
		}
	    }# if($hashdefine eq '')
	    ###########################################################################################################
	    if( !exists $getdefarg{$macname} && $hashdefine ne '' ){# then write out the current defn to defargs file
		open FILE, ">>$defargsfile";
		print FILE "$macname $propermacname $hashdefine\n";
		if($macname ne $propermacname){
		    print FILE "$propermacname $macname $hashdefine\n";
		}
		close FILE;
	    }# if macro name doesn't exist already and we found a defn for it
	    if( !exists $getdefarg{$propermacname} && $hashdefine ne '' ){# then write out the current defn to defargs file
		open FILE, ">>$defargsfile";
		print FILE "$propermacname $propermacname $hashdefine\n";
		close FILE;
	    }# if proper macro name doesn't exist already and we found a defn for it
	    ###########################################################################################################
	}# if headerfile exists
        ###########################################################################################################
	else{
	    open FILE, ">>log.txt"; print FILE "Where is the goddam header file for $filename??\n"; close FILE;
	}# if headerfile exists
	###########################################################################################################

    }## if function has a macro for args
    else{## function does not have a macro in its args but we are going to check headerfile anyway to see if a macro is defined there.
	## Some macros use macros that are defined but not used in their own class's args to its function
	if($Xisplugin[$i] == 0){## and not a plugin
	    $propermacname = "$Xclass[$i]"."_DEFARGS";
	    $macname = $propermacname;
	    $headerfile=$filename;   $headerfile =~ s/\.c/\.h/; # get the headerfile 
	    ###########################################################################################################
	    if( -e  $headerfile){
		open FILE, "$headerfile";   $headerstr='';
		while (<FILE>){ $headerstr .= $_; }
		close FILE;
		$getmacrodefine=&createdefinestr($macname);
		$headerstr =~ /$getmacrodefine/g;
		$hashdefine = $1;
		$hashdefine =~ s/\s+/ /g; # clean it up a bit and make it fit on one line
		#print "$hashdefine\n";
		###########################################################################################################
		if($hashdefine eq ''){ 
		    #print "Could not find definition for $macname. Try again by remangling macro name to proper but with ARGS instead of DEFARGS\n";
		    # Try what it might be
		    $macname = "$Xclass[$i]"."_ARGS";
		    $getmacrodefine=&createdefinestr($macname);
		    $headerstr =~ /$getmacrodefine/g;
		    $hashdefine = $1;
		    $hashdefine =~ s/\s+/ /g; # clean it up a bit and make it fit on one line
		    #print "$hashdefine\n";
		}# if($hashdefine eq '')
		## if we still don't find a macro here it doesn't matter..probably doesn't exist
		###########################################################################################################
		if( !exists $getdefarg{$macname} && $hashdefine ne '' ){# then write out the current defn to defargs file
		    open FILE, ">>$defargsfile";
		    print FILE "$macname $propermacname $hashdefine\n";
		    if($macname ne $propermacname){
			print FILE "$propermacname $macname $hashdefine\n";
		    }
		    close FILE;
		}# if macro name doesn't exist already and we found a defn for it
		if( !exists $getdefarg{$propermacname} && $hashdefine ne '' ){# then write out the current defn to defargs file
		    open FILE, ">>$defargsfile";
		    print FILE "$propermacname $propermacname $hashdefine\n";
		    close FILE;
		}# if proper macro name doesn't exist already and we found a defn for it
		###########################################################################################################
	    }# if headerfile exists
	    ###########################################################################################################
	    else{
		open FILE, ">>log.txt"; print FILE "Where is the goddam header file for $filename??\n"; close FILE;
	    }# if headerfile exists
	    ###########################################################################################################
	}
    }## does not have macro in functions args
    $i++;
    ###########################################################################################################
}
#################################################################################################################################
#################################################################################################################################
