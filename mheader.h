/*
**	M H E A D E R
**
**	Copyright 1992 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	@(#)mheader.h	4.1 94/05/16 jsloan@ncar.ucar.edu
*/

#ifndef H_MHEADER
#define H_MHEADER

#include "ipc.h"
#include "messages.h"
#include "mproto.h"

#define MSGSHORT	(16/8)
#define MSGLONG		(32/8)

struct mheader
	{
	char	mh_identity[MSGLONG];
	char	mh_length[MSGLONG];
	char	mh_function[MSGLONG];
	char	mh_status[MSGLONG];
	char	mh_id[MSGLONG];
	char	mh_sequence[MSGLONG];
	char	mh_name[MSGNAMELEN];
	};

typedef struct mheader MHEADER;

extern int mimheader MPROTO((IPCSOCKET s, MHEADER *header, MESSAGE **msgp));

extern int mexheader MPROTO((int length, MESSAGE *msg, MHEADER *header));

#endif /* !H_MHEADER */
