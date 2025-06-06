/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2010 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: struct_mem.h,v 1.14 2010/07/29 14:01:20 bzfheinz Exp $"

/**@file   struct_mem.h
 * @brief  datastructures for block memory pools and memory buffers
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_STRUCT_MEM_H__
#define __SCIP_STRUCT_MEM_H__


#include "scip/def.h"
#include "blockmemshell/memory.h"
#include "scip/type_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

/** various block memory buffers */
struct SCIP_Mem
{
   BMS_BLKMEM*           setmem;             /**< memory blocks for parameter settings */
   BMS_BLKMEM*           probmem;            /**< memory blocks for original problem and solution process: preprocessing, bab-tree, ... */
};

#ifdef __cplusplus
}
#endif

#endif
