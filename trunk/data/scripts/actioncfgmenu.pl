#!/usr/bin/perl
use POSIX;

my $name = "actions";
my $title = "actions";
my $menuwidthfactor = "1.5";

my $titlename = "title";
my $itemname = "item";
my $actionsdir = "/usr/local/share/waimea/actions";
my $homeactionsdir = "$ENV{HOME}/.waimea/actions";
@Actions = ();

if (! $ARGV[0]) {
    print_directory($actionsdir);
    print_directory($homeactionsdir);
} else {
    print_directory($ARGV[0]);
}

if (@Actions) {
    print "<?xml version=\"1.0\"?>\n";
    print "<!DOCTYPE waimea SYSTEM \"/usr/local/share/waimea/waimea.dtd\">\n";
    print "<waimea version=\"0.5.0\">\n";
    print "  <menu name=\"$name\" width_factor=\"$menuwidthfactor\">\n";
    print "    <item name=\"$titlename\" string=\"$title\" " .
	"image=\"/usr/local/share/waimea/scripts/icons/actions.png\"/>\n";
    
    foreach $Action (@Actions) {
        print $Action;
    }

    print "  </menu>\n";
    print "</waimea>\n";
}

sub print_directory {
    my $mode;
    my $temp;
    my @dir_list;
    my $found_action = 0;
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
                    if (/^action.conf$/) {
                         $found_action = 1;
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
    if ($found_action == 1) {
	$Item = "    <item name=\"$itemname\" " .
	    "string=\"$dirname\" " .
		"function=\"reloadwithactionfile\" parameter=\"$_[0]\/action.conf\"";
        push(@Actions, $Item);
        if ($found_logo == 1) {
            $Item = " image=\"$_[0]\/$dirname.png\"";
            push(@Actions, $Item);
        }
        $Item = "/>\n";
        push(@Actions, $Item);
    }
    return 0;
}
