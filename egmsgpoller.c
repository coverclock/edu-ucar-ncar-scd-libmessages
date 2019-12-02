/*
**	Receiver portion of message multiplexing service
**
**	Arguments:	<port number> <delay>
**
**	@(#)egmsgpoller.c	4.2 94/05/19 jsloan@ncar.ucar.edu
*/

#include <stdio.h>
#include "errors.h"
#include "libttypes.h"
#include <sys/wait.h>
#include "messages.h"
#include "mservice.h"

static char *argvzero="unknown";

static IPCSOCKET
getmsg(sock)
IPCSOCKET sock;
	{
	int rc;
	MESSAGE *msg;
	IPCSOCKET newsock;
	char address[IPCADDRESSLEN];

	msg=NULL;

	rc=mspoll(sock,&msg);
	if (rc<0)
		perror("msrecv"), exit(5);
	if (rc==0)
		return(-1);

	newsock=msg->msg_socket;

	printf(
	"%s: mspoll sock=%d state=%d count=%d %d:%d MESSAGE=%s\n",
		argvzero,msg->msg_socket,
		msstate(msg->msg_socket),mscount(msg->msg_socket),
		msg->msg_id,msg->msg_sequence,mextract(msg,"MESSAGE"));

	(void)mfree(&msg);
	fflush(stdout);

	return(newsock);
	}

main(argc,argv)
int argc;
char **argv;
	{
	int rc, delay;
	IPCSOCKET asock, sock;
	
	if (argc!=3)
		fprintf(stderr,"usage: %s port seconds\n",argv[0]), exit(1);
	argvzero=argv[0];
	printf("%s: port=%s delay=%s\n",argvzero,argv[1],argv[2]);

	asock=mserver(argv[1]);
	printf("%s: mserver=%d\n",argvzero,asock);
	if (asock<0)
		perror("mserver"), exit(2);

	if ((rc=mservice(asock))<0)
		perror("mservice"), exit(5);

	for (delay=atoi(argv[2]); delay>0; delay--)
		sleep(1);

	while ((sock=getmsg(MSVC_ANY))>=0)
		{
		while ((sock=getmsg(sock))>=0)
			;
		}

	ipcdump(stderr);
	bufpool(stderr);
	kvpool(stderr);
	mpool(stderr);
	mspool(stderr);
	nmallocinfo(stderr);
	nmallocpool(stderr);

	msshutdown();
	mshutdown();
	(void)mclose(asock);
	bufshutdown();
	kvshutdown();

	ipcdump(stderr);
	bufpool(stderr);
	kvpool(stderr);
	mpool(stderr);
	mspool(stderr);
	nmallocinfo(stderr);
	nmallocpool(stderr);

	exit(0);
	}
