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
#pragma ident "@(#) $Id: cons_soc.h,v 1.10 2010/09/03 19:25:22 bzfviger Exp $"

/**@file   cons_soc.h
 * @brief  constraint handler for second order cone constraints
 * @author Stefan Vigerske
 * 
 * This constraint handler implements the general case of second order cones, which
 * are of the following form:
 * \f[
 *    \left\{ x \;:\; \sqrt{\gamma + \sum_{i=1}^{n} (\alpha_i\, (x_i + \beta_i))^2} \leq \alpha_{n+1}\, (x_{n+1}+\beta_{n+1}) \right\}.
 * \f]
 * Here, \f$\gamma \geq 0\f$ and either \f$x_{n+1} \geq -\beta_{n+1}, \alpha_{n+1} \geq 0\f$ or
 * \f$x_{n+1} \leq -\beta_{n+1}, \alpha_{n+1} \leq 0\f$.
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_CONS_SOC_H__
#define __SCIP_CONS_SOC_H__

#include "scip/scip.h"
#include "nlpi/type_nlpi.h"

#ifdef __cplusplus
extern "C" {
#endif

/** creates the handler for second order cone constraints and includes it in SCIP */
extern
SCIP_RETCODE SCIPincludeConshdlrSOC(
   SCIP*                 scip                /**< SCIP data structure */
   );

/** creates and captures a second order cone constraint */
extern
SCIP_RETCODE SCIPcreateConsSOC(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS**           cons,               /**< pointer to hold the created constraint */
   const char*           name,               /**< name of constraint */
   int                   nvars,              /**< number of variables on left hand side of constraint (n) */
   SCIP_VAR**            vars,               /**< array with variables on left hand side (x_i) */
   SCIP_Real*            coefs,              /**< array with coefficients of left hand side variables (alpha_i), or NULL if all 1.0 */
   SCIP_Real*            offsets,            /**< array with offsets of variables (beta_i), or NULL if all 0.0 */
   SCIP_Real             constant,           /**< constant on left hand side (gamma) */
   SCIP_VAR*             rhsvar,             /**< variable on right hand side of constraint (x_{n+1}) */
   SCIP_Real             rhscoeff,           /**< coefficient of variable on right hand side (alpha_{n+1}) */
   SCIP_Real             rhsoffset,          /**< offset of variable on right hand side (beta_{n+1}) */
   SCIP_Bool             initial,            /**< should the LP relaxation of constraint be in the initial LP?
                                              *   Usually set to TRUE. Set to FALSE for 'lazy constraints'. */
   SCIP_Bool             separate,           /**< should the constraint be separated during LP processing?
                                              *   Usually set to TRUE. */
   SCIP_Bool             enforce,            /**< should the constraint be enforced during node processing?
                                              *   TRUE for model constraints, FALSE for additional, redundant constraints. */
   SCIP_Bool             check,              /**< should the constraint be checked for feasibility?
                                              *   TRUE for model constraints, FALSE for additional, redundant constraints. */
   SCIP_Bool             propagate,          /**< should the constraint be propagated during node processing?
                                              *   Usually set to TRUE. */
   SCIP_Bool             local,              /**< is constraint only valid locally?
                                              *   Usually set to FALSE. Has to be set to TRUE, e.g., for branching constraints. */
   SCIP_Bool             modifiable,         /**< is constraint modifiable (subject to column generation)?
                                              *   Usually set to FALSE. In column generation applications, set to TRUE if pricing
                                              *   adds coefficients to this constraint. */
   SCIP_Bool             dynamic,            /**< is constraint subject to aging?
                                              *   Usually set to FALSE. Set to TRUE for own cuts which 
                                              *   are seperated as constraints. */
   SCIP_Bool             removable           /**< should the relaxation be removed from the LP due to aging or cleanup?
                                              *   Usually set to FALSE. Set to TRUE for 'lazy constraints' and 'user cuts'. */
   );

/** Gets the SOC constraint as a nonlinear row representation.
 */
extern
SCIP_RETCODE SCIPgetNlRowSOC(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons,               /**< constraint */
   SCIP_NLROW**          nlrow               /**< a buffer where to store pointer to nonlinear row */
   );

/** Gets the number of variables on the left hand side of a SOC constraint.
 */
extern
int SCIPgetNLhsVarsSOC(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons                /**< constraint data */
   );

/** Gets the variables on the left hand side of a SOC constraint.
 */
extern
SCIP_VAR** SCIPgetLhsVarsSOC(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons                /**< constraint data */
   );

/** Gets the coefficients of the variables on the left hand side of a SOC constraint, or NULL if all are equal to 1.0.
 */
extern
SCIP_Real* SCIPgetLhsCoefsSOC(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons                /**< constraint data */
   );

/** Gets the offsets of the variables on the left hand side of a SOC constraint, or NULL if all are equal to 0.0.
 */
extern
SCIP_Real* SCIPgetLhsOffsetsSOC(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons                /**< constraint data */
   );

/** Gets the constant on the left hand side of a SOC constraint.
 */
extern
SCIP_Real SCIPgetLhsConstantSOC(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons                /**< constraint data */
   );

/** Gets the variable on the right hand side of a SOC constraint.
 */
extern
SCIP_VAR* SCIPgetRhsVarSOC(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons                /**< constraint data */
   );

/** Gets the coefficient of the variable on the right hand side of a SOC constraint.
 */
extern
SCIP_Real SCIPgetRhsCoefSOC(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons                /**< constraint data */
   );

/** Gets the offset of the variables on the right hand side of a SOC constraint.
 */
extern
SCIP_Real SCIPgetRhsOffsetSOC(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons                /**< constraint data */
   );

/** Adds the constraint to an NLPI problem.
 * Uses nonconvex formulation as quadratic function.
 */
extern
SCIP_RETCODE SCIPaddToNlpiProblemSOC(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons,               /**< SOC constraint */
   SCIP_NLPI*            nlpi,               /**< interface to NLP solver */
   SCIP_NLPIPROBLEM*     nlpiprob,           /**< NLPI problem where to add constraint */
   SCIP_HASHMAP*         scipvar2nlpivar,    /**< mapping from SCIP variables to variable indices in NLPI */
   SCIP_Bool             names               /**< whether to pass constraint names to NLPI */
   );

#ifdef __cplusplus
}
#endif

#endif
