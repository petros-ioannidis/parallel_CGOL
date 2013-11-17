#!/usr/bin/perl

open($MPI, "<MPI_time.plot");
open($OPENMP, "<OPENMP_time.plot");
open($SERIAL, "<SERIAL_time.plot");

open($MPI_OUT, ">MPI_accelaration.plot");
open($OPENMP_OUT, ">OPENMP_accelaration.plot");


@MPI_F = <$MPI>;
@OPENMP_F = <$OPENMP>;
@SERIAL_F = <$SERIAL>;

@MPI = ();
@OPENMP = ();
@SERIAL = ();

foreach $val (@MPI_F){
	chomp $val;
	push(@MPI,(split(',',$val)));
}

foreach $val (@OPENMP_F){
	chomp $val;
	push(@OPENMP,(split(',',$val)));
}

foreach $val (@SERIAL_F){
	chomp $val;
	push(@SERIAL,(split(',',$val)));
}

$i = 0;

while( $i < $#MPI+1){

	$MPI_acc = $SERIAL[$i+1]/$MPI[$i+1];
	$OPENMP_acc = $SERIAL[$i+1]/$OPENMP[$i+1];
	printf($MPI_OUT "$MPI[$i], $MPI_acc\n");
	printf($OPENMP_OUT "$OPENMP[$i], $OPENMP_acc\n");
	$i = $i+2
}

