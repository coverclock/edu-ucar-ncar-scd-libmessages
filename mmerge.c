/*
**	M M E R G E
**
**	Copyright 1989, 1990, 1991, 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Merge
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
static char sccsid[]="@(#)mmerge.c	4.1 94/05/16 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include "errors.h"
#include "parcel.h"

/*
**	mmerge:	Merge the key=value pairs from one MESSAGE structure
**	into another.
**
**	Arguments:
**		from	pointer to source MESSAGE structure
**		top	address of pointer to destination MESSAGE structure
**
**	Returns:	pointer to destination MESSAGE or NULL if error
*/
MESSAGE *
mmerge(MESSAGE *from, MESSAGE *to)
	{
	if (to==NULL)
		{
		seterrno(EINVAL);
		return(NULL);
		}
	if (rebundle(from->msg_parcel,&(to->msg_parcel))==NULL)
		return(NULL);
	return(to);
	}
