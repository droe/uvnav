#!/usr/bin/env perl

while(<STDIN>)
{
	chomp;
	s/^\s+//g;
	s/\s+$//g;
	s/\s+/\\s/g;
	print "    keyword whole $_ yellow/24\n";
}