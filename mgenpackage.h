/*
**	M G E N P A C K A G E
**
**	Copyright 1992	University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	@(#)mgenpackage.h	4.1 94/05/16 jsloan@ncar.ucar.edu
**
*/

#ifndef H_MGENPACKAGE
#define H_MGENPACKAGE

#include "buffers.h"
#include "mproto.h"

extern int mgfrepackage MPROTO((void *vp));

extern BUFFER mgencpackage MPROTO((void *vp, int *lengthp));

extern void *mgdecpackage MPROTO((BUFFER buffer, int length));

#endif /* !H_MGENPACKAGE */
