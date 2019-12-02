/*
**	S Y S T E M   V   M E S S A G E   P A S S I N G   I P C
**
**	Copyright 1989, 1990, 1991, 1992 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		IPC package based on System V facilities
**	Program		libipc
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Thu Jul 27 17:48:13 MDT 1989
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This module implements some IPC primitives at a slightly higher
**	level than the System V message queue IPC facilities on which
**	they are based.
*/

static char copyright[]="Copyright 1989, 1990, 1991, 1992 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)svmessage.c	4.2 96/07/08 jsloan@ncar.ucar.edu";

#include <stdio.h>
#include "libttypes.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include "libtvalues.h"
#include "errors.h"
#include "svmessage.h"

/*
**	SvmServer, SvmClient, SvmPeer: each of these functions
**	maps a message queue key to a unique message identifier
**	(not unlike how open maps a pathname to a unique file
**	descriptor).
**
**	SvmServer creates the message queue, returning an error
**	if it already exists.
**
**	SvmClient returns an error if the message queue does NOT
**	already exist.
**
**	SvmPeer creates the message queue if it does not already
**	exist.
**
**	Arguments:
**		key	message queue key
**
**	Returns:	a message queue identifier if all is copacetic
**			-1 otherwise (errno is set)
**
**	e.g.
**		smqid=svmserver(svmname("SPMQ"));
**		cmqid=svmclient(svmname("SPMQ"));
**		pmqid=svmpeer(mqkey);
*/
int
svmserver(key)
key_t key;
	{
	return(msgget(key,(IPC_CREAT|IPC_EXCL|SVMMODE)));
	}

int
svmclient(key)
key_t key;
	{
	return(msgget(key,0));
	}

int
svmpeer(key)
key_t key;
	{
	return(msgget(key,IPC_CREAT|SVMMODE));
	}

/*
**	SvmRemove: deletes a message queue from the System V
**	message queue space. Should probably only be done by
**	the process which created the message queue (i.e.
**	client processes will probably not delete the server
**	process message queue). Peers may want to call svmremove
**	and ignore the return code.
**
**	Arguments:
**		id	message queue identifier
**
**	Returns:	zero if all is copacetic
**			-1 otherwise (errno is set)
**
**	e.g.
**		(void)svmremove(smqid);
*/
int
svmremove(id)
int id;
	{
	return(msgctl(id,IPC_RMID,(struct msqid_ds *)0));
	}

/*
**	SvmMode: sets the mode on a message queue. The previous
**	mode is returned. Only the last nine bits of the new
**	mode is used (the traditional UNIX file permission
**	bits). It is not clear from the man pages what execute
**	permission implies; the ipcs command does not seem
**	to report it. If the new mode is zero, the mode is
**	not changed, merely reported.
**
**	Arguments:
**		id	message queue identifier
**		mode	mode
**
**	Returns:	the previous mode if all is copacetic
**			A negative number otherwise (errno is set)
**
**	e.g.
**		was=svmmode(spmqid,0600);
**		is=svmmode(spmqid,0);
*/
int
svmmode(id,mode)
int id;
int mode;
	{
	int old;
	struct msqid_ds status;

	if (msgctl(id,IPC_STAT,&status)!=0)
		return(-1);
	old=(status.msg_perm.mode&0777);
	if (mode)
		{
		status.msg_perm.mode=mode;
		if (msgctl(id,IPC_SET,&status)!=0)
			return(-1);
		}
	return(old);
	}

/*
**	SvmInit: initializes a svmessage struct by filling in
**	all the necessary fields, include the actual body
**	of the message. None of the arguments are validity
**	checked before being handed off to the system calls.
**	The supplied svmessage structure is assumed to be
**	of big enough to contain the message body.
**
**	Arguments:
**		msg	pointer to a message structure
**		typ	message type (must be > 0)
**		rep	reply message queue identifier
**		txt	pointer to actual message body
**		len	length of actual message body
**
**	Returns:	address of the svmessage structure (msg)
**
**	e.g.
**		buf="Ack!";
**		svmsend(yourq,svminit(&msg,1,myq,buf,strlen(buf));
*/
struct svmessage *
svminit(msg,typ,rep,txt,len)
struct svmessage *msg;
long typ;
int rep;
char txt[];
int len;
	{
	char *ptr;

	msg->svm_head.svm_type=typ;
	msg->svm_head.svm_reply=rep;
	msg->svm_head.svm_length=len;
	for (ptr=msg->svm_body; len>0; len--)
		*(ptr++)=(*(txt++));
	return(msg);
	}

/*
**	SvmSend: sends a message to a message queue. All of
**	the fields in the svmessage structure are assumed to
**	have been filled in, by svminit or otherwise. The
**	length is manipulated to take into account the
**	size of the extra fields in the svmessagehead
**	structure.
**
**	Arguments:
**		id	message queue identifier
**		msg	pointer to svmessage structure
**
**	Returns:	zero if all is copacetic
**			-1 otherwise (errno is set)
**
**	e.g.
**		buf="Ack!";
**		svmsend(yourq,svminit(&msg,1,myq,buf,strlen(buf));
*/
int
svmsend(id,msg)
int id;
struct svmessage *msg;
	{
	return(msgsnd(id,(struct msgbuf *)msg,msg->svm_head.svm_length+sizeof(msg->svm_head)-sizeof(msg->svm_head.svm_type),0));
	}

/*
**	SvmRecv, SvmCheck: receives a message from a message
**	queue. The fields in the svmessagehead structure will be
**	filled in. A specific message type can be requested.
**	The length is manipulated to take into account the
**	size of the extra fields in the svmessagehead
**	structure.
**
**	SvmRecv waits until a message arrives. SvmCheck returns
**	immediately with a -1 is there is no message.
**
**	Arguments:
**		id	message queue identifier
**		msg	pointer to svmessage structure
**		len	maximum length of body of message
**		typ	message type request (zero if don't care)
**
**	Returns:	number of bytes received if all is copacetic
**			0 if no messages pending (svmcheck)
**			-1 otherwise (errno is set)
**
**	e.g.
**		svmrecv(id,&msg,sizeof(buffer),0);
**
**	N.B.	The number of bytes returned is from the perspective of
**		msgrcv, and includes not only the size of the body
**		of the message but parts of the svmessagehead
**		structure as well. Use svmlength to get the actual
**		length of the message body.
*/
int
svmrecv(id,msg,len,typ)
int id;
struct svmessage *msg;
int len;
long typ;
	{
	return(msgrcv(id,(struct msgbuf *)msg,len+sizeof(msg->svm_head)-sizeof(msg->svm_head.svm_type),typ,0));
	}

int
svmcheck(id,msg,len,typ)
int id;
struct svmessage *msg;
int len;
long typ;
	{
	int rc;

	if ((rc=msgrcv(id,(struct msgbuf *)msg,len+sizeof(msg->svm_head)-sizeof(msg->svm_head.svm_type),typ,IPC_NOWAIT))<0)
		if (geterrno()==ENOMSG)
			rc=0;
	return(rc);
	}

/*
**	SvmType, SvmLength, SvmReply, SvmBody: each of these gives
**	the caller access to a field in the svmessage structure.
**
**	Arguments:
**		msg	pointer to svmessage structure
**
**	Returns:
**	svmtype		message type
**	svmlength	length of message body
**	svmreply	message queue identifier to which replies are sent
**	svmbody		address of message body
**
**	e.g.
**		printf("%*s\n",svmlength(&msg),svmbody(&msg));
**		svmsend(svmreply(&msg),&response);
**		svmrecv(id,&new,sizeof(buffer),svmtype(&old));
*/
long
svmtype(msg)
struct svmessage *msg;
	{
	return(msg->svm_head.svm_type);
	}

int
svmlength(msg)
struct svmessage *msg;
	{
	return(msg->svm_head.svm_length);
	}


int
svmreply(msg)
struct svmessage *msg;
	{
	return(msg->svm_head.svm_reply);
	}

char *
svmbody(msg)
struct svmessage *msg;
	{
	return(msg->svm_body);
	}

/*
**	SvmName: this function naively maps a character string
**	of length sizeof(long) to a long integer. Any additional
**	characters are ignored, and the long integer is zero padded.
**	Since both System V message queue keys and message types
**	are declared long, this is useful for using short ASCII
**	strings as queue names or type labels.
**
**	Arguments:
**		key	null terminated ASCII string
**
**	Returns:	the corresponding long integer
**			IPC_PRIVATE if strlen(key)==0
**
**	e.g.	id=svmserver(svmname("SPRQ"));
**		svmrecv(id,&msg,sizeof(buf),svmname("REQS"));
**
**	N.B.	There are faster algorithms to accomplish this but
**		this method is independent of the alignment of
**		the string.
**
**	N.B.	IPC_PRIVATE happens to be defined to be zero, so
**		the conditional in the return() is not necessary,
**		but it's probably best not to depend upon that.
*/
key_t
svmname(name)
char name[(sizeof(key_t))];
	{
	int ctr;
	char *ptr;
	union	{
		key_t key;
		char string[(sizeof(key_t))];
		} data;

	data.key=0;
	for (ptr=data.string,ctr=(sizeof(key_t)); (ctr>0)&&((*name)!='\0'); ctr--)
		*(ptr++)=(*(name++));
	return((data.key==0)?IPC_PRIVATE:data.key);
	}


/*
**	SvmSeq: given a string and an integer, generates a
**	key, similar to svmname. It does this by creating
**	a string of strlen no more than sizeof(long) from
**	the concatenation of some substrings of the string
**	and the ASCII representation of the integer. This
**	turns out to be useful for consistently generating
**	human-readable keys, when (for example) many maiboxes
**	are necessary for a particular application. For example,
**	if "job" is an integer representing a three digit job
**	number, svmseq("S",job) will generate keys of the
**	form S123 and S456 for jobs 123 and 456 respectively.
**
**	Arguments:
**		s	string
**		n	integer number
**
**	Returns:	key in the same form as svmname
**
**	N.B.: In order to generate the length of num in a
**	machine independent way, we use as its dimension the
**	sizeof(MAXINT) multiplied by the number of bit per byte,
**	which is known to be larger than the largest decimal
**	representation of any integer, but at the same time
**	not excessively long.
*/
key_t
svmseq(s,n)
char *s;
unsigned n;
	{
	int slen, len, nlen;
	char num[(sizeof(MAXINT)*BITSPERBYTE)+1];
	char buf[(sizeof(long))+1];

	slen=strlen(s);
	len=(slen>(sizeof(long)))?(sizeof(long)):slen;
	nlen=(sizeof(long))-len;
	(void)sprintf(num,"%u",n);
	(void)sprintf(buf,"%*.*s%*.*s",len,len,s,nlen,nlen,num);
	return(svmname(buf));
	}
