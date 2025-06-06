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
#pragma ident "@(#) $Id: sepa_oddcycle.h,v 1.1 2010/08/24 09:10:25 bzfwanie Exp $"

/**@file   sepa_oddcycle.h
 * @brief  oddcycle separator
 * @author Robert Waniek
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_SEPA_ODDCYCLE_H__
#define __SCIP_SEPA_ODDCYCLE_H__


#include "scip/scip.h"

#ifdef __cplusplus
extern "C" {
#endif

/** creates the oddcycle separator and includes it in SCIP */
extern
SCIP_RETCODE SCIPincludeSepaOddcycle(
   SCIP*                 scip                /**< SCIP data structure */
   );

#ifdef __cplusplus
}
#endif

#endif
