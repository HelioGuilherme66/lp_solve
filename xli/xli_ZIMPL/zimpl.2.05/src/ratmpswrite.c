#pragma ident "@(#) $Id: ratmpswrite.c,v 1.10 2006/08/22 20:11:09 bzfkocht Exp $"
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*   File....: mpswrite.c                                                    */
/*   Name....: MPS File Write                                                */
/*   Author..: Thorsten Koch                                                 */
/*   Copyright by Author, All rights reserved                                */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * Copyright (C) 2001-2006 by Thorsten Koch <koch@zib.de>
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

#include "bool.h"
#include "gmpmisc.h"
#include "ratlp.h"
#include "ratlpstore.h"

static void write_data(
   FILE*        fp,
   Bool         use_field5,
   const int    indicator1,
   const int    indicator2,
   const char*  name1,
   const char*  name2,
   const mpq_t  value)
{
   assert(name1 != NULL);
   assert(name2 != NULL);
   
   if (use_field5)
   {
      fprintf(fp, " %c%c %-8.8s  %-8.8s  %12g\n",
         toupper(indicator1), toupper(indicator2), name1, name2, mpq_get_d(value));
   }
   else
   {
      fprintf(fp, " %c%c %-8.8s  %-8.8s\n",
         toupper(indicator1), toupper(indicator2), name1, name2);
   }
}   

static void write_vars(
   const Lps* lp,
   FILE*      fp,
   VarClass   varclass,
   int        name_size)
{
   static Bool first = TRUE;

   const Var*  var;
   const Nzo*  nzo;
   char* vtmp;
   char* ctmp;
   mpq_t temp;
   
   assert(lp        != NULL);
   assert(fp        != NULL);

   vtmp = malloc((size_t)name_size);
   ctmp = malloc((size_t)name_size);
      
   assert(vtmp      != NULL);
   assert(ctmp      != NULL);

   mpq_init(temp);

   for(var = lp->var_root; var != NULL; var = var->next)
   {
      if (var->class != varclass)
         continue;

      /* Only variables with a cost not equal zero, need to be included
       * in the objective function
       */
      if (!mpq_equal(var->cost, const_zero))
      {
         lps_makename(vtmp, name_size, var->name, var->number);

         if (lp->direct == LP_MIN)
            mpq_set(temp, var->cost);
         else
         {
            mpq_neg(temp, var->cost);

            if (first)
            {
               fprintf(stderr, "%s\n%s\n",
                  "*** Warning: Objective function inverted to make",
                  "             minimization problem for MPS output\n");
               first = FALSE;
            }
         }
         write_data(fp, TRUE, ' ', ' ', vtmp, "OBJECTIV", temp);
      }
      for(nzo = var->first; nzo != NULL; nzo = nzo->var_next)
      {
         assert(nzo->var == var);
         assert(!mpq_equal(nzo->value, const_zero));

         lps_makename(vtmp, name_size, var->name, var->number);
         lps_makename(ctmp, name_size, nzo->con->name, nzo->con->number);

         write_data(fp, TRUE, ' ', ' ', vtmp, ctmp, nzo->value);
      }
   }   
   mpq_clear(temp);

   free(vtmp);
   free(ctmp);
}

void mps_write(
   const Lps*  lp,
   FILE*       fp,
   const char* text)
{
   const Var*  var;
   const Con*  con;
   int   indicator;
   Bool  has_ranges = FALSE;
   int   name_size;
   char* vtmp;
   char* ctmp;
   mpq_t temp;
   
   assert(lp != NULL);
   assert(fp != NULL);

   name_size = lps_getnamesize(lp, LP_FORM_MPS);
   vtmp      = malloc((size_t)name_size);
   ctmp      = malloc((size_t)name_size);

   assert(vtmp != NULL);
   assert(ctmp != NULL);
   
   mpq_init(temp);

   if (text != NULL)
      fprintf(fp, "* %s\n", text);
   
   fprintf(fp, "NAME        %8.8s\n", lp->name);
   fprintf(fp, "ROWS\n");
   
   write_data(fp, FALSE, 'N', ' ', "OBJECTIV", "", const_zero);

   for(con = lp->con_root; con != NULL; con = con->next)
   {
      if (con->size > 0)
      {
         lps_makename(ctmp, name_size, con->name, con->number);

         switch(lps_contype(con))
         {
         case CON_EQUAL:
            indicator = 'E';
            break;
         case CON_LHS:
            indicator = 'G';
            break;
         case CON_RHS:
            indicator = 'L';
            break;
         case CON_RANGE:
            indicator = 'E';
            has_ranges = TRUE;
            break;
         default :
            abort();
         }
         write_data(fp, FALSE, indicator, ' ', ctmp, "", const_zero);
      }
   }
   fprintf(fp, "COLUMNS\n");

   /* First integer/binary variables
    */
#if 0 /* ??? Would be nice if known in advance */
   if (lp->type != LP_LP)
#endif 
   {
      fprintf(fp, "    MARK0000  'MARKER'                 'INTORG'\n");

      write_vars(lp, fp, VAR_INT, name_size);
      write_vars(lp, fp, VAR_BIN, name_size);

      fprintf(fp, "    MARK0001  'MARKER'                 'INTEND'\n");
   }
   write_vars(lp, fp, VAR_CON, name_size);

   fprintf(fp, "RHS\n");
   
   for(con = lp->con_root; con != NULL; con = con->next)
   {
      lps_makename(ctmp, name_size, con->name, con->number);

      switch(lps_contype(con))
      {
      case CON_EQUAL:
         if (!mpq_equal(con->rhs, const_zero))
            write_data(fp, TRUE, ' ', ' ', "RHS", ctmp, con->rhs);
         break;
      case CON_LHS:
         if (!mpq_equal(con->lhs, const_zero))
            write_data(fp, TRUE, ' ', ' ', "RHS", ctmp, con->lhs);
         break;
      case CON_RHS:
         if (!mpq_equal(con->rhs, const_zero))
            write_data(fp, TRUE, ' ', ' ', "RHS", ctmp, con->rhs);
         break;
      case CON_RANGE:
         if (!mpq_equal(con->lhs, const_zero))
            write_data(fp, TRUE, ' ', ' ', "RHS", ctmp, con->lhs);
         break;
      default :
         abort();
      }
   }
   if (has_ranges)
   {
      fprintf(fp, "RANGES\n");
   
      for(con = lp->con_root; con != NULL; con = con->next)
      {
         if (lps_contype(con) == CON_RANGE)
         {
            lps_makename(ctmp, name_size, con->name, con->number);

            /* lhs > rhs => temp is positive
             * range is lhs <= x <= lhs + temp = rhs
             */
            mpq_sub(temp, con->rhs, con->lhs);
            
            assert(!mpq_equal(temp, const_zero));
            
            write_data(fp, TRUE, ' ', ' ', "RNG", ctmp, temp);
         }
      }
   }
   
   fprintf(fp, "BOUNDS\n");

   /* Variables with size == 0, have to be included because
    * they might contain bounds. Detailed checking should
    * be done in a preprocessing routine.
    */
   for(var = lp->var_root; var != NULL; var = var->next)
   {
      /* A variable without any entries in the matrix
       * or the objective function can be ignored.
       */
      if (var->size == 0 && mpq_equal(var->cost, const_zero))
         continue;

      /*   0, oo  -> nix
       *   l, oo  -> LO
       *   0, u   -> UP
       *   l = u  -> FX
       * -oo      -> MI
       *  oo      -> PL
       */
      lps_makename(vtmp, name_size, var->name, var->number);

      if (var->type == VAR_FIXED)
         write_data(fp, TRUE, 'F', 'X', "BOUND", vtmp, var->lower);
      else
      {
         if (var->type == VAR_LOWER || var->type == VAR_BOXED)
            write_data(fp, TRUE, 'L', 'O', "BOUND", vtmp, var->lower);
         else
            write_data(fp, FALSE, 'M', 'I', "BOUND", vtmp, const_zero);
         
         if (var->type == VAR_UPPER || var->type == VAR_BOXED)
            write_data(fp, TRUE, 'U', 'P', "BOUND", vtmp, var->upper);
         else
            write_data(fp, FALSE, 'P', 'L', "BOUND", vtmp, const_zero);
      }
   }
   fprintf(fp, "ENDATA\n");

   mpq_clear(temp);

   free(vtmp);
   free(ctmp);
}   














