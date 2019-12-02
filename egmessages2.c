/*
**	Multiple client processes and multiple server processes
**	on the localhost each exchange a single request/response
**	message sequence.
**
**	Arguments:	<port number> <number of clients>
**				[ <keyword=value pair> ... ]
**
**	@(#)egmessages2.c	4.3 95/12/24 jsloan@ncar.ucar.edu
*/

#include <stdio.h>
#include "errors.h"
#include "libttypes.h"
#include <sys/wait.h>
#include "messages.h"
#include "keyword.h"
#include "keyval.h"

static int clients=0;

main(argc,argv)
int argc;
char **argv;
	{
	long pid;
	int sock, newsock, i;

	if (argc<3)
		fprintf(stderr,"usage: %s port clients\n",argv[0]), exit(1);

	clients=atoi(argv[2]);

	fprintf(stderr,"SERVER port=%s clients=%d mserver=%d\n",
		argv[1],clients,(sock=mserver(argv[1])));

	massert((sock>=0),"SERVER mserver");

	for (i=0; i<clients; i++)
		if (pid=fork())
			{

			fprintf(stderr,"CLIENT %d/%ld begins\n",i,pid);
			client(i,argc,argv);
			fprintf(stderr,"CLIENT %d/%ld ends\n",i,pid);

			bufpool(stderr);
			kvpool(stderr);
			mpool(stderr);
			nmallocinfo(stderr);
			nmallocpool(stderr);
			ipcdump(stderr);

			exit(0);
			}

	i=0;

	while (1)
		{
		fprintf(stderr,"SERVER main %d continuing\n",i++);
		fprintf(stderr,"SERVER main maccept=%d\n",
			(newsock=maccept(sock)));
		massert((newsock>=0),"server maccept");
		if (pid=fork())
			{

			fprintf(stderr,"SERVER %d/%ld begins\n",newsock,pid);
			server(newsock);
			fprintf(stderr,"SERVER %d/%ld ends\n",newsock,pid);

			bufpool(stderr);
			kvpool(stderr);
			mpool(stderr);
			nmallocinfo(stderr);
			nmallocpool(stderr);
			ipcdump(stderr);

			exit(0);
			}
		}
	}

server(sock)
int sock;
	{
	MESSAGE *msg, *msg2;
	int rc;
	char serial[5];

	msg=NULL;

	while (1)
		{

		fprintf(stderr,"SERVER %d mrecv=%d\n",sock,
			(rc=mrecv(sock,&msg)));

		if (rc==0)
			{
			fprintf(stderr,"SERVER %d connection lost\n",sock);
			break;
			}

		massert((rc>0),"SERVER mrecv");

		fprintf(stderr,"SERVER %d recv parcel\n",sock);

		kvdump(msg->msg_parcel,stderr);

		msg->msg_id=sock;

		strcpy(msg->msg_name,"SERVER");

		sprintf(serial,"%d",sock);

		minsert(msg,"SERVER",serial);

		fprintf(stderr,"SERVER %d send parcel\n",sock);

		msg2=message(0L,0L,0L,0L,NULL);

		minsert(msg2,"MERG1","1");
		minsert(msg2,"MERG2","2");

		mmerge(msg2,msg);

		kvdump(msg->msg_parcel,stderr);

		fprintf(stderr,"SERVER %d msend=%d\n",sock,
			(rc=msend(sock,msg)));

		massert((rc>0),"SERVER msend");

		mfree(&msg2);
		}

	mfree(&msg);
	mclose(sock);
	}

client(serial,argc,argv)
int serial;
int argc;
char **argv;
	{
	char *lhs, *rhs;
	MESSAGE *msg;
	int sock, rc;

	fprintf(stderr,"CLIENT %d mclient=%d\n",serial,
		(sock=mclient("localhost",argv[1])));

	massert((sock>=0),"CLIENT mclient");

	msg=message(0L,0L,(long)serial,0L,"CLIENT");

	for (argc-=3, argv+=3; argc>0; argc--, argv++)
		{
		lhs=(*argv);
		rhs=keyword(lhs);
		minsert(msg,lhs,rhs);
		}

	fprintf(stderr,"CLIENT %d send parcel\n",serial);

	kvdump(msg->msg_parcel,stderr);

	fprintf(stderr,"CLIENT %d msend=%d\n",serial,
		(rc=msend(sock,msg)));

	massert((rc>=0),"CLIENT msend");

	fprintf(stderr,"CLIENT %d mrecv=%d\n",serial,
		(rc=mrecv(sock,&msg)));

	massert((rc>=0),"CLIENT mrecv");

	fprintf(stderr,"CLIENT %d recv parcel\n",serial);

	kvdump(msg->msg_parcel,stderr);

	mfree(&msg);

	mclose(sock);
	}

massert(bool,label)
int bool;
char *label;
	{
	if (!bool)
		{
		perror(label);
		exit(1);
		}
	}
