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
#pragma ident "@(#) $Id: type_sol.h,v 1.19 2010/01/04 20:35:51 bzfheinz Exp $"

/**@file   type_sol.h
 * @brief  type definitions for storing primal CIP solutions
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_TYPE_SOL_H__
#define __SCIP_TYPE_SOL_H__

#ifdef __cplusplus
extern "C" {
#endif

/** origin of solution: where to retrieve uncached elements */
enum SCIP_SolOrigin
{
   SCIP_SOLORIGIN_ORIGINAL  = 0,        /**< solution describes original variables; non-chached elements are zero */
   SCIP_SOLORIGIN_ZERO      = 1,        /**< all non-cached elements in solution are equal to zero */
   SCIP_SOLORIGIN_LPSOL     = 2,        /**< all non-cached elements in solution are equal to current LP solution */
   SCIP_SOLORIGIN_RELAXSOL  = 3,        /**< all non-cached elements in solution are equal to current relaxation solution */
   SCIP_SOLORIGIN_PSEUDOSOL = 4,        /**< all non-cached elements in solution are equal to current pseudo solution */
   SCIP_SOLORIGIN_UNKNOWN   = 5         /**< all non-cached elements in solution are unknown; they have to be treated
                                         *   as being an arbitrary value in the variable's bounds
                                         */
};
typedef enum SCIP_SolOrigin SCIP_SOLORIGIN;

typedef struct SCIP_Sol SCIP_SOL;                 /**< primal CIP solution */

#ifdef __cplusplus
}
#endif

#endif
