#!/usr/bin/perl
use POSIX;

my $name = "styles";
my $title = "styles";
my $menuwidthfactor = "1.0";

my $titlename = "title";
my $itemname = "item";
my $stylesdir = "/usr/local/share/waimea/styles";
my $homestylesdir = "$ENV{HOME}/.waimea/styles";
@Styles = ();

if (! $ARGV[0]) {
    print_directory($stylesdir);
    print_directory($homestylesdir);
} else {
    print_directory($ARGV[0]);
}

if (@Styles) {
    print "<?xml version=\"1.0\"?>\n";
    print "<!DOCTYPE waimea SYSTEM \"/usr/local/share/waimea/waimea.dtd\">\n";
    print "<waimea version=\"0.5.0\">\n";
    print "  <menu name=\"$name\" width_factor=\"$menuwidthfactor\">\n";
    print "    <item name=\"$titlename\" string=\"$title\"\n";
    print "          image=\"/usr/local/share/waimea/scripts/icons/styles.png\"/>\n";

    foreach $Style (@Styles) {
        print $Style;
    }

    print "  </menu>\n";
    print "</waimea>\n";
}

sub print_directory {
    my $mode;
    my $temp;
    my @dir_list;
    my $found_style = 0;
    my $found_logo = 0;
    $_ = $_[0];
    /.*\/(.+)$/;
    my $dirname = $1;

    opendir(DIR, $_[0]) || return 0;
    @dir_list = grep { /^[^\.].*/ || /^\.\..*/} readdir(DIR);
    closedir DIR;

    while ($temp = shift @dir_list) {
        $mode = (stat("$_[0]\/$temp"))[2];
        if (!($temp eq '..' || $temp eq '.')) {
	         if (S_ISDIR($mode)) {
		          print_directory("$_[0]\/$temp");
            } else {
                if (S_ISREG($mode)) {
                    $_ = $temp;
                    if (/^style.conf$/) {
                         $found_style = 1;
                    } else {
                        $_ = $temp;
                        if (/^$dirname.png$/) {
                            $found_logo = 1;
                        }
                    }
                }
	         }
	     }
    }
    if ($found_style == 1) {
	$Item = "    <item name=\"$itemname\" " .
	    "string=\"$dirname\" " .
		"function=\"reloadwithstylefile\" parameter=\"$_[0]\/style.conf\"";
        push(@Styles, $Item);
        if ($found_logo == 1) {
            $Item = " image=\"$_[0]\/$dirname.png\"";
            push(@Styles, $Item);
        }
        $Item = "/>\n";
        push(@Styles, $Item);
    }
    return 0;
}
