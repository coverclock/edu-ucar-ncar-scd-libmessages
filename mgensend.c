/*
**	M G E N S E N D
**
**	Copyright 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Generic Message Passing Send
**	Program		libipc
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Wed Dec 16 08:01:49 MST 1992
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This function is part of the generic message passing
**	package built on top of the generic IPC package.
*/

static char copyright[]="Copyright 1992, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mgensend.c	4.1 94/05/16 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include "errors.h"
#include "ipc.h"
#include "buffers.h"
#include "mgeneric.h"
#include "mgenerics.h"
#include "mgenheader.h"

/*
**	mgensend: send a generic message through a connected socket.
**
**	Arguments:
**		s	connected stream socket
**		m	pointer to generic message
**
**	Returns:	number of bytes sent,
**			0 if connection gone,
**			<0 if error
*/
int
mgensend(IPCSOCKET s, MGENERIC *m)
	{
	MGENHEADER header;
	BUFFER b;
	int length;
	int bytes, total;

	if (m==NULL)
		{
		seterrno(EINVAL);
		return(-10);
		}

	if ((b=(*mgtencapsulate(m->msg_type))(m->msg_body,&length))==NULL)
		return(-11);

	if (mgenexheader(length,m,&header)<0)
		{
		(void)buffree(b);
		return(-12);
		}

	if ((bytes=ipcsend(s,(char *)&header,(sizeof(MGENHEADER))))<=0)
		{
		(void)buffree(b);
		return(bytes);
		}

	total=bytes;

	if (length>0)
		if ((bytes=ipcsend(s,b,length))<=0)
			{
			(void)buffree(b);
			return(bytes);
			}
		else
			total+=bytes;

	(void)buffree(b);

	return(total);
	}
