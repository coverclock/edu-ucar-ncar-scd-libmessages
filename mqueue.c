/*
**	M E S S A G E   Q U E U E
**
**	Copyright 1990, 1991, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Queue
**	Program		TAGS
**	Project		Text And Graphics System
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Fri Aug 24 14:48:54 MDT 1990
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	If complex message passing applications, often a process wants
**	to receive only a particular flavor of message, not just the
**	first message that happens to be available on the socket.
**	This package implements a new receive function that takes
**	a bit mask that specifies what message flavors are acceptable.
**	Messages of the wrong flavor are queued on a linked list until
**	their flavors are requested. The flavor of a message is
**	identified by a caller-supplied characteristic function.
*/

static char copyright[]="Copyright 1990, 1991, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mqueue.c	4.1 94/05/16 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include "libtvalues.h"
#include "mqueue.h"

#define Q(i)	(&(queue[i]))
#define M       MESSAGE *

/*
**	mqueue: Initialize a queue array containing size queues
**	(that is, enough queues to hold size different message
**	flavors).
*/
void
mqueue(MQUEUE queue[], int size)
	{
	for (--size; size>=0; size--)
		(void)linit(Q(size));
	}

/*
**	mqfree: free all messages on an array of size queues.
*/
void
mqfree(MQUEUE queue[], int size)
	{
	MESSAGE *m;

	for (--size; size>=0; size--)
		for (m=(M)lget(Q(size)); m!=NULL; m=(M)lget(Q(size)))
			mfree(&m);
	}

/*
**	mqueued: return true if there is a message matching the
**	specified mask already queued. This can be used to establish
**	different priorities on message types.
*/
MQMASK
mqueued(register MQMASK mask, MQUEUE queue[])
	{
	register int i;
	register MQMASK try;
	register MQMASK found;

	found=MQNULL;
	if (mask!=MQNULL)
		for (i=0; i<MQMAX; i++)
			if (mask&(try=(1<<i)))
				if ((M)lpeek(Q(i))!=NULL)
					found|=try;
	return(found);
	}

/*
**	mqrecv: receive a message of a specific flavor.
**
**	Arguments:
**		sock	socket
**		msgp	address of message pointer
**		mask	mask indicating by the presence of a 1 bit
**			which message flavor(s) out of BITS(mask)
**			are acceptable
**		queue	array of queues
**		func	characteristic function of signature
**				int func(MESSAGE *m);
**			which returns an 0<=integer<=(BITS(mask)-1)
**			indicating the flavor of message m
**			
**	Returns:
**		<0	error
**		=0	connection list
**		>0	mask of returned message
*/
MQMASK
mqrecv(IPCSOCKET sock, MESSAGE **mp, register MQMASK mask, MQUEUE queue[], int (*func)())
	{
	MESSAGE *m;
	int rc;
	register int i;
	register MQMASK found;

	found=MQNULL;
	if (mask!=MQNULL)
		{
		for (m=NULL;;)
			{
			for (i=0; i<MQMAX; i++)
				if (mask&(found=(1<<i)))
					if ((m=(M)lget(Q(i)))!=NULL)
						break;
			if (m!=NULL)
				break;
			if ((rc=mrecv(sock,&m))<=0)
				return(rc);
			if ((i=(*func)(m))<0)
				return(i);
			(void)lput(Q(i),(LISTNODE *)m);
			}
		if (*mp!=NULL)
			(void)mfree(mp);
		*mp=m;
		}
	return(found);
	}
