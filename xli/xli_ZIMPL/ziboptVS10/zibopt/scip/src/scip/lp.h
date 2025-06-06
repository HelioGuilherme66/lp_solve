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
#pragma ident "@(#) $Id: lp.h,v 1.143 2010/09/18 19:26:25 bzfpfets Exp $"

/**@file   lp.h
 * @brief  internal methods for LP management
 * @author Tobias Achterberg
 * @author Marc Pfetsch
 * @author Kati Wolter
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_LP_H__
#define __SCIP_LP_H__


#include <stdio.h>

#include "scip/def.h"
#include "blockmemshell/memory.h"
#include "scip/type_set.h"
#include "scip/type_stat.h"
#include "scip/type_misc.h"
#include "scip/type_lp.h"
#include "scip/type_var.h"
#include "scip/type_prob.h"
#include "scip/type_sol.h"
#include "scip/pub_lp.h"

#include "scip/struct_lp.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Column methods
 */

/** creates an LP column */
extern
SCIP_RETCODE SCIPcolCreate(
   SCIP_COL**            col,                /**< pointer to column data */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_VAR*             var,                /**< variable, this column represents */
   int                   len,                /**< number of nonzeros in the column */
   SCIP_ROW**            rows,               /**< array with rows of column entries */
   SCIP_Real*            vals,               /**< array with coefficients of column entries */
   SCIP_Bool             removable           /**< should the column be removed from the LP due to aging or cleanup? */
   );

/** frees an LP column */
extern
SCIP_RETCODE SCIPcolFree(
   SCIP_COL**            col,                /**< pointer to LP column */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_LP*              lp                  /**< current LP data */
   );

/** adds a previously non existing coefficient to an LP column */
extern
SCIP_RETCODE SCIPcolAddCoef(
   SCIP_COL*             col,                /**< LP column */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_ROW*             row,                /**< LP row */
   SCIP_Real             val                 /**< value of coefficient */
   );

/** deletes coefficient from column */
extern
SCIP_RETCODE SCIPcolDelCoef(
   SCIP_COL*             col,                /**< column to be changed */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_ROW*             row                 /**< coefficient to be deleted */
   );

/** changes or adds a coefficient to an LP column */
extern
SCIP_RETCODE SCIPcolChgCoef(
   SCIP_COL*             col,                /**< LP column */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_ROW*             row,                /**< LP row */
   SCIP_Real             val                 /**< value of coefficient */
   );

/** increases value of an existing or nonexisting coefficient in an LP column */
extern
SCIP_RETCODE SCIPcolIncCoef(
   SCIP_COL*             col,                /**< LP column */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_ROW*             row,                /**< LP row */
   SCIP_Real             incval              /**< value to add to the coefficient */
   );

/** changes objective value of column */
extern
SCIP_RETCODE SCIPcolChgObj(
   SCIP_COL*             col,                /**< LP column to change */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_Real             newobj              /**< new objective value */
   );

/** changes lower bound of column */
extern
SCIP_RETCODE SCIPcolChgLb(
   SCIP_COL*             col,                /**< LP column to change */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_Real             newlb               /**< new lower bound value */
   );

/** changes upper bound of column */
extern
SCIP_RETCODE SCIPcolChgUb(
   SCIP_COL*             col,                /**< LP column to change */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_Real             newub               /**< new upper bound value */
   );

/** calculates the reduced costs of a column using the given dual solution vector */
extern
SCIP_Real SCIPcolCalcRedcost(
   SCIP_COL*             col,                /**< LP column */
   SCIP_Real*            dualsol             /**< dual solution vector for current LP rows */
   );

/** gets the reduced costs of a column in last LP or after recalculation */
extern
SCIP_Real SCIPcolGetRedcost(
   SCIP_COL*             col,                /**< LP column */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp                  /**< current LP data */
   );

/** gets the feasibility of (the dual row of) a column in last LP or after recalculation */
extern
SCIP_Real SCIPcolGetFeasibility(
   SCIP_COL*             col,                /**< LP column */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp                  /**< current LP data */
   );

/** calculates the Farkas coefficient y^T A_i of a column i using the given dual Farkas vector y */
extern
SCIP_Real SCIPcolCalcFarkasCoef(
   SCIP_COL*             col,                /**< LP column */
   SCIP_Real*            dualfarkas          /**< dense dual Farkas vector for current LP rows */
   );

/** gets the Farkas coefficient y^T A_i of a column i in last LP (which must be infeasible) */
extern
SCIP_Real SCIPcolGetFarkasCoef(
   SCIP_COL*             col,                /**< LP column */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp                  /**< current LP data */
   );

/** gets the Farkas value of a column in last LP (which must be infeasible), i.e. the Farkas coefficient y^T A_i times
 *  the best bound for this coefficient, i.e. max{y^T A_i x_i | lb <= x_i <= ub}
 */
extern
SCIP_Real SCIPcolGetFarkasValue(
   SCIP_COL*             col,                /**< LP column */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp                  /**< current LP data */
   );

/** start strong branching - call before any strongbranching */
extern
SCIP_RETCODE SCIPlpStartStrongbranch(
   SCIP_LP*              lp                  /**< LP data */
   );

/** end strong branching - call after any strongbranching */
extern
SCIP_RETCODE SCIPlpEndStrongbranch(
   SCIP_LP*              lp                  /**< LP data */
   );

/** gets strong branching information on a column variable with fractional value */
SCIP_RETCODE SCIPcolGetStrongbranchFrac(
   SCIP_COL*             col,                /**< LP column */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< dynamic problem statistics */
   SCIP_LP*              lp,                 /**< LP data */
   int                   itlim,              /**< iteration limit for strong branchings */
   SCIP_Real*            down,               /**< stores dual bound after branching column down */
   SCIP_Real*            up,                 /**< stores dual bound after branching column up */
   SCIP_Bool*            downvalid,          /**< stores whether the returned down value is a valid dual bound, or NULL;
                                              *   otherwise, it can only be used as an estimate value */
   SCIP_Bool*            upvalid,            /**< stores whether the returned up value is a valid dual bound, or NULL;
                                              *   otherwise, it can only be used as an estimate value */
   SCIP_Bool*            lperror             /**< pointer to store whether an unresolved LP error occurred */
   );

/** gets strong branching information on a column variable with integral value */
SCIP_RETCODE SCIPcolGetStrongbranchInt(
   SCIP_COL*             col,                /**< LP column */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< dynamic problem statistics */
   SCIP_LP*              lp,                 /**< LP data */
   int                   itlim,              /**< iteration limit for strong branchings */
   SCIP_Real*            down,               /**< stores dual bound after branching column down */
   SCIP_Real*            up,                 /**< stores dual bound after branching column up */
   SCIP_Bool*            downvalid,          /**< stores whether the returned down value is a valid dual bound, or NULL;
                                              *   otherwise, it can only be used as an estimate value */
   SCIP_Bool*            upvalid,            /**< stores whether the returned up value is a valid dual bound, or NULL;
                                              *   otherwise, it can only be used as an estimate value */
   SCIP_Bool*            lperror             /**< pointer to store whether an unresolved LP error occurred */
   );

/** gets strong branching information on column variables with fractional values */
extern
SCIP_RETCODE SCIPcolGetStrongbranchesFrac(
   SCIP_COL**            cols,               /**< LP columns */
   int                   ncols,              /**< number of columns */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< dynamic problem statistics */
   SCIP_LP*              lp,                 /**< LP data */
   int                   itlim,              /**< iteration limit for strong branchings */
   SCIP_Real*            down,               /**< stores dual bounds after branching columns down */
   SCIP_Real*            up,                 /**< stores dual bounds after branching columns up */
   SCIP_Bool*            downvalid,          /**< stores whether the returned down values are valid dual bounds, or NULL;
                                              *   otherwise, they can only be used as an estimate value */
   SCIP_Bool*            upvalid,            /**< stores whether the returned up values are valid dual bounds, or NULL;
                                              *   otherwise, they can only be used as an estimate value */
   SCIP_Bool*            lperror             /**< pointer to store whether an unresolved LP error occurred */
   );

/** gets strong branching information on column variables with integral values */
extern
SCIP_RETCODE SCIPcolGetStrongbranchesInt(
   SCIP_COL**            cols,               /**< LP columns */
   int                   ncols,              /**< number of columns */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< dynamic problem statistics */
   SCIP_LP*              lp,                 /**< LP data */
   int                   itlim,              /**< iteration limit for strong branchings */
   SCIP_Real*            down,               /**< stores dual bounds after branching columns down */
   SCIP_Real*            up,                 /**< stores dual bounds after branching columns up */
   SCIP_Bool*            downvalid,          /**< stores whether the returned down values are valid dual bounds, or NULL;
                                              *   otherwise, they can only be used as an estimate value */
   SCIP_Bool*            upvalid,            /**< stores whether the returned up values are valid dual bounds, or NULL;
                                              *   otherwise, they can only be used as an estimate value */
   SCIP_Bool*            lperror             /**< pointer to store whether an unresolved LP error occurred */
   );

/** gets last strong branching information available for a column variable;
 *  returns values of SCIP_INVALID, if strong branching was not yet called on the given column;
 *  keep in mind, that the returned old values may have nothing to do with the current LP solution
 */
extern
void SCIPcolGetStrongbranchLast(
   SCIP_COL*             col,                /**< LP column */
   SCIP_Real*            down,               /**< stores dual bound after branching column down, or NULL */
   SCIP_Real*            up,                 /**< stores dual bound after branching column up, or NULL */
   SCIP_Bool*            downvalid,          /**< stores whether the returned down value is a valid dual bound, or NULL;
                                              *   otherwise, it can only be used as an estimate value */
   SCIP_Bool*            upvalid,            /**< stores whether the returned up value is a valid dual bound, or NULL;
                                              *   otherwise, it can only be used as an estimate value */
   SCIP_Real*            solval,             /**< stores LP solution value of column at last strong branching call, or NULL */
   SCIP_Real*            lpobjval            /**< stores LP objective value at last strong branching call, or NULL */
   );

/** if strong branching was already applied on the column at the current node, returns the number of LPs solved after
 *  the LP where the strong branching on this column was applied;
 *  if strong branching was not yet applied on the column at the current node, returns INT_MAX
 */
extern
int SCIPcolGetStrongbranchLPAge(
   SCIP_COL*             col,                /**< LP column */
   SCIP_STAT*            stat                /**< dynamic problem statistics */
   );




/*
 * Row methods
 */

/** creates and captures an LP row */
extern
SCIP_RETCODE SCIProwCreate(
   SCIP_ROW**            row,                /**< pointer to LP row data */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   const char*           name,               /**< name of row */
   int                   len,                /**< number of nonzeros in the row */
   SCIP_COL**            cols,               /**< array with columns of row entries */
   SCIP_Real*            vals,               /**< array with coefficients of row entries */
   SCIP_Real             lhs,                /**< left hand side of row */
   SCIP_Real             rhs,                /**< right hand side of row */
   SCIP_Bool             local,              /**< is row only valid locally? */
   SCIP_Bool             modifiable,         /**< is row modifiable during node processing (subject to column generation)? */
   SCIP_Bool             removable           /**< should the row be removed from the LP due to aging or cleanup? */
   );

/** frees an LP row */
extern
SCIP_RETCODE SCIProwFree(
   SCIP_ROW**            row,                /**< pointer to LP row */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_LP*              lp                  /**< current LP data */
   );

/** ensures, that column array of row can store at least num entries */
extern
SCIP_RETCODE SCIProwEnsureSize(
   SCIP_ROW*             row,                /**< LP row */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   int                   num                 /**< minimum number of entries to store */
   );

/** increases usage counter of LP row */
extern
void SCIProwCapture(
   SCIP_ROW*             row                 /**< LP row */
   );

/** decreases usage counter of LP row, and frees memory if necessary */
extern
SCIP_RETCODE SCIProwRelease(
   SCIP_ROW**            row,                /**< pointer to LP row */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_LP*              lp                  /**< current LP data */
   );

/** enables delaying of row sorting */
extern
void SCIProwDelaySort(
   SCIP_ROW*             row                 /**< LP row */
   );

/** disables delaying of row sorting, sorts row and merges coefficients with equal columns */
extern
void SCIProwForceSort(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** adds a previously non existing coefficient to an LP row */
extern
SCIP_RETCODE SCIProwAddCoef(
   SCIP_ROW*             row,                /**< LP row */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_COL*             col,                /**< LP column */
   SCIP_Real             val                 /**< value of coefficient */
   );

/** deletes coefficient from row */
extern
SCIP_RETCODE SCIProwDelCoef(
   SCIP_ROW*             row,                /**< LP row */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_COL*             col                 /**< coefficient to be deleted */
   );

/** changes or adds a coefficient to an LP row */
extern
SCIP_RETCODE SCIProwChgCoef(
   SCIP_ROW*             row,                /**< LP row */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_COL*             col,                /**< LP column */
   SCIP_Real             val                 /**< value of coefficient */
   );

/** increases value of an existing or nonexisting coefficient in an LP column */
extern
SCIP_RETCODE SCIProwIncCoef(
   SCIP_ROW*             row,                /**< LP row */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_COL*             col,                /**< LP column */
   SCIP_Real             incval              /**< value to add to the coefficient */
   );

/** changes constant value of a row */
extern
SCIP_RETCODE SCIProwChgConstant(
   SCIP_ROW*             row,                /**< LP row */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_Real             constant            /**< new constant value */
   );

/** add constant value to a row */
extern
SCIP_RETCODE SCIProwAddConstant(
   SCIP_ROW*             row,                /**< LP row */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_Real             addval              /**< constant value to add to the row */
   );

/** changes left hand side of LP row */
extern
SCIP_RETCODE SCIProwChgLhs(
   SCIP_ROW*             row,                /**< LP row */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_Real             lhs                 /**< new left hand side */
   );

/** changes right hand side of LP row */
extern
SCIP_RETCODE SCIProwChgRhs(
   SCIP_ROW*             row,                /**< LP row */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_Real             rhs                 /**< new right hand side */
   );

/** tries to find a value, such that all row coefficients, if scaled with this value become integral */
extern
SCIP_RETCODE SCIProwCalcIntegralScalar(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_Real             mindelta,           /**< minimal relative allowed difference of scaled coefficient s*c and integral i */
   SCIP_Real             maxdelta,           /**< maximal relative allowed difference of scaled coefficient s*c and integral i */
   SCIP_Longint          maxdnom,            /**< maximal denominator allowed in rational numbers */
   SCIP_Real             maxscale,           /**< maximal allowed scalar */
   SCIP_Bool             usecontvars,        /**< should the coefficients of the continuous variables also be made integral? */
   SCIP_Real*            intscalar,          /**< pointer to store scalar that would make the coefficients integral */
   SCIP_Bool*            success             /**< stores whether returned value is valid */
   );

/** tries to scale row, s.t. all coefficients become integral */
extern
SCIP_RETCODE SCIProwMakeIntegral(
   SCIP_ROW*             row,                /**< LP row */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_Real             mindelta,           /**< minimal relative allowed difference of scaled coefficient s*c and integral i */
   SCIP_Real             maxdelta,           /**< maximal relative allowed difference of scaled coefficient s*c and integral i */
   SCIP_Longint          maxdnom,            /**< maximal denominator allowed in rational numbers */
   SCIP_Real             maxscale,           /**< maximal value to scale row with */
   SCIP_Bool             usecontvars,        /**< should the coefficients of the continuous variables also be made integral? */
   SCIP_Bool*            success             /**< stores whether row could be made rational */
   );

/** recalculates the current activity of a row */
extern
void SCIProwRecalcLPActivity(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_STAT*            stat                /**< problem statistics */
   );

/** returns the activity of a row in the current LP solution */
extern
SCIP_Real SCIProwGetLPActivity(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp                  /**< current LP data */
   );

/** returns the feasibility of a row in the current LP solution: negative value means infeasibility */
extern
SCIP_Real SCIProwGetLPFeasibility(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_LP*              lp                  /**< current LP data */
   );

/** calculates the current pseudo activity of a row */
extern
void SCIProwRecalcPseudoActivity(
   SCIP_ROW*             row,                /**< row data */
   SCIP_STAT*            stat                /**< problem statistics */
   );

/** returns the pseudo activity of a row in the current pseudo solution */
extern
SCIP_Real SCIProwGetPseudoActivity(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_STAT*            stat                /**< problem statistics */
   );

/** returns the pseudo feasibility of a row in the current pseudo solution: negative value means infeasibility */
extern
SCIP_Real SCIProwGetPseudoFeasibility(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_STAT*            stat                /**< problem statistics */
   );

/** returns the activity of a row for a given solution */
extern
SCIP_Real SCIProwGetSolActivity(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics data */
   SCIP_SOL*             sol                 /**< primal CIP solution */
   );

/** returns the feasibility of a row for the given solution */
extern
SCIP_Real SCIProwGetSolFeasibility(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics data */
   SCIP_SOL*             sol                 /**< primal CIP solution */
   );

/** returns the minimal activity of a row w.r.t. the columns' bounds */
extern
SCIP_Real SCIProwGetMinActivity(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat                /**< problem statistics data */
   );

/** returns the maximal activity of a row w.r.t. the columns' bounds */
extern
SCIP_Real SCIProwGetMaxActivity(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat                /**< problem statistics data */
   );

/** returns whether the row is unmodifiable and redundant w.r.t. the columns' bounds */
extern
SCIP_Bool SCIProwIsRedundant(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat                /**< problem statistics data */
   );

/** gets maximal absolute value of row vector coefficients */
extern
SCIP_Real SCIProwGetMaxval(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** gets minimal absolute value of row vector's non-zero coefficients */
extern
SCIP_Real SCIProwGetMinval(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** gets maximal column index of row entries */
int SCIProwGetMaxidx(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** gets minimal column index of row entries */
int SCIProwGetMinidx(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** returns row's efficacy with respect to the current LP solution: e = -feasibility/norm */
extern
SCIP_Real SCIProwGetLPEfficacy(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics data */
   SCIP_LP*              lp                  /**< current LP data */
   );

/** returns whether the row's efficacy with respect to the current LP solution is greater than the minimal cut efficacy */
extern
SCIP_Bool SCIProwIsLPEfficacious(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics data */
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_Bool             root                /**< should the root's minimal cut efficacy be used? */
   );

/** returns row's efficacy with respect to the given primal solution: e = -feasibility/norm */
extern
SCIP_Real SCIProwGetSolEfficacy(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics data */
   SCIP_SOL*             sol                 /**< primal CIP solution */
   );

/** returns whether the row's efficacy with respect to the given primal solution is greater than the minimal cut
 *  efficacy
 */
extern
SCIP_Bool SCIProwIsSolEfficacious(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics data */
   SCIP_SOL*             sol,                /**< primal CIP solution */
   SCIP_Bool             root                /**< should the root's minimal cut efficacy be used? */
   );

/** gets parallelism of row with objective function: if the returned value is 1, the row is parallel to the objective
 *  function, if the value is 0, it is orthogonal to the objective function
 */
extern
SCIP_Real SCIProwGetObjParallelism(
   SCIP_ROW*             row,                /**< LP row */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_LP*              lp                  /**< current LP data */
   );

/** includes event handler with given data in row's event filter */
extern
SCIP_RETCODE SCIProwCatchEvent(
   SCIP_ROW*             row,                /**< row */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTTYPE        eventtype,          /**< event type to catch */
   SCIP_EVENTHDLR*       eventhdlr,          /**< event handler to call for the event processing */
   SCIP_EVENTDATA*       eventdata,          /**< event data to pass to the event handler for the event processing */
   int*                  filterpos           /**< pointer to store position of event filter entry, or NULL */
   );

/** deletes event handler with given data from row's event filter */
extern
SCIP_RETCODE SCIProwDropEvent(
   SCIP_ROW*             row,                /**< row */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTTYPE        eventtype,          /**< event type mask of dropped event */
   SCIP_EVENTHDLR*       eventhdlr,          /**< event handler to call for the event processing */
   SCIP_EVENTDATA*       eventdata,          /**< event data to pass to the event handler for the event processing */
   int                   filterpos           /**< position of event filter entry returned by SCIPvarCatchEvent(), or -1 */
   );



/*
 * LP methods
 */

/** creates empty LP data object */
extern
SCIP_RETCODE SCIPlpCreate(
   SCIP_LP**             lp,                 /**< pointer to LP data object */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   const char*           name                /**< problem name */
   );

/** frees LP data object */
extern
SCIP_RETCODE SCIPlpFree(
   SCIP_LP**             lp,                 /**< pointer to LP data object */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter         /**< global event filter */
   );

/** resets the LP to the empty LP by removing all columns and rows from LP, releasing all rows, and flushing the
 *  changes to the LP solver
 */
extern
SCIP_RETCODE SCIPlpReset(
   SCIP_LP*              lp,                 /**< LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter         /**< global event filter */
   );

/** adds a column to the LP and captures the variable */
extern
SCIP_RETCODE SCIPlpAddCol(
   SCIP_LP*              lp,                 /**< LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_COL*             col,                /**< LP column */
   int                   depth               /**< depth in the tree where the column addition is performed */
   );

/** adds a row to the LP and captures it */
extern
SCIP_RETCODE SCIPlpAddRow(
   SCIP_LP*              lp,                 /**< LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory buffers */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter,        /**< global event filter */
   SCIP_ROW*             row,                /**< LP row */
   int                   depth               /**< depth in the tree where the row addition is performed */
   );

/** removes all columns after the given number of columns from the LP */
extern
SCIP_RETCODE SCIPlpShrinkCols(
   SCIP_LP*              lp,                 /**< LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   int                   newncols            /**< new number of columns in the LP */
   );

/** removes and releases all rows after the given number of rows from the LP */
extern
SCIP_RETCODE SCIPlpShrinkRows(
   SCIP_LP*              lp,                 /**< LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter,        /**< global event filter */
   int                   newnrows            /**< new number of rows in the LP */
   );

/** removes all columns and rows from LP, releases all rows */
extern
SCIP_RETCODE SCIPlpClear(
   SCIP_LP*              lp,                 /**< LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter         /**< global event filter */
   );

/** remembers number of columns and rows to track the newly added ones */
extern
void SCIPlpMarkSize(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** sets the remembered number of columns and rows to the given values */
extern
void SCIPlpSetSizeMark(
   SCIP_LP*              lp,                 /**< current LP data */
   int                   nrows,              /**< number of rows to set the size marker to */
   int                   ncols               /**< number of columns to set the size marker to */
   );

/** gets all indices of basic columns and rows: index i >= 0 corresponds to column i, index i < 0 to row -i-1 */
extern
SCIP_RETCODE SCIPlpGetBasisInd(
   SCIP_LP*              lp,                 /**< LP data */
   int*                  basisind            /**< pointer to store the basis indices */
   );

/** gets current basis status for columns and rows; arrays must be large enough to store the basis status */
extern
SCIP_RETCODE SCIPlpGetBase(
   SCIP_LP*              lp,                 /**< LP data */
   int*                  cstat,              /**< array to store column basis status, or NULL */
   int*                  rstat               /**< array to store row basis status, or NULL */
   );

/** gets a row from the inverse basis matrix B^-1 */
extern
SCIP_RETCODE SCIPlpGetBInvRow(
   SCIP_LP*              lp,                 /**< LP data */
   int                   r,                  /**< row number */
   SCIP_Real*            coef                /**< pointer to store the coefficients of the row */
   );

/** gets a column from the inverse basis matrix B^-1 */
extern
SCIP_RETCODE SCIPlpGetBInvCol(
   SCIP_LP*              lp,                 /**< LP data */
   int                   c,                  /**< column number of B^-1; this is NOT the number of the column in the LP
                                              *   returned by SCIPcolGetLPPos(); you have to call SCIPgetBasisInd()
                                              *   to get the array which links the B^-1 column numbers to the row and
                                              *   column numbers of the LP! c must be between 0 and nrows-1, since the
                                              *   basis has the size nrows * nrows */
   SCIP_Real*            coef                /**< pointer to store the coefficients of the column */
   );

/** gets a row from the product of inverse basis matrix B^-1 and coefficient matrix A (i.e. from B^-1 * A) */
extern
SCIP_RETCODE SCIPlpGetBInvARow(
   SCIP_LP*              lp,                 /**< LP data */
   int                   r,                  /**< row number */
   SCIP_Real*            binvrow,            /**< row in B^-1 from prior call to SCIPlpGetBInvRow(), or NULL */
   SCIP_Real*            coef                /**< pointer to store the coefficients of the row */
   );

/** gets a column from the product of inverse basis matrix B^-1 and coefficient matrix A (i.e. from B^-1 * A),
 *  i.e., it computes B^-1 * A_c with A_c being the c'th column of A
 */
extern
SCIP_RETCODE SCIPlpGetBInvACol(
   SCIP_LP*              lp,                 /**< LP data */
   int                   c,                  /**< column number which can be accessed by SCIPcolGetLPPos() */
   SCIP_Real*            coef                /**< pointer to store the coefficients of the column */
   );

/** calculates a weighted sum of all LP rows; for negative weights, the left and right hand side of the corresponding
 *  LP row are swapped in the summation
 */
extern
SCIP_RETCODE SCIPlpSumRows(
   SCIP_LP*              lp,                 /**< LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_PROB*            prob,               /**< problem data */
   SCIP_Real*            weights,            /**< row weights in row summation */
   SCIP_REALARRAY*       sumcoef,            /**< array to store sum coefficients indexed by variables' probindex */
   SCIP_Real*            sumlhs,             /**< pointer to store the left hand side of the row summation */
   SCIP_Real*            sumrhs              /**< pointer to store the right hand side of the row summation */
   );

/* calculates a MIR cut out of the weighted sum of LP rows; The weights of modifiable rows are set to 0.0, because these
 * rows cannot participate in a MIR cut.
 */
extern
SCIP_RETCODE SCIPlpCalcMIR(
   SCIP_LP*              lp,                 /**< LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_PROB*            prob,               /**< problem data */
   SCIP_SOL*             sol,                /**< the solution that should be separated, or NULL for LP solution */
   SCIP_Real             boundswitch,        /**< fraction of domain up to which lower bound is used in transformation */
   SCIP_Bool             usevbds,            /**< should variable bounds be used in bound transformation? */
   SCIP_Bool             allowlocal,         /**< should local information allowed to be used, resulting in a local cut? */
   SCIP_Bool             fixintegralrhs,     /**< should complementation tried to be adjusted such that rhs gets fractional? */
   int*                  boundsfortrans,     /**< bounds that should be used for transformed variables: vlb_idx/vub_idx,  
                                              *   -1 for global lb/ub, -2 for local lb/ub, or -3 for using closest bound;
                                              *   NULL for using closest bound for all variables */
   SCIP_BOUNDTYPE*       boundtypesfortrans, /**< type of bounds that should be used for transformed variables; 
                                              *   NULL for using closest bound for all variables */
   int                   maxmksetcoefs,      /**< maximal number of nonzeros allowed in aggregated base inequality */
   SCIP_Real             maxweightrange,     /**< maximal valid range max(|weights|)/min(|weights|) of row weights */
   SCIP_Real             minfrac,            /**< minimal fractionality of rhs to produce MIR cut for */
   SCIP_Real             maxfrac,            /**< maximal fractionality of rhs to produce MIR cut for */
   SCIP_Real*            weights,            /**< row weights in row summation */
   SCIP_Real             scale,              /**< additional scaling factor multiplied to all rows */
   SCIP_Real*            mksetcoefs,         /**< array to store mixed knapsack set coefficients: size nvars; or NULL */
   SCIP_Bool*            mksetcoefsvalid,    /**< pointer to store whether mixed knapsack set coefficients are valid; or NULL */
   SCIP_Real*            mircoef,            /**< array to store MIR coefficients: must be of size nvars */
   SCIP_Real*            mirrhs,             /**< pointer to store the right hand side of the MIR row */
   SCIP_Real*            cutactivity,        /**< pointer to store the activity of the resulting cut */
   SCIP_Bool*            success,            /**< pointer to store whether the returned coefficients are a valid MIR cut */
   SCIP_Bool*            cutislocal          /**< pointer to store whether the returned cut is only valid locally */
   );

/* calculates a strong CG cut out of the weighted sum of LP rows; The weights of modifiable rows are set to 0.0, because these
 * rows cannot participate in a strong CG cut.
 */
extern
SCIP_RETCODE SCIPlpCalcStrongCG(
   SCIP_LP*              lp,                 /**< LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_PROB*            prob,               /**< problem data */
   SCIP_Real             boundswitch,        /**< fraction of domain up to which lower bound is used in transformation */
   SCIP_Bool             usevbds,            /**< should variable bounds be used in bound transformation? */
   SCIP_Bool             allowlocal,         /**< should local information allowed to be used, resulting in a local cut? */
   int                   maxmksetcoefs,      /**< maximal number of nonzeros allowed in aggregated base inequality */
   SCIP_Real             maxweightrange,     /**< maximal valid range max(|weights|)/min(|weights|) of row weights */
   SCIP_Real             minfrac,            /**< minimal fractionality of rhs to produce strong CG cut for */
   SCIP_Real             maxfrac,            /**< maximal fractionality of rhs to produce strong CG cut for */
   SCIP_Real*            weights,            /**< row weights in row summation */
   SCIP_Real             scale,              /**< additional scaling factor multiplied to all rows */
   SCIP_Real*            strongcgcoef,       /**< array to store strong CG coefficients: must be of size nvars */
   SCIP_Real*            strongcgrhs,        /**< pointer to store the right hand side of the strong CG row */
   SCIP_Real*            cutactivity,        /**< pointer to store the activity of the resulting cut */
   SCIP_Bool*            success,            /**< pointer to store whether the returned coefficients are a valid strong CG cut */
   SCIP_Bool*            cutislocal          /**< pointer to store whether the returned cut is only valid locally */
   );

/** stores LP state (like basis information) into LP state object */
extern
SCIP_RETCODE SCIPlpGetState(
   SCIP_LP*              lp,                 /**< LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_LPISTATE**       lpistate            /**< pointer to LP state information (like basis information) */
   );

/** loads LP state (like basis information) into solver */
extern
SCIP_RETCODE SCIPlpSetState(
   SCIP_LP*              lp,                 /**< LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_LPISTATE*        lpistate            /**< LP state information (like basis information) */
   );

/** frees LP state information */
extern
SCIP_RETCODE SCIPlpFreeState(
   SCIP_LP*              lp,                 /**< LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_LPISTATE**       lpistate            /**< pointer to LP state information (like basis information) */
   );

/** sets the upper objective limit of the LP solver */
extern
SCIP_RETCODE SCIPlpSetCutoffbound(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_Real             cutoffbound         /**< new upper objective limit */
   );

/** applies all cached changes to the LP solver */
extern
SCIP_RETCODE SCIPlpFlush(
   SCIP_LP*              lp,                 /**< current LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_EVENTQUEUE*      eventqueue         /**< event queue */
   );

/** marks the LP to be flushed, even if the LP thinks it is not flushed */
extern
SCIP_RETCODE SCIPlpMarkFlushed(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** solves the LP with simplex algorithm, and copy the solution into the column's data */
extern
SCIP_RETCODE SCIPlpSolveAndEval(
   SCIP_LP*              lp,                 /**< LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory buffers */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter,        /**< global event filter */
   SCIP_PROB*            prob,               /**< problem data */
   int                   itlim,              /**< maximal number of LP iterations to perform, or -1 for no limit */
   SCIP_Bool             aging,              /**< should aging and removal of obsolete cols/rows be applied? */
   SCIP_Bool             keepsol,            /**< should the old LP solution be kept if no iterations were performed? */
   SCIP_Bool*            lperror             /**< pointer to store whether an unresolved LP error occurred */
   );

/** gets solution status of current LP */
extern
SCIP_LPSOLSTAT SCIPlpGetSolstat(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** sets whether the current lp is a relaxation of the current problem and its optimal objective value is a local lower bound */
extern
void SCIPlpSetIsRelax(
   SCIP_LP*              lp,                 /**< LP data */
   SCIP_Bool             isrelax             /**< is the current lp a relaxation? */
   );

/** returns whether the current lp is a relaxation of the current problem and its optimal objective value is a local lower bound */
extern
SCIP_Bool SCIPlpIsRelax(
   SCIP_LP*              lp                  /**< LP data */
   );

/** sets whether the root lp is a relaxation of the problem and its optimal objective value is a global lower bound */
extern
void SCIPlpSetRootLPIsRelax(
   SCIP_LP*              lp,                 /**< LP data */
   SCIP_Bool             isrelax             /**< is the root lp a relaxation of the problem? */
   );

/** returns whether the root lp is a relaxation of the problem and its optimal objective value is a global lower bound */
extern 
SCIP_Bool SCIPlpIsRootLPRelax(
   SCIP_LP*              lp                  /**< LP data */
   );

/** gets objective value of current LP */
extern
SCIP_Real SCIPlpGetObjval(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** gets part of objective value of current LP that results from COLUMN variables only */
extern
SCIP_Real SCIPlpGetColumnObjval(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** gets part of objective value of current LP that results from LOOSE variables only */
extern
SCIP_Real SCIPlpGetLooseObjval(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** remembers the current LP objective value as root solution value */
extern
void SCIPlpStoreRootObjval(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** invalidates the root LP solution value */
extern
void SCIPlpInvalidateRootObjval(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** gets current pseudo objective value */
extern
SCIP_Real SCIPlpGetPseudoObjval(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** gets pseudo objective value, if a bound of the given variable would be modified in the given way */
extern
SCIP_Real SCIPlpGetModifiedPseudoObjval(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_Real             oldbound,           /**< old value for bound */
   SCIP_Real             newbound,           /**< new value for bound */
   SCIP_BOUNDTYPE        boundtype           /**< type of bound: lower or upper bound */
   );

/** gets pseudo objective value, if a bound of the given variable would be modified in the given way;
 *  perform calculations with interval arithmetic to get an exact lower bound
 */
extern
SCIP_Real SCIPlpGetModifiedProvedPseudoObjval(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_VAR*             var,                /**< problem variable */
   SCIP_Real             oldbound,           /**< old value for bound */
   SCIP_Real             newbound,           /**< new value for bound */
   SCIP_BOUNDTYPE        boundtype           /**< type of bound: lower or upper bound */
   );

/** updates current pseudo and loose objective value for a change in a variable's objective value */
extern
SCIP_RETCODE SCIPlpUpdateVarObj(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_VAR*             var,                /**< problem variable that changed */
   SCIP_Real             oldobj,             /**< old objective value of variable */
   SCIP_Real             newobj              /**< new objective value of variable */
   );

/** updates current pseudo and loose objective value for a change in a variable's lower bound */
extern
SCIP_RETCODE SCIPlpUpdateVarLb(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_VAR*             var,                /**< problem variable that changed */
   SCIP_Real             oldlb,              /**< old lower bound of variable */
   SCIP_Real             newlb               /**< new lower bound of variable */
   );

/** updates current pseudo objective value for a change in a variable's upper bound */
extern
SCIP_RETCODE SCIPlpUpdateVarUb(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_VAR*             var,                /**< problem variable that changed */
   SCIP_Real             oldub,              /**< old upper bound of variable */
   SCIP_Real             newub               /**< new upper bound of variable */
   );

/** informs LP, that given variable was added to the problem */
extern
SCIP_RETCODE SCIPlpUpdateAddVar(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_VAR*             var                 /**< variable that is now a LOOSE problem variable */
   );

/** informs LP, that given variable is to be deleted from the problem */
extern
SCIP_RETCODE SCIPlpUpdateDelVar(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_VAR*             var                 /**< variable that will be deleted from the problem */
   );

/** informs LP, that given formerly loose problem variable is now a column variable */
extern
SCIP_RETCODE SCIPlpUpdateVarColumn(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_VAR*             var                 /**< problem variable that changed from LOOSE to COLUMN */
   );

/** informs LP, that given formerly column problem variable is now again a loose variable */
extern
SCIP_RETCODE SCIPlpUpdateVarLoose(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_VAR*             var                 /**< problem variable that changed from COLUMN to LOOSE */
   );

/** stores the LP solution in the columns and rows */
extern
SCIP_RETCODE SCIPlpGetSol(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_Bool*            primalfeasible,     /**< pointer to store whether the solution is primal feasible, or NULL */
   SCIP_Bool*            dualfeasible        /**< pointer to store whether the solution is dual feasible, or NULL */
   );

/** stores LP solution with infinite objective value in the columns and rows */
extern
SCIP_RETCODE SCIPlpGetUnboundedSol(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat                /**< problem statistics */
   );

/** stores the dual Farkas multipliers for infeasibility proof in rows */
extern
SCIP_RETCODE SCIPlpGetDualfarkas(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat                /**< problem statistics */
   );

/** get number of iterations used in last LP solve */
extern
SCIP_RETCODE SCIPlpGetIterations(
   SCIP_LP*              lp,                 /**< current LP data */
   int*                  iterations          /**< pointer to store the iteration count */
   );

/** increases age of columns with solution value 0.0 and rows with activity not at its bounds,
 *  resets age of non-zero columns and sharp rows
 */
extern
SCIP_RETCODE SCIPlpUpdateAges(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_STAT*            stat                /**< problem statistics */
   );

/** removes all non-basic columns and basic rows in the part of the LP created at the current node, that are too old */
extern
SCIP_RETCODE SCIPlpRemoveNewObsoletes(
   SCIP_LP*              lp,                 /**< current LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory buffers */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter         /**< global event filter */
   );

/** removes all non-basic columns and basic rows in whole LP, that are too old */
extern
SCIP_RETCODE SCIPlpRemoveAllObsoletes(
   SCIP_LP*              lp,                 /**< current LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory buffers */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter         /**< global event filter */
   );

/** removes all non-basic columns at 0.0 and basic rows in the part of the LP created at the current node */
extern
SCIP_RETCODE SCIPlpCleanupNew(
   SCIP_LP*              lp,                 /**< current LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory buffers */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter,        /**< global event filter */
   SCIP_Bool             root                /**< are we at the root node? */
   );

/** removes all non-basic columns at 0.0 and basic rows in the whole LP */
extern
SCIP_RETCODE SCIPlpCleanupAll(
   SCIP_LP*              lp,                 /**< current LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory buffers */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter,        /**< global event filter */
   SCIP_Bool             root                /**< are we at the root node? */
   );

/** removes all redundant rows that were added at the current node */
extern
SCIP_RETCODE SCIPlpRemoveRedundantRows(
   SCIP_LP*              lp,                 /**< current LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory buffers */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter         /**< global event filter */
   );

/** initiates LP diving */
extern
SCIP_RETCODE SCIPlpStartDive(
   SCIP_LP*              lp,                 /**< current LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** quits LP diving and resets bounds and objective values of columns to the current node's values */
extern
SCIP_RETCODE SCIPlpEndDive(
   SCIP_LP*              lp,                 /**< current LP data */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_STAT*            stat,               /**< problem statistics */
   SCIP_EVENTQUEUE*      eventqueue,         /**< event queue */
   SCIP_EVENTFILTER*     eventfilter,        /**< global event filter */
   SCIP_PROB*            prob,               /**< problem data */
   SCIP_VAR**            vars,               /**< array with all active variables */
   int                   nvars               /**< number of active variables */
   );

/** informs the LP that probing mode was initiated */
extern
SCIP_RETCODE SCIPlpStartProbing(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** informs the LP that probing mode was finished */
extern
SCIP_RETCODE SCIPlpEndProbing(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** gets proven lower (dual) bound of last LP solution */
extern
SCIP_RETCODE SCIPlpGetProvedLowerbound(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_Real*            bound               /**< pointer to store proven dual bound */
   );

/** gets proven dual bound of last LP solution */
extern
SCIP_RETCODE SCIPlpIsInfeasibilityProved(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_Bool*            proved              /**< pointer to store whether infeasibility is proven */
   );

/** writes LP to a file */
extern 
SCIP_RETCODE SCIPlpWrite(
   SCIP_LP*              lp,                 /**< current LP data */
   const char*           fname               /**< file name */
   );

/** writes MIP to a file */
extern 
SCIP_RETCODE SCIPlpWriteMip(
   SCIP_LP*              lp,                 /**< current LP data */
   SCIP_SET*             set,                /**< global SCIP settings */
   const char*           fname,              /**< file name */
   SCIP_Bool             genericnames,       /**< should generic names like x_i and row_j be used in order to avoid
                                              *   troubles with reserved symbols? */
   SCIP_Bool             origobj,            /**< should the original objective function be used? */
   SCIP_OBJSENSE         objsense,           /**< objective sense */
   SCIP_Real             objscale,           /**< objective scaling factor */
   SCIP_Real             objoffset           /**< objective offset, eg. caused by variable fixings in presolving */
);

/** recalculates Euclidean norm of objective function vector of column variables if it have gotten unreliable during calculation */
extern
void SCIPlpRecalculateObjSqrNorm(
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_LP*              lp                  /**< LP data */
   );

#ifndef NDEBUG

/* In debug mode, the following methods are implemented as function calls to ensure
 * type validity.
 */

/** gets array with columns of the LP */
extern
SCIP_COL** SCIPlpGetCols(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** gets current number of columns in LP */
extern
int SCIPlpGetNCols(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** gets array with rows of the LP */
extern
SCIP_ROW** SCIPlpGetRows(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** gets current number of rows in LP */
extern
int SCIPlpGetNRows(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** gets array with newly added columns after the last mark */
extern
SCIP_COL** SCIPlpGetNewcols(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** gets number of newly added columns after the last mark */
extern
int SCIPlpGetNNewcols(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** gets array with newly added rows after the last mark */
extern
SCIP_ROW** SCIPlpGetNewrows(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** gets number of newly added rows after the last mark */
extern
int SCIPlpGetNNewrows(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** gets Euclidean norm of objective function vector of column variables, only use this method if
 *  lp->objsqrnormunreliable == FALSE, so probably you have to call SCIPlpRecalculateObjSqrNorm before */
extern
SCIP_Real SCIPlpGetObjNorm(
   SCIP_LP*              lp                  /**< LP data */
   );

/** gets the objective value of the root node LP; returns SCIP_INVALID if the root node LP was not (yet) solved */
extern
SCIP_Real SCIPlpGetRootObjval(
   SCIP_LP*              lp                  /**< LP data */
   );

/** gets part of the objective value of the root node LP that results from COLUMN variables only;
 *  returns SCIP_INVALID if the root node LP was not (yet) solved
 */
extern
SCIP_Real SCIPlpGetRootColumnObjval(
   SCIP_LP*              lp                  /**< LP data */
   );

/** gets part of the objective value of the root node LP that results from LOOSE variables only;
 *  returns SCIP_INVALID if the root node LP was not (yet) solved
 */
extern
SCIP_Real SCIPlpGetRootLooseObjval(
   SCIP_LP*              lp                  /**< LP data */
   );

/** gets the LP solver interface */
extern
SCIP_LPI* SCIPlpGetLPI(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** returns whether the current LP is flushed and solved */
extern
SCIP_Bool SCIPlpIsSolved(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** returns whether the current LP solution is a basic solution */
extern
SCIP_Bool SCIPlpIsSolBasic(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** returns whether the LP is in diving mode */
extern
SCIP_Bool SCIPlpDiving(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** returns whether the LP is in diving mode and the objective value of at least one column was changed */
extern
SCIP_Bool SCIPlpDivingObjChanged(
   SCIP_LP*              lp                  /**< current LP data */
   );

/** marks the diving LP to have a changed objective function */
extern
void SCIPlpMarkDivingObjChanged(
   SCIP_LP*              lp                  /**< current LP data */
   );

#else

/* In optimized mode, the methods are implemented as defines to reduce the number of function calls and
 * speed up the algorithms.
 */

#define SCIPlpGetCols(lp)               ((lp)->cols)
#define SCIPlpGetNCols(lp)              ((lp)->ncols)
#define SCIPlpGetRows(lp)               ((lp)->rows)
#define SCIPlpGetNRows(lp)              ((lp)->nrows)
#define SCIPlpGetNewcols(lp)            (&((lp)->cols[(lp)->firstnewcol]))
#define SCIPlpGetNNewcols(lp)           ((lp)->ncols - (lp)->firstnewcol)
#define SCIPlpGetNewrows(lp)            (&((lp)->rows[(lp)->firstnewrow]))
#define SCIPlpGetNNewrows(lp)           ((lp)->nrows - (lp)->firstnewrow)
#define SCIPlpGetObjNorm(lp)            (SQRT((lp)->objsqrnorm))
#define SCIPlpGetRootObjval(lp)         (MIN((lp)->rootlpobjval + (lp)->rootlooseobjval, SCIP_INVALID))
#define SCIPlpGetRootColumnObjval(lp)   ((lp)->rootlpobjval)
#define SCIPlpGetRootLooseObjval(lp)    ((lp)->rootlooseobjval)
#define SCIPlpGetLPI(lp)                (lp)->lpi
#define SCIPlpIsSolved(lp)              ((lp)->flushed && (lp)->solved)
#define SCIPlpIsSolBasic(lp)            ((lp)->solisbasic)
#define SCIPlpDiving(lp)                (lp)->diving
#define SCIPlpDivingObjChanged(lp)      (lp)->divingobjchg
#define SCIPlpMarkDivingObjChanged(lp)  ((lp)->divingobjchg = TRUE)

#endif

#ifdef __cplusplus
}
#endif

#endif
