/*
**	<N> client threads and <N> server threads on the localhost
**	each exchange <M> pairs of request/response messages.
**	Some of this is pretty artificial and only exists to test
**	some of the serialization in libtools.
**
**	Arguments:	<port number> <N> <M>
**
**	@(#)egmsgthreaded.c	1.9 94/06/03 jsloan@ncar.ucar.edu
*/

#include "libmessages.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "errors.h"
#include "libttypes.h"
#include <sys/wait.h>
#include "messages.h"
#include "keyword.h"
#include "keyval.h"
#include "nmalloc.h"
#include "timeclock.h"
#include "parcel.h"
#include "package.h"
#include "portfolio.h"
#include "argvector.h"
#include "cache.h"

#ifdef PTHREADS_LIBTOOLS

struct connection
	{
	pthread_t	c_client;
	pthread_t	c_server;
	pthread_addr_t	c_cexit;
	pthread_addr_t	c_sexit;
	int		c_cserial;
	int		c_sserial;
	int		c_cstate;
	int		c_sstate;
#define			C_ACTIVE	(-1)
#define			C_UNUSED	(0)
#define			C_COMPLETE	(1)
	int		c_count;
	int		c_sock;
	int		c_retries;
	char *		c_port;
	};

static SEMAPHORE complete, caching;

static CACHE cache;
static CACHECONFIG config;

/*
**	Cache functions
*/
struct item
	{
	struct item *	i_next;
	char		i_name[16];
	};

static struct item *ihead=NULL, *itail=NULL;

static struct item *
canew(char *name)
	{
	struct item *i;

	if ((i=ihead)==NULL)
		{
		i=(struct item *)namalloc(sizeof(struct item));
		assert(i!=NULL);
		}
	else
		{
		ihead=i->i_next;
		if (ihead==NULL)
			itail=NULL;
		}
	strncpy(i->i_name,name,sizeof(i->i_name));
	i->i_next=NULL;
	return(i);
	}

static int
cafree(CACHEPAYLOAD *p, CACHESTATE *s)
	{
	struct item *i;

	i=(struct item *)p;
	i->i_next=itail;
	itail=i;
	if (ihead==NULL)
		ihead=i;
	return(0);
	}

static int
cacompare(CACHEPAYLOAD *a, CACHEPAYLOAD *b, CACHESTATE *s)
	{
	struct item *ia, *ib;

	ia=(struct item *)a;	
	ib=(struct item *)b;	
	return(strncmp(ia->i_name,ib->i_name,sizeof(ia->i_name)));
	}

static int
cadump(CACHEPAYLOAD *p, FILE *fp, CACHESTATE *s)
	{
	struct item *i;

	i=(struct item *)p;
	fprintf(fp,"ITEM ``%.*s''\n",sizeof(i->i_name),i->i_name);
	return(0);
	}

/*
**	server: this is the implementation of the server thread, which
**	handles a single conversation via socket, and responds to requests
**	by replying with its own timestamp, id and sequence number.
*/
static pthread_addr_t
server(pthread_addr_t vp)
	{
	struct connection *c;
	MESSAGE *msgr, *msgs;
	PORTFOLIO box;
	PACKAGE *pack;
	int rc, reply;
	char lhs[64], rhs[64], **vec, **v;
	struct item *i;
	CACHELINE *l;

	c=(struct connection *)vp;

	msgr=NULL;
	reply=0;
	while (1)
		{
		fprintf(stderr,"SERVER %d mrecv(%d)=%d\n",
			c->c_sserial,c->c_sock,(rc=mrecv(c->c_sock,&msgr)));
		if (rc==0)
			{
			fprintf(stderr,"SERVER %d connection closed\n",
				c->c_sserial);
			break;
			}
		assert(rc>0);

		fprintf(stderr,"SERVER %d recv %.*s %d message %d\n",
			c->c_sserial,
			sizeof(msgr->msg_name),msgr->msg_name,
			msgr->msg_id,msgr->msg_sequence);

		fprintf(stderr,"SERVER %d recv parcel\n",c->c_sserial);
		kvdump(msgr->msg_parcel,stderr);

		(void)initportfolio(&box);
		pack=unbundlepackage(msgr->msg_parcel);
		(void)stuffpackage(pack,&box);
		vec=unstuffvector(&box);
		for (v=vec; *v!=NULL; v++)
			{
			(void)keyword(*v);
			semP(&caching);
			i=canew(*v);
			if ((l=cachefind(&cache,(CACHEPAYLOAD *)i))==NULL)
				{
				l=cachewrite(&cache,(CACHEPAYLOAD *)i);
				assert(l!=NULL);
				(void)cachecomplete(&cache,l);
				}
			else
				{
				(void)cafree((CACHEPAYLOAD *)i,(void *)0);
				(void)cachecomplete(&cache,l);
				}
			semV(&caching);
			}
		(void)freevector(vec);
		(void)freepackage(pack);
		(void)freeportfolio(&box);

		msgs=message(0L,0L,(long)c->c_sserial,(long)reply,"server");
		sprintf(lhs,"server.%d.%d",c->c_sserial,reply);
		(void)tsymdhms(rhs);
		(void)minsert(msgs,lhs,rhs);
		mmerge(msgr,msgs);
		reply++;

		fprintf(stderr,"SERVER %d send parcel\n",c->c_sserial);
		kvdump(msgs->msg_parcel,stderr);

		fprintf(stderr,"SERVER %d msend(%d)=%d\n",
			c->c_sserial,c->c_sock,(rc=msend(c->c_sock,msgs)));
		assert(rc>0);

		mfree(&msgs);

		semSw();
		}

	mfree(&msgr);
	mclose(c->c_sock);

	fprintf(stderr,"SERVER %d ends\n",c->c_sserial);

	c->c_sstate=C_COMPLETE;
	semV(&complete);
	pthread_exit((pthread_addr_t)1);
	}

/*
**	client: this is the implementation of a client thread, which
**	makes server requests consisting of a sequence messages
**	containing timestamped parameters. A response to each request
**	is received before sending the next request.
*/
static pthread_addr_t
client(pthread_addr_t vp)
	{
	struct connection *c;
	MESSAGE *msgs, *msgr;
	int sock, rc, i;
	char lhs[64], rhs[64];

	c=(struct connection *)vp;

	fprintf(stderr,"CLIENT %d begins %d\n",c->c_cserial,c->c_count);

	/*
	** mclient may fail if the backlog is too great. We retry a few
	** times before giving up.
	*/
	for (i=1; i<=c->c_retries; i++)
		{
		sock=mclient("localhost",c->c_port);
		if (sock>=0)
			break;
		semSw();
		}
	fprintf(stderr,"CLIENT %d mclient(localhost,%s)=%d on %d/%d\n",
		c->c_cserial,c->c_port,sock,i,c->c_retries);
	assert(sock>=0);

	msgs=message(0L,0L,(long)c->c_cserial,0L,"client");
	msgr=NULL;
	for (i=0; i<c->c_count; i++)
		{
		msgs->msg_sequence=i;

		sprintf(lhs,"client.%d.%d",c->c_cserial,i);
		(void)tsymdhms(rhs);
		(void)minsert(msgs,lhs,rhs);

		fprintf(stderr,"CLIENT %d send parcel\n",c->c_cserial);
		kvdump(msgs->msg_parcel,stderr);

		fprintf(stderr,"CLIENT %d msend(%d)=%d\n",
			c->c_cserial,sock,
			(rc=msend(sock,msgs)));
		assert(rc>=0);

		fprintf(stderr,"CLIENT %d mrecv(%d)=%d\n",
			c->c_cserial,sock,
			(rc=mrecv(sock,&msgr)));
		assert(rc>=0);

		fprintf(stderr,"CLIENT %d recv %.*s %d message %d\n",
			c->c_cserial,
			sizeof(msgr->msg_name),msgr->msg_name,
			msgr->msg_id,msgr->msg_sequence);

		fprintf(stderr,"CLIENT %d recv parcel\n",c->c_cserial);
		kvdump(msgr->msg_parcel,stderr);

		mmerge(msgr,msgs);

		semSw();
		}

	mfree(&msgr);
	mfree(&msgs);
	mclose(sock);

	fprintf(stderr,"CLIENT %d ends\n",c->c_cserial);

	c->c_cstate=C_COMPLETE;
	semV(&complete);
	pthread_exit((pthread_addr_t)2);
	}

/*
**	housekeep: reap the terminated threads.
*/
void
housekeep(struct connection *c)
	{
	int rc;

	if (c->c_cstate==C_COMPLETE)
		{
		rc=pthread_join(c->c_client,(pthread_addr_t *)&c->c_cexit);
		if (rc<0)
			perror("pthread_join client");
		fprintf(stderr,"SERVER main client %d exit %d\n",
			c->c_cserial,c->c_cexit);
		rc=pthread_detach(&c->c_client);
		if (rc<0)
			perror("pthread_detach client");
		c->c_cstate=C_UNUSED;
		}
	if (c->c_sstate==C_COMPLETE)
		{
		rc=pthread_join(c->c_server,(pthread_addr_t *)&c->c_sexit);
		if (rc<0)
			perror("pthread_join server");
		fprintf(stderr,"SERVER main server %d exit %d\n",
			c->c_sserial,c->c_sexit);
		rc=pthread_detach(&c->c_server);
		if (rc<0)
			perror("pthread_detach server");
		c->c_sstate=C_UNUSED;
		}
	}

/*
**	Main Thread
*/
main(int argc, char **argv)
	{
	char *port;
	int rc, sock, newsock, i, j, servers, clients, count, size;
	struct connection *conn;

	if (argc<4)
		fprintf(stderr,"usage: %s port clients count\n",
			argv[0]), exit(1);

	/*
	** Initialization.
	*/

	port=argv[1];
	clients=atoi(argv[2]);
	count=atoi(argv[3]);

	fprintf(stderr,"SERVER port=%s clients=%d count=%d mserver=%d\n",
		argv[1],clients,count,(sock=mserver(port)));
	assert(sock>=0);

	size=clients*sizeof(struct connection);
	conn=(struct connection *)nmalloc(size);
	assert(conn!=NULL);
	(void)bzero((char *)conn,size);

	(void)bzero((char *)&config,sizeof(config));
	config.cf_free=cafree;
	config.cf_compare=cacompare;
	config.cf_dump=cadump;
	config.cf_limit[CA_PROB]=clients;
	config.cf_limit[CA_PROT]=clients*9;
	/* config.cf_trace=stderr; */
	rc=cacheinit(&cache,&config);
	assert(rc>=0);

	semInit(&complete,0);
	semInit(&caching,1);

	/*
	** Spawn off all our client threads.
	*/

	for (i=0; i<clients; i++)
		{
		conn[i].c_cserial=i;
		conn[i].c_cstate=C_ACTIVE;
		conn[i].c_count=count;
		conn[i].c_port=port;
		conn[i].c_retries=(clients*count)*10;
		rc=pthread_create(&conn[i].c_client,pthread_attr_default,
			client,(pthread_addr_t)&conn[i]);
		assert(rc>=0);
		}

	/*
	** Enter our server loop. Normally a real server would loop
	** forever, but for this test we bail out when we know we've
	** done enough connection accepts.
	*/

	servers=0;
	while (1)
		{

		/*
		** Accept a connection request. We check to see if
		** we've done enough connection requests, and if so,
		** bail out. A _real_ server would just sit on the
		** maccept forever, but we have a priori knowledge
		** of our clients.
		*/

		if (servers>=clients)
			break;

		fprintf(stderr,"SERVER main maccept(%d)=%d\n",
			sock,(newsock=maccept(sock)));
		assert(newsock>=0);
		servers++;

		/*
		** Collect terminated threads.
		** Find up an unused entry to use for this connection.
		*/

		for (j=clients, i=0; i<clients; i++)
			{
			housekeep(&conn[i]);
			if (conn[i].c_sstate==C_UNUSED)
				j=i;
			}
		assert(j<clients);

		/*
		** Create a new server.
		*/

		conn[j].c_sserial=j;
		conn[j].c_sstate=C_ACTIVE;
		conn[j].c_sock=newsock;
		rc=pthread_create(&conn[j].c_server,pthread_attr_default,
			server,(pthread_addr_t)&conn[j]);
		assert(rc>=0);
		}

	(void)mclose(sock);

	/*
	** Semaphore wait for everyone to complete.
	*/

	for (j=(clients+servers); j>0; j--)
		{
		(void)semP(&complete);
		for (i=0; i<clients; i++)
			housekeep(&conn[i]);
		}
	fprintf(stderr,"SERVER main complete\n");
	fprintf(stderr,"complete: "); semDump(&complete,stderr);
	fprintf(stderr,"caching: "); semDump(&caching,stderr);

	(void)cachedump(&cache,stderr);
	bufpool(stderr);
	kvpool(stderr);
	mpool(stderr);
	nmallocinfo(stderr);
	nmallocpool(stderr);
	ipcdump(stderr);
	libtoolsdump(stderr);
	libipcdump(stderr);
	libmessagesdump(stderr);

	exit(0);
	}

#else /* PTHREADS_LIBTOOLS */

main(int argc, char **argv)
	{
	fprintf(stderr,"egmsgthreaded: not built with pthreads\n");
	}

#endif /* PTHREADS_LIBTOOLS */
