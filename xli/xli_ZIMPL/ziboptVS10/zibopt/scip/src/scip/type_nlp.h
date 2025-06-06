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
#pragma ident "@(#) $Id: type_nlp.h,v 1.1 2010/05/20 16:05:06 bzfviger Exp $"

/**@file   type_nlp.h
 * @brief  type definitions for NLP management
 * @author Thorsten Gellermann
 * @author Stefan Vigerske
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_TYPE_NLP_H__
#define __SCIP_TYPE_NLP_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SCIP_NlRow SCIP_NLROW;        /**< nonlinear row of an NLP */
typedef struct SCIP_Nlp   SCIP_NLP;          /**< current NLP data */

#ifdef __cplusplus
}
#endif

#endif /* __SCIP_TYPE_NLP_H__ */
