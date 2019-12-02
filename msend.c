/*
**	M S E N D
**
**	Copyright 1989, 1990, 1991, 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Send
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
static char sccsid[]="@(#)msend.c	4.2 96/07/08 jsloan@ncar.ucar.edu";

#ifdef PACKAGE_LIBIPC
static char use_msg_package[]="@(#)msend.c	4.2 96/07/08 Using packages";
#else /* PACKAGE_LIBIPC */
static char use_msg_package[]="@(#)msend.c	4.2 96/07/08 Using packets";
#endif /* PACKAGE_LIBIPC */

#ifdef EBCDIC_LIBIPC
static char use_msg_ebcdic[]="@(#)msend.c	4.2 96/07/08 Using EBCDIC";
#else /* EBCDIC_LIBIPC */
static char use_msg_ebcdic[]="@(#)msend.c	4.2 96/07/08 Using ASCII";
#endif /* EBCDIC_LIBIPC */

#include "libmessages.h"
#include "errors.h"
#include "libtstrings.h"
#include "ipc.h"
#include "mheader.h"
#ifdef PACKAGE_LIBIPC
#include "package.h"
#else /* PACKAGE_LIBIPC */
#include "packet.h"
#endif /* PACKAGE_LIBIPC */
#ifdef EBCDIC_LIBIPC
#include "ebcdic.h"
#endif /* EBCDIC_LIBIPC */

/*
**	msend: send a message structure through a connected socket.
**
**	Arguments:
**		s	connected stream socket
**		mm	pointer to message structure
**
**	Returns:	number of bytes sent,
**			0 if connection gone,
**			<0 if error
*/
int
msend(IPCSOCKET s, MESSAGE *m)
	{
	MHEADER header;
#ifdef PACKAGE_LIBIPC
	PACKAGE *packet;
#else /* PACKAGE_LIBIPC */
	PACKET *packet;
#endif /* PACKAGE_LIBIPC */
	int length;
	int bytes, total;

	if (m==NULL)
		{
		seterrno(EINVAL);
		return(-10);
		}

	if (m->msg_parcel==NULL)
		{
		packet=NULL;
		length=0;
		}
	else
		{
#ifdef PACKAGE_LIBIPC
		if ((packet=unbundlepackage(m->msg_parcel))==NULL)
			return(-11);
		length=strlen(packet)+1;
#else /* PACKAGE_LIBIPC */
		if ((packet=unbundlepacket(m->msg_parcel))==NULL)
			return(-11);
		length=(int)packetlength(packet);
#endif /* PACKAGE_LIBIPC */

#ifdef EBCDIC_LIBIPC
		(void)Ebcdic2Ascii((char *)packet,length);
#endif /* EBCDIC_LIBIPC */
		}

	if (mexheader(length,m,&header)<0)
		{
#ifdef PACKAGE_LIBIPC
		(void)freepackage(packet);
#else /* PACKAGE_LIBIPC */
		(void)freepacket(packet);
#endif /* PACKAGE_LIBIPC */
		return(-12);
		}

	if ((bytes=ipcsend(s,(char *)&header,(sizeof(MHEADER))))<=0)
		{
#ifdef PACKAGE_LIBIPC
		(void)freepackage(packet);
#else /* PACKAGE_LIBIPC */
		(void)freepacket(packet);
#endif /* PACKAGE_LIBIPC */
		return(bytes);
		}

	total=bytes;

	if (length>0)
		if ((bytes=ipcsend(s,(char *)packet,length))<=0)
			{
#ifdef PACKAGE_LIBIPC
			(void)freepackage(packet);
#else /* PACKAGE_LIBIPC */
			(void)freepacket(packet);
#endif /* PACKAGE_LIBIPC */
			return(bytes);
			}
		else
			total+=bytes;

#ifdef PACKAGE_LIBIPC
	(void)freepackage(packet);
#else /* PACKAGE_LIBIPC */
	(void)freepacket(packet);
#endif /* PACKAGE_LIBIPC */

	return(total);
	}
