########################################################################
#
#			M E S S A G E S   L I B R A R Y
#
#				4 . 1
#
#			jsloan@ncar.ucar.edu
#
#	Copyright 1990-1996 University Corporation for Atmospheric Research
#			 All Rights Reserved
#
#	@(#)Makefile	4.15 96/07/02 jsloan@ncar.ucar.edu
#
#	Prior release of this package have been tested on the following
#	platforms. However, because many of these machines were loaners
#	and therefore only available for a limited time, the portability
#	of the most current release has not been established.
#
#		SMI Sun-4	SunOS 4.1
#		SMI Sun-3	SunOS 4.0
#		SMI Sun-3	SunOS 4.0 GCC
#		DEC VAX 8550	Ultrix-32 V3.0
#		IBM 370/4381	AIX/370 1.2
#		IBM RS6000/550	AIX 3.1, 3.2
#		H-P HP9000/720	HP-UX A.B8.05
#		Dell P133c XPS	Linux 2.1.13
#		SMI Ultra-2	Solaris 5.5.1
#		NEC SX-4	SUPER-UX
#
########################################################################

ROOT=		/crestone/home/jsloan
NAME=		libmessages

########################################################################
#	Symbolic Definitions 
########################################################################

INCLUDE=	$(ROOT)/include
LIB=		$(ROOT)/lib
PRIVATE=	$(NAME).a
PUBLIC=		$(LIB)/$(PRIVATE)
LIBS=		$(PUBLIC) $(LIB)/libipc.a $(LIB)/libtools.a -lm -lcurses -ltermcap
BIN=		$(ROOT)/bin
MAN=		$(ROOT)/man
TMPDIR=		.

DCENAME=	libmessagesdce
DCEPRIVATE=	$(DCENAME).a
DCEPUBLIC=	$(LIB)/$(DCEPRIVATE)
DCELIBS=	$(DCEPUBLIC) $(LIB)/libipcdce.a $(LIB)/libtoolsdce.a -lm -lcurses -ltermcap -lpthreads

########################################################################
#	File Definition
########################################################################

HEADERS=	libmessages.h messages.h \
		mgeneric.h mgenerics.h mgenheader.h \
		mgenpackage.h mgenpacket.h \
		mheader.h mproto.h mqueue.h mservice.h \
		svmessage.h

SOURCES=	libmessages.c \
		maccept.c mclient.c mclose.c message.c \
		mextract.c \
		mgeneric.c mgenerics.c mgenheader.c \
		mgenpackage.c mgenpacket.c mgenrecv.c mgensend.c \
		mheader.c minsert.c mmerge.c mpair.c \
		mqueue.c mready.c mrecv.c msend.c mserver.c mservice.c \
		mtimed.c mvextract.c mvinsert.c \
		svmessage.c

EGSOURCES=	egmessages1.c egmessages2.c egmessages3.c \
		egmgeneric1.c egmgeneric2.c \
		egmqueue.c \
		egmsgclient.c egmsgpoller.c egmsgreceiver.c \
		egmsgsender.c egmsgserver.c egmsgservice.c \
		egmsgthreaded.c

EGSCRIPTS=	EGMsgPoller EGMsgReceiver

MANPAGES=	messages.3l mqueue.3l mservice.3l \
		svmessage.3l

MISC=		ABSTRACT README LICENSE Makefile cpdate sccsunbundle

DISTRIBUTION=	$(MISC) $(HEADERS) $(SOURCES) $(EGSOURCES) $(EGSCRIPTS) \
		$(MANPAGES)

OBJECTS=	libmessages.o \
		maccept.o \
		mclient.o mclose.o \
		message.o mextract.o \
		mgeneric.o mgenerics.o mgenheader.o \
		mgenpackage.o mgenpacket.o mgenrecv.o mgensend.o \
		mheader.o \
		minsert.o \
		mmerge.o \
		mpair.o \
		mqueue.o \
		mready.o mrecv.o \
		msend.o mserver.o \
		mtimed.o \
		mvextract.o mvinsert.o \
		svmessage.o

EXAMPLES=	egmessages1 egmessages2 egmessages3 \
		egmgeneric1 egmgeneric2 \
		egmqueue egmsgclient egmsgserver \
		egmsgthreaded
#		egmsgpoller egmsgreceiver egmsgsender egmsgservice

EXAMPLE=	egdummy

TARGET=		install

########################################################################
#	default target
########################################################################

all:	$(PRIVATE)

########################################################################
#	Predefined Architecture Specific Targets
#
#	N.B.	Some of these configurations were added to this
#		Makefile after the ports were complete and the
#		platforms were no longer available to be tested
#		upon. Hence, there may be some syntax errors.
#		Also, later releases of the various operating
#		systems may require modifications.
########################################################################

STAT_DEFS=	-DMESSAGES_STATS -DMGENERIC_STATS

##
##	SMI/SPARC/SunOS w/o DCE
##

SUNOS_DEFS=	

sunos:
	make $(TARGET) \
		ROOT=$(ROOT) \
		CUSTOMIZATIONS="$(STAT_DEFS) $(SUNOS_DEFS)" \
		EXAMPLE=$(EXAMPLE) \
		CC=gcc \
		DEBUG="$(DEBUG)"

##
##	SMI/SPARC/SOLARIS w/o DCE
##

SOLARIS_DEFS=	

SOLARIS_LIBS=	-lsocket -lnsl

solaris:
	make $(TARGET) \
		ROOT=$(ROOT) \
		CUSTOMIZATIONS="$(STAT_DEFS) $(SOLARIS_DEFS)" \
		EXAMPLE=$(EXAMPLE) \
		LIBS="$(LIBS) $(SOLARIS_LIBS)" \
		CC="/opt/SUNWspro/bin/cc" \
		DEBUG="$(DEBUG)"

##
##	IBM/RS6K/AIX w/o DCE
##

AIX_DEFS=	

aix:
	make $(TARGET) \
		ROOT=$(ROOT) \
		CUSTOMIZATIONS="$(AIX_DEFS)" \
		EXAMPLE=$(EXAMPLE) \
		DEBUG="$(DEBUG)" \
		CC="cc -qlanglvl=ansi -qro -qroconst"

##
##	IBM/RS6K/AIX w. DCE
##

AIXDCE_DEFS=	$(AIX_DEFS) -DPTHREADS_LIBTOOLS

AIXDCE_LIBS=	 -lpthreads

aixdce:
	make $(TARGET) \
		ROOT=$(ROOT) \
		CUSTOMIZATIONS="$(STAT_DEFS) $(AIX_DEFS)" \
		NAME="$(DCENAME)" \
		LIBS="$(DCELIBS) $(AIXDCE_LIBS)" \
		EXAMPLE=$(EXAMPLE) \
		CC=cc \
		DEBUG="$(DEBUG)"

##
##	DEC/ALPHA/OSF1 w/o DCE
##

OSF1_DEFS=	

osf1:
	make $(TARGET) \
		ROOT=$(ROOT) \
		CUSTOMIZATIONS="($STAT_DEFS) $(OSF1_DEFS)" \
		EXAMPLE=$(EXAMPLE) \
		CC=cc \
		DEBUG="$(DEBUG)"

##
##	DEC/ALPHA/OSF1 w. DCE
##

OSF1DCE_DEFS=	$(OSF1_DEFS) -DPTHREADS_LIBTOOLS

OSF1DCE_LIBS=	-lpthreads -lcmalib -lmach

osf1dce:
	make $(TARGET) \
		ROOT=$(ROOT) \
		CUSTOMIZATIONS="($STAT_DEFS) $(OSF1DCE_DEFS)" \
		NAME="$(DCENAME)" \
		LIBS="$(DCELIBS) $(OSF1DCE_LIBS)" \
		EXAMPLE=$(EXAMPLE) \
		CC=cc \
		DEBUG="$(DEBUG)"

##
##	CRI/YMP/UNICOS w/o DCE
##
##	N.B.	Ignore warnings from the UNICOS C compiler of the form
##
##		cc-172 cc: WARNING File = <somefile>.c, Line = <someline>
##		No code is generated for a superfluous expression.
##
##	This is the result of some code that is no-op'ed out if
##	DCE is not enabled; the P and V semaphore operations
##	are conditionally compiled as "{}" to preserve correct
##	syntax. The compiler is doing exactly what is intended.
##

UNICOS_DEFS=	

unicos:
	make $(TARGET) \
		ROOT=$(ROOT) \
		CUSTOMIZATIONS="$(STAT_DEFS) $(UNICOS_DEFS)" \
		EXAMPLE=$(EXAMPLE) \
		CC="cc -T," \
		RANLIB="ls -l" \
		DEBUG="$(DEBUG)"

##
##	SGI/IRIX w/o DCE
##

IRIX_DEFS=	

irix:
	make $(TARGET) \
		ROOT=$(ROOT) \
		CUSTOMIZATIONS="$(IRIX_DEFS)" \
		EXAMPLE=$(EXAMPLE) \
		DEBUG="$(DEBUG)" \
		CC="cc" \
		RANLIB="ar tvs"

##
##	NEC/SX-4/SUPER-UX w/o DCE
##

SUX_DEFS=	

sux:
	make $(TARGET) \
		ROOT=$(ROOT) \
		CUSTOMIZATIONS="$(STAT_DEFS) $(SUX_DEFS)" \
		EXAMPLE=$(EXAMPLE) \
		DEBUG="$(DEBUG)" \
		CC="cc" \
		RANLIB="ar tvs"

##
##	P5/Linux w/o DCE
##

LINUX_DEFS=	

linux:
	make $(TARGET) \
		ROOT=$(ROOT) \
		CUSTOMIZATIONS="$(STAT_DEFS) $(LINUX_DEFS)" \
		EXAMPLE=$(EXAMPLE) \
		DEBUG="$(DEBUG)" \
		CC="cc" \
		RANLIB="ar tvs"

########################################################################
#	Option Definitions
########################################################################
#
#	It is the stuff in this section that you may want to peruse
#	and modify in order to tailor libmessages for your particular
#	configuration. Some combinations of options that have worked
#	on various platforms in the past are commented out. These
#	may offer some useful guidance.
#
#	The following compiler symbols may be defined while building
#	libmessages to alter the resulting product. (For the most part,
#	the prefix of each symbol indicates which module it affects.)
#	If the symbol is in the form <MODULE>_<OPTION> then it affects
#	a single module and is probably used only in the .c file. If the
#	symbol is of the form <OPTION>_LIBTOOLS, <OPTION>_LIBIPC or
#	<OPTION>_LIBMESSAGES, then it affects many modules, may be used
#	in .h files, and can be used when compiling applications which
#	include the .h files.
#
#	EBCDIC_LIBMESSAGES	Translate EBCDIC to ASCII in message
#	ERRNO_LIBTOOLS		Use errno functions instead of variable
#	MESSAGES_STATS		Maintain message statistics
#	NMALLOC_LIBTOOLS	Use libtools "new" malloc functions
#	PACKAGE_LIBMESSAGES	Use packages not packets for messages
#	PROTOTYPE_LIBIPC	Define function prototypes for libipc
#	PROTOTYPE_LIBMESSAGES	Define function prototypes for libmessages
#	PROTOTYPE_LIBTOOLS	Define function prototypes for libtools
#	PTHREADS_LIBTOOLS	Applications will use OSF DCE Threads
#
#	For the record, I normally try to write everything in ANSI C
#	with function prototypes, and stick to POSIX compliant stuff.
#	Libtools _must_ be installed using an ANSI C compiler that
#	supports function prototypes. However, applications using
#	libmessages may or may not use function prototypes, depending on
#	the definition of PROTOTYPE_LIBMESSAGES.
#
#	Below, CONDITIONALS are applied to applications built using
#	libmessages, and CUSTOMIZATIONS are applied to the installation
#	of libmessages itself.
#
########################################################################

CUSTOMIZATIONS=-DMESSAGES_STATS -DMGENERIC_STATS

#CUSTOMIZATIONS=-DMESSAGES_STATS -DMGENERIC_STATS -DPTHREADS_LIBTOOLS

CONDITIONALS=-DPROTOTYPE_LIBTOOLS -DPROTOTYPE_LIBIPC -DPROTOTYPE_LIBMESSAGES

DEBUG=-g
#DEBUG=-O
CC=gcc ### Gnu CC
CFLAGS=$(DEBUG) $(CSYSTEM) $(CUSTOMIZATIONS)
CINCS=-I$(INCLUDE)

AR=ar
ARFLAGS=rv
RANLIB=ranlib
#RANLIB=ar tvs # SGI

LDSYSTEM=
LDLIBS=$(LIBS) $(LDSYSTEM) $(LDFLAGS)

COMPRESS=gzip -fv

########################################################################
#	application targets
########################################################################

install:	all include $(PUBLIC)

examples:
	for F in $(EXAMPLES); do make example CUSTOMIZATIONS="$(CUSTOMIZATIONS)" LIBS="$(LIBS)" EXAMPLE=$$F ROOT=$(ROOT) DEBUG="$(DEBUG)" CC="$(CC)"; done

example:
	make $(EXAMPLE) CUSTOMIZATIONS="$(CUSTOMIZATIONS)" LIBS="$(LIBS)" EXAMPLE=$(EXAMPLE) ROOT=$(ROOT) DEBUG="$(DEBUG)" CC="$(CC)"

manpages:	unbundle
	cp *.3l $(MAN)/man3

distribution:	unbundle $(DISTRIBUTION)
	tar cvf - $(DISTRIBUTION) > $(TMPDIR)/$(NAME).tar
	$(COMPRESS) $(TMPDIR)/$(NAME).tar

technote.out:	tn377ia.me
	soelim tn377ia.me | eqn | expand | tbl | ditroff -me > technote.out

technote:	technote.out
	dimp -t technote.out | ipr -Pscdimagen -Limpress

## If your target system supports asynchronous I/O, you can try
## building the optional message service facility. So far I've
## gotten this to work on SPARC/SunOS and RS6K/AIX.
mservice:
	make $(INCLUDE)/mservice.h HEADER=mservice.h ROOT=$(ROOT) CUSTOMIZATIONS="$(CUSTOMIZATIONS)" LIBS="$(LIBS)" DEBUG="$(DEBUG)" CC="$(CC)"
	make install-object OBJECT=mservice.o ROOT=$(ROOT) CUSTOMIZATIONS="$(CUSTOMIZATIONS)" LIBS="$(LIBS)" DEBUG="$(DEBUG)" CC="$(CC)"
	make example ROOT=$(ROOT) EXAMPLE=egmsgservice CUSTOMIZATIONS="$(CUSTOMIZATIONS) $(CONDITIONALS)" LIBS="$(LIBS)" DEBUG="$(DEBUG)" CC="$(CC)"
	make example ROOT=$(ROOT) EXAMPLE=egmsgsender CUSTOMIZATIONS="$(CUSTOMIZATIONS) $(CONDITIONALS)" LIBS="$(LIBS)" DEBUG="$(DEBUG)" CC="$(CC)"
	make example ROOT=$(ROOT) EXAMPLE=egmsgreceiver CUSTOMIZATIONS="$(CUSTOMIZATIONS) $(CONDITIONALS)" LIBS="$(LIBS)" DEBUG="$(DEBUG)" CC="$(CC)"
	make example ROOT=$(ROOT) EXAMPLE=egmsgpoller CUSTOMIZATIONS="$(CUSTOMIZATIONS) $(CONDITIONALS)" LIBS="$(LIBS)" DEBUG="$(DEBUG)" CC="$(CC)"

## If your target systems supports Sun RPC, you can try
## building the example RPC applications which use parcels
## with remote procedure calls.
egrpc:	egrpcserver egrpcclient

egrpcserver:	egrpcserver.c egrpcserver.h egrpcserver_svc.c
	$(CC) $(CFLAGS) $(CINCS) -o egrpcserver egrpcserver.c egrpcserver_svc.c $(LDLIBS)

egrpcclient:	egrpcclient.c egrpcserver.h egrpcserver_clnt.c
	$(CC) $(CFLAGS) $(CINCS) -o egrpcclient egrpcclient.c egrpcserver_clnt.c $(LDLIBS)

egrpcserver.h egrpcserver_svc.c egrpcserver_clnt.c:	egrpcserver.x
	rpcgen egrpcserver.x

########################################################################
#	utility targets
########################################################################

clean:
	rm -f $(PRIVATE) *.o a.out core makefile technote.out
	-sccs clean

depend:	makefile

makefile:	unbundle Makefile
	cat Makefile > makefile
	$(CC) $(CINCS) -M *.c >> makefile

unbundle:	sccsunbundle
	-sh sccsunbundle

lint:	unbundle
	lint -abchu $(CINCS) *.c

include:
	-mkdir $(INCLUDE)
	for h in $(HEADERS);do make $(INCLUDE)/$$h HEADER=$$h ROOT=$(ROOT);done

lib:	$(PRIVATE)

########################################################################
#	maintenance targets
########################################################################

$(DISTRIBUTION):
	-test -f $@ || sccs get $@

$(INCLUDE)/$(HEADER):	cpdate $(HEADER)
	-mkdir $(INCLUDE)
	-chmod u+w $(INCLUDE)/$(HEADER)
	-sh cpdate $(INCLUDE)/$(HEADER)
	cp $(HEADER) $(INCLUDE)/$(HEADER)
	chmod u-w $(INCLUDE)/$(HEADER)

install-object: $(OBJECT)
	$(AR) $(ARFLAGS) $(PUBLIC) $(OBJECT)
	$(RANLIB) $(PUBLIC)

$(PRIVATE):	$(OBJECTS)
	$(AR) $(ARFLAGS) $(PRIVATE) $(OBJECTS)
	$(RANLIB) $(PRIVATE)

$(PUBLIC):	cpdate $(PRIVATE)
	-mkdir $(LIB)
	-sh cpdate $(PUBLIC)
	cp $(PRIVATE) $(PUBLIC)
	$(RANLIB) $(PUBLIC)

$(EXAMPLE):	$(EXAMPLE).o
	$(CC) $(CFLAGS) $(CINCS) -o $(EXAMPLE) $(EXAMPLE).o $(LDLIBS)

.c.o:
	$(CC) $(CFLAGS) $(CINCS) -c $<

########################################################################
#	miscellaneous targets
########################################################################

get-manpages:	$(MANPAGES)

get-examples:	$(EGSOURCES)

get-sources:	$(SOURCES)

get-headers:	$(HEADERS)

########################################################################
#	Anything after this line is added by ``make depend''
#	and is not part of the production ``Makefile''. If you
#	are editing something past this message, then you are
#	probably editing the wrong file.
########################################################################

