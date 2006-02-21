#!/usr/bin/perl -w
# UV Navigator - Auswertungsvisualisierung fuer Universum V
# Copyright (C) 2004-2005 Daniel Roethlisberger <roe@chronator.ch>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see http://www.gnu.org/copyleft/
#
# $Id$

# Interaktive Navigationshilfe fuer UV-Zuege

use strict;

use Term::ReadLine;
use Math::Round qw(nearest round);

my $term = new Term::ReadLine 'Navigate';
my $prompt = "navigate> ";
my $OUT = $term->OUT || \*STDOUT;
while ( defined ($_ = $term->readline($prompt)) )
{
	if(/help/i)
	{
		print $OUT <<EOF;
	waypoint coords    -123123,12345
	fixed velocity     v=12
	maximum velocity   vmax=120
	multiple engines   vmax=2x60
EOF
	}
	elsif(/\S/)
	{
		navigate($_);
	}
	$term->addhistory($_) if /\S/;
}
print $OUT "\n";

sub pad2
{
	my $ret = sprintf("%02d", shift());
	if($ret == 100) { $ret = "00"; }
	return $ret;
}

sub navigate
{
	my ($data) = @_;
	$data =~ s/^\s+//g;
	$data =~ s/\s+$//g;
	my @tok = split(/\s/, $data);

	my $n = 0;		# number of waypoints
	my $v = 0;		# current fixed velocity in KpZ (local)

	my @x = ();		# waypoint x coords
	my @y = ();		# waypoint y coords
	my @v = ();		# fixed velocity to waypoint
	my $vmax = 0;	# maximum velocity in KpZ (global)
	my $engs = 1;	# number of engines
	my @d = ();		# distance from last waypoint
	my $sdyn = 0;	# total dynamically speed-adjusted distance
	my $tdyn = 100;	# total dynamically speed-adjusted time
	my @dt = ();	# delta time for each waypoint

	foreach my $token (@tok)
	{
		if($token =~ /^(-?\d+)[,:;-_](-?\d+)$/)
		{
			$x[$n] = $1;
			$y[$n] = $2;
			$v[$n] = $v;
			if($n)
			{
				$d[$n] =  sqrt(($x[$n]-$x[$n-1])**2 + ($y[$n]-$y[$n-1])**2);
				if($v)
				{
					$dt[$n] = round($d[$n] / $v[$n]);
					$tdyn -= $dt[$n];
				}
				else
				{
					$sdyn += $d[$n];
				}
			}
			$n++;
		}
		elsif($token =~ /^vmax=((?:\d+x)?)([\d.]+)$/i)
		{
			my $engs_catch = $1;
			chop($engs_catch) if($engs_catch);
			$engs = $engs_catch || 1;
			$vmax = $engs * $2;
		}
		elsif($token =~ /^v=((?:\d+x)?)([\d.]+)$/i)
		{
			my $engs_catch = $1;
			chop($engs_catch) if($engs_catch);
			$engs = $engs_catch || 1;
			$v = $engs * $2;
		}
		else
		{
			print $OUT "Syntax error: $token\n";
		}
	}

	#--- Start:  129120,16314
	#--- Ziel 1: 129916,13767  d=2668.48  t=01..45  v=60  s=2700  D=..
	#--- Ziel 2: 130477,12316  d=1555.67  t=46..71  v=60  s=1560  D=..
	#--- Ziel 3: 130000,20000  d=infinit  t=72..00  v=60  s=1740  D=..

	# $n, $vmax, $v[$i], $x[$i], $y[$i], $d[$i], $sdyn, $tdyn, $dt[$i]
	my $time = 1;
	for(my $i = 0; $i < $n; $i++)
	{
		if($i == 0)
		{
			print $OUT "--- Start:   ". $x[$i] .",". $y[$i] ."\n";
		}
		else
		{
			if($vmax && !$v[$i])
			{
				# dynamically assign velocity
				$v[$i] = $sdyn / $tdyn;
				$v[$i] = $vmax if($v[$i] > $vmax);
				$dt[$i] = round($d[$i] / $v[$i]);
			}
			if($v[$i])
			{
				print $OUT "--- Ziel $i:  ". $x[$i] .",". $y[$i]
				          ."  d=". round($d[$i])
				          ."  t=". pad2($time) ."..". pad2($time+$dt[$i]-1)
				          ."  v=". (($engs > 1) ? $engs . "x" .
				                    nearest(0.001, $v[$i]/$engs) :
				                    nearest(0.001, $v[$i]) )
				          ."  s=". round($v[$i]*$dt[$i])
				          ."  D=". round($v[$i]*$dt[$i] - $d[$i])
				          ."\n";
				$time += $dt[$i];
			}
			else
			{
				print $OUT "*** Error: Need global vmax or local v!\n";
			}
		}
	}
}
