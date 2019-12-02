
		@(#)README	4.1 94/05/16


INTRODUCTION

This is the README file for the Tools, IPC, and Messages libraries of C
functions originally developed as part of the TAGS project by the
Scientific Computing Division at the National Center for Atmospheric
Research in Boulder, Colorado. TAGS is a multiprocessor output
production system whose distributed processes use message passing to
communicate.  Although the these libraries were developed specifically
for TAGS, some care was taken to insure that they are independent and
usable for a variety of applications.

The Tools library contains functions to manipulate data structures,
parse C-shell-like input, translate C-style escape sequences, and the
like. The IPC library contains functions to implement a network session
layer that uses Berkeley stream sockets.  The functions in the Messages
library implement a presentation layer that provides a simple but
flexible message passing scheme. (Originally the IPC and Messages
libraries were a single distribution, but starting with Release 2 they
were split apart.)


RELEASES

There are several releases of the both the Tools and the IPC libraries
in the FTP directory.

Release 0

	libtoolsR0.tar.Z
	libipcR0.tar.Z

	These are the production libraries developed in the original
	TAGS project in 1989 through 1991, and as such can be
	considered quite stable, although they do contain some bugs
	that have been fixed in later releases.

Release 1

	libtoolsR1.tar.Z
	libipcR1.tar.Z

	These contain some design and implementation changes, and there
	is no guarantee of cross-compatibility or even application
	compatibility with the prior release. These have never been
	used in production. They do have some bug fixes.

Release 2.x

	libtools.tar.Z
	libipc.tar.Z
	libmessages.tar.Z

	These contain the latest release of the software. Some of it
	has never been tested in production, but like all prior
	releases it has been regression tested using the included test
	programs. It is be the release we would use for new development
	here at NCAR. The code is thread-safe in that each routine is
	either reentrant, or includes hooks you can use to implement
	single-threading (e.g. DCE mutex) by defining macros.  Beware,
	there is some experimental code that I'm playing with, but it
	is marked in the sources as such. The only thing that I believe
	not to work correctly is the experimental AVL tree balancing
	code, but applications which might have used this instead use
	the new Skiplist data type, which is superior to binary trees
	in virtually all respects. The Skiplist data type has been
	extensively used in an actual project.


ACQUISITION

NCAR's FTP server is named "ftp.ucar.edu" (128.117.64.2). The compressed
tar files are in the "tagslib" subdirectory of the ftp home directory.
A command sequence something like this should work:

	ftp ftp.ucar.edu
	anonymous
	<enter your email address>
	cd tagslib
	binary
	get libtools.tar.Z
	get libipc.tar.Z
	get libmessages.tar.Z
	quit


EXTRACTION

To extract the sources from the compressed tar files, something like
the following:

	mkdir tags
	mkdir tags/include tags/lib tags/src
	mkdir tags/src/libtools tags/src/libipc tags/src/libmessages

	mv libtools.tar.Z tags/src/libtools
	mv libipc.tar.Z tags/src/libipc
	mv libmessages.tar.Z tags/src/libmessages

	(cd tags/src/libtools; zcat libtools.tar.Z | tar xvf -)
	(cd tags/src/libipc; zcat libipc.tar.Z | tar xvf -)
	(cd tags/src/libmessages; zcat libmessages.tar.Z | tar xvf -)

will work handily. You need to unload the tar'ed sources into different
directories since some of the file names may collide.


INSTALLATION

Peruse the Makefiles and make any modifications necessary for your
particular environment. You'll want to change the value of ROOT, the
parent directory which contains the subdirectories lib and include into
which the libraries and the header files are installed. No path names
are embedded into the code, so feel free to install in a private
directory, then move the header files and libraries to wherever you
want to use them. You may also want to change the values of CC,
CSYSTEM, and CFLAGS. Some examples for various environments are
commented out in the Makefile.

Once you're satisfied with the Makefile, you can use "make all" to see
if the code compiles at all, and "make install" to actually build the
library.  Don't worry if programs like "sccsunbundle" and "cpdate"
cannot be found. They are local version control software which are not
necessary for the installation; the make will not fail if they are not
present.

Since the libraries build on top of one another, you must build them
in this order: Tools, IPC, Messages.


LICENSE

Included with this software is a license agreement written by our
lawyers.  It's not bad at all; I understood it and I'm just a
programmer. It looks to me as if it entitles academic, scientific and
otherwise non-commercial sites to use this software at no charge.  It
also includes the usual disclaimers.  If after having read it you find
that you can live with it, I ask that you please print it out, fill in
the blanks, and mail it back to me at the address below.  I'll pass it
along to the suits in the administrative building.  In return, if you
scribble your email address at the bottom of the license, I'll try to
notify you electronically of any bug fixes, improvements, additions,
etc.

If you want to use the software commercially, contact me and I will
route you to the appropriate suits. By the way, I get part of the
proceeds if this package makes a profit; them's the rules around here.
Never the less, I prefer to give it away, and my employer has
generously and graciously allowed me to do so for non-commercial purposes.

This stuff is completely unsupported. Naturally, I'd be delighted to
receive bug reports, design criticisms, and suggestions for
improvements. I will respond as time permits.

John Sloan                     Carpe Datum.              +1 303 497 1243
NCAR/SCD                                             Fax +1 303 497 1818
Boulder CO 80307-3000 USA         ML48B             jsloan@ncar.ucar.edu
Logical Disclaimer: belong(opinions,jsloan). belong(opinions,_):-!,fail.
