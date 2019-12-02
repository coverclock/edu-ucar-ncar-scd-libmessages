/*
**	M S E R V I C E
**
**	Copyright 1992 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	@(#)mservice.h	4.1 94/05/16 jsloan@ncar.ucar.edu
**
*/

#ifndef H_MSERVICE
#define H_MSERVICE

#include <stdio.h>
#include "ipc.h"
#include "messages.h"
#include "mproto.h"

/*
**	Using this as a socket descriptor n msrecv() or mspoll()
**	means select message from any socket.
*/
#define MSVC_ANY	(-1)

/*
** These are the state values which msstate may return.
*/
#define MSVC_CLOSED	0
#define MSVC_ACCEPTING	1
#define	MSVC_IDLE	2
#define MSVC_BUSY	3
#define MSVC_FAILED	4
#define MSVC_OTHER	5

extern int msfree MPROTO((IPCSOCKET sock));

extern int mscreate MPROTO((IPCSOCKET sock));

extern int msclose MPROTO((IPCSOCKET sock));

extern int mservice MPROTO((IPCSOCKET sock));

extern int mspoll MPROTO((IPCSOCKET sock, MESSAGE **mp));

extern int msrecv MPROTO((IPCSOCKET sock, MESSAGE **mp));

extern int msstate MPROTO((IPCSOCKET sock));

extern int mscount MPROTO((IPCSOCKET sock));

extern void msshutdown MPROTO((void));

extern void mspool MPROTO((FILE *fp));

#endif /* !H_MSERVICE */
