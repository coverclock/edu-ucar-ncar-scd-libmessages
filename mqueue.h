/*
**	M E S S A G E   Q U E U E
**
**	Copyright 1990, 1991 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	@(#)mqueue.h	4.1 94/05/16 jsloan@ncar.ucar.edu
**
*/

#ifndef H_MQUEUE
#define H_MQUEUE

#include "libtvalues.h"
#include "ipc.h"
#include "messages.h"
#include "lists.h"
#include "mproto.h"

#define MQMASK	long
#define MQNULL	0L
#define MQMAX	(BITS(MQMASK)-1)

typedef LIST MQUEUE;

extern void mqueue MPROTO((MQUEUE queue[], int size));

extern void mqfree MPROTO((MQUEUE queue[], int size));

extern MQMASK mqrecv MPROTO((IPCSOCKET sock, MESSAGE **msgp, MQMASK mask, MQUEUE queue[], int (*func)()));

extern MQMASK mqueued MPROTO((MQMASK mask, MQUEUE queue[]));

#endif /* !H_MQUEUE */
