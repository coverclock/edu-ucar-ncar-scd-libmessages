/*
**	Receiver portion of message multiplexing service
**
**	Arguments:	<port number> <delay>
**
**	@(#)egmsgreceiver.c	4.2 94/05/19 jsloan@ncar.ucar.edu
*/

#include <stdio.h>
#include "errors.h"
#include "libttypes.h"
#include <sys/wait.h>
#include "messages.h"
#include "mservice.h"

main(argc,argv)
int argc;
char **argv;
	{
	int sock, rc, delaying, delay;
	MESSAGE *msg;
	char address[IPCADDRESSLEN];
	
	if (argc!=3)
		fprintf(stderr,"usage: %s port seconds\n",argv[0]), exit(1);
	fprintf(stderr,"%s: port=%s delay=%s\n",
		argv[0],argv[1],argv[2]);

	sock=mserver(argv[1]);
	fprintf(stderr,"%s: mserver=%d\n",argv[0],sock);
	if (sock<0)
		perror("mserver"), exit(2);

	if ((rc=mservice(sock))<0)
		perror("mservice"), exit(5);

	delaying=atoi(argv[2]);
	for (delay=delaying; delay>0; delay--)
		sleep(1);
	fprintf(stderr,"%s: dozed for %d seconds\n",argv[0],delaying);

	msg=NULL;
	while (1)
		{
		rc=msrecv(MSVC_ANY,&msg);
		if (rc<0)
			perror("msrecv"), exit(5);

		fprintf(stderr,
	"%s: msrecv sock=%d state=%d count=%d addr=%s %d:%d\n",
			argv[0],msg->msg_socket,
			msstate(msg->msg_socket),mscount(msg->msg_socket),
			ipcformat(msg->msg_address,msg->msg_port,address),
			msg->msg_id,msg->msg_sequence);

		fflush(stdout);
		}
	}
