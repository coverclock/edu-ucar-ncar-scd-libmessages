/*
**	M V I N S E R T
**
**	Copyright 1989, 1990, 1991, 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Vector Insert
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
static char sccsid[]="@(#)mvinsert.c	4.1 94/05/16 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include "errors.h"
#include "parcel.h"
#include "pile.h"

/*
**	mvinsert: insert into a MESSAGE structure all parameters
**	from an argv-like structure.
**
**	Arguments:
**		msg	pointer to MESSAGE structure
**		block	pointer to argv-thing
**
**	Returns:	pointer to MESSAGE structure
*/
MESSAGE *
mvinsert(msg,block)
MESSAGE *msg;
PILE block;
	{
	if (msg==NULL)
		{
		seterrno(EINVAL);
		return(NULL);
		}
	if (bundlepile(block,&msg->msg_parcel)==NULL)
		return(NULL);
	return(msg);
	}
