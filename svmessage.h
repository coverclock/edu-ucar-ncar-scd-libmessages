/*
**	S Y S T E M   V   M E S S A G E   I P C   H E A D E R
**
**	Copyright 1990, 1991 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	@(#)svmessage.h	4.1 94/05/16 jsloan@ncar.ucar.edu
**
*/

#ifndef H_SVMESSAGES
#define H_SVMESSAGES

#include "libttypes.h"
#include "mproto.h"

/*
**	All messages are assumed to have this header immediately
**	preceding the body of the message. The type field is
**	required for the System V IPC facility. The rest is required
**	by this module. Typically the user does not need to
**	deal with these fields. They are filled in by svminit,
**	and accessed using the routines in the corresponding
**	implementation module. The abstract data type is not
**	enforced by the C compiler, but it is still a useful
**	convention.
*/
struct svmessagehead
	{
	long	svm_type;	/* message type (value must be > 0) */
	int	svm_reply;	/* id of message queue to reply to */
	int	svm_length;	/* length of body of message */
	};

/*
**	This is the general structure of a message. A user would never
**	use this structure to actually allocate message buffers
**	(because of the one character body), but it is useful for
**	declaring generic message pointers (which is how it is used
**	in the associated implementation module).
*/
struct svmessage
	{
	struct svmessagehead	svm_head;
	char			svm_body[1];
	};

/*
**	The intent here is for the user to be able to code (for
**	example)
**
**		SVMESSAGE(100)	mpool[10];
**
**	which would declare an array of 10 svmessage structures, called
**	"mpool", each of which would have a text area 100 bytes long.
**	If the user want something more complicated than a character
**	array (e.g. a structure), then they are on their own. At least
**	we have the svmessagehead structure predefined for them.
*/
#define SVMESSAGE(l)	struct { struct svmessagehead mhead; char mbody[l]; }

#define SVMMODE		0620
#define SVMANYTYPE	0

extern int svmserver MPROTO((key_t key));

extern int svmclient MPROTO((key_t key));

extern int svmpeer MPROTO((key_t key));

extern int svmremove MPROTO((int id));

extern int svmmode MPROTO((int id, int mode));

extern int svmsend MPROTO((int id, struct svmessage *msg));

extern int svmrecv MPROTO((int id, struct svmessage *msg, int len, long typ));

extern int svmcheck MPROTO((int id, struct svmessage *msg, int len, long typ));

extern long svmtype MPROTO((struct svmessage *msg));

extern int svmlength MPROTO((struct svmessage *msg));

extern int svmreply MPROTO((struct svmessage *msg));

extern char * svmbody MPROTO((struct svmessage *msg));

extern struct svmessage *svminit MPROTO((struct svmessage *msg, long typ, key_t rep, char txt[], int len));

extern key_t svmname MPROTO((char name[sizeof(key_t)]));

extern key_t svmseq MPROTO((char *string, int number));

#endif /* !H_SVMESSAGES */
