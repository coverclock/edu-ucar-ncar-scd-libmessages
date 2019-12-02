/*
**	Server portion of multiprocessor message mux test.
**
**	Arguments:	<port number> <response string>
**
**	@(#)egmsgservice.c	4.2 94/05/19 jsloan@ncar.ucar.edu
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
	int sock, rc;
	MESSAGE *msg;
	char address[IPCADDRESSLEN];
	
	if (argc!=3)
		fprintf(stderr,"usage: %s port response\n",argv[0]), exit(1);

	fprintf(stderr,"%s: port=%s, response=%s\n",
		argv[0],argv[1],argv[2]);

	sock=mserver(argv[1]);
	fprintf(stderr,"%s: mserver=%d\n",argv[0],sock);
	if (sock<0)
		perror("mserver"), exit(2);

	if ((rc=mservice(sock))<0)
		perror("mservice"), exit(5);

	msg=NULL;

	while (1)
		{
		rc=msrecv(MSVC_ANY,&msg);
		if (rc<0)
			perror("msrecv"), exit(5);

		printf(
	"%s: msrecv sock=%d state=%d count=%d addr=%s uid=%d REQUEST=%d\n",
			argv[0],msg->msg_socket,
			msstate(msg->msg_socket),mscount(msg->msg_socket),
			ipcformat(msg->msg_address,msg->msg_port,address),
			msg->msg_identity,mextract(msg,"REQUEST"));

		(void)minsert(msg,"RESPONSE",argv[2]);

		rc=msend(msg->msg_socket,msg);
		if (rc<0)
			perror("msend"), exit(6);

		mfree(&msg);
		}
	}
