#pragma ident "@(#) $Id: set4.c,v 1.12 2006/01/19 20:53:07 bzfkocht Exp $"
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*   File....: set.c                                                         */
/*   Name....: Set Functions                                                 */
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

#include "bool.h"
#include "mshell.h"
#include "ratlptypes.h"
#include "mme.h"
#include "set4.h"

#define TEST_DUBLICATE   0

#define SET_SID          0x5365745f

#if !defined INLINE

#if defined __cplusplus
  #define INLINE inline
#elif defined _WIN32 || defined WIN32
  #define INLINE _inline
#else
  #define INLINE
#endif

#endif


#ifndef NDEBUG
static
#endif
SetVTab* set_vtab_global = NULL;

void set_init()
{
   assert(set_vtab_global == NULL);

   set_vtab_global = calloc((size_t)SET_TYPES, sizeof(*set_vtab_global));

   assert(set_vtab_global != NULL);

   set_empty_init(set_vtab_global);
   set_pseudo_init(set_vtab_global);
   set_list_init(set_vtab_global);
   set_range_init(set_vtab_global);
   set_prod_init(set_vtab_global);
   set_multi_init(set_vtab_global);
#if 0
   set_union_init(set_vtab_global);
   set_inter_init(set_vtab_global);
   set_minus_init(set_vtab_global);
   set_sdiff_init(set_vtab_global);
#endif
}

void set_exit()
{
   assert(set_vtab_global != NULL);
   
   free(set_vtab_global);

   set_vtab_global = NULL;
}

Set* set_new_from_list(const List* list, SetCheckType check)
{
   ListElem* le  = NULL;
   int       dim;
   Set*      set = NULL;

   assert(list_is_valid(list));
   assert(list_get_elems(list) > 0);
   
   if (list_is_elemlist(list))
      set = set_list_new_from_elems(list, check);
   else if (list_is_tuplelist(list))
   {
      dim = tuple_get_dim(list_get_tuple(list, &le));

      if (dim == 1)
         set = set_list_new_from_tuples(list, check);
      else
         set = set_multi_new_from_list(list, check);
   }
   else
   {
      assert(list_is_entrylist(list));

      dim = tuple_get_dim(entry_get_tuple(list_get_entry(list, &le)));
   
      if (dim == 1)
         set = set_list_new_from_entries(list, check);
      else
         set = set_multi_new_from_list(list, check);
   }
   assert(set_is_valid(set));

   return set;
}

void set_free(Set* set)
{
   set_vtab_global[set->head.type].set_free(set);
}

Bool set_is_valid(const Set* set)
{
   return set != NULL && set_vtab_global[set->head.type].set_is_valid(set);
}

Set* set_copy(const Set* set)
{
   return set_vtab_global[set->head.type].set_copy(set);
}

INLINE int set_lookup_idx(const Set* set, const Tuple* tuple, int offset)
{
   return set_vtab_global[set->head.type].set_lookup_idx(set, tuple, offset);
}

Bool set_lookup(const Set* set, const Tuple* tuple)
{
   if (set->head.dim != tuple_get_dim(tuple))
      return FALSE;
   
   return set_vtab_global[set->head.type].set_lookup_idx(set, tuple, 0) >= 0;
}

INLINE void set_get_tuple_intern(const Set* set, int idx, Tuple* tuple, int offset)
{
   set_vtab_global[set->head.type].set_get_tuple(set, idx, tuple, offset);
}

Tuple* set_get_tuple(const Set* set, int idx)
{
   Tuple* tuple;

   tuple = tuple_new(set->head.dim);

   assert(set_is_valid(set));
   assert(idx >= 0);
   assert(idx <  set->head.members);

   set_get_tuple_intern(set, idx, tuple, 0);
   
   return tuple;
}
     
INLINE SetIter* set_iter_init_intern(const Set* set, const Tuple* pattern, int offset)
{
   return set_vtab_global[set->head.type].iter_init(set, pattern, offset);
}

SetIter* set_iter_init(const Set* set, const Tuple* pattern)
{
   return set_iter_init_intern(set, pattern, 0);
}

INLINE Bool set_iter_next_intern(SetIter* iter, const Set* set, Tuple* tuple, int offset)
{
   return set_vtab_global[set->head.type].iter_next(iter, set, tuple, offset);
}

Tuple* set_iter_next(SetIter* iter, const Set* set)
{
   Tuple* tuple;

   tuple = tuple_new(set->head.dim);
   
   if (set_iter_next_intern(iter, set, tuple, 0))
      return tuple;

   tuple_free(tuple);

   return NULL;
}
     
INLINE void set_iter_exit_intern(SetIter* iter, const Set* set)
{
   set_vtab_global[set->head.type].iter_exit(iter, set);
}

void set_iter_exit(SetIter* iter, const Set* set)
{
   set_iter_exit_intern(iter, set);
}
     
INLINE void set_iter_reset_intern(SetIter* iter, const Set* set)
{
   set_vtab_global[set->head.type].iter_reset(iter, set);
}

int set_get_dim(const Set* set)
{
   assert(set_is_valid(set));

   return set->head.dim;   
}

int set_get_members(const Set* set)
{
   assert(set_is_valid(set));

   return set->head.members;   
}

void set_print(FILE* fp, const Set* set)
{
   SetIter* iter;
   Tuple*   tuple;
   Bool     first = TRUE;

   assert(fp != NULL);
   assert(set_is_valid(set));

   switch(set->head.type)
   {
   case SET_EMPTY :
      fprintf(fp, "Empty: ");
      break;
   case SET_PSEUDO :
      fprintf(fp, "Pseudo: ");
      break;
   case SET_LIST :
      fprintf(fp, "List: ");
      break;
   case SET_RANGE :
      fprintf(fp, "Range: ");
      break;
   case SET_PROD :
      fprintf(fp, "Product: ");
      break;
   case SET_MULTI :
      fprintf(fp, "Multi: ");
      break;
   default :
      abort();
   }
   
   fprintf(fp, "|%d|", set->head.dim);
   fprintf(fp, "{");

   iter = set_iter_init(set, NULL);

   while(NULL != (tuple = set_iter_next(iter, set)))
   {
      if (first)
         first = FALSE;
      else
         fprintf(fp, ",");
      
      tuple_print(fp, tuple);
      tuple_free(tuple);
   }
   set_iter_exit(iter, set);
   
   fprintf(fp, "}");
}

/* In A or in B */
Set* set_union(const Set* set_a, const Set* set_b)
{
   Set*     set;
   SetIter* iter;
   List*    list = NULL;
   Tuple*   tuple;
   
   assert(set_is_valid(set_a));
   assert(set_is_valid(set_b));
   assert(set_a->head.dim  == set_b->head.dim
      ||  set_a->head.type == SET_EMPTY
      ||  set_b->head.type == SET_EMPTY);

   iter = set_iter_init(set_a, NULL);

   while(NULL != (tuple = set_iter_next(iter, set_a)))
   {
      if (list == NULL)
         list  = list_new_tuple(tuple);
      else
         list_add_tuple(list, tuple);

      tuple_free(tuple);
   }
   set_iter_exit(iter, set_a);
   
   iter = set_iter_init(set_b, NULL);

   while(NULL != (tuple = set_iter_next(iter, set_b)))
   {
      if (!set_lookup(set_a, tuple))
      {
         if (list == NULL)
            list  = list_new_tuple(tuple);
         else
            list_add_tuple(list, tuple);
      }
      tuple_free(tuple);
   }
   set_iter_exit(iter, set_b);

   if (list == NULL)
   {
      assert(set_get_members(set_a) + set_get_members(set_b) == 0);

      set = set_empty_new(
         set_a->head.type == SET_EMPTY ? set_b->head.dim : set_a->head.dim);
   }
   else
   {
      set = set_new_from_list(list, SET_CHECK_NONE);

      assert(set_get_members(set) <= set_get_members(set_a) + set_get_members(set_b));

      list_free(list);
   }
   return set;
}

/* In A and in B */
Set* set_inter(const Set* set_a, const Set* set_b)
{
   Set*     set;
   SetIter* iter;
   List*    list = NULL;
   Tuple*   tuple;
   
   assert(set_is_valid(set_a));
   assert(set_is_valid(set_b));
   assert(set_a->head.dim  == set_b->head.dim
      ||  set_a->head.type == SET_EMPTY
      ||  set_b->head.type == SET_EMPTY);

   iter = set_iter_init(set_a, NULL);

   while(NULL != (tuple = set_iter_next(iter, set_a)))
   {
      if (set_lookup(set_b, tuple))
      {
         if (list == NULL)
            list  = list_new_tuple(tuple);
         else
            list_add_tuple(list, tuple);
      }
      tuple_free(tuple);
   }
   set_iter_exit(iter, set_a);
   
   if (list == NULL)
      set = set_empty_new(
         set_a->head.type == SET_EMPTY ? set_b->head.dim : set_a->head.dim);
   else
   {
      set = set_new_from_list(list, SET_CHECK_NONE);

      assert(set_get_members(set) <= set_get_members(set_a) + set_get_members(set_b));

      list_free(list);
   }
   return set;
}

/* In A but not in B */
Set* set_minus(const Set* set_a, const Set* set_b)
{
   Set*     set;
   SetIter* iter;
   List*    list = NULL;
   Tuple*   tuple;
   
   assert(set_is_valid(set_a));
   assert(set_is_valid(set_b));
   assert(set_a->head.dim  == set_b->head.dim
      ||  set_a->head.type == SET_EMPTY
      ||  set_b->head.type == SET_EMPTY);
   
   iter = set_iter_init(set_a, NULL);

   while(NULL != (tuple = set_iter_next(iter, set_a)))
   {
      if (!set_lookup(set_b, tuple))
      {
         if (list == NULL)
            list  = list_new_tuple(tuple);
         else
            list_add_tuple(list, tuple);
      }
      tuple_free(tuple);
   }
   set_iter_exit(iter, set_a);
   
   if (list == NULL)
   {
      assert(set_is_subseteq(set_a, set_b));

      set = set_empty_new(
         set_a->head.type == SET_EMPTY ? set_b->head.dim : set_a->head.dim);
   }
   else
   {
      set = set_new_from_list(list, SET_CHECK_NONE);

      assert(set_get_members(set) <= set_get_members(set_a));

      list_free(list);
   }
   return set;
}

/* In A and not in B or in B and not in A  (Symetric difference) */
Set* set_sdiff(const Set* set_a, const Set* set_b)
{
   Set*     set;
   SetIter* iter;
   List*    list = NULL;
   Tuple*   tuple;
   
   assert(set_is_valid(set_a));
   assert(set_is_valid(set_b));
   assert(set_a->head.dim  == set_b->head.dim
      ||  set_a->head.type == SET_EMPTY
      ||  set_b->head.type == SET_EMPTY);
   
   iter = set_iter_init(set_a, NULL);

   while(NULL != (tuple = set_iter_next(iter, set_a)))
   {
      if (!set_lookup(set_b, tuple))
      {
         if (list == NULL)
            list  = list_new_tuple(tuple);
         else
            list_add_tuple(list, tuple);
      }
      tuple_free(tuple);
   }
   set_iter_exit(iter, set_a);
   
   iter = set_iter_init(set_b, NULL);

   while(NULL != (tuple = set_iter_next(iter, set_b)))
   {
      if (!set_lookup(set_a, tuple))
      {
         if (list == NULL)
            list  = list_new_tuple(tuple);
         else
            list_add_tuple(list, tuple);
      }
      tuple_free(tuple);
   }
   set_iter_exit(iter, set_b);

   if (list == NULL)
   {
      assert(set_is_equal(set_a, set_b));

      set = set_empty_new(
         set_a->head.type == SET_EMPTY ? set_b->head.dim : set_a->head.dim);
   }
   else
   {
      set = set_new_from_list(list, SET_CHECK_NONE);

      assert(set_get_members(set) <= set_get_members(set_a) + set_get_members(set_b));

      list_free(list);
   }
   return set;
}

/* project set_a to a new set, using the elements index in the tuple.
 */
Set* set_proj(const Set* set, const Tuple* pattern)
{
   Tuple*   tuple;
   Tuple*   new_tuple;
   SetIter* iter;
   List*    list = NULL;
   int      i;
   int      dim;
   int*     idx;
   Set*     new_set;
   
   assert(set_is_valid(set));
   assert(tuple_is_valid(pattern));

   dim = tuple_get_dim(pattern);
   idx = malloc(sizeof(*idx) * dim);

   assert(idx != NULL);
   
   for(i = 0; i < dim; i++)
   {
      assert(numb_is_int(elem_get_numb(tuple_get_elem(pattern, i))));
      
      idx[i] = numb_toint(elem_get_numb(tuple_get_elem(pattern, i))) - 1;
   }
   iter = set_iter_init(set, NULL);

   while(NULL != (tuple = set_iter_next(iter, set)))
   {
      new_tuple = tuple_new(dim);

      for(i = 0; i < dim; i++)
         tuple_set_elem(new_tuple, i, elem_copy(tuple_get_elem(tuple, idx[i]))); 

      if (list == NULL)
         list  = list_new_tuple(new_tuple);
      else
         list_add_tuple(list, new_tuple);

      tuple_free(tuple);
      tuple_free(new_tuple);
   }
   set_iter_exit(iter, set);
   
   free(idx);

   if (list == NULL)
   {
      assert(set_get_members(set) == 0);

      new_set = set_empty_new(dim);
   }
   else
   {
      new_set = set_new_from_list(list, SET_CHECK_QUIET);

      assert(set_get_members(new_set) <= set_get_members(set));

      list_free(list);
   }
   return new_set;
}

/* Is A subset (or equal) of B */
Bool set_is_subseteq(const Set* set_a, const Set* set_b)
{
   SetIter* iter;
   Tuple*   tuple;
   Bool     is_subset = TRUE;
   
   assert(set_is_valid(set_a));
   assert(set_is_valid(set_b));

   /* If A is the empty set and B is non empty,
    * A is a subset of B
    */
   if (set_a->head.members == 0)
      return TRUE;
   
   if (set_a->head.dim != set_b->head.dim)
   {
      if (verbose > VERB_QUIET)
      {
         fprintf(stderr, "--- Warning 165: Comparison of different dimension sets.\n");
         set_print(stderr, set_a);
         fputc('\n', stderr);
         set_print(stderr, set_b);
         fputc('\n', stderr);
      }
      return FALSE;
   }
   if (set_a->head.members > set_b->head.members)
      return FALSE;

   iter = set_iter_init(set_a, NULL);
   
   while(is_subset && NULL != (tuple = set_iter_next(iter, set_a)))
   {
      is_subset = set_lookup(set_b, tuple);

      tuple_free(tuple);
   }
   set_iter_exit(iter, set_a);
   
   return is_subset;
}

/* A is real subset of B */
Bool set_is_subset(const Set* set_a, const Set* set_b)
{
   assert(set_is_valid(set_a));
   assert(set_is_valid(set_b));

   if (set_a->head.members >= set_b->head.members)
      return FALSE;

   return set_is_subseteq(set_a, set_b);
}

/* A has the same elements as B */
Bool set_is_equal(const Set* set_a, const Set* set_b)
{
   assert(set_is_valid(set_a));
   assert(set_is_valid(set_b));

   if (set_a->head.members != set_b->head.members)
      return FALSE;

   return set_is_subseteq(set_a, set_b);
}

/* n elements in set
 * k elements in subset
 * i index for counter
 */
/*lint -sem(counter_inc, 1p == 3n && 2n > 0 && 3n > 0 && 4n >= 0, @n > 0) */
static int counter_inc(int* counter, int n, int k, int i)
{
   int ret = 0;
   
   counter[i]++;

   if (counter[i] == n - i)
   {
      if (i == k - 1)
         return 1;
      
      ret = counter_inc(counter, n, k, i + 1);

      counter[i] = counter[i + 1] + 1;
   }
   return ret;
}

List* set_subsets_list(
   const Set* set,
   int        subset_size,
   List*      list,
   int*       idx)
{
   int*   counter;
   int    i;
   Set*   subset;
   Tuple* tuple;
   Entry* entry;
   Numb*  numb;
   List*  subset_list;
   
   assert(set_is_valid(set));
   assert(subset_size >= 0);
   assert(subset_size <= set->head.members);
   assert(idx         != NULL);

   if (subset_size == 0)
   {
      subset = set_empty_new(set_get_dim(set));
      numb   = numb_new_integer(*idx);
      *idx  += 1;
      tuple  = tuple_new(1);
      tuple_set_elem(tuple, 0, elem_new_numb(numb));
      entry  = entry_new_set(tuple, subset);
      list   = list_new_entry(entry);

      numb_free(numb);
      entry_free(entry);
      tuple_free(tuple);
      set_free(subset);

      return list;
   }
   
   counter = malloc(sizeof(*counter) * subset_size);

   assert(counter != NULL);
   
   for(i = 0; i < subset_size; i++)
      counter[i] = subset_size - 1 - i;

   do
   {
      subset_list = NULL;

      for(i = 0; i < subset_size; i++)
      {
         tuple = set_get_tuple(set, counter[i]);
         
         if (subset_list == NULL)
            subset_list = list_new_tuple(tuple);
         else
            list_add_tuple(subset_list, tuple);

         tuple_free(tuple);
      }
      assert(subset_list != NULL);
      
      subset = set_new_from_list(subset_list, SET_CHECK_NONE); /* NO_HASH ? */

      list_free(subset_list);
      
      numb   = numb_new_integer(*idx);
      *idx  += 1;
      tuple  = tuple_new(1);
      tuple_set_elem(tuple, 0, elem_new_numb(numb));
      entry  = entry_new_set(tuple, subset);

      if (list == NULL)
         list = list_new_entry(entry);
      else
         list_add_entry(list, entry);

      numb_free(numb);
      entry_free(entry);
      tuple_free(tuple);
      set_free(subset);
   }
   while(!counter_inc(counter, set->head.members, subset_size, 0));

   free(counter);

   assert(list != NULL);

   return list;
}

