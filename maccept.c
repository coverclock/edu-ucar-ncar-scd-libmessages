/*
**	M A C C E P T
**
**	Copyright 1989, 1990, 1991, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Accept
**	Program		TAGS libipc
**	Project		Text And Graphics System
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Mon Oct  2 16:39:05 MDT 1989
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This function is part of a TAGS-specific message passing
**	package built on top of the generic IPC package.
*/

static char copyright[]="Copyright 1989, 1990, 1991, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)maccept.c	4.1 94/05/16 jsloan@ncar.ucar.edu";

#include "libmessages.h"

/*
**	maccept: accept a connection request to an open socket,
**	returning an identical socket with which to actually service
**	the connection.
**
**	Arguments:
**		sock	unconnected but bound socket
**
**	Returns:	new socket identical to original or <0 if error
*/
IPCSOCKET
maccept(IPCSOCKET sock)
	{
	IPCSOCKET newsock;

	if ((newsock=ipcaccept(sock))<0)
		return(-1);
#ifdef DEBUG
	fprintf(stderr,"maccept: socket=%d, newsocket=%d\n",sock,newsock);
#endif
	return(newsock);
	}
