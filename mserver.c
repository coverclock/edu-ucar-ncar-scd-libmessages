/*
**	M S E R V E R
**
**	Copyright 1989, 1990, 1991, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Server
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
static char sccsid[]="@(#)mserver.c	4.1 94/05/16 jsloan@ncar.ucar.edu";

#include "libmessages.h"

/*
**	mserver: establish a connected socket bound to a specified
**	service.
**
**	Arguments:
**		service	pointer to string containing service name
**			or port number.
**
**	Returns:	connected stream socket, or <0 if error
*/
IPCSOCKET
mserver(char *service)
	{
	IPCPORT port;

	if ((port=ipcport(service))==0)
		return(-10);
	return(ipcserver(port));
	}
