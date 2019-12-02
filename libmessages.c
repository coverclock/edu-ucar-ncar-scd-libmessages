/*
**	L I B M E S S A G E S
**
**	Copyright 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	Title		Libmessages Initialization
**	Program		libmessages
**	Project		IPC Library
**	Author		John Sloan
**	Email		jsloan@ncar.ucar.edu
**	Date		Tue May 17 16:16:34 MDT 1994
**	Organization	NCAR, P.O. Box 3000, Boulder CO 80307
*/

static char copyright[]="Copyright 1994 University Corporation for Atmospheric Research - All Rights Reserved";
static char sccsid[]="@(#)libmessages.c	1.3 94/05/23 jsloan@ncar.ucar.edu";

#include "libmessages.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef PTHREADS_LIBTOOLS

#define SEMAPHORES	(4)

static SEMAPHORE semaphores[SEMAPHORES];

const SEMAPHORE *sem_messages_libm=&semaphores[0];
const SEMAPHORE *sem_mgeneric_libm=&semaphores[1];
const SEMAPHORE *sem_mgenerics_libm=&semaphores[2];
const SEMAPHORE *sem_mservice_libm=&semaphores[3];

static char *names[]=
	{
	"sem_messages_libm",
	"sem_mgeneric_libm",
	"sem_mgenerics_libm",
	"sem_mservice_libm"
	};

void
libmessagesdump(FILE *fp)
	{
	int i;

	for (i=0; i<SEMAPHORES; i++)
		{
		fputs(names[i],fp);
		fputs(": ",fp);
		semDump(&semaphores[i],fp);
		}
	}


static void
initmessages(void)
	{
	int i;

	for (i=0; i<SEMAPHORES; i++)
		semInit(&semaphores[i],1);
	}

void
libmessages(void)
	{
	int rc;
	static pthread_once_t once=pthread_once_init;

	rc=pthread_once(&once,initmessages);
	assert(rc>=0);
	}

#endif /* PTHREADS_LIBTOOLS */
