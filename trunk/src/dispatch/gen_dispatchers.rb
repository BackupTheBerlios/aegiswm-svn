#!/usr/bin/env ruby
# file name:  gen_dispatchers.rb
# author:     Michael Brailsford
# created:    Thu Jun 23 22:14:44 CDT 2005
# copyright:  (c) 2005 Michael Brailsford
# version:    $Revision$
# contents:   This generates a set of EventDispatcher subclasses to deal with all the cases where an
#             XEvent uses an XEvent::xany->window other than what is needed by the window manager to
#             handle the event.  For every entry in the XEvents.def file a .h file will be generated
#             which will have the code needed to create a dispatcher for that type of event.

require 'pp'

# this is the definition file that specifies only the odd cases where AegisWM needs a different
# Window than the one provided with XEvent::xany->window.
# The format of this file is simple:
# 		<XEvent>:<window member>
#
# 		Where:
# 			- <XEvent> is the XEvent, such as XMapRequestEvent
# 			- <window member> is the correct member of the struct that will give AegisWM the needed
# 			  window
def_file = 'XEvents.def'
hsh = Hash.new
print_line = false
IO.popen("man XEvent").each { |l|
	print_line = true if l =~ /typedef union _XEvent/

	line = l.strip.delete(";").split(' ')
	#puts l if print_line
	#puts line if print_line
	hsh[line[0]] = line[1] if print_line

	print_line = false if l =~ /^\s*\} XEvent;/
}

PP::pp hsh
raise

File.open(def_file, 'r') { |f|
	f.each { |line|

		
	}
}
