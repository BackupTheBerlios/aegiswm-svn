#!/usr/bin/perl
use POSIX;

if (! $ARGV[0]) {
     exit(1);
}

my $textopname = $ARGV[0];

if ($ARGV[1]) {
    $textopname = $ARGV[1];
}

open(FILE, $ARGV[0]) or die "Can't open file $ARGV[0]: $!\n";

print "<waimea version=\"0.5.0\">\n  <text name=\"$textopname\" static=\"true\"><![CDATA[";

while (<FILE>) {
    print $_;
}
print "]]></text>\n</waimea>\n";

close(FILE);

exit(0);
