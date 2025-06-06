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
#pragma ident "@(#) $Id: struct_disp.h,v 1.17 2010/03/12 14:54:30 bzfwinkm Exp $"

/**@file   struct_disp.h
 * @brief  datastructures for displaying runtime statistics
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_STRUCT_DISP_H__
#define __SCIP_STRUCT_DISP_H__


#include "scip/def.h"
#include "scip/type_disp.h"

#ifdef __cplusplus
extern "C" {
#endif

/** display column */
struct SCIP_Disp
{
   char*                 name;               /**< name of display column */
   char*                 desc;               /**< description of display column */
   char*                 header;             /**< head line of display column */
   SCIP_DECL_DISPCOPY    ((*dispcopy));      /**< copy method of display column or NULL if you don't want to copy your plugin into subscips */
   SCIP_DECL_DISPFREE    ((*dispfree));      /**< destructor of display column */
   SCIP_DECL_DISPINIT    ((*dispinit));      /**< initialize display column */
   SCIP_DECL_DISPEXIT    ((*dispexit));      /**< deinitialize display column */
   SCIP_DECL_DISPINITSOL ((*dispinitsol));   /**< solving process initialization method of display column */
   SCIP_DECL_DISPEXITSOL ((*dispexitsol));   /**< solving process deinitialization method of display column */
   SCIP_DECL_DISPOUTPUT  ((*dispoutput));    /**< output method */
   SCIP_DISPDATA*        dispdata;           /**< display column data */
   int                   width;              /**< width of display column (no. of chars used) */
   int                   priority;           /**< priority of display column */
   int                   position;           /**< relative position of display column */
   SCIP_DISPSTATUS       dispstatus;         /**< display activation status of display column */
   SCIP_Bool             stripline;          /**< should the column be separated with a line from its right neighbour? */
   SCIP_Bool             initialized;        /**< is display column initialized? */
   SCIP_Bool             active;             /**< should column be displayed to the screen? */
};

#ifdef __cplusplus
}
#endif

#endif
