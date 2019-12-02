/*
**	M G E N H E A D E R
**
**	Copyright 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Generic Message Passing Header
**	Program		libipc
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Wed Dec 16 07:32:08 MST 1992
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This function is part of a generic message passing
**	package built on top of the generic IPC package.
*/

static char copyright[]="Copyright 1992, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mgenheader.c	4.3 96/07/02 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include "libtbase.h"
#include "libttypes.h"
#include "libtin.h"
#include "errors.h"
#include "ipc.h"
#include "extract.h"
#include "buffers.h"
#include "mgenheader.h"

/*
**	mgenimheader: imports information from a message header into
**	a regular message. This is NOT intended for application
**	programs.
**
**	N.B.	This routine actually allocates the message structure
**		because it's not until the header is decoded that the
**		message type is known.
*/
int
mgenimheader(IPCSOCKET s, MGENHEADER *h, MGENERIC **mp)
	{
	int length, type;
	MGENERIC *m;

#ifdef DEBUG
	fprintf(stderr,"mgenheader: sock=%d h=0x%lx mp=0x%lx ",s,h,mp);
	if (h!=NULL)
		(void)hexprint(stderr,(char *)h,sizeof(MGENHEADER));
	fputc('\n',stderr);
#endif /* DEBUG */

	if ((h==NULL)||(mp==NULL))
		{
		seterrno(EINVAL);
		return(-1);
		}

	type=ntohl(extract32(h->mgh_type));

	if ((m=mgenalloc(type))==NULL)
		return(-2);

	m->msg_socket=s;
	m->msg_type=type;
	m->msg_identity=ntohl(extract32(h->mgh_identity));
	(void)ipcthatend(s,&(m->msg_address),&(m->msg_port));
	m->msg_body=NULL;

	length=(int)ntohl(extract32(h->mgh_length));

	*mp=m;

	return(length);
	}

/*
**	mgenexheader: export info from a message into a
**	header. This is NOT intended for an application
**	program.
*/
int
mgenexheader(int length, MGENERIC *m, MGENHEADER *h)
	{
	if ((m==NULL)||(h==NULL))
		{
		seterrno(EINVAL);
		return(-1);
		}

	(void)insert32(htonl((libt_unsigned32)m->msg_type),h->mgh_type);
	(void)insert32(htonl((libt_unsigned32)geteuid()),h->mgh_identity);
	(void)insert32(htonl((libt_unsigned32)length),h->mgh_length);

	return(0);
	}
