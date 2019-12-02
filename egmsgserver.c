/*
**	Server portion of multiprocessor message passing test.
**
**	Arguments:	<port number> <response string>
**
**	@(#)egmsgserver.c	4.2 94/05/19 jsloan@ncar.ucar.edu
*/

#include <stdio.h>
#include "errors.h"
#include "libttypes.h"
#include <sys/wait.h>
#include "messages.h"

main(argc,argv)
int argc;
char **argv;
	{
	int sock, newsock;
	
	if (argc!=3)
		fprintf(stderr,"usage: %s port response\n",argv[0]), exit(1);
	fprintf(stderr,"%s: port=%s, response=%s\n",
		argv[0],argv[1],argv[2]);
	sock=mserver(argv[1]);
	fprintf(stderr,"%s: mserver=%d\n",argv[0],sock);
	if (sock<0)
		perror("mserver"), exit(2);
	while (1)
		{
		newsock=mready();
		fprintf(stderr,"%s: mready=%d\n",argv[0],newsock);
		if (newsock<0)
			perror("mready"), exit(3);
		else if (newsock==sock)
			{
			newsock=maccept(sock);
			fprintf(stderr,"%s: maccept=%d\n",argv[0],newsock);
			if (newsock<0)
				perror("maccept"), exit(4);
			if (fork()==0)
				{
				MESSAGE *msg;

				msg=NULL;
				while (1)
					{
					int rc;

					rc=mrecv(newsock,&msg);
					if (rc<0)
						perror("mrecv"), exit(5);
					if (rc==0)
						break;
					printf("%s=%s\n","REQUEST",mextract(msg,"REQUEST"));
					minsert(msg,"RESPONSE",argv[2]);
					rc=msend(newsock,msg);
					if (rc<0)
						perror("msend"), exit(6);
					}
				mfree(&msg);
				mclose(newsock);
				exit(0);
				}
			mclose(newsock);
			}
		}
	}
