#!/usr/bin/perl

print "here\n";
$pwd = `pwd`;
chomp $pwd;
$src_file = "$pwd/dispatcher_factory";

#{{{
sub get_classes {
	my @classes;
	foreach $file (<*_dispatcher.h>) {
		$file =~ s/_dispatcher.h//;
		next if $file eq 'event';

		push @classes, join '', map { ucfirst } split /_/, $file;
	}

	sort @classes;

	return @classes;
}
#}}}
#{{{
sub generate_forward_decls {
	@classes = get_classes();

	open FACTORY, "$src_file.h" or die "Whoops";
	@lines = <FACTORY>;
	close FACTORY;

	$at_class_section = 0;
	open FACTORY, ">$$src_file.h" or die "Double Whoops";
	foreach $line (@lines) {
		#print "$at_class_section -- $line";
		if($line =~ qr{^////\*\* BEGIN GENERATED CODE \*\*////$}) {
			print FACTORY $line;
			$at_class_section = 1;
		}
		elsif($line =~ qr{^////\*\*  END GENERATED CODE  \*\*////$}) {
			$at_class_section = 0;
		}

		if($at_class_section == 1) {
			print FACTORY join "\n", map { "class $_" . "Dispatcher;" } @classes;
			print FACTORY "\n";
			$at_class_section = 2;
		}
		elsif($at_class_section == 0) {
			print FACTORY $line;
		}
	}
}
#}}}
#{{{
sub generate_includes_to_cpp {
	@classes = get_classes();
	@dot_h_files = <*_dispatcher.h>;
	@dot_h_files = grep !/event_dispatcher.h/, @dot_h_files;

	open FACTORY, "$src_file.cpp" or die "Whoops";
	@lines = <FACTORY>;
	close FACTORY;

	$at_class_section = 0;
	open FACTORY, ">$src_file.cpp" or die "Double Whoops";
	foreach $line (@lines) {
		#print "$at_class_section -- $line";
		if($line =~ qr{^\s*////\*\* BEGIN GENERATED INCLUDES \*\*////$}) {
			print FACTORY $line;
			$at_class_section = 1;
		}
		elsif($line =~ qr{^\s*////\*\*  END GENERATED INCLUDES  \*\*////$}) {
			$at_class_section = 0;
		}

		if($at_class_section == 1) {
			print FACTORY join "\n", map { "#include \"$_\"" } @dot_h_files;
			print FACTORY "\n";
			$at_class_section = 2;
		}
		elsif($at_class_section == 0) {
			print FACTORY $line;
		}
	}
}
#}}}
#{{{
sub generate_build_dispatcher_code {
	@event_types = get_classes();

	open FACTORY, "$src_file.cpp" or die "Whoops";
	@lines = <FACTORY>;
	close FACTORY;

	$at_section = 0;
	$leading_space = '';
	open FACTORY, ">$src_file.cpp" or die "Double Whoops";
	foreach $line (@lines) {
		#print "$at_section -- $line";
		if($line =~ qr{^(\s*)////\*\*\s*BEGIN GENERATED FACTORY CODE\s*\*\*////\s*$}) {
			$leading_space = $1;
			print FACTORY $line;
			$at_section = 1;
		}
		elsif($line =~ qr{^\s*////\*\*\s*END GENERATED FACTORY CODE\s*\*\*////\s*$}) {
			$at_section = 0;
		}

		if($at_section == 1) {
			foreach $type (@event_types) {
				print FACTORY "$leading_space" . "switch $type:\n";
				print FACTORY "$leading_space" . "\trv = new $type" . "Dispatcher(event_type);\n";
				print FACTORY "$leading_space" . "\tbreak;\n";
			}
			$at_section = 2;
		}
		elsif($at_section == 0) {
			print FACTORY $line;
		}
	}
}
#}}}

generate_forward_decls();
generate_includes_to_cpp();
generate_build_dispatcher_code();

