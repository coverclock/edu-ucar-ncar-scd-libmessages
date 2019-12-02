/*
**	M P A I R
**
**	Copyright 1989, 1990, 1991, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Pair
**	Program		TAGS libipc
**	Project		Text And Graphics System
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Wed Oct 25 16:41:54 MDT 1989
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This function is part of a TAGS-specific message passing
**	package built on top of the generic IPC package.
*/

static char copyright[]="Copyright 1989, 1990, 1991, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mpair.c	4.1 94/05/16 jsloan@ncar.ucar.edu";

#include "libmessages.h"

/*
**	mpair: return a pair of connected sockets.
**
**	Arguments:
**		sockp	value/result parameter into which the
**			"unregistered" (unknown to mready())
**			socket is placed.
**
**	Returns:	connected socket or <0 if error
*/
IPCSOCKET
mpair(IPCSOCKET *sockp)
	{
	return(ipcpair(sockp));
	}
