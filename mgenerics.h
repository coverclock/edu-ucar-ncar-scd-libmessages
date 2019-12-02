/*
**	M G E N E R I C S
**
**	Copyright 1992	University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	@(#)mgenerics.h	4.1 94/05/16 jsloan@ncar.ucar.edu
**
*/

#ifndef H_MGENERICS
#define H_MGENERICS

#include "buffers.h"
#include "mproto.h"

/*
**	These message types are predefined.
*/
#define	MGEN_PACKAGE		(0)
#define MGEN_PACKET		(1)

extern int mgenregister MPROTO((int type, BUFFER (*encapf)(), void *(*decapf)(),  int (*freef)()));

extern int (*mgtfree MPROTO((int type)))();

extern BUFFER (*mgtencapsulate MPROTO((int type)))();

extern void *(*mgtdecapsulate MPROTO((int type)))();

#endif /* !H_MGENERICS */
