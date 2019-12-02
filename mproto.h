/*
**	M P R O T O
**
**	Copyright 1994 University Corporation for Atmospheric Research
**			All Rights Reserved
**
**	@(#)mproto.h	4.1 94/05/16 jloan@ncar.ucar.edu
**
**	This is a little macro that simplifies prototyping function
**	declarations for porting between ANSI C and non-ANSI C
**	compilers.
*/

#ifndef H_MPROTO
#define H_MPROTO

#ifdef PROTOTYPE_LIBMESSAGES
#define MPROTO(list)	list
#else /* PROTOTYPE_LIBMESSAGES */
#define MPROTO(list)	()
#endif /* PROTOTYPE_LIBMESSAGES */

#endif /* !H_MPROTO */
