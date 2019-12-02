/*
**	A client process and a single server process on
**	the localhost exchange a generic message
**	sequences.
**
**	Arguments:	<port> [ <keyword=value pair> ... ]
**
**	@(#)egmgeneric2.c	4.2 94/05/19 jsloan@ncar.ucar.edu
*/

#include <stdio.h>
#include "errors.h"
#include "libttypes.h"
#include <sys/wait.h>
#include <signal.h>
#include "ipc.h"
#include "messages.h"
#include "mgeneric.h"
#include "mgenerics.h"
#include "parcel.h"
#include "keyword.h"

#define MASSERT(b,l)	{ \
			if (!(b)) \
				{ \
				perror(l); \
				exit(1); \
				} \
			}

main(argc,argv)
int argc;
char **argv;
	{
	char *lhs, *rhs, **arg;
	MGENERIC *m;
	long pid;
	int clientsock, serversock, newsock, rc;
	char address[IPCADDRESSLEN];

	if (argc<2)
		fprintf(stderr,"usage: %s port\n",argv[0]), exit(1);

	if (pid=fork())
		{

		fprintf(stderr,"SERVER port=%s ipcport=%d mserver=%d\n",
			argv[1],ipcport(argv[1]),(serversock=mserver(argv[1])));
		MASSERT((serversock>=0),"SERVER mserver");

		fprintf(stderr,"SERVER maccept=%d\n",
			(newsock=maccept(serversock)));
		MASSERT((newsock>=0),"SERVER maccept");

		while (1)
			{

			fprintf(stderr,"SERVER mgenrecv=%d\n",
				(rc=mgenrecv(newsock,&m)));
			MASSERT((rc>=0),"SERVER mgenrecv");

			if (rc==0)
				break;

			fprintf(stderr,"SERVER type=%d addr=%s uid=%d\n",
				m->msg_type,
				ipcformat(m->msg_address,m->msg_port,address),
				m->msg_identity);

			fprintf(stderr,"SERVER recv parcel\n");
			kvdump((PARCEL)m->msg_body,stderr);

			(void)bundle("SERVER","rules",(PARCEL *)&(m->msg_body));
			fprintf(stderr,"SERVER send %s=%s\n",
				"SERVER",unbundle("SERVER",(PARCEL)m->msg_body));

			fprintf(stderr,"SERVER send parcel\n");
			kvdump((PARCEL)m->msg_body,stderr);

			fprintf(stderr,"SERVER mgensend=%d\n",
				(rc=mgensend(newsock,m)));
			MASSERT((rc>=0),"SERVER mgensend");

			mgenfree(m);
			}

		mclose(newsock);
		mclose(serversock);

		while (1)
			if (wait(NULL)<0)
				if (errno!=EINTR)
					break;

		bufpool(stderr);
		kvpool(stderr);
		mgenpool(stderr);
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
		MASSERT((clientsock>=0),"CLIENT mclient");

		for (arg=&argv[2]; *arg!=0; arg++)
			{

			m=mgenalloc(MGEN_PACKET);
			MASSERT((m!=NULL),"CLIENT mgenalloc");
			fprintf(stderr,"CLIENT type=%d\n",m->msg_type);

			rhs=keyword(*arg);
			lhs=(*arg);
			(void)bundle(lhs,rhs,(PARCEL *)&(m->msg_body));
			fprintf(stderr,"CLIENT send %s=%s\n",
				lhs,unbundle(lhs,(PARCEL)m->msg_body));

			fprintf(stderr,"CLIENT send parcel\n");
			kvdump((PARCEL)m->msg_body,stderr);

			fprintf(stderr,"CLIENT mgensend=%d\n",
				(rc=mgensend(clientsock,m)));
			MASSERT((rc>=0),"CLIENT mgensend");

			(void)mgenfree(m);

			fprintf(stderr,"CLIENT mgenrecv=%d\n",
				(rc=mgenrecv(clientsock,&m)));
			MASSERT((rc>=0),"CLIENT mgenrecv");
			fprintf(stderr,"CLIENT type=%d addr=%s uid=%d\n",
				m->msg_type,
				ipcformat(m->msg_address,m->msg_port,address),
				m->msg_identity);

			fprintf(stderr,"CLIENT recv parcel\n");
			kvdump((PARCEL)m->msg_body,stderr);

			mgenfree(m);
			}

		mclose(clientsock);

		bufpool(stderr);
		kvpool(stderr);
		mgenpool(stderr);
		nmallocinfo(stderr);
		nmallocpool(stderr);
		ipcdump(stderr);

		exit(0);
		}
	}
