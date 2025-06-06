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
#pragma ident "@(#) $Id: sepa_zerohalf.h,v 1.6 2010/01/04 20:35:49 bzfheinz Exp $"

/**@file   sepa_zerohalf.h
 * @brief  {0,1/2}-cuts separator
 * @author Manuel Kutschka
 *         Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_SEPA_ZEROHALF_H__
#define __SCIP_SEPA_ZEROHALF_H__


#include "scip/scip.h"

#ifdef __cplusplus
extern "C" {
#endif

/** creates the {0,1/2}-cut separator and includes it in SCIP */
extern
SCIP_RETCODE SCIPincludeSepaZerohalf(
   SCIP*                 scip                /**< SCIP data structure */
   );

#ifdef __cplusplus
}
#endif

#endif
