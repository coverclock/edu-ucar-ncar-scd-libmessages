/*
**	M G E N P A C K E T
**
**	Copyright 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Generic Packet Support
**	Program		libipc
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Tue Dec 15 15:36:32 MST 1992
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This module contains the functions necessary to support
**	packet-based generic messages.
**
**	N.B.:	the module refers to "packet" even through the
**		data structure involved is a parcel because the
**		parcel is encapsulated as a pacet (versus a
**		package).
*/

static char copyright[]="Copyright 1992, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mgenpacket.c	4.2 96/07/08 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include "errors.h"
#include "buffers.h"
#include "packet.h"
#ifdef EBCDIC_LIBIPC
#include "ebcdic.h"
#endif /* EBCDIC_LIBIPC */

/*
**	mgfrepacket: free a parcel inside a generic message.
*/
int
mgfrepacket(void *vp)
	{
	return(freeparcel((PARCEL)vp));
	}

/*
**	mgencpacket: encapsulate a parcel into a packet for
**	insertion into a generic message.
*/
BUFFER
mgencpacket(void *vp, int *lengthp)
	{
	PACKET *packet;

	if (lengthp==NULL)
		{
		seterrno(EINVAL);
		return(NULL);
		}

	if ((packet=unbundlepacket((PARCEL)vp))!=NULL)
		{
		*lengthp=(int)packetlength(packet);
#ifdef EBCDIC_LIBIPC
		(void)ebcdic2ascii((char *)packet,*lengthp);
#endif /* EBCDIC_LIBIPC */
		}

	return((BUFFER)packet);
	}

/*
**	mgdecpacket: decapsulate a packet from a generic
**	message into a parcel.
*/
void *
mgdecpacket(BUFFER buffer, int length)
	{
	PARCEL parcel;

	if (buffer==NULL)
		{
		seterrno(EINVAL);
		return(NULL);
		}

#ifdef EBCDIC_LIBIPC
	(void)ascii2ebcdic(buffer,length);
#endif /* EBCDIC_LIBIPC */

	(void)initparcel(&parcel);

	(void)bundlepacket((PACKET *)buffer,&parcel);

	return((void *)parcel);
	}
