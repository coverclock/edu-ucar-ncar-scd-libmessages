/*
**	M G E N P A C K A G E
**
**	Copyright 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Generic Package Support
**	Program		libipc
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Tue Dec 15 13:58:01 MST 1992
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This module contains the functions necessary to support
**	package-based generic messages.
**
**	N.B.:	the module refers to "package" even through the
**		data structure involved is a parcel because the
**		parcel is encapsulated as a package (versus a
**		packet).
*/

static char copyright[]="Copyright 1992, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mgenpackage.c	4.1 94/05/16 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include "errors.h"
#include "libtstrings.h"
#include "buffers.h"
#include "package.h"
#ifdef EBCDIC_LIBIPC
#include "ebcdic.h"
#endif /* EBCDIC_LIBIPC */

/*
**	mgfrepackage: free a parcel inside a generic message.
*/
int
mgfrepackage(void *vp)
	{
	return(freeparcel((PARCEL)vp));
	}

/*
**	mgencpackage: encapsulate a parcel into a package for
**	insertion into a generic message.
*/
BUFFER
mgencpackage(void *vp, int *lengthp)
	{
	PACKAGE *package;

	if (lengthp==NULL)
		{
		seterrno(EINVAL);
		return(NULL);
		}

	if ((package=unbundlepackage((PARCEL)vp))!=NULL)
		{
		*lengthp=strlen(package)+1;
#ifdef EBCDIC_LIBIPC
		(void)ebcdic2ascii((char *)package,*lengthp);
#endif /* EBCDIC_LIBIPC */
		}

	return((BUFFER)package);
	}

/*
**	mgdecpackage: decapsulate a package from a generic
**	message into a parcel.
*/
void *
mgdecpackage(BUFFER buffer, int length)
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

	(void)bundlepackage((PACKAGE *)buffer,&parcel);

	return((void *)parcel);
	}
