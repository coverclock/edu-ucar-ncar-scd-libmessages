/*
**	M G E N E R I C
**
**	Copyright 1992	University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	@(#)mgeneric.h	4.1 94/05/16 jsloan@ncar.ucar.edu
**
*/

#ifndef H_MGENERIC
#define H_MGENERIC

#include <stdio.h>
#include "queues.h"
#include "ipc.h"
#include "mproto.h"

#define MGENLOCALHOST	((IPCADDRESS)(0x7f000001))

struct mgeneric
	{
	QUEUENODE	msg_next;		/* for (double) linked list */
	int		msg_type;		/* message type */
	IPCSOCKET	msg_socket;		/* receiving socket */
	IPCADDRESS	msg_address;		/* remote end IP address */
	IPCPORT		msg_port;		/* remote end port */
	int		msg_identity;		/* remote end uid */
	void *		msg_body;		/* user structure */
	};

typedef struct mgeneric MGENERIC;

extern MGENERIC *mgenalloc MPROTO((int type));

extern int mgenfree MPROTO((MGENERIC *msg));

extern void mgenpool MPROTO((FILE *fp));

extern void mgenshutdown MPROTO((void));

extern int mgensend MPROTO((IPCSOCKET sock, MGENERIC *msg));

extern int mgenrecv MPROTO((IPCSOCKET sock, MGENERIC **msgp));

extern int mgentrecv MPROTO((IPCSOCKET sock, MGENERIC **msgp, int seconds));

#endif /* !H_MGENERIC */
