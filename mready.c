/*
**	M R E A D Y
**
**	Copyright 1989, 1990, 1991, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Ready
**	Program		TAGS libipc
**	Project		Text And Graphics System
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Tue Oct 10 10:12:41 MDT 1989
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This function is part of a TAGS-specific message passing
**	package built on top of the generic IPC package.
*/

static char copyright[]="Copyright 1989, 1990, 1991, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mready.c	4.1 94/05/16 jsloan@ncar.ucar.edu";

#include "libmessages.h"

/*
**	mready: wait for and return next socket that is ready for
**	reading.
**
**	Returns:	ready socket or <0 if error
*/
IPCSOCKET
mready()
	{
	return(ipcready());
	}
