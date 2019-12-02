/*
**	M H E A D E R
**
**	Copyright 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Header
**	Program		libipc
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Wed Dec  9 12:32:33 MST 1992
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This function is part of a message passing
**	package built on top of the generic IPC package.
*/

static char copyright[]="Copyright 1992, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mheader.c	4.3 96/07/02 jsloan@ncar.ucar.edu";

#ifdef PACKAGE_LIBIPC
static char use_msg_package[]="@(#)mheader.c	4.3 96/07/02 Using packages";
#else /* PACKAGE_LIBIPC */
static char use_msg_package[]="@(#)mheader.c	4.3 96/07/02 Using packets";
#endif /* PACKAGE_LIBIPC */

#ifdef EBCDIC_LIBIPC
static char use_msg_ebcdic[]="@(#)mheader.c	4.3 96/07/02 Using EBCDIC";
#else /* EBCDIC_LIBIPC */
static char use_msg_ebcdic[]="@(#)mheader.c	4.3 96/07/02 Using ASCII";
#endif /* EBCDIC_LIBIPC */

#include "libmessages.h"
#include "libtbase.h"
#include "libttypes.h"
#include "libtin.h"
#include "errors.h"
#include "parcels.h"
#include "extract.h"
#include "buffers.h"
#include "mheader.h"
#ifdef EBCDIC_LIBIPC
#include "ebcdic.h"
#endif /* EBCDIC_LIBIPC */

/*
**	mimheader: imports information from a message header into
**	a regular message. This is NOT intended for application
**	programs.
*/
int
mimheader(IPCSOCKET s, MHEADER *h, MESSAGE **mp)
	{
	MESSAGE *m;
	int length;

#ifdef DEBUG
	fprintf(stderr,"mheader: sock=%d h=0x%lx mp=0x%lx ",s,h,mp);
	if (h!=NULL)
		(void)hexprint(stderr,(char *)h,sizeof(MHEADER));
	fputc('\n',stderr);
#endif /* DEBUG */

	if ((h==NULL)||(mp==NULL))
		{
		seterrno(EINVAL);
		return(-1);
		}

	if (*mp==NULL)
		{
		if ((m=message(0L,0L,0L,0L,NULL))==NULL)
			return(-2);
		else
			*mp=m;
		}
	else
		{
		m=(*mp);
		if (m->msg_parcel!=NULL)
			{
			(void)freeparcel(m->msg_parcel);
			(void)initparcel(&m->msg_parcel);
			}
		}

	m->msg_socket=s;

	(void)ipcthatend(s,&(m->msg_address),&(m->msg_port));
	m->msg_identity=ntohl(extract32(h->mh_identity));

	m->msg_function=ntohl(extract32(h->mh_function));
	m->msg_status=ntohl(extract32(h->mh_status));
	m->msg_id=ntohl(extract32(h->mh_id));
	m->msg_sequence=ntohl(extract32(h->mh_sequence));
	(void)bcopy(h->mh_name,m->msg_name,MSGNAMELEN);

#ifdef EBCDIC_LIBIPC
	(void)Ascii2Ebcdic((char *)m->msg_name,MSGNAMELEN);
#endif /* EBCDIC_LIBIPC */

	length=ntohl(extract32(h->mh_length));

	return(length);
	}

/*
**	mexheader: export info from a message into a
**	header. This is NOT intended for an application
**	program.
*/
int
mexheader(int l, MESSAGE *m, MHEADER *h)
	{
	if ((m==NULL)||(h==NULL))
		{
		seterrno(EINVAL);
		return(-1);
		}

	(void)insert32(htonl((libt_unsigned32)m->msg_function),h->mh_function);
	(void)insert32(htonl((libt_unsigned32)m->msg_status),h->mh_status);
	(void)insert32(htonl((libt_unsigned32)m->msg_id),h->mh_id);
	(void)insert32(htonl((libt_unsigned32)m->msg_sequence),h->mh_sequence);
	(void)bcopy(m->msg_name,h->mh_name,MSGNAMELEN);

#ifdef EBCDIC_LIBIPC
	(void)Ebcdic2Ascii(h->mh_name,MSGNAMELEN);
#endif /* EBCDIC_LIBIPC */

	(void)insert32(htonl((libt_unsigned32)geteuid()),h->mh_identity);

	(void)insert32(htonl((libt_unsigned32)l),h->mh_length);

	return(0);
	}
