/*
**	M G E N R E C V
**
**	Copyright 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Generic Message Passing Receive
**	Program		libipc
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Wed Dec 16 08:02:49 MST 1992
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This function is part of the generic message passing
**	package built on top of the generic IPC package.
*/

static char copyright[]="Copyright 1992, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mgenrecv.c	4.1 94/05/16 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include "errors.h"
#include "ipc.h"
#include "buffers.h"
#include "mgenerics.h"
#include "mgeneric.h"
#include "mgenheader.h"

static int
dorecv(IPCSOCKET s, MGENERIC **mp, int seconds)
	{
	int bytes, total, length;
	MGENHEADER header;
	MGENERIC *m;
	BUFFER buffer;

	if (mp==NULL)
		{
		seterrno(EINVAL);
		return(-10);
		}

	total=0;

	if ((bytes=((seconds>=0)?
			ipctrecv(s,(char *)&header,sizeof(MGENHEADER),seconds):
			ipcrecv(s,(char *)&header,sizeof(MGENHEADER))
			)
		)<=0)
		return(bytes);

	total+=bytes;

	/*
	** N.B.	mgenimheader() automatically allocates a message
	**	of the appropriate type.
	*/

	if ((length=mgenimheader(s,&header,&m))<0)
		return(-11);

	if (length>0)
		{
		if ((buffer=bufalloc(length))==NULL)
			{
			(void)mgenfree(m);
			return(-12);
			}

		if ((bytes=((seconds>=0)?ipctrecv(s,buffer,length,seconds):
		                         ipcrecv(s,buffer,length)))<=0)
			{
			(void)buffree(buffer);
			(void)mgenfree(m);
			return(bytes);
			}

		total+=bytes;

		if ((m->msg_body=(*mgtdecapsulate(m->msg_type))(buffer,length))==NULL)
			{
			(void)buffree(buffer);
			(void)mgenfree(m);
			return(-13);
			}

		(void)buffree(buffer);
		}

	*mp=m;

	return(total);
	}

/*
**	mgentrecv: receive a message from a connected socket.
**	Return if the timeout interval elapses first.
**
**	Arguments:
**		s	connected stream socket
**		mp	pointer to generic message pointer
**		seconds	seconds in timeout interval
**
**	Returns:	number of byte received,
**			0 if connection gone, <0 if error
*/
int
mgentrecv(IPCSOCKET s, MGENERIC **mp, int seconds)
	{
	return(dorecv(s,mp,seconds));
	}

/*
**	mgenrecv: receive a message from a connected socket.
**
**	Arguments:
**		s	connected stream socket
**		mp	pointer to generic message pointer
**
**	Returns:	number of byte received,
**			0 if connection gone, <0 if error
*/
int
mgenrecv(IPCSOCKET s, MGENERIC **mp)
	{
	return(dorecv(s,mp,-1));
	}
