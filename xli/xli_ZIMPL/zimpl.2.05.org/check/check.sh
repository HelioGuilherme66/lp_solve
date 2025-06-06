#!/bin/sh
# $Id: check.sh,v 1.13 2005/07/10 10:19:17 bzfkocht Exp $
#* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#*                                                                           */
#*   File....: check.sh                                                      */
#*   Name....: check script                                                  */
#*   Author..: Thorsten Koch                                                 */
#*   Copyright by Author, All rights reserved                                */
#*                                                                           */
#* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#*
#* Copyright (C) 2003 by Thorsten Koch <koch@zib.de>
#* 
#* This program is free software; you can redistribute it and/or
#* modify it under the terms of the GNU General Public License
#* as published by the Free Software Foundation; either version 2
#* of the License, or (at your option) any later version.
#* 
#* This program is distributed in the hope that it will be useful,
#* but WITHOUT ANY WARRANTY; without even the implied warranty of
#* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#* GNU General Public License for more details.
#* 
#* You should have received a copy of the GNU General Public License
#* along with this program; if not, write to the Free Software
#* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#*
# $1 = Binary
PASS=0
COUNT=0
for i in expr param set subto var bool define vinst
do
   COUNT=`expr $COUNT + 1` 
   $1 -v0 $i.zpl
   diff $i.lp $i.lp.ref >/dev/null
   case $? in
    0) echo Test $i "(lp)" OK; PASS=`expr $PASS + 1` ;;
    1) echo Test $i "(lp)" FAIL ;;
    *) echo Test $i "(lp)" ERROR ;;
   esac
   COUNT=`expr $COUNT + 1` 
   diff $i.tbl $i.tbl.ref >/dev/null
   case $? in
    0) echo Test $i "(tbl)" OK; PASS=`expr $PASS + 1`  ;;
    1) echo Test $i "(tbl)" FAIL ;;
    *) echo Test $i "(tbl)" ERROR ;;
   esac
   rm $i.tbl $i.lp
done 
for i in presol
do
   COUNT=`expr $COUNT + 1` 
   $1 -v0 -Distart=5 -O -t mps -r -m -n cm $i.zpl
   diff $i.mps $i.mps.ref >/dev/null
   case $? in
    0) echo Test $i "(mps)" OK; PASS=`expr $PASS + 1` ;;
    1) echo Test $i "(mps)" FAIL ;;
    *) echo Test $i "(mps)" ERROR ;;
   esac
   COUNT=`expr $COUNT + 1` 
   diff $i.tbl $i.tbl.ref >/dev/null
   case $? in
    0) echo Test $i "(tbl)" OK; PASS=`expr $PASS + 1`  ;;
    1) echo Test $i "(tbl)" FAIL ;;
    *) echo Test $i "(tbl)" ERROR ;;
   esac
   COUNT=`expr $COUNT + 1` 
   diff $i.mst $i.mst.ref >/dev/null
   case $? in
    0) echo Test $i "(mst)" OK; PASS=`expr $PASS + 1`  ;;
    1) echo Test $i "(mst)" FAIL ;;
    *) echo Test $i "(mst)" ERROR ;;
   esac
   COUNT=`expr $COUNT + 1` 
   diff $i.ord $i.ord.ref >/dev/null
   case $? in
    0) echo Test $i "(ord)" OK; PASS=`expr $PASS + 1`  ;;
    1) echo Test $i "(ord)" FAIL ;;
    *) echo Test $i "(ord)" ERROR ;;
   esac
   rm $i.tbl $i.mps $i.mst $i.ord
done 
#
   COUNT=`expr $COUNT + 1` 
   $1 -v0 -Distart=4 -t hum -n cf presol.zpl
   diff presol.hum presol.hum.ref >/dev/null
   case $? in
    0) echo Test presol.zpl "(hum)" OK; PASS=`expr $PASS + 1` ;;
    1) echo Test presol.zpl "(hum)" FAIL ;;
    *) echo Test presol.zpl "(hum)" ERROR ;;
   esac
   rm presol.hum
# 
#
   COUNT=`expr $COUNT + 1` 
   $1 -v0 print.zpl >print.out
   diff print.out print.out.ref >/dev/null
   case $? in
    0) echo Test print.zpl "(out)" OK; PASS=`expr $PASS + 1` ;;
    1) echo Test print.zpl "(out)" FAIL ;;
    *) echo Test print.zpl "(out)" ERROR ;;
   esac
   rm print.out print.tbl print.lp
# 
#
for i in sos
do
   COUNT=`expr $COUNT + 1` 
   $1 -v0 $i.zpl
   diff $i.sos $i.sos.ref >/dev/null
   case $? in
    0) echo Test $i "(sos)" OK; PASS=`expr $PASS + 1` ;;
    1) echo Test $i "(sos)" FAIL ;;
    *) echo Test $i "(sos)" ERROR ;;
   esac
   rm $i.tbl $i.lp $i.sos
done 
#
cd warnings
for i in w*.zpl
do
   COUNT=`expr $COUNT + 1` 
   NAME=`basename $i .zpl`
   ../$1 $i 2>$NAME.warn >/dev/null
   diff $NAME.warn $NAME.warn.ref >/dev/null
   case $? in
    0) echo Test $i "(warn)" OK; PASS=`expr $PASS + 1`  ;;
    1) echo Test $i "(warn)" FAIL ;;
    *) echo Test $i "(warn)" ERROR ;;
   esac
   rm -f $NAME.warn $NAME.tbl $NAME.lp $NAME.sos
done 2>/dev/null
cd ..
cd errors
#
for i in e[1-6]*.zpl
do
   COUNT=`expr $COUNT + 1` 
   NAME=`basename $i .zpl`
   ../$1 -v0 $i 2>$NAME.err
   diff $NAME.err $NAME.err.ref >/dev/null
   case $? in
    0) echo Test $i "(err)" OK; PASS=`expr $PASS + 1`  ;;
    1) echo Test $i "(err)" FAIL ;;
    *) echo Test $i "(err)" ERROR ;;
   esac
   rm $NAME.err
done 2>/dev/null
#
# Error 700 to 900 can vary
#
for i in e[789]*.zpl
do
#   COUNT=`expr $COUNT + 1` 
   NAME=`basename $i .zpl`
   # DIFFOPT=`awk -f ../exdiffopt.awk $NAME.err.ref`
   ../$1 -v0 $i 2>$NAME.err
   diff $NAME.err $NAME.err.ref >/dev/null
   case $? in
    0) echo Test $i "(err)" OK;; 
    1) echo Test $i "(err)" FAIL "(ignored)";;
    *) echo Test $i "(err)" ERROR ;;
   esac
   rm $NAME.err
done 2>/dev/null

if [ $PASS -eq $COUNT ] ; then echo All $PASS tests passed; 
else echo FAILURE! Only $PASS of $COUNT tests passed; 
fi







