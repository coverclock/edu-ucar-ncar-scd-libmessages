/*
**	M V E X T R A C T
**
**	Copyright 1989, 1990, 1991, 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Vector Extract
**	Program		libipc
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Thu Oct  5 12:58:47 MDT 1989
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This function is part of a message passing
**	package built on top of the generic IPC package.
*/

static char copyright[]="Copyright 1989, 1990, 1991, 1992, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mvextract.c	4.1 94/05/16 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include "errors.h"
#include "parcel.h"
#include "pile.h"

/*
**	mvextract: extract from a MESSAGE structure all parameters
**	and return in an argv-like structure.
**
**	Arguments:
**		msg	pointer to MESSAGE structure
**
**	Returns:	pointer to argv-thing
*/
PILE
mvextract(MESSAGE *msg)
	{
	if (msg==NULL)
		{
		seterrno(EINVAL);
		return(NULL);
		}
	return(unbundlepile(msg->msg_parcel));
	}
