/*
**	A single client process and a single server process on
**	the localhost exchange a single request/response message
**	sequence.
**
**	Arguments:	<port number> [ <keyword=value pair> ... ]
**
**	@(#)egmessages1.c	4.2 94/05/19 jsloan@ncar.ucar.edu
*/

#include <stdio.h>
#include "errors.h"
#include "libttypes.h"
#include <sys/wait.h>
#include "messages.h"
#include "parcels.h"
#include "keyword.h"

main(argc,argv)
int argc;
char **argv;
	{
	char *lhs, *rhs;
	MESSAGE *msgp;
	long pid;
	int clientsock, serversock, newsock, rc;

	if (argc<2)
		fprintf(stderr,"usage: %s port\n",argv[0]), exit(1);

	if (pid=fork())
		{

		fprintf(stderr,"SERVER port=%s ipcport=%d mserver=%d\n",
			argv[1],ipcport(argv[1]),(serversock=mserver(argv[1])));

		massert((serversock>=0),"SERVER mserver");

		msgp=message(1,2,3,4,"SERVER");
		fprintf(stderr,"SERVER maccept=%d\n",
			(newsock=maccept(serversock)));

		massert((newsock>=0),"SERVER maccept");

		fprintf(stderr,"SERVER mrecv=%d\n",
			(rc=mrecv(newsock,&msgp)));

		massert((rc>=0),"SERVER mrecv");

		mdump("SERVER recvd",msgp);

		minsert(msgp,"SERVER","rules");

		mdump("SERVER sendg",msgp);

		fprintf(stderr,"SERVER msend=%d\n",
			(rc=msend(newsock,msgp)));

		massert((rc>=0),"SERVER msend");

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

		msgp=message(4,5,6,7,"CLIENT");

		for (argc-=2, argv+=2; argc>0; argc--, argv++)
			{
			lhs=(*argv);
			rhs=keyword(lhs);
			minsert(msgp,lhs,rhs);
			}

		massert((clientsock>=0),"CLIENT mclient");

		mdump("CLIENT sendg",msgp);

		fprintf(stderr,"CLIENT msend=%d\n",
			(rc=msend(clientsock,msgp)));

		massert((rc>=0),"CLIENT msend");

		fprintf(stderr,"CLIENT mrecv=%d\n",
			(rc=mrecv(clientsock,&msgp)));

		massert((rc>=0),"CLIENT mrecv");

		mdump("CLIENT recvd",msgp);

		mfree(&msgp);
		mclose(clientsock);

		bufpool(stderr);
		kvpool(stderr);
		mpool(stderr);
		nmallocinfo(stderr);
		nmallocpool(stderr);
		ipcdump(stderr);

		exit(0);
		}
	}

massert(bool,label)
int bool;
char *label;
	{
	if (!bool)
		perror(label);
	}

mdump(label,msgp)
char *label;
MESSAGE *msgp;
	{
	PACKAGE *package;

	if (msgp!=NULL)
		{
		fprintf(stderr,"%s %ld %ld %ld %-8.8s ``%s''\n",
			label,
			msgp->msg_function,
			msgp->msg_status,
			msgp->msg_id,
			msgp->msg_name,
			package=unbundlepackage(msgp->msg_parcel));
		freepackage(package);
		}
	}
