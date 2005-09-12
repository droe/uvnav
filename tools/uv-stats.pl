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

# Berechnet Auswertungsstatistiken ueber mehrere Auswertungen.

use strict;

my $opt_h = '0';
my $param = shift();
if(defined($param) && $param eq '-h')
{
	$opt_h = '1';
}
else
{
	unshift(@ARGV, $param) if(defined($param));
}

my %data = ();

sub short
{
	my $n = shift();
	unless($opt_h)
	{
		return $n;
	}
	my $l = length($n);
	if($l <= 6)
	{
		return $n;
	}
	elsif($l <= 9)
	{
		return substr($n, 0, $l-3).'K';
	}
	elsif($l <= 12)
	{
		return substr($n, 0, $l-6).'M';
	}
	elsif($l <= 15)
	{
		return substr($n, 0, $l-9).'G';
	}
	else
	{
		return substr($n, 0, $l-12).'T';
	}
}

sub auswertung
{
	my $file = shift();

	open(FILE, "<$file") || die "$!";

	my $sz = 0;
	my $name = '';

	my $punkte = 0;
	my $konto = 0;
	my $legal = 0;
	my $sch_cnt = 0;
	my $sch_brt = 0;
	my $sch_lag = 0;	# Waren in Schiffslager in BRT
	my $zon_cnt = 0;
	my $zon_fus = 0;
	my $wer_cnt = 0;
	my $wer_erz = 0;
	my $for_cnt = 0;    # Anzahl Forschungsstationen
	my $for_wp = 0;     # Anzahl Wissenspunkte
	my $for_dwp = 0;    # Anzahl Wissenspunkte neu erforscht
	my $sta_cnt = 0;
	my $sta_fus = 0;
	my $sta_ew = 0;
	my $sta_pot = 0;    # Potentielle Einnahmen
	my $min_cnt = 0;
	my $min_fus = 0;
	my $min_ert = 0;
	my $agr_cnt = 0;
	my $agr_fus = 0;
	my $agr_ert = 0;
	my $spe_cnt = 0;
	my $spe_fus = 0;
	my $spe_lag = 0;	# Waren in Speicherfeldern in BRT
	my $steuern = 0;	# Stadteinnahmen in Credits
	my $verkauf = 0;	# Summe aller Warenverkäufe in Credits

	my $my_zone = 0;

	while($_ = <FILE>)
	{
		if(!$sz && /^Sternzeit: (\d+)\r?$/)
		{
			$sz = $1;
		}
		elsif(!$name && /^Name: (.+?)\r?$/)
		{
			$name = $1;
		}
		elsif(!$punkte && /^Punkte: (\d+)\r?$/)
		{
			$punkte = $1;
		}
		elsif(!$konto && /^Konto: (\d+) Credits\r?$/)
		{
			$konto = $1;
		}
		elsif(!$legal && /^Legalit.tsstatus:\s+(-?\d+)\r?$/)
		{
			$legal = $1;
		}
		elsif(/^\* Schiff.*\($name\) (\d+) BRT/)
		{
			$sch_cnt++;
			$sch_brt += $1;
		}
		elsif(/^   \d+\. .+ \((\d+) BRT, $name\)/)
		{
			$sch_lag += $1;
		}
		elsif(/^Zone .*\($name\) \((\d+) FUs\)/)
		{
			$my_zone = 1;
			$zon_cnt++;
			$zon_fus += $1;
		}
		elsif(/^Zone /)
		{
			$my_zone = 0;
		}
		elsif($my_zone && /^# Werft .*, Erzlager: (\d+)/)
		{
			$wer_cnt++;
			$wer_erz += $1;
		}
		elsif($my_zone && /^# Forschungsstation .*: (\d+) Wissenspunkte/)
		{
			$for_cnt++;
			$for_wp += $1;
		}
		elsif($my_zone && /^# Stadt .*, (\d+) FUs: (\d+) Seelen/)
		{
			$sta_cnt++;
			$sta_fus += $1;
			$sta_pot += $1 * 12500;
			$sta_ew += $2;
		}
		elsif($my_zone && /^# Mine .*, (\d+) FUs(?:: .*\.(\d+) BRT Ertrag)?/)
		{
			$min_cnt++;
			$min_fus += $1;
			$min_ert += $2 || 0;
		}
		elsif($my_zone && /^# Agrarfeld .*, (\d+) FUs(?:: .*\.(\d+) BRT Ertrag)?/)
		{
			$agr_cnt++;
			$agr_fus += $1;
			$agr_ert += $2 || 0;
		}
		elsif($my_zone && /^# SpeicherFeld .*, (\d+) FUs(?:: .*BRT Speicherplatz\..* (\d+) BRT)?/)
		{
			$spe_cnt++;
			$spe_fus += $1;
			$spe_lag += $2 || 0;
		}
		elsif(/^- Die Forschung in der Station .* hat ihnen (\d+) Wissenspunkte eingebracht\./)
		{
			$for_dwp += $1;
		}
		elsif(/^- Die Stadt .* hat (\d+) Steuern eingebracht\./)
		{
			$steuern += $1;
		}
		elsif(/^- Sie haben das Lager .* gesamthaft (\d+) Cr .* zu Ihren Gunsten/)
		{
			$verkauf += $1;
		}
		elsif(/^- Ein Lager .* zu Ihren Gunsten .* gesamthaft (\d+) Cr/)
		{
			$verkauf += $1;
		}
	}
	close(FILE);

	die "Keine Sternzeit!" unless $sz;

	$data{$sz}{'punkte'} = $punkte;
	$data{$sz}{'konto'} = $konto;
	$data{$sz}{'legal'} = $legal;
	$data{$sz}{'sch_cnt'} = $sch_cnt;
	$data{$sz}{'sch_brt'} = $sch_brt;
	$data{$sz}{'sch_lag'} = $sch_lag;
	$data{$sz}{'zon_cnt'} = $zon_cnt;
	$data{$sz}{'zon_fus'} = $zon_fus;
	$data{$sz}{'wer_cnt'} = $wer_cnt;
	$data{$sz}{'wer_erz'} = $wer_erz;
	$data{$sz}{'for_cnt'} = $for_cnt;
	$data{$sz}{'for_wp'} = $for_wp;
	$data{$sz}{'for_dwp'} = $for_dwp;
	$data{$sz}{'sta_cnt'} = $sta_cnt;
	$data{$sz}{'sta_fus'} = $sta_fus;
	$data{$sz}{'sta_ew'} = $sta_ew;
	$data{$sz}{'sta_pot'} = $sta_pot;
	$data{$sz}{'min_cnt'} = $min_cnt;
	$data{$sz}{'min_fus'} = $min_fus;
	$data{$sz}{'min_ert'} = $min_ert;
	$data{$sz}{'agr_cnt'} = $agr_cnt;
	$data{$sz}{'agr_fus'} = $agr_fus;
	$data{$sz}{'agr_ert'} = $agr_ert;
	$data{$sz}{'spe_cnt'} = $spe_cnt;
	$data{$sz}{'spe_fus'} = $spe_fus;
	$data{$sz}{'spe_lag'} = $spe_lag;
	$data{$sz}{'steuern'} = $steuern;
	$data{$sz}{'verkauf'} = $verkauf;
}

foreach(@ARGV)
{
	auswertung($_);
}

print 'sz'
	."\t". 'punkte'
	."\t". 'legal'
	."\t". 'konto'
	."\t". 'steuern'
	."\t". 'verkauf'
	."\t". 'sch_cnt'
	."\t". 'sch_brt'
	."\t". 'sch_lag'
	."\t". 'zon_cnt'
	."\t". 'zon_fus'
	."\t". 'wer_cnt'
	."\t". 'wer_erz'
	."\t". 'for_cnt'
	."\t". 'for_wp'
	."\t". 'for_dwp'
	."\t". 'sta_cnt'
	."\t". 'sta_fus'
	."\t". 'sta_ew'
	."\t". 'sta_pot'
	."\t". 'min_cnt'
	."\t". 'min_fus'
	."\t". 'min_ert'
	."\t". 'agr_cnt'
	."\t". 'agr_fus'
	."\t". 'agr_ert'
	."\t". 'spe_cnt'
	."\t". 'spe_fus'
	."\t". 'spe_lag'
	."\n";
foreach(sort(keys(%data)))
{
	print "$_";
	print "\t" . short($data{$_}{'punkte'});
	print "\t" . short($data{$_}{'legal'});
	print "\t" . short($data{$_}{'konto'});
	print "\t" . short($data{$_}{'steuern'});
	print "\t" . short($data{$_}{'verkauf'});
	print "\t" . short($data{$_}{'sch_cnt'});
	print "\t" . short($data{$_}{'sch_brt'});
	print "\t" . short($data{$_}{'sch_lag'});
	print "\t" . short($data{$_}{'zon_cnt'});
	print "\t" . short($data{$_}{'zon_fus'});
	print "\t" . short($data{$_}{'wer_cnt'});
	print "\t" . short($data{$_}{'wer_erz'});
	print "\t" . short($data{$_}{'for_cnt'});
	print "\t" . short($data{$_}{'for_wp'});
	print "\t" . short($data{$_}{'for_dwp'});
	print "\t" . short($data{$_}{'sta_cnt'});
	print "\t" . short($data{$_}{'sta_fus'});
	print "\t" . short($data{$_}{'sta_ew'});
	print "\t" . short($data{$_}{'sta_pot'});
	print "\t" . short($data{$_}{'min_cnt'});
	print "\t" . short($data{$_}{'min_fus'});
	print "\t" . short($data{$_}{'min_ert'});
	print "\t" . short($data{$_}{'agr_cnt'});
	print "\t" . short($data{$_}{'agr_fus'});
	print "\t" . short($data{$_}{'agr_ert'});
	print "\t" . short($data{$_}{'spe_cnt'});
	print "\t" . short($data{$_}{'spe_fus'});
	print "\t" . short($data{$_}{'spe_lag'});
	print "\n";
}

