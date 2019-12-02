/*
**	M G E N P A C K E T
**
**	Copyright 1992	University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	@(#)mgenpacket.h	4.1 94/05/16 jsloan@ncar.ucar.edu
**
*/

#ifndef H_MGENPACKET
#define H_MGENPACKET

#include "buffers.h"
#include "mproto.h"

extern int mgfrepacket MPROTO((void *vp));

extern BUFFER mgencpacket MPROTO((void *vp, int *lengthp));

extern void *mgdecpacket MPROTO((BUFFER buffer, int length));

#endif /* !H_MGENPACKET */
