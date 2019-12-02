/*
**	A single client process and a single server process on
**	the localhost exchange a several typed requests. The server
**	receives the requests in order by type while the client
**	sends the requests out of order.
**
**	Arguments:	<port number> [ <keyword=value pair> ... ]
**
**	@(#)egmqueue.c	4.2 94/05/19 jsloan@ncar.ucar.edu
*/

#include <stdio.h>
#include "errors.h"
#include "libttypes.h"
#include <sys/wait.h>
#include "messages.h"
#include "parcel.h"
#include "keyword.h"
#include "mqueue.h"

#define TYPES	5
#define COUNT	3

main(argc,argv)
int argc;
char **argv;
	{
	MESSAGE *msgp;
	long pid;
	int clientsock, serversock, newsock, rc;
	int i, t;
	long mask;
	MQUEUE queue[TYPES];
	extern int flavor();

	if (argc<2)
		fprintf(stderr,"usage: %s port\n",argv[0]), exit(1);

	msgp=NULL;

	if (pid=fork())
		{
		fprintf(stderr,"SERVER port=%s ipcport=%d mserver=%d\n",
			argv[1],ipcport(argv[1]),(serversock=mserver(argv[1])));
		massert((serversock>=0),"SERVER mserver");
		fprintf(stderr,"SERVER maccept=%d\n",
			(newsock=maccept(serversock)));
		massert((newsock>=0),"SERVER maccept");
		mqueue(queue,TYPES);
		for (t=0, mask=(1<<0); t<TYPES; t++, mask=(1<<t))
			for (i=COUNT; i>0; i--)
				{
				fprintf(stderr,"SERVER mqueued(%#lx)=%d\n",
					mask,mqueued(mask,queue));
				rc=mqrecv(newsock,&msgp,mask,queue,flavor);
				massert((rc>=0),"SERVER mrecv");
				fprintf(stderr,
					"SERVER %d %d %ld %ld mqrecv=%d\n",
					i,t,
					msgp->msg_id,
					msgp->msg_sequence,
					rc);
				}
		mfree(&msgp);
		mclose(newsock);
		mclose(serversock);
		sleep(2);

		bufpool(stderr);
		kvpool(stderr);
		mpool(stderr);
		nmallocinfo(stderr);
		nmallocpool(stderr);
		ipcdump(stderr);

		exit(0);
		}
	else
		{
		sleep(2);
		fprintf(stderr,"CLIENT ipcport=%d mclient=%d\n",
			ipcport(argv[1]),
			(clientsock=mclient("localhost",argv[1])));
		massert((clientsock>=0),"CLIENT mclient");
		for (i=0; i<COUNT; i++)
			for (t=TYPES-1; t>=0; t--)
				{	
				msgp=message(0,1,t,i,"CLIENT");
				fprintf(stderr,"CLIENT %d %d msend=%d\n",
					i,t,(rc=msend(clientsock,msgp)));
				massert((rc>=0),"CLIENT msend");
				mfree(&msgp);
				}
		(void)mclose(clientsock);

		bufpool(stderr);
		kvpool(stderr);
		mpool(stderr);
		nmallocinfo(stderr);
		nmallocpool(stderr);
		ipcdump(stderr);

		exit(0);
		}
	}

int
flavor(m)
MESSAGE *m;
	{
	return((int)m->msg_id);
	}

massert(bool,label)
int bool;
char *label;
	{
	if (!bool)
		perror(label);
	}
