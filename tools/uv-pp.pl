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

# Einfacher Zugdatei-Praeprozessor, derzeit noch ohne direkten
# Auswertungsbezug.

# TODO: autobackup original into Backup/$sz.bak.$i, overwrite zug

my $sz = shift() || die "Usage: $0 <sternzeit>\n";

open(INPUT, "<$sz.uvzug");
my @in = <INPUT>;
close(INPUT);

my @out;
while(@in)
{
	$_ = shift(@in);

	if(/^---\s*Sternzeit:\s*([0-9]+)/)
	{
		if($sz != $1)
		{
			die("Sternzeit mismatch! ($sz!=$1)\n");
		}
	}

	if(/^---\s*Generiert/)
	{
		my $date = `date +%Y-%m-%d`; chomp $date;
		my $id = `id -un`; chomp $id;
		push(@out, "--- Generiert aus $sz.uvzug ($date/$id)\n");
	}
	else
	{
		push(@out, $_);
	}

	if(/^---!\s*(\w+)\s*(.*)$/)
	{
		my $handler = 'handle_'.lc($1);
		&$handler($2);
	}
}

open(OUTPUT, ">$sz.uvzug.pp");
while(@out)
{
	print OUTPUT shift(@out);
}
close(OUTPUT);


#------ CRON TEST
#---!CRON 212 % 2
#---? --- Frachter beladen
#---? FINGER,"Erzfrachter"
#---!NEXT
#--- Frachter entladen
#INFO,"Erzfrachter"
#---!END
sub handle_cron
{
	my ($p) = @_;
	if($p =~ /^\s*([0-9]+)\s*%\s*([0-9]+)\s*$/)
	{
		my $start = $1;
		my $mod = $2;

		for(my $i = 0; $i < $mod; $i++)
		{
			while(@in)
			{
				$_ = shift(@in);
				if(/^---!/)
				{
					push(@out, $_);
					last;
				}
				elsif(/^(?:---\?\s*)?(.*)$/)
				{
					if(($start+$i) % $mod == $sz % $mod)
					{
						$_ = $1."\n";
					}
					else
					{
						$_ = '---? '.$1."\n";
					}
					push(@out, $_);
				}
				else
				{
					die("Internal regexp error!\n");
				}
			}
		}
	}
	else
	{
		die "Illegal parameters to CRON: $p\n";
	}

}


#------ AT TEST
#---!AT 212
#---? --- Frachter beladen
#---? FINGER,"Erzfrachter"
#---!NEXT
#--- Frachter entladen
#INFO,"Erzfrachter"
#---!END
sub handle_at
{
	my ($p) = @_;
#	if($p =~ /^\s*([0-9]+)\s*%\s*([0-9]+)\s*$/)
	if($p =~ /^\s*([0-9]+)\s*$/)
	{
		my $start = $1;

		for(my $i = 0; (!/^---!END/ && @in); $i++)
		{
			while(@in)
			{
				$_ = shift(@in);
				if(/^---!/)
				{
					push(@out, $_);
					last;
				}
				elsif(/^(?:---\?\s*)?(.*)$/)
				{
					if($start + $i == $sz)
					{
						$_ = $1."\n";
					}
					else
					{
						$_ = '---? '.$1."\n";
					}
					push(@out, $_);
				}
				else
				{
					die("Internal regexp error!\n");
				}
			}
		}
	}
	else
	{
		die "Illegal parameters to AT: $p\n";
	}

}


#------ GENERATE TEST
#--- jeweils nur duplizieren soweit noetig
#---!GENERATE 2345 3456; 1..15
#---? ------  (%1)
#---? ZONE ABFRAGEN,%1,%2
#---?
#---!DATA
#---!END
sub handle_generate
{
	my ($p) = @_;

	my @stages = split(/\s*;\s*/, $p);
#print "stages: ".join('/', @stages)." (\$#=$#stages)\n";
	for(my $i = 0; $i <= $#stages; $i++)
	{
		if($stages[$i] =~ /[0-9]+\.\.[0-9]+/)
		{
			my $seq = eval('join(" ", '.$&.');');
			$stages[$i] = $`.$seq.$';
		}
	}

	my @templates;
	while(@in)
	{
		$_ = shift(@in);
		s/^---\?\s*//g;
		if(/^---!DATA/)
		{
			push(@out, $_);
			last;
		}
		push(@out, "---? $_");
		push(@templates, $_);
	}

	while(@in)
	{
		$_ = shift(@in);
		if(/^---!END/)
		{
			last;
		}
	}

	if(!/^---!END/)
	{
		die("Unexpected end of file!\n");
	}

	foreach my $template (@templates)
	{
		$template =~ s/^---\?\s*//;
		my (@used, @which, @refs);
		while($template =~ /\%([0-9]+)/g)
		{

#print "\"$template\" =~ m/$&/g: $1\n";

			$n = $1 - 1;
			if($n < 0)
			{
				die("Indexes smaller than 1 not allowed!\n");
			}
			unless($used[$n])
			{
				if(!defined($stages[$n]))
				{
					die("Uninitialised generator variable used!\n");
				}
				my @token = split(/\s+/, $stages[$n]);
				push(@refs, \@token);
				push(@which, $n);
			}
			$used[$n] = 1;
		}
		if(@refs)
		{
#print "refs is true!\n";
			my $done = shift(@refs);
			while(@refs)
			{
				my $product = cartesian_product($done, shift(@refs));
				my @newdone;
				while(($a,$b) = &$product())
				{
					push(@newdone, "$a $b");
				}
				$done = \@newdone;
			}

			foreach my $tuple (@$done)
			{
				my @values = split(/\s+/, $tuple);
				my $t = $template;
				for(my $i = 0; $i <= $#which; $i++)
				{
					my $key = $which[$i] + 1;
#print "s/\%$key/$values[$which[$i]]/g\n";
					$t =~ s/\%$key/$values[$which[$i]]/g;
				}
				push(@out, $t);
			}
		}
		else
		{
#print "refs is false!\n";
			chomp($template);
			push(@out, "$template\n");
		}
	}
	push(@out, "---!END\n");
}

sub cartesian_product {
	my ($a, $b) = @_;
	my ($i, $j) = (0, 0);
	return sub {
		($j = 0, ++$i) if $j >= @$b;
		($i = 0, return ()) if $i >= @$a;
		return ($a->[$i], $b->[$j++]);
	}
}

