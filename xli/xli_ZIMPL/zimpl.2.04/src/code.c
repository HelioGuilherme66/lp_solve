#pragma ident "@(#) $Id: code.c,v 1.27 2005/09/27 09:17:06 bzfkocht Exp $"
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*   File....: code.c                                                        */
/*   Name....: Code Node Functions                                           */
/*   Author..: Thorsten Koch                                                 */
/*   Copyright by Author, All rights reserved                                */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * Copyright (C) 2001 by Thorsten Koch <koch@zib.de>
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
#include <stdarg.h>

#include "bool.h"
#include "mshell.h"
#include "ratlptypes.h"
#include "mme.h"
#include "code.h"
#include "inst.h"

typedef union code_value CodeValue;

union code_value
{
   Numb*        numb;
   const char*  strg;
   const char*  name;
   Tuple*       tuple;
   Set*         set;
   Term*        term;
   Entry*       entry;
   IdxSet*      idxset;
   Bool         bool;
   int          size;
   List*        list;
   VarClass     varclass;
   ConType      contype;
   RDef*        rdef;
   RPar*        rpar;
   unsigned int bits;
   Symbol*      sym;
   Define*      def;
   Bound*       bound;
};

#define MAX_CHILDS 7

struct code_node
{
   SID
   CodeType    type;
   Inst        eval;
   CodeValue   value;
   CodeNode*   child[MAX_CHILDS];
   const Stmt* stmt;
   int         column;
};

#define CODE_SID  0x436f6465

#if !defined INLINE

#if defined __cplusplus
  #define INLINE inline
#elif defined _WIN32 || defined WIN32
  #define INLINE _inline
#else
  #define INLINE
#endif

#endif

static CodeNode*    root;
static unsigned int inst_count = 0;

INLINE Bool code_is_valid(const CodeNode* node)
{
   return ((node != NULL) && SID_ok(node, CODE_SID));
}

CodeNode* code_new_inst(Inst inst, int childs, ...)
{
   va_list   ap;
   CodeNode* node = calloc(1, sizeof(*node));
   int       i;
   CodeNode* child;
   
   assert(inst != INST_NULL);
   assert(node != NULL);

   node->type   = CODE_ERR; /* call to eval() will set this */
   node->eval   = inst;
   /* Das sollte eigendlich ein parameter sein, aber wozu bei jedem
    * code_new diese Funktionen mit uebergeben.
    */
   node->stmt   = scan_get_stmt();
   node->column = scan_get_column();
   
   SID_set(node, CODE_SID);
   assert(code_is_valid(node));

   /*lint -save -e826 */
   va_start(ap, childs);

   for(i = 0; i < childs; i++)
   {
      child = va_arg(ap, CodeNode*);
      assert((child == NULL) || code_is_valid(child));
      node->child[i] = child;
   }
   va_end(ap);
   /*lint -restore */
   
   return node;
}

/* We eat the numb, i.e. we will free it!
 */
CodeNode* code_new_numb(Numb* numb)
{
   CodeNode* node = calloc(1, sizeof(*node));

   assert(node != NULL);

   node->type       = CODE_NUMB;
   node->eval       = i_nop;
   node->value.numb = numb; 
   node->stmt       = scan_get_stmt();
   node->column     = scan_get_column();

   SID_set(node, CODE_SID);
   assert(code_is_valid(node));
   
   return node;
}

CodeNode* code_new_strg(const char* strg)
{
   CodeNode* node = calloc(1, sizeof(*node));

   assert(strg != NULL);
   assert(node != NULL);

   node->type       = CODE_STRG;
   node->eval       = i_nop;
   node->value.strg = strg;
   node->stmt       = scan_get_stmt();
   node->column     = scan_get_column();

   SID_set(node, CODE_SID);
   assert(code_is_valid(node));
   
   return node;
}

CodeNode* code_new_name(const char* name)
{
   CodeNode* node = calloc(1, sizeof(*node));

   assert(name != NULL);
   assert(node != NULL);

   node->type       = CODE_NAME;
   node->eval       = i_nop;
   node->value.name = name;
   node->stmt       = scan_get_stmt();
   node->column     = scan_get_column();

   SID_set(node, CODE_SID);
   assert(code_is_valid(node));
   
   return node;
}

CodeNode* code_new_size(int size)
{
   CodeNode* node = calloc(1, sizeof(*node));

   assert(size >= 0);
   assert(node != NULL);

   node->type       = CODE_SIZE;
   node->eval       = i_nop;
   node->value.size = size;
   node->stmt       = scan_get_stmt();
   node->column     = scan_get_column();

   SID_set(node, CODE_SID);
   assert(code_is_valid(node));
   
   return node;
}

CodeNode* code_new_varclass(VarClass varclass)
{
   CodeNode* node = calloc(1, sizeof(*node));

   assert(node != NULL);

   node->type           = CODE_VARCLASS;
   node->eval           = i_nop;
   node->value.varclass = varclass;
   node->stmt           = scan_get_stmt();
   node->column         = scan_get_column();

   SID_set(node, CODE_SID);
   assert(code_is_valid(node));
   
   return node;
}

CodeNode* code_new_contype(ConType contype)
{
   CodeNode* node = calloc(1, sizeof(*node));

   assert(node != NULL);

   node->type          = CODE_CONTYPE;
   node->eval          = i_nop;
   node->value.contype = contype;
   node->stmt          = scan_get_stmt();
   node->column        = scan_get_column();

   SID_set(node, CODE_SID);
   assert(code_is_valid(node));
   
   return node;
}

CodeNode* code_new_bits(unsigned int bits)
{
   CodeNode* node = calloc(1, sizeof(*node));

   assert(node != NULL);

   node->type       = CODE_BITS;
   node->eval       = i_nop;
   node->value.bits = bits;
   node->stmt       = scan_get_stmt();
   node->column     = scan_get_column();

   SID_set(node, CODE_SID);
   assert(code_is_valid(node));
   
   return node;
}

CodeNode* code_new_symbol(Symbol* sym)
{
   CodeNode* node = calloc(1, sizeof(*node));

   assert(node != NULL);

   node->type       = CODE_SYM;
   node->eval       = i_nop;
   node->value.sym  = sym;
   node->stmt       = scan_get_stmt();
   node->column     = scan_get_column();

   SID_set(node, CODE_SID);
   assert(code_is_valid(node));
   
   return node;
}

CodeNode* code_new_define(Define* def)
{
   CodeNode* node = calloc(1, sizeof(*node));

   assert(node != NULL);

   node->type       = CODE_DEF;
   node->eval       = i_nop;
   node->value.def  = def;
   node->stmt       = scan_get_stmt();
   node->column     = scan_get_column();

   SID_set(node, CODE_SID);
   assert(code_is_valid(node));
   
   return node;
}

/* We eat the bound, i.e. we will free it!
 */
CodeNode* code_new_bound(BoundType type)
{
   CodeNode* node = calloc(1, sizeof(*node));

   assert(node != NULL);
   assert(type == BOUND_INFTY || type == BOUND_MINUS_INFTY);

   node->type        = CODE_BOUND;
   node->eval        = i_nop;
   node->value.bound = bound_new(type, NULL); 
   node->stmt        = scan_get_stmt();
   node->column      = scan_get_column();

   SID_set(node, CODE_SID);
   assert(code_is_valid(node));
   
   return node;
}

static INLINE void code_free_value(const CodeNode* node)
{
   assert(code_is_valid(node));

   switch(node->type)
   {
   case CODE_ERR :
   case CODE_VOID :
      /* Kann passieren, wenn bei code_value_() ein bis dahin unbenutzter
       * Knoten verwendet wird.
       */
      break;
   case CODE_NUMB :
      numb_free(node->value.numb);
      break;
   case CODE_STRG :
   case CODE_NAME :
      break;
   case CODE_TUPLE :
      tuple_free(node->value.tuple);
      break;
   case CODE_SET :
      set_free(node->value.set);
      break;
   case CODE_TERM :
      term_free(node->value.term);
      break;
   case CODE_ENTRY :
      entry_free(node->value.entry);
      break;
   case CODE_IDXSET :
      idxset_free(node->value.idxset);
      break;
   case CODE_BOOL :
   case CODE_SIZE :
      break;
   case CODE_LIST :
      list_free(node->value.list);
      break;
   case CODE_VARCLASS :
   case CODE_CONTYPE :
      break;
   case CODE_RDEF :
      rdef_free(node->value.rdef);
      break;
   case CODE_RPAR :
      rpar_free(node->value.rpar);
      break;
   case CODE_BITS :
      break;
   case CODE_SYM :
      break;
   case CODE_DEF :
      break;
   case CODE_BOUND :
      bound_free(node->value.bound);
      break;
   default :
      abort();
   }
}

void code_free(CodeNode* node)
{
   int i;

   for(i = 0; i < MAX_CHILDS; i++)
      if (node->child[i] != NULL)
         code_free(node->child[i]);

   code_free_value(node);
   free(node);
}

void code_set_child(CodeNode* node, int idx, CodeNode* child)
{
   assert(code_is_valid(node));
   assert(idx   >= 0);
   assert(idx   <  MAX_CHILDS);
   assert(child != NULL);

   node->child[idx] = child;
}

CodeType code_get_type(const CodeNode* node)
{
   assert(code_is_valid(node));

   return node->type;
}

Inst code_get_inst(const CodeNode* node)
{
   assert(code_is_valid(node));

   return node->eval;
}
     
void code_set_root(CodeNode* node)
{
   assert(code_is_valid(node));
   
   root = node;
}

CodeNode* code_get_root(void)
{
   return root;
}

unsigned int code_get_inst_count()
{
   return inst_count;
}
     
static INLINE CodeNode* code_check_type(CodeNode* node, CodeType expected)
{
   static const char* tname[] =
   {
      "Error", "Number", "String", "Name", "Tuple", "Set", "Term", "Bool", "Size",
      "IndexSet", "List", "Nothing", "Entry", "VarClass", "ConType",
      "ReadDefinition", "ReadParameter", "BitFlag", "Symbol", "Define", "Bound"
   };
   assert(code_is_valid(node));
   assert(sizeof(tname) / sizeof(tname[0]) > (size_t)node->type);
   
   if (node->type != expected)
   {
      assert(sizeof(tname) / sizeof(tname[0]) > (size_t)expected);
      
      fprintf(stderr, "*** Error 159: Type error, expected %s got %s\n",
         tname[expected], tname[node->type]);
      code_errmsg(node);
      zpl_exit(EXIT_FAILURE);
   }
   return node;
}

void code_errmsg(const CodeNode* node)
{
   fprintf(stderr, "*** File: %s Line %d\n",
      stmt_get_filename(node->stmt),
      stmt_get_lineno(node->stmt));

   show_source(stderr, stmt_get_text(node->stmt), node->column);

   if (verbose >= VERB_CHATTER)
      local_print_all(stderr);
}

INLINE CodeNode* code_eval(CodeNode* node)
{
   assert(code_is_valid(node));

   inst_count++;
   
   return (*node->eval)(node);
}

/* ----------------------------------------------------------------------------
 * Get Funktionen
 * ----------------------------------------------------------------------------
 */

INLINE CodeNode* code_get_child(const CodeNode* node, int no)
{
   assert(code_is_valid(node));
   assert(no              >= 0);
   assert(no              <  MAX_CHILDS);
   assert(node->child[no] != NULL);
   
   return node->child[no];
}

INLINE const Numb* code_get_numb(CodeNode* node)
{
   return code_check_type(node, CODE_NUMB)->value.numb;
}

INLINE const char* code_get_strg(CodeNode* node)
{
   return code_check_type(node, CODE_STRG)->value.strg;
}

INLINE const char* code_get_name(CodeNode* node)
{
   return code_check_type(node, CODE_NAME)->value.name;
}

INLINE const Tuple* code_get_tuple(CodeNode* node)
{
   return code_check_type(node, CODE_TUPLE)->value.tuple;
}

INLINE const Set* code_get_set(CodeNode* node)
{
   return code_check_type(node, CODE_SET)->value.set;
}

INLINE const IdxSet* code_get_idxset(CodeNode* node)
{
   return code_check_type(node, CODE_IDXSET)->value.idxset;
}

INLINE const Entry* code_get_entry(CodeNode* node)
{
   return code_check_type(node, CODE_ENTRY)->value.entry;
}

INLINE const Term* code_get_term(CodeNode* node)
{
   return code_check_type(node, CODE_TERM)->value.term;
}

INLINE int code_get_size(CodeNode* node)
{
   return code_check_type(node, CODE_SIZE)->value.size;
}

INLINE Bool code_get_bool(CodeNode* node)
{
   return code_check_type(node, CODE_BOOL)->value.bool;
}

INLINE const List* code_get_list(CodeNode* node)
{
   return code_check_type(node, CODE_LIST)->value.list;
}

INLINE VarClass code_get_varclass(CodeNode* node)
{
   return code_check_type(node, CODE_VARCLASS)->value.varclass;
}

INLINE ConType code_get_contype(CodeNode* node)
{
   return code_check_type(node, CODE_CONTYPE)->value.contype;
}

INLINE const RDef* code_get_rdef(CodeNode* node)
{
   return code_check_type(node, CODE_RDEF)->value.rdef;
}

INLINE const RPar* code_get_rpar(CodeNode* node)
{
   return code_check_type(node, CODE_RPAR)->value.rpar;
}

INLINE unsigned int code_get_bits(CodeNode* node)
{
   return code_check_type(node, CODE_BITS)->value.bits;
}

INLINE Symbol* code_get_symbol(CodeNode* node)
{
   return code_check_type(node, CODE_SYM)->value.sym;
}

INLINE Define* code_get_define(CodeNode* node)
{
   return code_check_type(node, CODE_DEF)->value.def;
}

INLINE const Bound* code_get_bound(CodeNode* node)
{
   return code_check_type(node, CODE_BOUND)->value.bound;
}

/* ----------------------------------------------------------------------------
 * Value Funktionen
 * ----------------------------------------------------------------------------
 */
void code_value_numb(CodeNode* node, Numb* numb)
{
   assert(code_is_valid(node));

   code_free_value(node);
   
   node->type       = CODE_NUMB;
   node->value.numb = numb;
}

void code_value_strg(CodeNode* node, const char* strg)
{
   assert(code_is_valid(node));
   assert(strg != NULL);
   
   code_free_value(node);

   node->type       = CODE_STRG;
   node->value.strg = strg;
}

void code_value_name(CodeNode* node, const char* name)
{
   assert(code_is_valid(node));
   assert(name != NULL);
   
   code_free_value(node);

   node->type       = CODE_NAME;
   node->value.name = name;
}

void code_value_tuple(CodeNode* node, Tuple* tuple)
{
   assert(code_is_valid(node));
   assert(tuple_is_valid(tuple));

   code_free_value(node);

   node->type        = CODE_TUPLE;
   node->value.tuple = tuple;
}

void code_value_set(CodeNode* node, Set* set)
{
   assert(code_is_valid(node));
   assert(set_is_valid(set));

   code_free_value(node);

   node->type      = CODE_SET;
   node->value.set = set;
}

void code_value_idxset(CodeNode* node, IdxSet* idxset)
{
   assert(code_is_valid(node));
   assert(idxset_is_valid(idxset));

   code_free_value(node);

   node->type         = CODE_IDXSET;
   node->value.idxset = idxset;
}

void code_value_entry(CodeNode* node, Entry* entry)
{
   assert(code_is_valid(node));
   assert(entry_is_valid(entry));

   code_free_value(node);

   node->type        = CODE_ENTRY;
   node->value.entry = entry;
}

void code_value_term(CodeNode* node, Term* term)
{
   assert(code_is_valid(node));
   assert(term_is_valid(term));

   code_free_value(node);

   node->type       = CODE_TERM;
   node->value.term = term;
}

void code_value_bool(CodeNode* node, Bool bool)
{
   assert(code_is_valid(node));

   code_free_value(node);

   node->type       = CODE_BOOL;
   node->value.bool = bool;
}

void code_value_size(CodeNode* node, int size)
{
   assert(code_is_valid(node));

   code_free_value(node);

   node->type       = CODE_SIZE;
   node->value.size = size;
}

void code_value_list(CodeNode* node, List* list)
{
   assert(code_is_valid(node));
   assert(list_is_valid(list));

   code_free_value(node);

   node->type       = CODE_LIST;
   node->value.list = list;
}

void code_value_varclass(CodeNode* node, VarClass varclass)
{
   assert(code_is_valid(node));

   code_free_value(node);

   node->type           = CODE_VARCLASS;
   node->value.varclass = varclass;
}

void code_value_contype(CodeNode* node, ConType contype)
{
   assert(code_is_valid(node));

   code_free_value(node);

   node->type          = CODE_CONTYPE;
   node->value.contype = contype;
}

void code_value_rdef(CodeNode* node, RDef* rdef)
{
   assert(code_is_valid(node));
   assert(rdef_is_valid(rdef));

   code_free_value(node);

   node->type       = CODE_RDEF;
   node->value.rdef = rdef;
}

void code_value_rpar(CodeNode* node, RPar* rpar)
{
   assert(code_is_valid(node));
   assert(rpar_is_valid(rpar));

   code_free_value(node);

   node->type       = CODE_RPAR;
   node->value.rpar = rpar;
}

void code_value_bits(CodeNode* node, unsigned int bits)
{
   assert(code_is_valid(node));

   code_free_value(node);

   node->type       = CODE_BITS;
   node->value.bits = bits;
}

void code_value_bound(CodeNode* node, Bound* bound)
{
   assert(code_is_valid(node));

   code_free_value(node);
   
   node->type        = CODE_BOUND;
   node->value.bound = bound;
}

void code_value_void(CodeNode* node)
{
   assert(code_is_valid(node));

   code_free_value(node);

   node->type = CODE_VOID;
}

void code_copy_value(CodeNode* dst, const CodeNode* src)
{
   assert(code_is_valid(dst));
   assert(code_is_valid(src));
   
   code_free_value(dst);

   switch(src->type)
   {
   case CODE_NUMB :
      dst->value.numb = numb_copy(src->value.numb);
      break;
   case CODE_STRG :
      dst->value.strg = src->value.strg;
      break;
   case CODE_NAME :
      dst->value.name = src->value.name;
      break;
   case CODE_TUPLE:
      dst->value.tuple = tuple_copy(src->value.tuple);
      break;
   case CODE_SET :
      dst->value.set = set_copy(src->value.set);
      break;
   case CODE_IDXSET :
      dst->value.idxset = idxset_copy(src->value.idxset);
      break;
   case CODE_ENTRY :
      dst->value.entry = entry_copy(src->value.entry);
      break;
   case CODE_TERM :
      dst->value.term = term_copy(src->value.term);
      break;
   case CODE_BOOL :
      dst->value.bool = src->value.bool;
      break;
   case CODE_SIZE :
      dst->value.size = src->value.size;
      break;
   case CODE_LIST :
      dst->value.list = list_copy(src->value.list);
      break;
   case CODE_VARCLASS :
      dst->value.varclass = src->value.varclass;
      break;
   case CODE_CONTYPE :
      dst->value.contype = src->value.contype;
      break;
   case CODE_RDEF :
      dst->value.rdef = rdef_copy(src->value.rdef);
      break;
   case CODE_RPAR :
      dst->value.rpar = rpar_copy(src->value.rpar);
      break;
   case CODE_BITS :
      dst->value.bits = src->value.bits;
      break;
   case CODE_BOUND :
      dst->value.bound = bound_copy(src->value.bound);
      break;
   case CODE_VOID :
      break;
   default :
      abort();
   }
   dst->type = src->type;
}

/* ----------------------------------------------------------------------------
 * Macro Funktionen for better inlining
 * ----------------------------------------------------------------------------
 */
CodeNode* code_eval_child(const CodeNode* node, int no)
{
   return code_eval(code_get_child(node, no));
}

const Numb* code_eval_child_numb(const CodeNode* node, int no)
{
   return code_get_numb(code_eval(code_get_child(node, no)));
}

const char* code_eval_child_strg(const CodeNode* node, int no)
{
   return code_get_strg(code_eval(code_get_child(node, no)));
}

const char*code_eval_child_name(const CodeNode* node, int no)
{
   return code_get_name(code_eval(code_get_child(node, no)));
}

const Tuple* code_eval_child_tuple(const CodeNode* node, int no)
{
   return code_get_tuple(code_eval(code_get_child(node, no)));
}

const Set* code_eval_child_set(const CodeNode* node, int no)
{
   return code_get_set(code_eval(code_get_child(node, no)));
}

const IdxSet* code_eval_child_idxset(const CodeNode* node, int no)
{
   return code_get_idxset(code_eval(code_get_child(node, no)));
}

const Entry* code_eval_child_entry(const CodeNode* node, int no)
{
   return code_get_entry(code_eval(code_get_child(node, no)));
}

const Term* code_eval_child_term(const CodeNode* node, int no)
{
   return code_get_term(code_eval(code_get_child(node, no)));
}

int code_eval_child_size(const CodeNode* node, int no)
{
   return code_get_size(code_eval(code_get_child(node, no)));
}

Bool code_eval_child_bool(const CodeNode* node, int no)
{
   return code_get_bool(code_eval(code_get_child(node, no)));
}

const List* code_eval_child_list(const CodeNode* node, int no)
{
   return code_get_list(code_eval(code_get_child(node, no)));
}

VarClass code_eval_child_varclass(const CodeNode* node, int no)
{
   return code_get_varclass(code_eval(code_get_child(node, no)));
}

ConType code_eval_child_contype(const CodeNode* node, int no)
{
   return code_get_contype(code_eval(code_get_child(node, no)));
}

const RDef* code_eval_child_rdef(const CodeNode* node, int no)
{
   return code_get_rdef(code_eval(code_get_child(node, no)));
}

const RPar* code_eval_child_rpar(const CodeNode* node, int no)
{
   return code_get_rpar(code_eval(code_get_child(node, no)));
}

unsigned int code_eval_child_bits(const CodeNode* node, int no)
{
   return code_get_bits(code_eval(code_get_child(node, no)));
}

Symbol* code_eval_child_symbol(const CodeNode* node, int no)
{
   return code_get_symbol(code_eval(code_get_child(node, no)));
}

Define* code_eval_child_define(const CodeNode* node, int no)
{
   return code_get_define(code_eval(code_get_child(node, no)));
}

const Bound* code_eval_child_bound(const CodeNode* node, int no)
{
   return code_get_bound(code_eval(code_get_child(node, no)));
}








