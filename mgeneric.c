/*
**	M G E N E R I C
**
**	Copyright 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Generic Message Allocation and Free
**	Program		libipc
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Tue Dec 15 16:32:03 MST 1992
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This module contains the supporting routines for passing
**	generic structures as messages.
*/

static char copyright[]="Copyright 1992, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)mgeneric.c	4.2 94/05/17 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include "errors.h"
#include "lists.h"
#include "serialize.h"
#include "nmalloc.h"
#include "mgenerics.h"
#include "mgeneric.h"

static LIST pool={NULL,NULL};	/* thread safe: sem_mgeneric_libm */
static int allocated=0;		/* thread safe: sem_mgeneric_libm */
static int active=0;		/* thread safe: sem_mgeneric_libm */

#ifdef MGENERIC_STATS
#define ALLOCATING()	{ \
			P(sem_mgeneric_libm); \
			allocated++; \
			V(sem_mgeneric_libm); \
                        }
#define USING()		{ \
			P(sem_mgeneric_libm); \
			active++; \
			V(sem_mgeneric_libm); \
			}
#define FREEING()	(active--)
#else /* MGENERIC_STATS */
#define ALLOCATING()
#define USING()
#define FREEING()
#endif /* MGENERIC_STATS */

/*
**	mgenalloc: dynamically allocate and initialize a generic
**	message.
*/
MGENERIC *
mgenalloc(int type)
	{
	MGENERIC *m;
	void *vp;

	P(sem_mgeneric_libm);
	m=(MGENERIC *)lget(&pool);
	V(sem_mgeneric_libm);

	if (m==NULL)
		if ((m=(MGENERIC *)nmalloc(sizeof(MGENERIC)))==NULL)
			return(NULL);
		else
			ALLOCATING();

	USING();

	(void)bzero(m,sizeof(MGENERIC));

	m->msg_type=type;
	m->msg_address=MGENLOCALHOST;
	m->msg_identity=geteuid();

	return(m);
	}

/*
**	mgenfree: free all dynamically acquired memory associated
**	with a generic message.
*/
int
mgenfree(MGENERIC *m)
	{
	if (m==NULL)
		{
		seterrno(EINVAL);
		return(-1);
		}

	if (m->msg_body!=NULL)
		(*(mgtfree(m->msg_type)))(m->msg_body);

	P(sem_mgeneric_libm);
	(void)lput(&pool,(LISTNODE *)&(m->msg_next));
	FREEING();
	V(sem_mgeneric_libm);

	return(0);
	}

/*
**	mgenpool: print on fp stats about internally held dynamically
**	acquired storage.
*/
void
mgenpool(FILE *fp)
	{
	P(sem_mgeneric_libm);
	fprintf(fp,"mgeneric: %8dbytes %8dactive %8dallocated\n",
		(allocated*sizeof(MGENERIC)),active,allocated);
	V(sem_mgeneric_libm);
	}

/*
**	mgenshutdown: release all internally held dynamically acquired
**	storage.
*/
void
mgenshutdown(void)
	{
	MGENERIC *m;

	P(sem_mgeneric_libm);

	for (m=(MGENERIC *)lget(&pool); m!=NULL; m=(MGENERIC *)lget(&pool))
		(void)nfree((void *)m);

	allocated=active;

	V(sem_mgeneric_libm);
	}
