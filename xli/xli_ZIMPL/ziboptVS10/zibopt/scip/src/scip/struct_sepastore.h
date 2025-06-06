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
#pragma ident "@(#) $Id: struct_sepastore.h,v 1.26 2010/01/04 20:35:50 bzfheinz Exp $"

/**@file   struct_sepastore.h
 * @brief  datastructures for storing separated cuts
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_STRUCT_SEPASTORE_H__
#define __SCIP_STRUCT_SEPASTORE_H__


#include "scip/def.h"
#include "scip/type_lp.h"
#include "scip/type_var.h"
#include "scip/type_sepastore.h"

#ifdef __cplusplus
extern "C" {
#endif

/** storage for separated cuts */
struct SCIP_SepaStore
{
   SCIP_ROW**            cuts;               /**< array with separated cuts sorted by score */
   SCIP_Real*            efficacies;         /**< efficacy for each separated cut: distance of hyperplane to LP solution */
   SCIP_Real*            objparallelisms;    /**< parallelism of cut to the objective function */
   SCIP_Real*            orthogonalities;    /**< minimal orthogonality of cut with all other cuts of larger score */
   SCIP_Real*            scores;             /**< score for each separated cut: weighted sum of efficacy and orthogonality */
   int                   cutssize;           /**< size of cuts and score arrays */
   int                   ncuts;              /**< number of separated cuts (max. is set->sepa_maxcuts) */
   int                   nforcedcuts;        /**< number of forced separated cuts (first positions in cuts array) */
   int                   ncutsfound;         /**< total number of cuts found so far */
   int                   ncutsfoundround;    /**< number of cuts found so far in this separation round */
   int                   ncutsapplied;       /**< total number of cuts applied to the LPs */
   SCIP_Bool             initiallp;          /**< is the separation storage currently being filled with the initial LP rows? */
   SCIP_Bool             forcecuts;          /**< should the cuts be used despite the number of cuts parameter limit? */
};

#ifdef __cplusplus
}
#endif

#endif
