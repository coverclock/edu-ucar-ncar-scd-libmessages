/*
**	Client portion of multiple processor message passing test.
**
**	Arguments:	<host name> <port number> <request string>
**				<number of messages>
**
**	@(#)egmsgclient.c	4.2 94/05/19 jsloan@ncar.ucar.edu
*/

#include <stdio.h>
#include "errors.h"
#include "libttypes.h"
#include <sys/wait.h>
#include "messages.h"
#include "keyword.h"
#include "trees.h"
#include "keyval.h"

main(argc,argv)
int argc;
char **argv;
	{
	int sock, pid, rc, count;
	MESSAGE *msg;

	if (argc!=5)
		fprintf(stderr,"usage: %s host port request messages\n",
			argv[0]), exit(1);
	fprintf(stderr,"%s: host=%s, port=%s, request=%s, count=%s\n",
		argv[0],argv[1],argv[2],argv[3],argv[4]);
	sock=mclient(argv[1],argv[2]);
	fprintf(stderr,"%s: sock=%d\n",argv[0],sock);
	if (sock<0)
		perror("mclient"), exit(2);
	pid=getpid();
	for (count=atoi(argv[4]); count>0; count--)
		{
		msg=message(0L,0L,(long)pid,0L,"CLIENT");
		if (msg==NULL)
			perror("message"), exit(3);
		minsert(msg,"REQUEST",argv[3]);
		printf("%s=%s\n","REQUEST",mextract(msg,"REQUEST"));
		rc=msend(sock,msg);
		if (rc<0)
			perror("msend"), exit(4);
		rc=mrecv(sock,&msg);
		if (rc<0)
			perror("mrecv"), exit(5);
		printf("%s=%s\n","RESPONSE",mextract(msg,"RESPONSE"));
		mfree(&msg);
		}
	mclose(sock);
	}
