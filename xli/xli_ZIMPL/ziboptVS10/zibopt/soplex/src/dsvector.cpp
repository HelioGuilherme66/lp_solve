/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the class library                   */
/*       SoPlex --- the Sequential object-oriented simPlex.                  */
/*                                                                           */
/*    Copyright (C) 1996      Roland Wunderling                              */
/*                  1996-2010 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SoPlex is distributed under the terms of the ZIB Academic Licence.       */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SoPlex; see the file COPYING. If not email to soplex@zib.de.  */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: dsvector.cpp,v 1.23 2010/09/16 17:45:02 bzfgleix Exp $"

#include <assert.h>
#include <iostream>

#include "spxdefines.h"
#include "dsvector.h"
#include "spxalloc.h"
#include "message.h"

namespace soplex
{

//-----------------------------------------------------------
//   Construction / destruction
//-----------------------------------------------------------

DSVector::DSVector(int n)
   : theelem( 0 )
{
   allocMem((n < 1) ? 2 : n + 1);

   assert(DSVector::isConsistent());
}

DSVector::~DSVector()
{
   if(theelem)
      spx_free(theelem);
}

DSVector::DSVector(const Vector& vec)
   : theelem( 0 )
{
   allocMem((vec.dim() < 1) ? 2 : vec.dim() + 1);
   *this = vec;

   assert(DSVector::isConsistent());
}

DSVector::DSVector(const SVector& old)
   : theelem( 0 )
{
   allocMem(old.size() + 1);
   SVector::operator= ( old );

   assert(DSVector::isConsistent());
}

DSVector::DSVector(const SSVector& old)
   : theelem( 0 )
{
   allocMem(old.size() + 1);
   SVector::operator= ( old );

   assert(DSVector::isConsistent());
}

DSVector::DSVector(const DSVector& old)
   : SVector()
   , theelem( 0 )
{
   allocMem(old.size() + 1);
   SVector::operator= ( old );

   assert(DSVector::isConsistent());
}

DSVector& DSVector::operator=(const Vector& vec)
{
   clear();
   setMax(vec.dim());
   SVector::operator=(vec);

   assert(DSVector::isConsistent());

   return *this;
}

//-----------------------------------------------------------
//   memory stuff
//-----------------------------------------------------------

void DSVector::allocMem(int len)
{
   spx_alloc(theelem, len);
   setMem(len, theelem);
}

void DSVector::setMax(int newmax)
{
   int siz = size();
   int len = ((newmax < siz) ? siz : newmax) + 1;

   spx_realloc(theelem, len);
   setMem (len, theelem);
   set_size( siz );
}

//-----------------------------------------------------------
//   consistency check
//-----------------------------------------------------------

#ifndef NO_CONSISTENCY_CHECKS
bool DSVector::isConsistent() const
{
   if ((theelem != 0) && (mem() != theelem))
      return MSGinconsistent("DSVector");

   return true;
}
#endif
} // namespace soplex

//-----------------------------------------------------------------------------
//Emacs Local Variables:
//Emacs mode:c++
//Emacs c-basic-offset:3
//Emacs tab-width:8
//Emacs indent-tabs-mode:nil
//Emacs End:
//-----------------------------------------------------------------------------


