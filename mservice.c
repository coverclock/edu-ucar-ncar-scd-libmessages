/*
**	M S E R V I C E
**
**	Copyright 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Asynchronous Service
**	Program		libipc
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Tue Mar  3 12:09:19 MST 1992
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**	Status		E X P E R I M E N T A L
**
**	Abstract
**
**	This module will test the idea of multiplexing message
**	reception across multiple sockets simultaneously, and
**	automatically queueing the messages for later perusal.
**	This provides a poor man's implementation of asynchronous
**	message passing using the quasisynchronous messages facility
**	using Berkeley internet stream sockets that was developed
**	for TAGS.
**
**	Since portions of this code is interrupt driven (via SIGIO),
**	the problem arises of race conditions, particularly when
**	modifying the message queue head. There's some effort
**	expended to synchronize this.
**
**	Not all UNIX platforms support SIGIO. Hence the SIGIO portions
**	of the code are conditionally compiled. Even without SIGIO,
**	the software is useful for message multiplexing, but lacks
**	the asynchronous feature.
*/

static char copyright[]="Copyright 1992, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mservice.c	4.3 94/05/19 jsloan@ncar.ucar.edu";

#ifdef PACKAGE_LIBIPC
static char use_msg_package[]="@(#)mservice.c	4.3 94/05/19 Using packages";
#else /* PACKAGE_LIBIPC */
static char use_msg_package[]="@(#)mservice.c	4.3 94/05/19 Using packets";
#endif /* PACKAGE_LIBIPC */

#ifdef EBCDIC_LIBIPC
static char use_msg_ebcdic[]="@(#)mservice.c	4.3 94/05/19 Using EBCDIC";
#else /* EBCDIC_LIBIPC */
static char use_msg_ebcdic[]="@(#)mservice.c	4.3 94/05/19 Using ASCII";
#endif /* EBCDIC_LIBIPC */

#include "libmessages.h"
#include <signal.h>
#include <sys/param.h>
#include "libttypes.h"
#include "libtin.h"
#include <sys/ioctl.h>
/*
** #include <sys/sockio.h>
** #include <sys/filio.h>
*/
#include "mservice.h"
#include "mheader.h"
#include "lists.h"
#include "queues.h"
#ifdef PACKAGE_LIBIPC
#include "package.h"
#else /* PACKAGE_LIBIPC */
#include "packet.h"
#endif /* PACKAGE_LIBIPC */
#include "errors.h"
#include "buffers.h"
#include "nmalloc.h"
#include "serialize.h"
#ifdef EBCDIC_LIBIPC
#include "ebcdic.h"
#endif /* EBCDIC_LIBIPC */

#define GOODSOCK(sock)	((0<=(sock))&&((sock)<nservices))

#ifdef	SIGIO
static char mservice_sigio[]="@(#)mservice.c	4.3 94/05/19 Using SIGIO";
#define MSIGNAL		(SIGIO)
#define CSMask		int CSmask
#define CSBegin		(CSmask=sigblock(sigmask(MSIGNAL)))
#define CSEnd		(void)sigsetmask(CSmask)
#else /* SIGIO */
static char mservice_sigio[]="@(#)mservice.c	4.3 94/05/19 Lacking SIGIO";
#define CSMask
#define CSBegin
#define	CSEnd
#endif /* SIGIO */

/*
**	These are the states in which an FSA that assembles
**	messages can be.
*/
enum state
	{
	Accepting,	/* accepting connections */
	Initialize,	/* initialize machine */
	ReadyHeader,	/* prepare to receive header */
	ReceiveHeader,	/* receiving header */
	ReadyPacket,	/* prepare to receive packet */
	ReceivePacket,	/* receiving packet */
	Done,		/* message complete */
	Error,		/* error in reception */
	End,		/* closing connection */
	Failed,		/* service failed */
	Closed		/* service closed */
	};

#ifdef DEBUG
static char *statename[]=	/* thread safe: read-only */
	{
	"Accepting",
	"Initialize",
	"ReadyHeader",
	"ReceiveHeader",
	"ReadyPacket",
	"ReceivePacket",
	"Done",
	"Error",
	"End",
	"Failed",
	"Closed"
	};
#endif /* DEBUG */

/*
**	Each "service" has associated with it a socket and a
**	service structure which is dynamically allocated. Even
**	if the socket has been closed, there may still be
**	unread messages queued from it, hence a service may
**	linger on after a connection goes away.
*/
struct service
	{
	LISTNODE		svc_next;
	enum state		svc_state;	/* current state */
	long			svc_need;	/* length needed */
	BUFFER			svc_buffer;	/* packet buffer */
	char *			svc_current;	/* current position */
	MHEADER			svc_header;	/* message header */
	MESSAGE *		svc_message;	/* message in progress */
	int			svc_packet;	/* packet length */
	int			svc_count;	/* messages processed */
	};

static struct service **services=NULL;	/* thread safe: sem_mservice_libm */
static int nservices=0;			/* thread safe: sem_mservice_libm */

static LIST spool={ NULL };		/* thread safe: sem_mservice_libm */
static int allocated=0;			/* thread safe: sem_mservice_libm */
static int active=0;			/* thread safe: sem_mservice_libm */

static QUEUE mqueue={ 0 };		/* thread safe: sem_mservice_libm */

static int interrupting=0;		/* thread safe: sem_mservice_libm */

/*
**	Initialize: perform common initialization.
*/
static int
initialize(void)
	{
	int ndx;

	if (services!=NULL)
		{
		seterrno(EINVAL);
		return(-1);
		}

	nservices=getdtablesize();
	if ((services=(struct service **)nmalloc(sizeof(struct service *)*nservices))==NULL)
		return(-2);

	for (ndx=0; ndx<nservices; ndx++)
		services[ndx]=NULL;

	(void)linit(&spool);
	(void)qinit(&mqueue);

	interrupting=0;

#ifdef DEBUG
	fprintf(stderr,"mservice initialize: %d\n",nservices);
#endif /* DEBUG */

	return(0);
	}

/*
**	validate: validate socket and return service pointer.
*/
static struct service * 
validate(IPCSOCKET sock)
	{
	struct service *s;

	s=NULL;
	if (!GOODSOCK(sock))
		seterrno(EBADF);
	else if (services==NULL)
		seterrno(EINVAL);
	else if ((s=services[sock])==NULL)
		seterrno(ENOENT);

	return(s);
	}

#ifdef MSIGNAL
/*
**	Async: make the specified socket asynchronous.
*/
static int
async(IPCSOCKET sock)
	{
	int flag, pid;

	flag=1;
	if (ioctl(sock,FIOASYNC,(caddr_t)&flag)<0)
		return(-20);

	pid=getpid();
#ifdef SIOCSPGRP
	if (ioctl(sock,SIOCSPGRP,(caddr_t)&pid)<0)
		return(-21);
#else /* SIOCSPGRP */
#ifdef FIOSETOWN
	if (ioctl(sock,FIOSETOWN,(caddr_t)&pid)<0)
		return(-21);
#endif /* FIOSETOWN */
#endif /* SIOCSPGRP */

	return(0);
	}
#endif /* MSIGNAL */

/*
**	Msclose: close a socket associated with a service. This
**	does not free the service. Hence, queued messages are still
**	available.
*/
int
Msclose(IPCSOCKET sock)
	{
	struct service *s;

	if ((s=validate(sock))==NULL)
		return(-3);

	switch (s->svc_state)
		{
	case Closed:
	case Failed:
		break;
	default:
		s->svc_state=Closed;
		if (mclose(sock)<0)
			return(-31);
		break;
		}

	return(0);
	}

/*
**	Msfree: release a service associated with a socket. The
**	socket is not closed; the caller may want to handle
**	the socket I/O independent of mservice.
*/
static int
Msfree(IPCSOCKET sock)
	{
	struct service *s;

	if ((s=validate(sock))==NULL)
		return(-6);

	if (s->svc_message!=NULL)
		{
		(void)mfree(&(s->svc_message));
		s->svc_message=NULL;
		}

	if (s->svc_buffer!=NULL)
		{
		(void)buffree(s->svc_buffer);
		s->svc_buffer=NULL;
		}

	(void)lput(&spool,(LISTNODE *)s);
	active--;
	services[sock]=NULL;

	return(0);
	}

/*
**	Mscreate: create a service to be associated with a socket.
*/
static int
Mscreate(IPCSOCKET sock)
	{
	struct service *s;
	int rc;

	if ((s=validate(sock))==NULL)
		{
		if (geterrno()!=ENOENT)
			return(-7);
		}
	else if ((rc=Msfree(sock))<0)
		return(-8);

	if ((s=(struct service *)lget(&spool))==NULL)
		if ((s=(struct service *)nmalloc(sizeof(struct service)))==NULL)
			return(-9);
		else
			allocated++;
	active++;

	s->svc_state=Initialize;
	s->svc_message=NULL;
	s->svc_buffer=NULL;
	s->svc_count=0;

	services[sock]=s;

	return(sock);
	}

/*
**	Machine: implement a finite state machine which handles
**	any incoming message from the specified socket based on the
**	associated service's state, and if necessary, transition to
**	a new state. Return the number of new messages completely
**	received.
**
**	N.B.:	To prevent race conditions, machine() should
**		never be called except from multiplex().
*/
static int
machine(IPCSOCKET sock)
	{
	struct service *s;
	MESSAGE *m;
	int bytes, total, rc;
#ifdef DEBUG
	char address[IPCADDRESSLEN];
#endif /* DEBUG */

	if ((s=validate(sock))==NULL)
		return(-10);

	for (total=0;;)
		{

#ifdef DEBUG
		fprintf(stderr,
		"mservice machine(%d): %s need=%d msgs=%d\n",
			sock,statename[s->svc_state],s->svc_need,
			qsize(&mqueue));
#endif /* DEBUG */

		switch (s->svc_state)
			{

		case Accepting:
			if ((sock=ipcaccept(sock))<0)
				return(-17);

			if ((rc=Mscreate(sock))<0)
				return(rc);

#ifdef	MSIGNAL
			if (async(sock)<0)
				perror("async");
#endif /* MSIGNAL */

			return(total);

		case Initialize:
			s->svc_current=(char *)&s->svc_header;
			s->svc_need=sizeof(MHEADER);
			s->svc_packet=0;
			s->svc_message=NULL;
			s->svc_state=ReceiveHeader;
			break;

		case ReadyHeader:
			s->svc_current=(char *)&s->svc_header;
			s->svc_need=sizeof(MHEADER);
			s->svc_packet=0;
			s->svc_message=NULL;
			s->svc_state=ReceiveHeader;
			return(total);

		case ReceiveHeader:
			bytes=ipcvrecv(sock,s->svc_current,s->svc_need);
			if (bytes<0)
				{
				s->svc_state=Error;
				break;
				}
			if (bytes==0)
				{
				s->svc_state=End;
				break;
				}

			s->svc_current+=bytes;
			s->svc_need-=bytes;
			if (s->svc_need>0)
				return(total);

			s->svc_state=ReadyPacket;
			break;

		case ReadyPacket:
			bytes=mimheader(sock,&(s->svc_header),&(s->svc_message));
			if (bytes<0)
				return(-26);
#ifdef DEBUG
			fprintf(stderr,
		"mservice machine(%d): %s sock=%d addr=%s uid=%d bytes=%d\n",
				sock,statename[s->svc_state],
				s->svc_message->msg_socket,
				ipcformat(s->svc_message->msg_address,
					s->svc_message->msg_port,
					address),
				s->svc_message->msg_identity,bytes);
#endif /* DEBUG */

			s->svc_packet=bytes;

			if (bytes==0)
				{
				s->svc_state=Done;
				break;
				}

			if ((s->svc_buffer=(s->svc_buffer==NULL)?
					bufalloc(bytes):
					bufinsure(s->svc_buffer,bytes))==NULL)
				return(-12);

			s->svc_need=bytes;
			s->svc_current=(char *)s->svc_buffer;
			s->svc_state=ReceivePacket;

			return(total);

		case ReceivePacket:
			bytes=ipcvrecv(sock,s->svc_current,s->svc_need);
			if (bytes<0)
				{
				s->svc_state=Error;
				break;
				}
			if (bytes==0)
				{
				s->svc_state=End;
				break;
				}

			s->svc_current+=bytes;
			s->svc_need-=bytes;
			if (s->svc_need>0)
				return(total);

			s->svc_state=Done;
			break;

		case Done:
			m=s->svc_message;

			if (s->svc_packet>0)
				{
#ifdef EBCDIC_LIBIPC
				(void)Ascii2Ebcdic(s->svc_buffer,s->svc_packet);
#endif /* EBCDIC_LIBIPC */
#ifdef PACKAGE_LIBIPC
				(void)bundlepackage((PACKAGE *)s->svc_buffer,&m->msg_parcel);
#else /* PACKAGE_LIBIPC */
				(void)bundlepacket((PACKET *)s->svc_buffer,&m->msg_parcel);
#endif /* PACKAGE_LIBIPC */
				}

			(void)qinsert(&mqueue,qtail(&mqueue),(QUEUENODE *)m);
			total++;

			s->svc_count++;
			s->svc_message=NULL;

			s->svc_state=ReadyHeader;
			break;

		case Error:
			(void)Msclose(sock);
			s->svc_state=Failed;
			break;

		case End:
			(void)Msclose(sock);
			s->svc_state=Closed;
			break;

		case Failed:
		case Closed:
		default:
			return(total);

			}
		}
	}

/*
**	Multiplex: provide multiplexing of incoming messages for
**	all services which have sockets containing data to be read.
**	Return the number of new messages completely received.
**
**	If waiting is true, multiplex() waits until at least one
**	message is available. Otherwise it returns immediately,
**	possibly with no new messages.
**
**	N.B.	To prevent race conditions, multiplex() should
**		never be called except from service().
*/
static int
multiplex(int waiting)
	{
	IPCSOCKET sock;
	int rc, count, total;

	if (services==NULL)
		{
		seterrno(EINVAL);
		return(-15);
		}

	for (total=0;;)
		{

		sock=waiting?ipcready():ipcpoll();

#ifdef DEBUG
		fprintf(stderr,"mservice multiplex: %s socket %d\n",
			waiting?"waiting on":"polling",sock);
#endif /* DEBUG */

		if (sock<0)
			if (geterrno()==ETIMEDOUT)
				break;
			else if (geterrno()==EINTR)
				continue;
			else
				return(-16);

		if (!GOODSOCK(sock))
			continue;

		if ((count=machine(sock))>0)
			{
			total+=count;
			waiting=0;
			}
		}

	return(total);
	}

/*
**	Service: embeds multiplex() inside a critical section
**	in which MSIGNAL is blocked. This is the ONLY way in
**	which multiplex() and machine() should be invoked.
*/
static int
service(int flag)
	{
	int rc;
	CSMask;

	CSBegin;
	rc=multiplex(flag);
	CSEnd;

	return(rc);
	}

#ifdef MSIGNAL
/*
**	Iosignal: provide a signal handling function which calls
**	service() to multiplex all services upon a MSIGNAL.
*/
static void
iosignal(int sig)
	{
	int rc;

#ifdef DEBUG
	fprintf(stderr,"mservice iosignal: SIGIO\n");
#endif /* DEBUG */

	P(sem_mservice_libm);
	rc=service(0);
	V(sem_mservice_libm);

#ifdef DEBUG
	fprintf(stderr,"mservice iosignal: SIGIO service=%d\n",rc);
#endif /* DEBUG */
	}
#endif /* MSIGNAL */

/*
**	Mservice: install the message service facility. Sock is
**	the socket through which connection requests arrive.
**
*/
static int
Mservice(IPCSOCKET sock)
	{
	int rc;
	struct service *s;
#ifdef	MSIGNAL
	struct sigaction action;
#endif /* MSIGNAL */

	if (services==NULL)
		if ((rc=initialize())<0)
			return(rc);

	if ((rc=Mscreate(sock))<0)
		return(rc);

	if ((s=validate(sock))==NULL)
		return(-18);

	s->svc_state=Accepting;

#ifdef	MSIGNAL
	(void)bzero((char *)&action,sizeof(action));
	action.sa_handler=iosignal;
	if (sigaction(MSIGNAL,&action,NULL)<0)
		perror("signal");
	else if (async(sock)<0)
		perror("async");
	else
		interrupting=1;
#endif /* MSIGNAL */

	return(0);
	}

/*
**	Scanq: scan the queue looking for a message that
**	arrived on the specified socket. If such exists,
**	return the pointer to the message, else NULL. If
**	sock==MSVC_ANY, return the queue head.
*/
static MESSAGE *
scanq(IPCSOCKET sock)
	{
	MESSAGE *m;

	for (m=(MESSAGE *)qhead(&mqueue); m!=NULL; m=(MESSAGE *)qnext((QUEUENODE *)m))
		if (sock==MSVC_ANY)
			break;
		else if (m->msg_socket==sock)
			break;

	return(m);
	}

/*
**	msrecv: return a message from the specified socket, or
**	any socket if MSVC_ANY. If a message is not available
**	on the queue, wait until one of the right flavor arrives.
**	Since a message is always returned, msrecv always returns
**	true, or a negative number in the event of an error.
*/
static int
Msrecv(IPCSOCKET sock, MESSAGE **mp)
	{
	MESSAGE *m;
	CSMask;

	if (services==NULL)
		{
		seterrno(EINVAL);
		return(-22);
		}

	if (mp==NULL)
		{
		seterrno(EINVAL);
		return(-23);
		}

	CSBegin;

	for (m=scanq(sock); m==NULL; m=scanq(sock))
		{
		CSEnd;
		(void)service(1);
		CSBegin;
		}

	(void)qdelete(&mqueue,(QUEUENODE *)m);

	CSEnd;

	if (*mp!=NULL)
		mfree(mp);
	*mp=m;

	return(1);
	}

/*
**	mspoll: return a message of the specified flavor from the
**	queue if one is available, and return TRUE. If no message
**	is available, return FALSE. If error, return <0.
*/
static int
Mspoll(IPCSOCKET sock, MESSAGE **mp)
	{
	MESSAGE *m;

	if (services==NULL)
		{
		seterrno(EINVAL);
		return(-29);
		}

	if (mp==NULL)
		{
		seterrno(EINVAL);
		return(-30);
		}

	if ((m=scanq(sock))!=NULL)
		{
		(void)qdelete(&mqueue,(QUEUENODE *)m);
		if (*mp!=NULL)
			(void)mfree(mp);
		*mp=m;
		}

	return(m!=NULL);
	}

/*
**	msstate: return the current state of the specified socket.
*/
static int
Msstate(IPCSOCKET sock)
	{
	struct service *s;
	int state;

	if ((s=validate(sock))==NULL)
		return(-28);

	switch (s->svc_state)
		{

	case Accepting:
		state=MSVC_ACCEPTING;
		break;

	case Initialize:
	case Done:
	case ReadyHeader:
		state=MSVC_IDLE;
		break;

	case ReceiveHeader:
	case ReadyPacket:
	case ReceivePacket:
		state=MSVC_BUSY;
		break;

	case Error:
	case Failed:
		state=MSVC_FAILED;
		break;

	case End:
	case Closed:
		state=MSVC_CLOSED;
		break;

	default:
		state=MSVC_OTHER;
		break;
		}

	return(state);
	}

/*
**	mscount: return the message count of the specified socket.
*/
static int
Mscount(IPCSOCKET sock)
	{
	struct service *s;

	if ((s=validate(sock))==NULL)
		return(-24);

	return(s->svc_count);
	}

/*
**	msshutdown: shut down mservice completely and release all
**	internally held dynamically acquired storage. This includes
**	all messages on the message queue!
*/
static void
Msshutdown(void)
	{
	IPCSOCKET sock;
	struct service *s;
	MESSAGE *m;
#ifdef	MSIGNAL
	struct sigaction action;
#endif /* MSIGNAL */

	if (services!=NULL)
		{
#ifdef MSIGNAL
		(void)bzero((char *)&action,sizeof(action));
		action.sa_handler=SIG_IGN;
		(void)sigaction(MSIGNAL,&action,NULL);
		interrupting=0;
#endif /* MSIGNAL */

		m=NULL;
		while (Mspoll(MSVC_ANY,&m)>0)
			continue;
		if (m!=NULL)
			(void)mfree(&m);

		for (sock=0; sock<nservices; sock++)
			if ((s=services[sock])!=NULL)
				{
				if (s->svc_state!=Accepting)
					(void)Msclose(sock);
				(void)Msfree(sock);
				}

		for (s=(struct service *)lget(&spool); s!=NULL;
				s=(struct service *)lget(&spool))
			(void)nfree((void *)s);

		(void)nfree(services);

		services=NULL;
		nservices=0;
		allocated=0;
		active=0;
		}
	}

/*
**	mspool: print the memory stats.
*/
void
Mspool(FILE *fp)
	{
	fprintf(fp,
	"mservice: %8dbytes %8dactive %8dallocated vector\n",
		sizeof(struct service *)*nservices,nservices,nservices);

	fprintf(fp,
	"mservice: %8dbytes %8dactive %8dallocated services\n",
		sizeof(struct service)*allocated,active,allocated);
	}

/*******************************
**    Actual Entry Points     **
*******************************/

int
mscreate(IPCSOCKET sock)
	{
	int rc;

	P(sem_mservice_libm);
	rc=Mscreate(sock);
	V(sem_mservice_libm);
	return(rc);
	}

int
msclose(IPCSOCKET sock)
	{
	int rc;

	P(sem_mservice_libm);
	rc=Msclose(sock);
	V(sem_mservice_libm);
	return(rc);
	}

int
msfree(IPCSOCKET sock)
	{
	int rc;

	P(sem_mservice_libm);
	rc=Msfree(sock);
	V(sem_mservice_libm);
	return(rc);
	}

int
mservice(IPCSOCKET sock)
	{
	int rc;

	P(sem_mservice_libm);
	rc=Mservice(sock);
	V(sem_mservice_libm);
	return(rc);
	}

int
mspoll(IPCSOCKET sock, MESSAGE **mp)
	{
	int rc;

	P(sem_mservice_libm);
	rc=Mspoll(sock,mp);
	V(sem_mservice_libm);
	return(rc);
	}

int
msrecv(IPCSOCKET sock, MESSAGE **mp)
	{
	int rc;

	P(sem_mservice_libm);
	rc=Msrecv(sock,mp);
	V(sem_mservice_libm);
	return(rc);
	}

int
msstate(IPCSOCKET sock)
	{
	int rc;

	P(sem_mservice_libm);
	rc=Msstate(sock);
	V(sem_mservice_libm);
	return(rc);
	}

int
mscount(IPCSOCKET sock)
	{
	int rc;

	P(sem_mservice_libm);
	rc=Mscount(sock);
	V(sem_mservice_libm);
	return(rc);
	}

void
msshutdown(void)
	{
	P(sem_mservice_libm);
	Msshutdown();
	V(sem_mservice_libm);
	}

void
mspool(FILE *fp)
	{
	P(sem_mservice_libm);
	Mspool(fp);
	V(sem_mservice_libm);
	}
