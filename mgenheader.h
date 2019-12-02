/*
**	M G E N H E A D E R
**
**	Copyright 1992 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	@(#)mgenheader.h	4.1 94/05/16 jsloan@ncar.ucar.edu
*/

#ifndef H_MGENHEADER
#define H_MGENHEADER

#include "ipc.h"
#include "mgeneric.h"
#include "mproto.h"

#define MSGSHORT	(16/8)
#define MSGLONG		(32/8)

struct mgenheader
	{
	char	mgh_type[MSGLONG];
	char	mgh_identity[MSGLONG];
	char	mgh_length[MSGLONG];
	};

typedef struct mgenheader MGENHEADER;

extern int mgenimheader MPROTO((IPCSOCKET s, MGENHEADER *header, MGENERIC **mp));

extern int mgenexheader MPROTO((int length, MGENERIC *m, MGENHEADER *header));

#endif /* !H_MGENHEADER */
