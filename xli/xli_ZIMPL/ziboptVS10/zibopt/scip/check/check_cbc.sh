#!/usr/bin/env bash
#* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#*                                                                           *
#*                  This file is part of the program and library             *
#*         SCIP --- Solving Constraint Integer Programs                      *
#*                                                                           *
#*    Copyright (C) 2002-2010 Konrad-Zuse-Zentrum                            *
#*                            fuer Informationstechnik Berlin                *
#*                                                                           *
#*  SCIP is distributed under the terms of the ZIB Academic License.         *
#*                                                                           *
#*  You should have received a copy of the ZIB Academic License              *
#*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      *
#*                                                                           *
#* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

TSTNAME=$1
CBCBIN=$2
SETNAME=$3
BINNAME=$CBCBIN.$4
TIMELIMIT=$5
NODELIMIT=$6
MEMLIMIT=$7
THREADS=$8
FEASTOL=$9
MIPGAP=${10}
CONTINUE=${11}

if test ! -e results
then
    mkdir results
fi
if test ! -e settings
then
    mkdir settings
fi

OUTFILE=results/check.$TSTNAME.$BINNAME.$SETNAME.out
ERRFILE=results/check.$TSTNAME.$BINNAME.$SETNAME.err
RESFILE=results/check.$TSTNAME.$BINNAME.$SETNAME.res
TEXFILE=results/check.$TSTNAME.$BINNAME.$SETNAME.tex
TMPFILE=results/check.$TSTNAME.$BINNAME.$SETNAME.tmp
SETFILE=results/check.$TSTNAME.$BINNAME.$SETNAME.cmd

SETTINGS=settings/$SETNAME.cbcset

if test "$CONTINUE" = "true"
then
    MVORCP=cp
else
    MVORCP=mv
fi

DATEINT=`date +"%s"`
if test -e $OUTFILE
then
    $MVORCP $OUTFILE $OUTFILE.old-$DATEINT
fi
if test -e $ERRFILE
then
    $MVORCP $ERRFILE $ERRFILE.old-$DATEINT
fi

if test "$CONTINUE" = "true"
then
    LASTPROB=`./getlastprob.awk $OUTFILE`
    echo Continuing benchmark. Last solved instance: $LASTPROB
    echo "" >> $OUTFILE
    echo "----- Continuing from here. Last solved: $LASTPROB -----" >> $OUTFILE
    echo "" >> $OUTFILE
else
    LASTPROB=""
fi

uname -a >>$OUTFILE
uname -a >>$ERRFILE
date >>$OUTFILE
date >>$ERRFILE

# we add 10% to the hard time limit and additional 10 seconds in case of small time limits
HARDTIMELIMIT=`expr \`expr $TIMELIMIT + 10\` + \`expr $TIMELIMIT / 10\``

# since bash counts cpu time we need the time limit for each thread
HARDTIMELIMIT=`expr $HARDTIMELIMIT \* $THREADS`

# we add 10% to the hard memory limit and additional 100mb to the hard memory limit
HARDMEMLIMIT=`expr \`expr $MEMLIMIT + 100\` + \`expr $MEMLIMIT / 10\``
HARDMEMLIMIT=`expr $HARDMEMLIMIT \* 1024`

echo "hard time limit: $HARDTIMELIMIT s" >>$OUTFILE
echo "hard mem limit: $HARDMEMLIMIT k" >>$OUTFILE

for i in `cat $TSTNAME.test`
do
    if test "$LASTPROB" = ""
    then
        LASTPROB=""
        if test -f $i
        then
            rm -f $SETFILE
            echo @01 $i ===========
            echo @01 $i ===========                 >> $ERRFILE
            echo @05 SETTINGS: $SETNAME
            if test $SETNAME != "default"
            then
                cp $SETTINGS $TMPFILE
            else
                echo ""                              > $TMPFILE
            fi
            if test $FEASTOL != "default"
            then
                echo primalTolerance $FEASTOL       >> $TMPFILE
                echo integerTolerance $FEASTOL      >> $TMPFILE
            fi
#workaround: since CBC only looks at cpu-time, we multiply the timelimit with the number of threads
            TIMELIMIT=`expr $TIMELIMIT \* $THREADS`
            echo seconds $TIMELIMIT                 >> $TMPFILE
#$MEMLIMIT not supported (version 2.4)
            echo threads $THREADS                 >> $TMPFILE
            if test $MIPGAP != "default"
            then
                echo ratioGap $MIPGAP               >> $TMPFILE
            fi
            echo maxNodes $NODELIMIT                >> $TMPFILE
            echo import $i                          >> $TMPFILE
            echo ratioGap                           >> $TMPFILE
            echo allowableGap                       >> $TMPFILE
            echo seconds                            >> $TMPFILE
            echo stat                               >> $TMPFILE
            echo solve                              >> $TMPFILE
            echo quit                               >> $TMPFILE
            cp $TMPFILE $SETFILE
            echo -----------------------------
            date
            date >>$ERRFILE
            echo -----------------------------
            date +"@03 %s"
            bash -c "ulimit -t $HARDTIMELIMIT; ulimit -v $HARDMEMLIMIT; ulimit -f 1000000; $CBCBIN < $TMPFILE" 2>>$ERRFILE
            date +"@04 %s"
            echo -----------------------------
            date
            date >>$ERRFILE
            echo -----------------------------
            echo =ready=
        else
            echo @02 FILE NOT FOUND: $i ===========
            echo @02 FILE NOT FOUND: $i =========== >>$ERRFILE
        fi
    else
        echo skipping $i
        if test "$LASTPROB" = "$i"
        then
            LASTPROB=""
        fi
    fi
done | tee -a $OUTFILE

rm -f $TMPFILE

date >>$OUTFILE
date >>$ERRFILE

if test -f $TSTNAME.solu
then
    awk -f check_cbc.awk -vTEXFILE=$TEXFILE $TSTNAME.solu $OUTFILE | tee $RESFILE
else
    awk -f check_cbc.awk -vTEXFILE=$TEXFILE $OUTFILE | tee $RESFILE
fi
