/*
**	M I N S E R T
**
**	Copyright 1989, 1990, 1991, 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Insert
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
static char sccsid[]="@(#)minsert.c	4.1 94/05/16 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include "errors.h"
#include "parcel.h"

/*
**	minsert: insert/change/delete a key=value pair into a
**	MESSAGE structure. Used to create a new message or modify
**	an existing message prior to sending.
**
**	Arguments:
**		msg	pointer to MESSAGE structure
**		key	pointer to key string
**		value	pointer to value string
**
**	Returns:	pointer to MESSAGE structure or NULL if error
*/
MESSAGE *
minsert(MESSAGE *msg, char *key, char *value)
	{
	if ((msg==NULL)||(key==NULL))
		{
		seterrno(EINVAL);
		return(NULL);
		}
	if (bundle(key,value,&msg->msg_parcel)==NULL)
		return(NULL);
	return(msg);
	}
