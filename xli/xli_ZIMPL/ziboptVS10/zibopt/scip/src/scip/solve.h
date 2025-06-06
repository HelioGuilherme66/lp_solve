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
#pragma ident "@(#) $Id: solve.h,v 1.60 2010/09/03 14:50:16 bzfviger Exp $"

/**@file   solve.h
 * @brief  internal methods for main solving loop and node processing
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_SOLVE_H__
#define __SCIP_SOLVE_H__


#include <stdio.h>

#include "scip/def.h"
#include "blockmemshell/memory.h"
#include "scip/type_retcode.h"
#include "scip/type_set.h"
#include "scip/type_stat.h"
#include "scip/type_event.h"
#include "scip/type_lp.h"
#include "scip/type_prob.h"
#include "scip/type_primal.h"
#include "scip/type_tree.h"
#include "scip/type_pricestore.h"
#include "scip/type_sepastore.h"
#include "scip/type_cutpool.h"
#include "scip/type_conflict.h"

#ifdef __cplusplus
extern "C" {
#endif

/** returns whether the solving process will be / was stopped before proving optimality;
 *  if the solving process was stopped, stores the reason as status in stat
 */
extern
SCIP_Bool SCIPsolveIsStopped(
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< dynamic problem statistics */
   SCIP_Bool             checknodelimits     /**< should the node limits be involved in the check? */
   );

/** applies domain propagation on current node and flushes the conflict storage afterwards */
extern
SCIP_RETCODE SCIPpropagateDomains(
   BMS_BLKMEM*           blkmem,             /**< block memory buffers */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< dynamic problem statistics */
   SCIP_PROB*            prob,               /**< transformed problem after presolve */
   SCIP_PRIMAL*          primal,             /**< primal data */
   SCIP_TREE*            tree,               /**< branch and bound tree */
   SCIP_CONFLICT*        conflict,           /**< conflict analysis data */
   int                   depth,              /**< depth level to use for propagator frequency checks */
   int                   maxrounds,          /**< maximal number of propagation rounds (-1: no limit, 0: parameter settings) */
   SCIP_Bool*            cutoff              /**< pointer to store whether the node can be cut off */
   );

/** constructs the LP of the current node, but does not load the LP state and warmstart information  */
extern
SCIP_RETCODE SCIPconstructCurrentLP(
   BMS_BLKMEM*           blkmem,             /**< block memory buffers */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< dynamic problem statistics */
   SCIP_PROB*            prob,               /**< transformed problem after presolve */
   SCIP_TREE*            tree,               /**< branch and bound tree */
   SCIP_LP*              lp,                 /**< LP data */
   SCIP_PRICESTORE*      pricestore,         /**< pricing storage */
   SCIP_SEPASTORE*       sepastore,          /**< separation storage */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter,        /**< global event filter */
   SCIP_Bool*            cutoff              /**< pointer to store whether the node can be cut off */
   );

/** calls primal heuristics */
extern
SCIP_RETCODE SCIPprimalHeuristics(
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< dynamic problem statistics */
   SCIP_PRIMAL*          primal,             /**< primal data */
   SCIP_TREE*            tree,               /**< branch and bound tree */
   SCIP_LP*              lp,                 /**< LP data */
   SCIP_NODE*            nextnode,           /**< next node that will be processed, or NULL if no more nodes left */
   SCIP_HEURTIMING       heurtiming,         /**< current point in the node solving process */
   SCIP_Bool*            foundsol            /**< pointer to store whether a solution has been found */
   );

/** applies one round of separation on the given primal solution or on the LP solution */
extern
SCIP_RETCODE SCIPseparationRound(
   BMS_BLKMEM*           blkmem,             /**< block memory buffers */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< dynamic problem statistics */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter,        /**< global event filter */
   SCIP_PROB*            prob,               /**< transformed problem after presolve */
   SCIP_LP*              lp,                 /**< LP data */
   SCIP_SEPASTORE*       sepastore,          /**< separation storage */
   SCIP_SOL*             sol,                /**< primal solution that should be separated, or NULL for LP solution */
   int                   actdepth,           /**< current depth in the tree */
   SCIP_Bool             onlydelayed,        /**< should only delayed separators be called? */
   SCIP_Bool*            delayed,            /**< pointer to store whether a separator was delayed */
   SCIP_Bool*            cutoff              /**< pointer to store whether the node can be cut off */
   );

/** solves the current LP completely with pricing in new variables */
extern
SCIP_RETCODE SCIPpriceLoop(
   BMS_BLKMEM*           blkmem,             /**< block memory buffers */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< dynamic problem statistics */
   SCIP_PROB*            prob,               /**< transformed problem after presolve */
   SCIP_PRIMAL*          primal,             /**< primal data */
   SCIP_TREE*            tree,               /**< branch and bound tree */
   SCIP_LP*              lp,                 /**< LP data */
   SCIP_PRICESTORE*      pricestore,         /**< pricing storage */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter,        /**< global event filter */
   SCIP_Bool             pretendroot,        /**< should the pricers be called as if we are at the root node? */
   SCIP_Bool             displayinfo,        /**< should info lines be displayed after each pricing round? */
   int                   maxpricerounds,     /**< maximal number of pricing rounds (-1: no limit);
                                              *   a finite limit means that the LP might not be solved to optimality! */
   int*                  npricedcolvars,     /**< pointer to store number of column variables after problem vars were priced */
   SCIP_Bool*            mustsepa,           /**< pointer to store TRUE if a separation round should follow */
   SCIP_Real*            lowerbound,         /**< lower bound computed by the pricers */
   SCIP_Bool*            lperror,            /**< pointer to store whether an unresolved error in LP solving occured */
   SCIP_Bool*            aborted             /**< pointer to store whether the pricing was aborted and the lower bound must 
                                              *   not be used */
   );

/** main solving loop */
extern
SCIP_RETCODE SCIPsolveCIP(
   BMS_BLKMEM*           blkmem,             /**< block memory buffers */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< dynamic problem statistics */
   SCIP_MEM*             mem,                /**< block memory pools */
   SCIP_PROB*            prob,               /**< transformed problem after presolve */
   SCIP_PRIMAL*          primal,             /**< primal data */
   SCIP_TREE*            tree,               /**< branch and bound tree */
   SCIP_LP*              lp,                 /**< LP data */
   SCIP_RELAXATION*      relaxation,         /**< global relaxation data */
   SCIP_PRICESTORE*      pricestore,         /**< pricing storage */
   SCIP_SEPASTORE*       sepastore,          /**< separation storage */
   SCIP_CUTPOOL*         cutpool,            /**< global cut pool */
   SCIP_BRANCHCAND*      branchcand,         /**< branching candidate storage */
   SCIP_CONFLICT*        conflict,           /**< conflict analysis data */
   SCIP_EVENTFILTER*     eventfilter,        /**< event filter for global (not variable dependent) events */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_Bool*            restart             /**< should solving process be started again with presolving? */
   );

#ifdef __cplusplus
}
#endif

#endif
