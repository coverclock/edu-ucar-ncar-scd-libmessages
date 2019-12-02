/*
**	M R E C V
**
**	Copyright 1989, 1990, 1991, 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Receive
**	Program		libipc
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Mon Oct  2 16:39:05 MDT 1989
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This function is part of a TAGS-specific message passing
**	package built on top of the generic IPC package.
*/

static char copyright[]="Copyright 1989, 1990, 1991, 1992, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mrecv.c	4.2 94/06/03 jsloan@ncar.ucar.edu";

#ifdef PACKAGE_LIBIPC
static char use_msg_package[]="@(#)mrecv.c	4.2 94/06/03 Using packages";
#else /* PACKAGE_LIBIPC */
static char use_msg_package[]="@(#)mrecv.c	4.2 94/06/03 Using packets";
#endif /* PACKAGE_LIBIPC */

#ifdef EBCDIC_LIBIPC
static char use_msg_ebcdic[]="@(#)mrecv.c	4.2 94/06/03 Using EBCDIC";
#else /* EBCDIC_LIBIPC */
static char use_msg_ebcdic[]="@(#)mrecv.c	4.2 94/06/03 Using ASCII";
#endif /* EBCDIC_LIBIPC */

#include "libmessages.h"
#include "errors.h"
#include "mheader.h"
#ifdef PACKAGE_LIBIPC
#include "package.h"
#else /* PACKAGE_LIBIPCC */
#include "packet.h"
#endif /* PACKAGE_LIBIPC */
#include "buffers.h"
#ifdef EBCDIC_LIBIPC
#include "ebcdic.h"
#endif /* EBCDIC_LIBIPC */
#ifdef DEBUG
#include "hexprint.h"
#endif /* DEBUG */

static int
mgenrecv(IPCSOCKET s, MESSAGE **mp, int seconds)
	{
	BUFFER buffer;
	MESSAGE *m;
	int bytes, total, length;
	MHEADER header;

	if (mp==NULL)
		{
		seterrno(EINVAL);
		return(-10);
		}

	total=0;

	if ((bytes=((seconds>=0)?
			ipctrecv(s,(char *)&header,sizeof(header),seconds):
			ipcrecv(s,(char *)&header,sizeof(header))
			)
		)<=0)
		return(bytes);

#ifdef DEBUG
	fprintf(stderr,"mrecv: header[%d] identity(0x",bytes);
	(void)hexprint(stderr,header.mh_identity,sizeof(header.mh_identity));
	fputs(") length(0x",stderr);
	(void)hexprint(stderr,header.mh_length,sizeof(header.mh_length));
	fputs(") function(0x",stderr);
	(void)hexprint(stderr,header.mh_function,sizeof(header.mh_function));
	fputs(") status(0x",stderr);
	(void)hexprint(stderr,header.mh_status,sizeof(header.mh_status));
	fputs(") id(0x",stderr);
	(void)hexprint(stderr,header.mh_id,sizeof(header.mh_id));
	fputs(") sequence(0x",stderr);
	(void)hexprint(stderr,header.mh_sequence,sizeof(header.mh_sequence));
	fputs(") name(0x",stderr);
	(void)hexprint(stderr,header.mh_name,sizeof(header.mh_name));
	fputs(")\n",stderr);
#endif /* DEBUG */

	total+=bytes;

	if ((length=mimheader(s,&header,mp))<0)
		return(-11);
	m=(*mp);

	if (length>0)
		{
#ifdef DEBUG
		fprintf(stderr,"mrecv: data[%d] data(",length);
#endif /* DEBUG */

		if ((buffer=bufalloc(length))==NULL)
			return(-12);

		if ((bytes=((seconds>=0)?ipctrecv(s,buffer,length,seconds):
		                         ipcrecv(s,buffer,length)))<=0)
			{
			(void)buffree(buffer);
			return(bytes);
			}

		total+=bytes;

#ifdef EBCDIC_LIBIPC
		(void)Ascii2Ebcdic(buffer,length);
#endif /* EBCDIC_LIBIPC */

#ifdef DEBUG
		hexprint(stderr,buffer,(-length));
		fputc('\n',stderr);
#endif /* DEBUG */

#ifdef PACKAGE_LIBIPC
		(void)bundlepackage((PACKAGE *)buffer,&m->msg_parcel);
#else /* PACKAGE_LIBIPC */
		(void)bundlepacket((PACKET *)buffer,&m->msg_parcel);
#endif /* PACKAGE_LIBIPC */

		(void)buffree(buffer);

		}

	return(total);
	}

/*
**	mtrecv: receive a message from a connected socket. The
**	message structure is allocated if one does not already
**	exist (*mp==NULL) or the memory associated with the
**	old structure is released as necessary. Return if the
**	timeout interval elapses first.
**
**	Arguments:
**		s	connected stream socket
**		mp	(value/return) address of pointer
**			to MESSAGE structure
**		seconds	seconds in timeout interval
**
**	Returns:	number of byte received,
**			0 if connection gone, <0 if error
*/
int
mtrecv(IPCSOCKET s, MESSAGE **mp, int seconds)
	{
	return(mgenrecv(s,mp,seconds));
	}

/*
**	mrecv: receive a message from a connected socket. The
**	message structure is allocated if one does not already
**	exist (*mp==NULL) or the memory associated with the
**	old structure is released as necessary.
**
**	Arguments:
**		s	connected stream socket
**		mp	(value/return) address of pointer
**			to MESSAGE structure
**
**	Returns:	number of byte received,
**			0 if connection gone, <0 if error
*/
int
mrecv(IPCSOCKET s, MESSAGE **mp)
	{
	return(mgenrecv(s,mp,-1));
	}
