#!/usr/bin/perl 
use strict;

use XML::XPath;

my $LOCAL_XML = "./last2order.xml.tmp";
my $LOCAL_DTD = "./last2order.dtd";

my $DTD = <<'EODTD';
<!ELEMENT last2order_tbl (last2order*) >

<!ELEMENT  last2order EMPTY>
<!ATTLIST last2order
  front_video     CDATA "EMPTY"
  front_sub_video CDATA "EMPTY"
  front_audio     CDATA "EMPTY"
  rear_video      CDATA "EMPTY"
  rear_audio      CDATA "EMPTY"
  ordername       CDATA #REQUIRED
>
EODTD

sub usage(){
  print STDERR "USAGE:$0 *.xml *.cfo\n";
}

#==== MAIN ====
my $ret=system("which xmllint > /dev/null");
if($ret != 0){
  die("xmllint is not installed \n");
}

if(2 != @ARGV){
  usage();
  exit 1;
}

my $inXmlFile= $ARGV[0];
my $outCfgXmlFile= $ARGV[1];


#ARG CHECK
if(! -e $inXmlFile){
  die("$inXmlFile not found");
} 


system("cp $inXmlFile $LOCAL_XML");

open FOUT,'>',$LOCAL_DTD or die("can't open $LOCAL_DTD");
print FOUT $DTD;
close FOUT;

$ret=system("xmllint --noout --valid $LOCAL_XML ");
if($ret != 0){
  die("$inXmlFile is NOT VAILD");
}

my $xml = XML::XPath->new(filename=>$LOCAL_XML);

my $last2order_tbl = $xml->find('/last2order_tbl/last2order');

my @nodelist = $last2order_tbl->get_nodelist;
my $numOfElement = @nodelist;

printf("num of element :$numOfElement\n");

my $writeValue;
open FOUT,'+>',$outCfgXmlFile or die("can't open $outCfgXmlFile");
binmode(FOUT);

$writeValue = pack("A4","CTOO"); print FOUT $writeValue; 
$writeValue = pack("L",$numOfElement); print FOUT $writeValue; 

foreach my $last2order( @nodelist ) {
  $writeValue = $last2order->findvalue('@front_video');
  printf("FV:$writeValue  ");
  $writeValue = pack("a128",$writeValue); print FOUT $writeValue; 

  $writeValue = $last2order->findvalue('@front_sub_video');
  printf("FSV:$writeValue  ");
  $writeValue = pack("a128",$writeValue); print FOUT $writeValue; 

  $writeValue = $last2order->findvalue('@front_audio');
  printf("FA:$writeValue  ");
  $writeValue = pack("a128",$writeValue); print FOUT $writeValue; 

  $writeValue = $last2order->findvalue('@rear_video');
  printf("RV:$writeValue  ");
  $writeValue = pack("a128",$writeValue); print FOUT $writeValue; 

  $writeValue = $last2order->findvalue('@rear_audio');
  printf("RA:$writeValue  ");
  $writeValue = pack("a128",$writeValue); print FOUT $writeValue; 

  $writeValue = $last2order->findvalue('@ordername');
  printf("ON:$writeValue  ");
  $writeValue = pack("a64",$writeValue); print FOUT $writeValue; 

  printf("\n");
}

close(FOUT);

system("rm $LOCAL_XML $LOCAL_DTD");


