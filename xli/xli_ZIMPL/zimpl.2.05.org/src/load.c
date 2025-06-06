#pragma ident "@(#) $Id: load.c,v 1.31 2006/09/18 13:11:24 bzfkocht Exp $"
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*   File....: load.c                                                        */
/*   Name....: Modell Loading Routines                                       */
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
#include <assert.h>
#include <ctype.h>

#include "lint.h"
#include "bool.h"
#include "mshell.h"
#include "ratlptypes.h"
#include "mme.h"

#define BUF_EXT 65536

static char* get_line(char** buf, int* size, const MFP* fp, int* lineno)
{
   Bool in_string = FALSE;
   int  cnt = 0;
   int  c;

   for(;;)
   {
      assert(cnt <= *size);
      
      if (cnt == *size - 1)
      {
         *size += BUF_EXT;
         *buf   = realloc(*buf, (size_t)*size);
      }
      assert(*buf != NULL);
      
      c = mio_getc(fp);

      if (in_string && ((c == EOF) || c == '\n'))
      {
         fprintf(stderr, "*** Error 161: Line %d: Unterminated string\n", *lineno);
         zpl_exit(EXIT_FAILURE);
      }
      if (c == EOF)
      {
         if (cnt > 0)
         {
            (*buf)[cnt] = '\0';
            if (verbose > VERB_QUIET)
               fprintf(stderr, "--- Warning 162: Line %d: Trailing \"%.20s\" ignored\n",
                  *lineno, *buf);
         }
         return NULL;
      }
      if (c == '\n')
      {
         c = ' ';
         (*lineno)++;
      }
      if (iscntrl(c))
         c = ' ';
      
      /* Skip leading white space
       */
      if (cnt == 0 && isspace(c))
         continue;
      
      if (c == '"')
         in_string = !in_string;

      /* Remove comments
       */
      if (!in_string && (c == '#'))
      {
         do { c = mio_getc(fp); } while((c != EOF) && (c != '\n'));
         (*lineno)++;
         continue;
      }
      (*buf)[cnt++] = (char)c;

      if (!in_string && (c == ';'))
         break;      
   }
   (*buf)[cnt] = '\0';
   
   return *buf;
}

static const char* make_pathname(
   char*       target,
   const char* pathname,
   const char* filename)
{
   char* s;

   assert(target   != NULL);
   assert(pathname != NULL);
   assert(filename != NULL);

   /* Absolute Name ? */
   if (*filename == DIRSEP)
      strcpy(target, filename);
   else
   {
      strcpy(target, pathname);
   
      if (NULL == (s = strrchr(target, DIRSEP)))
         strcpy(target, filename);
      else
         strcpy(s + 1, filename);
   }
   return target;
}

static void add_stmt(
   Prog*       prog,
   const char* filename,
   const int   lineno,
   const char* text)
{
   StmtType type = STMT_ERR;

   assert(prog     != NULL);
   assert(filename != NULL);
   assert(text     != NULL);

   if (!strncmp(text, "set", 3) && isspace(text[3]))
      type = STMT_SET;
   else if (!strncmp(text, "param", 5) && isspace(text[5]))
      type = STMT_PARAM;
   else if (!strncmp(text, "var", 3) && isspace(text[3]))
      type = STMT_VAR;
   else if (!strncmp(text, "minimize", 8) && isspace(text[8]))
      type = STMT_MIN;
   else if (!strncmp(text, "maximize", 8) && isspace(text[8]))
      type = STMT_MAX;
   else if (!strncmp(text, "subto", 5) && isspace(text[5]))
      type = STMT_CONS;
   else if (!strncmp(text, "defnumb", 7) && isspace(text[7]))
      type = STMT_DEF;
   else if (!strncmp(text, "defstrg", 7) && isspace(text[7]))
      type = STMT_DEF;
   else if (!strncmp(text, "defbool", 7) && isspace(text[7]))
      type = STMT_DEF;
   else if (!strncmp(text, "defset", 6) && isspace(text[6]))
      type = STMT_DEF;
   else if (!strncmp(text, "do", 2) && isspace(text[2]))
      type = STMT_DO;
   else if (!strncmp(text, "sos", 3) && isspace(text[3]))
      type = STMT_SOS;
   else
   {
      fprintf(stderr, "*** Error 163: Line %d: Syntax Error\n", lineno);
      show_source(stderr, text, 1);

      zpl_exit(EXIT_FAILURE);
   }
   prog_add_stmt(prog, stmt_new(type, filename, lineno, text));
}

void prog_load(Prog* prog, const char* filename)
{
   int   bufsize = BUF_EXT;
   char* buf     = malloc((size_t)bufsize);
   MFP*  fp;
   char* s;
   int   lineno  = 1;
   char  newname [1024];
   char* temp;
   
   assert(prog     != NULL);
   assert(filename != NULL);
   assert(buf      != NULL);
   assert(filename != NULL);

   if (NULL == (fp = mio_open(filename, ".zpl")))
      zpl_exit(EXIT_FAILURE);

   if (verbose)
      printf("Reading %s\n", filename);
   
   while((s = get_line(&buf, &bufsize, fp, &lineno)) != NULL)
   {
      assert(!isspace(*s));

      /* This could happen if we have a ;; somewhere.
       */
      if (*s == '\0')
         continue;

      if (1 == sscanf(s, "include \"%1023[^\"]\"", newname))
      {
         temp = malloc(strlen(filename) + strlen(newname) + 2);
         prog_load(prog, make_pathname(temp, filename, newname));
         free(temp);
      }
      else
      { 
         add_stmt(prog, filename, lineno, s);
      }
   }
   mio_close(fp);
   free(buf);
}




