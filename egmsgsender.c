/*
**	Sender portion of message multiplexing service
**
**	Arguments:	<host name> <port number> <message string>
**				<number of messages>
**
**	@(#)egmsgsender.c	4.2 94/05/19 jsloan@ncar.ucar.edu
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
	int sock, pid, rc, count, limit;
	MESSAGE *msg;

	if (argc!=5)
		fprintf(stderr,"usage: %s host port message count\n",
			argv[0]), exit(1);
	fprintf(stderr,"%s: host=%s, port=%s, request=%s, count=%s\n",
		argv[0],argv[1],argv[2],argv[3],argv[4]);
	sock=mclient(argv[1],argv[2]);
	fprintf(stderr,"%s: sock=%d\n",argv[0],sock);
	if (sock<0)
		perror("mclient"), exit(2);
	pid=getpid();
	limit=atoi(argv[4]);
	for (count=0; count<limit; count++)
		{
		msg=message(0L,0L,(long)pid,(long)count,"CLIENT");
		if (msg==NULL)
			perror("message"), exit(3);
		minsert(msg,"MESSAGE",argv[3]);
		printf("%s=%s\n","MESSAGE",mextract(msg,"MESSAGE"));
		rc=msend(sock,msg);
		sleep(1);
		if (rc<0)
			perror("msend"), exit(4);
		mfree(&msg);
		}
	mclose(sock);
	exit(0);
	}
