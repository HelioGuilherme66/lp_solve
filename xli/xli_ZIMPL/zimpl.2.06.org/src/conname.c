#pragma ident "@(#) $Id: conname.c,v 1.12 2007/08/02 08:36:55 bzfkocht Exp $"
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*   File....: conname.c                                                     */
/*   Name....: Instructions                                                  */
/*   Author..: Thorsten Koch                                                 */
/*   Copyright by Author, All rights reserved                                */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * Copyright (C) 2001-2007 by Thorsten Koch <koch@zib.de>
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
#include <assert.h>

#include "bool.h"
#include "mshell.h"
#include "ratlptypes.h"
#include "mme.h"
#include "xlpglue.h"

static char*        cpfix  = NULL;
static int          count  = 1;
static char*        cname  = NULL;
static unsigned int clen   = 0;
static ConNameForm  cform  = CON_FORM_NAME;

void conname_format(ConNameForm format)
{
   cform = format;
}

void conname_free()
{
   assert(cname != NULL);
   assert(cpfix != NULL);

   free(cname);
   free(cpfix);
   
   cname = NULL;
   clen  = 0;
}

/* return False if we are in mode CON_FORM_NAME and
 * already a constraint with the prefix exists. Otherwise this
 * is unimportant, because all constraints will get a unique
 * number anyway.
 */
Bool conname_set(const char* prefix)
{
   assert(prefix != NULL);
   assert(cname  == NULL);

   cpfix = strdup(prefix);
   clen  = strlen(cpfix) + 16;
   cname = malloc(clen);

   assert(cname != NULL);

   if (cform != CON_FORM_NAME)
      return TRUE;

   assert(cform == CON_FORM_NAME);
   
   count = 1;
   
   strcpy(cname, cpfix);
   strcat(cname, "_1");

   if (xlp_conname_exists(cname))
      return FALSE;

   strcat(cname, "_a_0");

   return !xlp_conname_exists(cname);
}

const char* conname_get()
{
   char*        localstr;
   unsigned int newlen;
   
   assert(cpfix != NULL);
   assert(cname != NULL);

   switch(cform)
   {
   case CON_FORM_MAKE :
      sprintf(cname, "c%d", count);
      break;
   case CON_FORM_NAME :
      sprintf(cname, "%s_%d", cpfix, count);
      break;
   case CON_FORM_FULL :
      localstr = local_tostrall();
      newlen   = strlen(localstr) + strlen(cpfix) + 16;

      if (newlen > clen)
      {
         clen  = newlen;
         cname = realloc(cname, clen);

         assert(cname != NULL);
      }
      sprintf(cname, "%s_%s%s",
         cpfix,
         strlen(localstr) > 0 ? ";" : "",
         localstr);

      free(localstr);
      break;
   }
   assert(strlen(cname) < clen);
   
   return cname;
}

void conname_next()
{
   count++;
}

