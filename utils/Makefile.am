## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2002 G.J. Paulissen
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

EXTRA_DIST           = empty_pipes.html empty_pipes.prc

CLEANFILES           = empty_pipes.html pod2htm*.tmp

POD2HTML             = pod2html

empty_pipes.html: empty_pipes.prc
# MinGW make uses CreateProcess which can not find /usr/bin/perl
#	@PERL@ -S pod2html --infile=$< --outfile=$@
	sed 's/-- =pod/=pod/g;s/-- =cut/=cut/g' $< | @PERL@ -S $(POD2HTML) --outfile=$@

html: empty_pipes.html
