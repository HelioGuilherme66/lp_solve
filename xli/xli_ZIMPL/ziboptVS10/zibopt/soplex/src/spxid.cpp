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
#pragma ident "@(#) $Id: spxid.cpp,v 1.9 2010/09/16 17:45:03 bzfgleix Exp $"

#include <stdlib.h>
#include <assert.h>

#include "spxid.h"
#include "exceptions.h"

namespace soplex
{
SPxColId::SPxColId(const DataKey& p_key) 
   : DataKey(p_key)
{
   info = SPxId::COL_ID;
}

SPxColId::SPxColId(const SPxId& p_key) 
   : DataKey(p_key)
{
   assert(!p_key.isSPxRowId());

   info = SPxId::COL_ID;
}

SPxRowId::SPxRowId(const DataKey& p_key) 
   : DataKey(p_key)
{
   info = SPxId::ROW_ID;
}

SPxRowId::SPxRowId(const SPxId& p_key) 
   : DataKey(p_key)
{
   assert(!p_key.isSPxColId());

   info = SPxId::ROW_ID;
}

std::ostream& operator<<(std::ostream& os, const SPxId& id)
{
   switch(id.type())
   {
   case SPxId::ROW_ID:
      os << "row ";
      break;
   case SPxId::COL_ID :
      os << "col ";
      break;
   case SPxId::INVALID :
      os << "Invalid ";
      break;
   default :
      throw SPxInternalCodeException("XSPXID01 This should never happen.");
   }
   os << id.idx << " (" << id.info << ")";

   return os;
}

} // namespace soplex

//-----------------------------------------------------------------------------
//Emacs Local Variables:
//Emacs mode:c++
//Emacs c-basic-offset:3
//Emacs tab-width:8
//Emacs indent-tabs-mode:nil
//Emacs End:
//-----------------------------------------------------------------------------




