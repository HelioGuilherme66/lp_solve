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
#pragma ident "@(#) $Id: reader_bpa.h,v 1.1 2010/09/09 12:21:14 bzfheinz Exp $"

/**@file   reader_bpa.h
 * @brief  binpacking problem reader file reader
 * @author Timo Berthold
 * @author Stefan Heinz
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_READER_BPA_H__
#define __SCIP_READER_BPA_H__


#include "scip/scip.h"


/** includes the binpacking file reader into SCIP */
extern
SCIP_RETCODE SCIPincludeReaderBpa(
   SCIP*                 scip                /**< SCIP data structure */
   );

#endif
