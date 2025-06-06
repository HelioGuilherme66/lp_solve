#pragma ident "@(#) $Id: ratsoswrite.c,v 1.1 2005/07/09 18:51:21 bzfkocht Exp $"
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*   File....: ratsoswrite.c                                                 */
/*   Name....: SOS File Write                                                */
/*   Author..: Thorsten Koch, Daniel Junglas                                 */
/*   Copyright by Author, All rights reserved                                */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * Copyright (C) 2005 by Thorsten Koch <koch@zib.de>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <gmp.h>

#include "mshell.h"
#include "bool.h"
#include "ratlp.h"
#include "ratlpstore.h"
#include "mme.h"

/* Write SOS definitions to a file.
 * A specification for the SOS file format can be found in the
 * ILOG CPLEX Reference Manual.
 */
void lps_sosfile(
   const Lps*  lp,
   FILE*       fp,
   LpFormat    format,
   const char* text)
{
   const Sos*  sos;
   const Sse*  sse;
   char*       vtmp;
   int         namelen;

   assert(lp     != NULL);
   assert(fp     != NULL);
   assert(format == LP_FORM_LPF || format == LP_FORM_MPS);

   namelen = (format == LP_FORM_MPS) ? MPS_NAME_LEN : LPF_NAME_LEN;
   vtmp    = malloc((size_t)namelen + 1);

   assert(vtmp != NULL);

   if (text != NULL)
      fprintf(fp, "* %s\n", text);
   
   fprintf(fp, "NAME        %8.8s\n", lp->name);
   
   for(sos = lp->sos_root; sos != NULL; sos = sos->next)
   {
      fprintf(fp, "* %s\n", sos->name);

      fprintf(fp, " S%d   %d\n",
         sos->type == SOS_TYPE1 ? 1 : 2,
         sos->priority);
      
      for (sse = sos->first; sse != NULL; sse = sse->next)
      {
         lps_makename(vtmp, namelen + 1, sse->var->name, sse->var->number);
         fprintf(fp, "    %-*s  %.10g\n",
            namelen + 1, vtmp, mpq_get_d(sse->weight));
      }
   }
   fprintf(fp, "ENDATA\n");

   free(vtmp);
}   


