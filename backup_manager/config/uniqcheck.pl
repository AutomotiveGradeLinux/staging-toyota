#!/usr/bin/perl
use strict;
use warnings;

my @checkxpath = ('//item/@name', '//item/@id', '//category/@name');

sub uniqcheck($$)
{
  my $xpath = shift(@_);
  my $list = shift(@_);
  my %u = ();
  foreach my $key (@{$list}) {
    if (exists($u{$key})) {
      print "detect duplicate $xpath $key\n";
      exit 1;
    }
    $u{$key} = 1;
  }
}

sub getlist($$)
{
  my $xmlfile = shift(@_);
  my $xpath = shift(@_);

  $xmlfile =~ s/([\[\]\*\(\)])/\\$1/g;

  open(my $rs, "xmllint --xpath \"$xpath\" $xmlfile |") or die "Cannot open $xmlfile:$!";
  my $result = join('', <$rs>);
  close $rs;
  $result =~ s/^ +//;
  my @result_array = split(/ +/, $result);
  return @result_array;
}

if ($#ARGV < 0 || !(-f $ARGV[0])) {
  print "No such file or directory.\n";
  exit 1;
}

foreach my $xpath (@checkxpath) {
  my @result_array = getlist($ARGV[0], $xpath);
  uniqcheck($xpath, \@result_array);
}

0;
