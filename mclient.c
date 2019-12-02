/*
**	M C L I E N T
**
**	Copyright 1989, 1990, 1991, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Client
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
static char sccsid[]="@(#)mclient.c	4.1 94/05/16 jsloan@ncar.ucar.edu";

#include "libmessages.h"

/*
**	mclient: return a socket connected to the specified host and
**	service.
**
**	Arguments:
**		host	string identifying host (name or address)
**		service	string identifying service (service name or port
**			number)
**
**	Returns:	connected socket or <0 if error
*/
IPCSOCKET
mclient(char *host, char *service)
	{
	IPCADDRESS address;
	IPCPORT port;

	if ((port=ipcport(service))==0)
		return(-10);
	if ((address=ipcaddress(host))==0L)
		return(-11);
	return(ipcclient(address,port));
	}
