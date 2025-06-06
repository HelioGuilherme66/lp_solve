#pragma ident "@(#) $Id: mshell.h,v 1.7 2007/04/21 10:34:30 bzfkocht Exp $"
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*   File....: mshell.h                                                      */
/*   Name....: Memory Allocation Shell                                       */
/*   Author..: Thorsten Koch                                                 */
/*   Copyright by Author, All rights reserved                                */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * Copyright (C) 2007 by Thorsten Koch <koch@zib.de>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef _MSHELL_H_
#define _MSHELL_H_  

/*lint -sem(  mem_malloc, nulterm(2), 1n > 0 && 2p, @P == malloc(1n)) */
extern void*  mem_malloc(size_t, const char*, const int);
/*lint -sem(  mem_calloc, nulterm(3), 1n > 0 && 2n > 0 && 3p, @P == malloc(1n * 2n)) */
extern void*  mem_calloc(size_t, size_t, const char*, const int);
/*lint -sem(  mem_realloc, custodial(1), nulterm(3), 1p > 0 && 2n > 0 && 3p, @P == malloc(2n)) */
extern void*  mem_realloc(void*, size_t, const char*, const int);
/*lint -sem(  mem_strdup, nulterm(1), nulterm(2), 1p > 0 && 2p > 0, @P == malloc(1P) && nulterm(@p)) */
extern char*  mem_strdup(const char*, const char*, const int);
/*lint -sem(  mem_free, custodial(1), nulterm(2), 1p > 0 && 2p) */
extern void   mem_free(void*, const char*, const int);

#ifndef _MSHELL_C_ 

#ifdef strdup
#undef strdup
#endif /* strdup */

/*lint -e652 -e683*/
#define malloc(a)       mem_malloc((a), __FILE__, __LINE__)
#define calloc(a, b)    mem_calloc((a), (b), __FILE__, __LINE__)
#define realloc(a, b)   mem_realloc((a), (b), __FILE__, __LINE__)
#define strdup(a)       mem_strdup((a), __FILE__, __LINE__)
#define free(a)         mem_free((a), __FILE__, __LINE__) 
/*line +e652 +e683*/

#endif /* _MSHELL_C_ */

#ifndef NO_MSHELL 

extern size_t mem_used(void);
extern void   mem_maximum(FILE* fp);
extern void   mem_display(FILE* fp);
extern void   mem_check_x(const void* p, const char* file, const int line);
extern void   mem_check_all_x(const char* file, const int line);
extern void   mem_hide_x(void* p, const char* file, const int line);

#define mem_check(a)    mem_check_x(a, __FILE__, __LINE__)
#define mem_check_all() mem_check_all_x(__FILE__, __LINE__)
#define mem_hide(a)     mem_hide_x(a, __FILE__, __LINE__)

#else /* NO_MSHELL */

#define mem_used()          /**/
#define mem_maximum(a)      /**/
#define mem_display(a)      /**/
#define mem_check(a)        /**/
#define mem_check_all()     /**/
#define mem_hide(a)         /**/

#endif /* !NO_MSHELL */

#endif /* _MSHELL_H_ */


