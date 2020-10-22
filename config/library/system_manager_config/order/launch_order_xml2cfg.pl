#!/usr/bin/perl -w 
use strict;

use XML::XPath;
use File::Copy;

my $main_file="launch_order_main.xml";
my $body_file="launch_order_body.xml";


sub usage(){
  print STDERR "USAGE:$0 *.order *.cfg\n"
}
    



#=== MAIN ===
my $ret=system("which xmllint > /dev/null");
if($ret != 0){
    print STDERR  "It need xmllint. Please install it.\n";
    exit 1;
}

if(2 != @ARGV){
    usage();
    exit 1;
}

my $infile=$ARGV[0];
my $launchfile=$ARGV[1];


#ARG CHECK
if($infile !~ /.+\.order$/){
  print STDERR "$infile is not *.order\n";
  exit 1;
}elsif(! -e $infile){
  print "$infile not found\n";
  usage();
  exit 1;
}

if($launchfile !~ /.+\.cfg$/){
  print STDERR "$launchfile is not *.cfg\n";
  exit 1;
}elsif(! -e $launchfile){
  print "$launchfile not found\n";
  usage();
  exit 1;
}


system("cp -f $infile $body_file");

#check xml vaild 
$ret=system("xmllint --noout --valid $main_file");
if($ret != 0){
  print STDERR "XML is Invalid. \n";
  exit 1;
}

my $date=`LANG=en date`;
print "#  This file is created from $infile and $launchfile\n";
print "#  created date : $date\n";

my %gnamemap;
my %gidmap;

open(FIN,"<$launchfile")  or die("error :$!");
while (my $line = <FIN>){
  if($line =~ /^Launch/){
    $line =~ s/^[^|]+=//g;
    my @args =  split(/\|/,$line);
    $gnamemap{$args[0]}=$args[1];
    $gidmap{$args[1]}=$args[0];
  }
}
close(FIN);

foreach my $key (sort{$a <=> $b} keys(%gidmap)){
   print "#$key:$gidmap{$key}\n";
}


my $xml = XML::XPath->new(filename=>$main_file);

my $orders = $xml->find('/launch_order/order');
foreach my $order ($orders->get_nodelist) {
  my $groups = $order->find('./group');
  
  my $oname=$order->findvalue('@oname');
  my $fixed_group=$order->findvalue('@fixed_group');

  print "[$oname]\n";
  print "order=";
  
  my %l_gidmap = %gidmap;
  my $sep="";
  #fix groups
  if($fixed_group ne "NULL"){
    if(! exists $gnamemap{$fixed_group}){
      die("$fixed_group not found");
    }
    foreach my $gid (sort{$a <=> $b} keys(%l_gidmap)){
      print "$sep$gid";
      $sep="|";
      my $str=$l_gidmap{$gid};
      delete $l_gidmap{$gid};
      if($str eq $fixed_group){
        last;
      }
    }
  }

  # re-order group via *.order
  my %waitow;
  foreach my $group ($groups->get_nodelist) {
    my $gname=$group->findvalue('@gname');
    my $waittime=XML::XPath::Number::value($group->findvalue('@waittime'));
    
    my $gid=$gnamemap{$gname};
    
    if(! exists $gnamemap{$gname}){
      die("$oname:$gname is not defined");
    }

    if(! exists $l_gidmap{$gid}){
      die("$oname:$gname is multiple defined");
    }

    print "$sep$gid";
    $sep="|";
    delete $l_gidmap{$gid};
    
    if($waittime > 0){
      $waitow{$gname} = $waittime;
    }
  }
  
#  Output waittime configuration

  #out rest groups
  foreach my $gid (sort{$a <=> $b} keys(%l_gidmap)){
    print "$sep$gid";
    $sep="|";
  }
  
  print "\n";
  if(keys(%waitow) > 0 ){
    print "owlist=";
    $sep="";
    foreach my $owgroup ( keys(%waitow)) {
        print "${sep}oww_${owgroup}";
        $sep="|";
    }
    print "\n";
    
    foreach my $owgroup ( keys(%waitow)) {
        print "oww_${owgroup}=$waitow{$owgroup}\n";
    }

    
  }
  print "\n";
}





