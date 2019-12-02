/*
**	M G E N E R I C S
**
**	Copyright 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Generic Message Passing Table
**	Program		libipc
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Tue Dec 15 11:47:06 MST 1992
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This module contains the supporting routines for passing
**	generic structures as messages.
*/

static char copyright[]="Copyright 1992, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mgenerics.c	4.2 94/05/17 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include "errors.h"
#include "buffers.h"
#include "serialize.h"
#include "mgenerics.h"

/*
**	These are the header files for the predefined generic
**	transformation functions.
*/
#include "mgenpackage.h"
#include "mgenpacket.h"

/*
**	These functions are the default functions pointed
**	to by the available entries in the mgentable.
*/
static int
deffree(void *vp)
	{
	seterrno(EINVAL);
	return(-1);
	}

static BUFFER
defencapsulate(void *vp, int *lengthp)
	{
	seterrno(EINVAL);
	return(NULL);
	}

static void *
defdecapsulate(BUFFER buffer, int length)
	{
	seterrno(EINVAL);
	return(NULL);
	}

/*
**	This structure stores pointers to functions that
**	implement the free, encapsulate and
**	decapsulate functions for a particular data type
**	for generic message passing.
*/
struct mgentable
	{
	int	(*mgt_free)(void *);
	BUFFER	(*mgt_encapsulate)(void *, int *);
	void *	(*mgt_decapsulate)(BUFFER, int);
	};

/*
**	This table contains mgentable structures for
**	generic message passing. It is indexed by
**	message type. Some entries are available to
**	be redefined; in the meantime, they point to
**	default routines that return error.
**
**	N.B.	This is a reader-writer problem that
**		I haven't really solved. Simultaneous
**		reads are allowed, and writes are
**		serialized. But simultaneous reads and
**		writes are not protected against.
*/
static struct mgentable mgenerics[]=	/* thread safe: sem_mgenerics_libm */
	{
	{ mgfrepackage,	mgencpackage,	mgdecpackage	}, /* package */
	{ mgfrepacket,	mgencpacket,	mgdecpacket	}, /* packet */
	{ deffree,	defencapsulate,	defdecapsulate	}, /* unused */
	{ deffree,	defencapsulate,	defdecapsulate	}, /* unused */
	{ deffree,	defencapsulate,	defdecapsulate	}, /* unused */
	};

static int mgensize=sizeof(mgenerics)/sizeof(struct mgentable);

#define MGTVALID(type)	((0<=(type))&&((type)<mgensize))

/*
**	mgenregister: registers the the functions to support a
**	particular structure so that it may be passed in a generic
**	message.
*/
int
mgenregister(int type, BUFFER (*encapf)(), void *(*decapf)(), int (*freef)())
	{
	if (!MGTVALID(type))
		{
		seterrno(EINVAL);
		return(-1);
		}

	P(sem_mgenerics_libm);
	mgenerics[type].mgt_free=freef;
	mgenerics[type].mgt_encapsulate=encapf;
	mgenerics[type].mgt_decapsulate=decapf;
	V(sem_mgenerics_libm);

	return(type);
	}

/*
**	These functions map a type code into the address of a function.
**	If the type code is out of range, the default function is
**	returned.
*/
int
(*mgtfree(int type))(void *)
	{
	return(MGTVALID(type)?mgenerics[type].mgt_free:deffree);
	}

BUFFER
(*mgtencapsulate(int type))(void *, int *)
	{
	return(MGTVALID(type)?mgenerics[type].mgt_encapsulate:defencapsulate);
	}

void *
(*mgtdecapsulate(int type))(BUFFER, int)
	{
	return(MGTVALID(type)?mgenerics[type].mgt_decapsulate:defdecapsulate);
	}
