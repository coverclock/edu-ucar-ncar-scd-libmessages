/*
**	M E X T R A C T
**
**	Copyright 1989, 1990, 1991, 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Extract
**	Program		libipc
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Mon Oct  2 16:39:05 MDT 1989
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This function is part of a message passing
**	package built on top of the generic IPC package.
*/

static char copyright[]="Copyright 1989, 1990, 1991, 1992, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mextract.c	4.1 94/05/16 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include "errors.h"
#include "parcel.h"

/*
**	mextract: extract from a MESSAGE structure the value string
**	corresponding to a key string.
**
**	Arguments:
**		msg	pointer to MESSAGE structure
**		key	pointer to key string
**
**	Returns:	pointer to value string if found, NULL otherwise
*/
char *
mextract(MESSAGE *msg, char *key)
	{
	if (msg==NULL)
		{
		seterrno(EINVAL);
		return(NULL);
		}
	return(unbundle(key,msg->msg_parcel));
	}
