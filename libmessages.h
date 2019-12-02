/*
**	L I B M E S S A G E S
**
**	Copyright 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	@(#)libmessages.h	1.6 94/06/01 jsloan@ncar.ucar.edu
**
**	For internal use only.
**	Not recommended for inclusion by applications which use LIBIPC.
*/

#ifndef H_LIBMESSAGES
#define H_LIBMESSAGES

#ifndef PROTOTYPE_LIBTOOLS
#define PROTOTYPE_LIBTOOLS
#endif /* PROTOTYPE_LIBTOOLS */
#ifndef PROTOTYPE_LIBIPC
#define PROTOTYPE_LIBIPC
#endif /* PROTOTYPE_LIBIPC */
#ifndef PROTOTYPE_LIBMESSAGES
#define PROTOTYPE_LIBMESSAGES
#endif /* PROTOTYPE_LIBMESSAGES */

#include "serialize.h"
#include <stdio.h>
#include "ipc.h"
#include "messages.h"

#ifdef PTHREADS_LIBTOOLS

extern const SEMAPHORE *sem_messages_libm;
extern const SEMAPHORE *sem_mgeneric_libm;
extern const SEMAPHORE *sem_mgenerics_libm;
extern const SEMAPHORE *sem_mservice_libm;

extern void libmessages(void);
extern void libmessagesdump(FILE *fp);

#define P(sem)          (libmessages(),semP((SEMAPHORE *)sem))
#define V(sem)          (libmessages(),semV((SEMAPHORE *)sem))
#define SW()		(semSw())

#else /* PTHREADS_LIBTOOLS */

#define P(sem)        	(0) 
#define V(sem)          (0)
#define SW()		(0)

#endif /* PTHREADS_LIBTOOLS */

#endif /* !H_LIBMESSAGES */
