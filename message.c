/*
**	M E S S A G E
**
**	Copyright 1989, 1990, 1991, 1992, 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Message Passing Message
**	Program		libipc
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Mon Oct  2 16:39:05 MDT 1989
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
**
**	Abstract
**
**	This function is part of a TAGS-specific message passing
**	package built on top of the generic IPC package.
*/

static char copyright[]="Copyright 1989, 1990, 1991, 1992, 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)message.c	4.3 96/07/08 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include "libtstrings.h"
#include "parcel.h"
#include "lists.h"
#include "serialize.h"
#include "nmalloc.h"

static LIST pool={NULL,NULL};	/* thread safe: sem_messages_libm */
static int allocated=0;		/* thread safe: sem_messages_libm */
static int active=0;		/* thread safe: sem_messages_libm */

#ifdef MESSAGES_STATS
#define ALLOCATING()	{ \
			P(sem_messages_libm); \
			allocated++; \
			V(sem_messages_libm); \
                        }
#define USING()		{ \
			P(sem_messages_libm); \
			active++; \
			V(sem_messages_libm); \
			}
#define FREEING()	(active--)
#else /* MESSAGES_STATS */
#define ALLOCATING()
#define USING()
#define FREEING()
#endif /* MESSAGES_STATS */

/*
**	message: dynamically allocate and initialize a MESSAGE
**	structure.
**
**	Arguments:
**		function	initial value of function field
**		status		initial value of status field
**		id		initial value of id field
**		sequence	initial value of sequence field
**		name		identifying character string
**
**	Returns:	address of allocated MESSAGE structure
*/
MESSAGE *
message(long function, long status, long id, long sequence, char *name)
	{
	MESSAGE *m;

	P(sem_messages_libm);
	m=(MESSAGE *)lget(&pool);
	V(sem_messages_libm);

	if (m==NULL)
		if ((m=(MESSAGE *)nmalloc(sizeof(MESSAGE)))==NULL)
			return(NULL);
		else
			ALLOCATING();

	USING();

	m->msg_socket=0;
	m->msg_address=0;
	m->msg_port=0;
	m->msg_identity=0;
	m->msg_function=function;
	m->msg_status=status;
	m->msg_id=id;
	m->msg_sequence=sequence;
	if (name==NULL)
		*m->msg_name='\0';
	else
		(void)strncpy(m->msg_name,name,MSGNAMELEN);
	(void)initparcel(&(m->msg_parcel));

	return(m);
	}

/*
**	mfree: free all dynamically acquired memory associated
**	with a MESSAGE structure. Not usually necessary since the
**	other m* routines handle allocation and deallocation when
**	needed.
**
**	Arguments:
**		mp	address of pointer to MESSAGE structure
**
**	Returns:	void
*/
void
mfree(MESSAGE **mp)
	{
	if (mp!=NULL)
		if (*mp!=NULL)
			{

			if ((*mp)->msg_parcel!=NULL)
				(void)freeparcel((*mp)->msg_parcel);

			P(sem_messages_libm);
			(void)lput(&pool,(LISTNODE *)&((*mp)->msg_next));
			FREEING();
			V(sem_messages_libm);

			*mp=NULL;
			}
	}

/*
**	mpool: print on fp stats about internally held dynamically
**	acquired storage.
*/
void
mpool(FILE *fp)
	{
	P(sem_messages_libm);
	fprintf(fp,"messages: %8dbytes %8dactive %8dallocated\n",
		(allocated*sizeof(MESSAGE)),active,allocated);
	V(sem_messages_libm);
	}

/*
**	mshutdown: release all internally held dynamically acquired
**	storage.
*/
void
mshutdown(void)
	{
	MESSAGE *m;

	P(sem_messages_libm);

	for (m=(MESSAGE *)lget(&pool); m!=NULL; m=(MESSAGE *)lget(&pool))
		(void)nfree((void *)m);

	allocated=active;

	V(sem_messages_libm);
	}
