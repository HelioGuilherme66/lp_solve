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
#pragma ident "@(#) $Id: reader_ccg.h,v 1.4 2010/01/04 20:35:46 bzfheinz Exp $"

/**@file   reader_ccg.h
 * @brief  Column connectivity graph file reader (actually, only a writer)
 * @author Marc Pfetsch
 *
 * Write weighted column connectivity graph, see reader_ccg.c.
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_READER_CCG_H__
#define __SCIP_READER_CCG_H__


#include "scip/scip.h"

#ifdef __cplusplus
extern "C" {
#endif

/** includes the ccg file reader into SCIP */
extern
SCIP_RETCODE SCIPincludeReaderCcg(
   SCIP*                 scip                /**< SCIP data structure */
   );


/* writes problem to file */
extern
SCIP_RETCODE SCIPwriteCcg(
   SCIP*              scip,               /**< SCIP data structure */
   FILE*              file,               /**< output file, or NULL if standard output should be used */
   const char*        name,               /**< problem name */
   SCIP_Bool          transformed,        /**< TRUE iff problem is the transformed problem */
   SCIP_VAR**         vars,               /**< array with active variables ordered binary, integer, implicit, continuous */
   int                nvars,              /**< number of mutable variables in the problem */
   SCIP_CONS**        conss,              /**< array with constraints of the problem */
   int                nconss,             /**< number of constraints in the problem */
   SCIP_RESULT*       result              /**< pointer to store the result of the file writing call */
   );

#ifdef __cplusplus
}
#endif

#endif
