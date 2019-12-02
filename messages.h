/*
**	M E S S A G E S
**
** Copyright 1990, 1991, 1992 University Corporation for Atmospheric Research
**				All Rights Reserved
**
**	@(#)messages.h	4.1 94/05/16 jsloan@ncar.ucar.edu
*/

#ifndef H_MESSAGES
#define H_MESSAGES

#include "queues.h"
#include "ipc.h"
#include "parcel.h"
#include "pile.h"
#include "mproto.h"

#define MSGNAMELEN	8

struct message
	{
	QUEUENODE	msg_next;		/* for (double) linked list */
	IPCSOCKET	msg_socket;		/* receiving socket */
	IPCADDRESS	msg_address;		/* remote end IP address */
	IPCPORT		msg_port;		/* remote end port */
	int		msg_identity;		/* remote end uid */
	PARCEL		msg_parcel;		/* user parcel */
	long		msg_function;		/* user binary field */
	long		msg_status;		/* user binary field */
	long		msg_id;			/* user binary field */
	long		msg_sequence;		/* user binary field */
	char		msg_name[MSGNAMELEN];	/* user character field */
	};

typedef struct message MESSAGE;

extern IPCSOCKET mserver MPROTO((char *service));

extern IPCSOCKET maccept MPROTO((IPCSOCKET sock));

extern IPCSOCKET mready MPROTO((void));

extern IPCSOCKET mtimed MPROTO((int seconds));

extern IPCSOCKET mclient MPROTO((char *host, char *service));

extern IPCSOCKET mpair MPROTO((IPCSOCKET *sockp));

extern int mclose MPROTO((IPCSOCKET sock));

extern int msend MPROTO((IPCSOCKET sock, MESSAGE *msg));

extern int mrecv MPROTO((IPCSOCKET sock, MESSAGE **msgp));

extern int mtrecv MPROTO((IPCSOCKET sock, MESSAGE **msgp, int seconds));

extern char *mextract MPROTO((MESSAGE *msg, char *key));

extern PILE mvextract MPROTO((MESSAGE *msg));

extern MESSAGE *minsert MPROTO((MESSAGE *msg, char *key, char *value));

extern MESSAGE *mvinsert MPROTO((MESSAGE *msg, PILE block));

extern MESSAGE *mmerge MPROTO((MESSAGE *from, MESSAGE *to));

extern MESSAGE *message MPROTO((long function, long status, long id, long sequence, char *name));

extern void mfree MPROTO((MESSAGE **msgp));

extern void mpool MPROTO((FILE *fp));

extern void mshutdown MPROTO((void));

#endif /* !H_MESSAGES */
