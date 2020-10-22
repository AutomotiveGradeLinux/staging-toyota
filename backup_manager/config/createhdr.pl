#!/usr/bin/perl
use strict;
use warnings;

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

my @item_array = getlist($ARGV[0], '//item/@name');

print<<'HEADER';
/*
 * @copyright Copyright (c) 2017-2020 TOYOTA MOTOR CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file ns_backup_id.h
 * @brief backup id define
 */

#ifndef BACKUP_MANAGER_CONFIG_NS_BACKUP_ID_H_
#define BACKUP_MANAGER_CONFIG_NS_BACKUP_ID_H_

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup backup_manager
 *  @ingroup native_service
 *  @{
 */

HEADER

foreach my $item (@item_array) {
  if ($item =~ /^name="(.+)"$/) {
    printf("#define %s \"%s\"\n", $1, $1);
  }
  else {
    print "Invalid input:$item\n";
    exit 1;
  }
}

print<<'FOOTER';

/**@}*/  // end of backup_manager
/**@}*/  // end of native_service
/**@}*/  // end of BaseSystem

#endif  // BACKUP_MANAGER_CONFIG_NS_BACKUP_ID_H_
FOOTER

0;
