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
#pragma ident "@(#) $Id: heur_feaspump.c,v 1.70 2010/09/27 17:20:21 bzfheinz Exp $"

/**@file   heur_feaspump.c
 * @ingroup PRIMALHEURISTICS
 * @brief  feasibility pump primal heuristic
 * @author Timo Berthold
 * @author Domenico Salvagnin
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include <assert.h>
#include <string.h>

#include "scip/heur_feaspump.h"
#include "scip/cons_linear.h"
#include "scip/scipdefplugins.h"

#define HEUR_NAME             "feaspump"
#define HEUR_DESC             "objective feasibility pump 2.0"
#define HEUR_DISPCHAR         'F'
#define HEUR_PRIORITY         -1000000
#define HEUR_FREQ             20
#define HEUR_FREQOFS          0
#define HEUR_MAXDEPTH         -1
#define HEUR_TIMING           SCIP_HEURTIMING_AFTERLPPLUNGE
#define HEUR_USESSUBSCIP      FALSE  /**< does the heuristic use a secondary SCIP instance? */

#define DEFAULT_MAXLPITERQUOT    0.01   /**< maximal fraction of diving LP iterations compared to node LP iterations */
#define DEFAULT_MAXLPITEROFS     1000   /**< additional number of allowed LP iterations */
#define DEFAULT_MAXSOLS             2   /**< total number of feasible solutions found up to which heuristic is called
                                         *   (-1: no limit) */
#define DEFAULT_MAXLOOPS        10000   /**< maximal number of pumping rounds (-1: no limit) */
#define DEFAULT_MAXSTALLLOOPS      10   /**< maximal number of pumping rounds without fractionality improvement (-1: no limit) */
#define DEFAULT_MINFLIPS           10   /**< minimum number of random variables to flip, if a 1-cycle is encountered */
#define DEFAULT_CYCLELENGTH         3   /**< maximum length of cycles to be checked explicitly in each round */
#define DEFAULT_PERTURBFREQ       100   /**< number of iterations until a random perturbation is forced */
#define DEFAULT_OBJFACTOR         1.0   /**< factor by which the regard of the objective is decreased in each round, 
                                         *   1.0 for dynamic, depending on solutions already found */
#define DEFAULT_ALPHADIFF         1.0   /**< threshold difference for the convex parameter to perform perturbation */
#define DEFAULT_BEFORECUTS       TRUE   /**< should the feasibility pump be called at root node before cut separation? */
#define DEFAULT_USEFP20         FALSE   /**< should an iterative round-and-propagate scheme be used to find the integral points? */
#define DEFAULT_PERTSOLFOUND     TRUE   /**< should a random perturbation be performed if a feasible solution was found? */
#define DEFAULT_STAGE3          FALSE   /**< should we solve a local branching sub-MIP if no solution could be found? */
#define DEFAULT_NEIGHBORHOODSIZE  18    /**< radius of the neighborhood to be searched in stage 3 */

#define MINLPITER                5000   /**< minimal number of LP iterations allowed in each LP solving call */



/** primal heuristic data */
struct SCIP_HeurData
{
   SCIP_SOL*             sol;                /**< working solution */
   SCIP_SOL*             roundedsol;         /**< rounded solution */ 
   SCIP_Longint          nlpiterations;      /**< number of LP iterations used in this heuristic */
   SCIP_Real             maxlpiterquot;      /**< maximal fraction of diving LP iterations compared to node LP iterations */
   SCIP_Real             objfactor;          /**< factor by which the regard of the objective is decreased in each round, 
                                              *   1.0 for dynamic, depending on solutions already found */
   SCIP_Real             alphadiff;          /**< threshold difference for the convex parameter to perform perturbation */

   int                   maxlpiterofs;       /**< additional number of allowed LP iterations */
   int                   maxsols;            /**< total number of feasible solutions found up to which heuristic is called
                                              *   (-1: no limit) */
   int                   maxloops;           /**< maximum number of loops (-1: no limit) */ 
   int                   maxstallloops;      /**< maximal number of pumping rounds without fractionality improvement (-1: no limit) */
   int                   minflips;           /**< minimum number of random variables to flip, if a 1-cycle is encountered */
   int                   cyclelength;        /**< maximum length of cycles to be checked explicitly in each round */
   int                   perturbfreq;        /**< number of iterations until a random perturbation is forced */
   int                   nsuccess;           /**< number of runs that produced at least one feasible solution */
   int                   neighborhoodsize;   /**< radius of the neighborhood to be searched in stage 3 */

   unsigned int          randseed;           /**< seed value for random number generator */
   SCIP_Bool             beforecuts;         /**< should the feasibility pump be called at root node before cut separation? */
   SCIP_Bool             usefp20;            /**< should an iterative round-and-propagate scheme be used to find the integral points? */
   SCIP_Bool             pertsolfound;       /**< should a random perturbation be performed if a feasible solution was found? */
   SCIP_Bool             stage3;             /**< should we solve a local branching sub-MIP if no solution could be found? */
};

/* copies SCIP to probing SCIP and creates variable hashmap */
static
SCIP_RETCODE setupProbingSCIP(
   SCIP*                 scip,                /**< SCIP data structure  */
   SCIP**                probingscip,         /**< subSCIP data structure  */
   SCIP_HASHMAP**        varmapfw,            /**< mapping of SCIP variables to subSCIP variables */  
   SCIP_Bool*            success              /**< was copying successful? */
   )
{
   /* initializing the subproblem */  
   SCIP_CALL( SCIPcreate(probingscip) );
      
   /* create the variable mapping hash map */
   SCIP_CALL( SCIPhashmapCreate(varmapfw, SCIPblkmem(*probingscip), SCIPgetNVars(scip)) );
   *success = FALSE;

   /* copy SCIP instance */
   SCIP_CALL( SCIPcopy(scip, *probingscip, *varmapfw, NULL, "feaspump", FALSE, FALSE, success) );

   return SCIP_OKAY;
}

/** checks whether a variable is one of the currently most fractional ones */
static
void insertFlipCand( 
   SCIP_VAR**            mostfracvars,       /**< sorted array of the currently most fractional variables */
   SCIP_Real*            mostfracvals,       /**< array of their fractionality, decreasingly sorted */
   int*                  nflipcands,         /**< number of fractional variables already labeled to be flipped*/
   int                   maxnflipcands,      /**< typically randomized number of maximum amount of variables to flip */
   SCIP_VAR*             var,                /**< variable to be checked */
   SCIP_Real             frac                /**< fractional value of the variable */
   )
{
   int i;

   assert(mostfracvars != NULL);
   assert(mostfracvals != NULL);
   assert(nflipcands != NULL);

   /* instead of the fractional value use the fractionality */
   if( frac > 0.5 )
      frac = 1 - frac;

   /* if there are already enough candidates and the variable is less fractional, return, else reserve the last entry */
   if( *nflipcands >= maxnflipcands )
   {
      if( frac <= mostfracvals[*nflipcands-1] )
         return;
      else
         (*nflipcands)--;
   }

   /* shifting var and frac through the (sorted) arrays */
   for( i = *nflipcands; i > 0 && mostfracvals[i-1] < frac; i-- )
   {
      mostfracvars[i] = mostfracvars[i-1];
      mostfracvals[i] = mostfracvals[i-1];
   }
   assert(0 <= i && i <= *nflipcands && *nflipcands < maxnflipcands);

   /* insert the variable and its fractionality */
   mostfracvars[i] = var;
   mostfracvals[i] = frac;
   
   /* we've found another candidate */
   (*nflipcands)++;
}

/** flips the roundings of the most fractional variables, if a 1-cycle was found */
static
SCIP_RETCODE handle1Cycle(
   SCIP*                 scip,               /**< SCIP data structure  */
   SCIP_HEURDATA*        heurdata,           /**< data of this special heuristic */
   SCIP_VAR**            mostfracvars,       /**< sorted array of the currently most fractional variables */
   int                   nflipcands,         /**< number of variables to flip */
   SCIP_Real             alpha               /**< factor how much the original objective is regarded */
   )
{
   SCIP_VAR* var;
   SCIP_Real solval;
   SCIP_Real frac;
   SCIP_Real newobjcoeff;  
   SCIP_Real orgobjcoeff;
   int       i;

   /* just flipping the objective coefficients from +1 to -1 and the rounding from floor to ceil */
   for( i = 0; i < nflipcands; i++ )
   {
      var = mostfracvars[i];
      solval = SCIPvarGetLPSol(var);   
      orgobjcoeff = SCIPvarGetObj(var);
      frac = SCIPfeasFrac(scip, solval);

      if( frac > 0.5 )
      {
         newobjcoeff = (1.0 - alpha) + alpha * orgobjcoeff;
         solval = SCIPfeasFloor(scip, solval);
      }         
      else
      {
         newobjcoeff = - (1.0 - alpha) + alpha * orgobjcoeff;
         solval = SCIPfeasCeil(scip, solval);
      }
      /* updating the rounded solution and the objective */
      SCIP_CALL( SCIPsetSolVal(scip, heurdata->roundedsol, var, solval) );
      SCIP_CALL( SCIPchgVarObjDive(scip, var, newobjcoeff) );
   }
   return SCIP_OKAY;
}

/** flips the roundings of randomly chosen fractional variables, preferring highly fractional ones, 
 *  if a longer cycle was found
 */
static
SCIP_RETCODE handleCycle(
   SCIP*                 scip,               /**< SCIP data structure  */
   SCIP_HEURDATA*        heurdata,           /**< data of this special heuristic */
   SCIP_VAR**            vars,               /**< array of all variables */
   int                   nbinandintvars,     /**< number of general integer and 0-1 variables */
   SCIP_Real             alpha               /**< factor how much the original objective is regarded */
   )
{
   SCIP_VAR* var;
   SCIP_Real solval;
   SCIP_Real frac;
   SCIP_Real newobjcoeff;  
   SCIP_Real orgobjcoeff;
   SCIP_Real flipprob;
   int i;

   /* just flipping the objective coefficients from +1 to -1 and the rounding from floor to ceil */
   for( i = 0; i < nbinandintvars; i++ )
   {
      /* decide arbitraryly whether the variable will be flipped or not */
      var = vars[i];
      solval = SCIPvarGetLPSol(var);   
      orgobjcoeff = SCIPvarGetObj(var);
      frac = SCIPfeasFrac(scip, solval);
      flipprob = -0.3 + SCIPgetRandomReal(0.0, 1.0, &heurdata->randseed);

      /* flip, iff the sum of the randomized number and the fractionality is big enough */
      if( MIN(frac, 1.0-frac) + MAX(flipprob, 0.0) > 0.5 )
      {
         if( frac > 0.5 )
         {
            newobjcoeff = (1.0 - alpha) + alpha * orgobjcoeff;
            solval = SCIPfeasFloor(scip, solval);
         }         
         else
         {
            newobjcoeff = - (1.0 - alpha) + alpha * orgobjcoeff;
            solval = SCIPfeasCeil(scip, solval);
         } 
         SCIP_CALL( SCIPsetSolVal(scip, heurdata->roundedsol, var, solval) );
         SCIP_CALL( SCIPchgVarObjDive(scip, var, newobjcoeff) );
      }
   }

   return SCIP_OKAY;
}

/** create the extra constraint of local branching and add it to subscip */
static
SCIP_RETCODE addLocalBranchingConstraint(
   SCIP*                 scip,               /**< SCIP data structure of the original problem     */
   SCIP*                 probingscip,        /**< SCIP data structure of the subproblem           */
   SCIP_HASHMAP*         varmapfw,           /**< mapping of SCIP variables to subSCIP variables */ 
   SCIP_SOL*             bestsol,            /**< SCIP solution                                   */
   SCIP_Real             neighborhoodsize    /**< rhs for LB constraint                           */ 
   )
{
   SCIP_CONS* cons;                        /* local branching constraint to create */  
   SCIP_VAR** consvars;
   SCIP_VAR** vars;

   int nbinvars;
   int i;   
   SCIP_Real lhs;
   SCIP_Real rhs;
   SCIP_Real* consvals;
   char consname[SCIP_MAXSTRLEN];

   (void) SCIPsnprintf(consname, SCIP_MAXSTRLEN, "%s_localbranchcons", SCIPgetProbName(scip));
   
   /* get vars data */
   SCIP_CALL( SCIPgetVarsData(scip, &vars, NULL, &nbinvars, NULL, NULL, NULL) );
   /* memory allociation */
   SCIP_CALL( SCIPallocBufferArray(scip, &consvars, nbinvars) );  
   SCIP_CALL( SCIPallocBufferArray(scip, &consvals, nbinvars) );
   
   /* set initial left and right hand sides of local branching constraint */
   lhs = 0.0;
   rhs = neighborhoodsize;

   /* create the distance (to incumbent) function of the binary variables */
   for( i = 0; i < nbinvars; i++ )
   {
      SCIP_Real solval;

      solval = SCIPgetSolVal(scip, bestsol, vars[i]);
      assert( SCIPisFeasIntegral(scip,solval) );

      /* is variable i part of the binary support of closest sol? */
      if( SCIPisFeasEQ(scip,solval,1.0) )
      {
         consvals[i] = -1.0;
         rhs -= 1.0;
         lhs -= 1.0;
      }
      else
         consvals[i] = 1.0;
      consvars[i] = (SCIP_VAR*) SCIPhashmapGetImage(varmapfw, vars[i]);
      SCIP_CALL( SCIPchgVarObj(probingscip, consvars[i], consvals[i]) );
      assert( SCIPvarGetType(consvars[i]) == SCIP_VARTYPE_BINARY );
   }
      
   /* creates localbranching constraint and adds it to subscip */
   SCIP_CALL( SCIPcreateConsLinear(probingscip, &cons, consname, nbinvars, consvars, consvals,
         lhs, rhs, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE) );
   SCIP_CALL( SCIPaddCons(probingscip, cons) );
   SCIP_CALL( SCIPreleaseCons(probingscip, &cons) );
   
   /* free local memory */
   SCIPfreeBufferArray(scip, &consvals);
   SCIPfreeBufferArray(scip, &consvars);

   return SCIP_OKAY;
}

/** creates a new solution for the original problem by copying the solution of the subproblem */
static
SCIP_RETCODE createNewSol(
   SCIP*                 scip,               /**< original SCIP data structure                        */
   SCIP*                 subscip,            /**< SCIP structure of the subproblem                    */
   SCIP_VAR**            subvars,            /**< the variables of the subproblem                     */
   SCIP_HEUR*            heur,               /**< RENS heuristic structure                            */
   SCIP_SOL*             subsol,             /**< solution of the subproblem                          */
   SCIP_Bool*            success             /**< used to store whether new solution was found or not */
   )
{
   SCIP_VAR** vars;                          /* the original problem's variables                */
   int        nvars;
   SCIP_Real* subsolvals;                    /* solution values of the subproblem               */
   SCIP_SOL*  newsol;                        /* solution to be created for the original problem */
        
   assert( scip != NULL );
   assert( subscip != NULL );
   assert( subvars != NULL );
   assert( subsol != NULL );

   /* get variables' data */
   SCIP_CALL( SCIPgetVarsData(scip, &vars, &nvars, NULL, NULL, NULL, NULL) );
   assert( nvars == SCIPgetNOrigVars(subscip) );  
 
   SCIP_CALL( SCIPallocBufferArray(scip, &subsolvals, nvars) );

   /* copy the solution */
   SCIP_CALL( SCIPgetSolVals(subscip, subsol, nvars, subvars, subsolvals) );
       
   /* create new solution for the original problem */
   SCIP_CALL( SCIPcreateSol(scip, &newsol, heur) );
   SCIP_CALL( SCIPsetSolVals(scip, newsol, nvars, vars, subsolvals) );

   /* try to add new solution to scip and free it immediately */
   SCIP_CALL( SCIPtrySolFree(scip, &newsol, FALSE, TRUE, TRUE, TRUE, success) );

   SCIPfreeBufferArray(scip, &subsolvals);

   return SCIP_OKAY;
}

/** copy method for primal heuristic plugins (called when SCIP copies plugins) */
static
SCIP_DECL_HEURCOPY(heurCopyFeaspump)
{  /*lint --e{715}*/
   assert(scip != NULL);
   assert(heur != NULL);
   assert(strcmp(SCIPheurGetName(heur), HEUR_NAME) == 0);

   /* call inclusion method of primal heuristic */
   SCIP_CALL( SCIPincludeHeurFeaspump(scip) );
 
   return SCIP_OKAY;
}

/** destructor of primal heuristic to free user data (called when SCIP is exiting) */
static
SCIP_DECL_HEURFREE(heurFreeFeaspump)
{   /*lint --e{715}*/
   SCIP_HEURDATA* heurdata;

   assert(heur != NULL);
   assert(strcmp(SCIPheurGetName(heur), HEUR_NAME) == 0);
   assert(scip != NULL);

   /* free heuristic data */
   heurdata = SCIPheurGetData(heur);
   assert(heurdata != NULL);
   SCIPfreeMemory(scip, &heurdata);
   SCIPheurSetData(heur, NULL);

   return SCIP_OKAY;
}


/** initialization method of primal heuristic (called after problem was transformed) */
static
SCIP_DECL_HEURINIT(heurInitFeaspump)
{  /*lint --e{715}*/
   SCIP_HEURDATA* heurdata;

   assert(heur != NULL);
   assert(strcmp(SCIPheurGetName(heur), HEUR_NAME) == 0);

   /* get heuristic data */
   heurdata = SCIPheurGetData(heur);
   assert(heurdata != NULL);

   /* create working solution */
   SCIP_CALL( SCIPcreateSol(scip, &heurdata->sol, heur) );
   SCIP_CALL( SCIPcreateSol(scip, &heurdata->roundedsol, heur) );

   /* initialize data */
   heurdata->nlpiterations = 0;
   heurdata->nsuccess = 0;
   heurdata->randseed = 0;

   return SCIP_OKAY;
}

/** deinitialization method of primal heuristic (called before transformed problem is freed) */
static
SCIP_DECL_HEUREXIT(heurExitFeaspump)
{  /*lint --e{715}*/
   SCIP_HEURDATA* heurdata;

   assert(heur != NULL);
   assert(strcmp(SCIPheurGetName(heur), HEUR_NAME) == 0);

   /* get heuristic data */
   heurdata = SCIPheurGetData(heur);
   assert(heurdata != NULL);

   /* free working solution */
   SCIP_CALL( SCIPfreeSol(scip, &heurdata->sol) );
   SCIP_CALL( SCIPfreeSol(scip, &heurdata->roundedsol) );

   return SCIP_OKAY;
}


/** solving process initialization method of primal heuristic (called when branch and bound process is about to begin) */
static
SCIP_DECL_HEURINITSOL(heurInitsolFeaspump)
{
   SCIP_HEURDATA* heurdata;

   heurdata = SCIPheurGetData(heur);
   assert(heurdata != NULL);

   /* if the heuristic is called at the root node, we may want to be called directly after the initial root LP solve */
   if( heurdata->beforecuts && SCIPheurGetFreqofs(heur) == 0 )
      SCIPheurSetTimingmask(heur, SCIP_HEURTIMING_DURINGLPLOOP);

   return SCIP_OKAY;
}


/** solving process deinitialization method of primal heuristic (called before branch and bound process data is freed) */
static
SCIP_DECL_HEUREXITSOL(heurExitsolFeaspump)
{
   /* reset the timing mask to its default value */
   SCIPheurSetTimingmask(heur, HEUR_TIMING);

   return SCIP_OKAY;
}


/** calculates an adjusted maximal number of LP iterations */
static
SCIP_Longint adjustedMaxNLPIterations(
   SCIP_Longint          maxnlpiterations,   /**< regular maximal number of LP iterations */
   SCIP_Longint          nsolsfound,         /**< total number of solutions found so far by SCIP */
   int                   nstallloops         /**< current number of stalling rounds */
   )
{
   if( nstallloops <= 1 )
   {
      if( nsolsfound == 0 )
         return 4*maxnlpiterations;
      else
         return 2*maxnlpiterations;
   }
   else
      return maxnlpiterations;
}

/** execution method of primal heuristic */
static
SCIP_DECL_HEUREXEC(heurExecFeaspump)
{  /*lint --e{715}*/
   SCIP_HEURDATA* heurdata; 
   SCIP_SOL* tmpsol;          /* only used for swapping */
   SCIP_SOL** lastroundedsols;/* solutions of the last pumping rounds (depending on heurdata->cyclelength) */
   SCIP_SOL* closestsol;      /* rounded solution closest to the LP relaxation: used for stage3 */
   SCIP_Real* lastalphas;     /* alpha values associated to solutions in lastroundedsols */

   SCIP* probingscip;         /* copied SCIP structure, used for round-and-propagate loop of feasibility pump 2.0 */
   SCIP_HASHMAP* varmapfw;    /* mapping of SCIP variables to subSCIP variables */    


   SCIP_VAR** vars;
   SCIP_VAR** pseudocands;
   SCIP_VAR** tmppseudocands;
   SCIP_VAR** mostfracvars;   /* the 30 most fractional variables, needed to avoid 1-cycles */
   SCIP_VAR* var;

   SCIP_Real* mostfracvals;   /* the values of the variables above */
   SCIP_Real newobjcoeff;     /* used for changing the objective */
   SCIP_Real orgobjcoeff;     /* used for regarding the original objective */
   SCIP_Real oldsolval;       /* one value of the last solution */ 
   SCIP_Real solval;          /* one value of the actual solution */ 
   SCIP_Real frac;            /* the fractional part of the value above */  
   SCIP_Real objfactor;       /* factor by which the regard of the objective is decreased in each round, in [0,0.99] */
   SCIP_Real alpha;           /* factor how the original objective is regarded, used for convex combination of two functions */
   SCIP_Real objnorm;         /* Euclidean norm of the objective function, used for scaling */
   SCIP_Real scalingfactor;   /* factor to scale the original objective function with */
   SCIP_Real mindistance;     /* distance of the closest rounded solution from the LP relaxation: used for stage3 */

   SCIP_Longint nlpiterations;    /* number of LP iterations done during one pumping round */
   SCIP_Longint maxnlpiterations; /* maximum number of LP iterations fpr this heuristic */
   SCIP_Longint nsolsfound;       /* number of solutions found by this heuristic */
   SCIP_Longint ncalls;           /* number of calls of this heuristic */  
   SCIP_Longint nbestsolsfound;   /* current total number of best solution updates in SCIP */

   SCIP_LPSOLSTAT lpsolstat;  /* status of the LP solution */

   int nvars;            /* number of variables  */
   int nbinvars;         /* number of 0-1-variables */
   int nintvars;         /* number of integer variables */
   int nfracs;           /* number of fractional variables updated after each pumping round*/
   int nflipcands;       /* how many flipcands (most frac. var.) have been found */
   int npseudocands;
   int maxnflipcands;    /* maximal number of candidates to flip in the current pumping round */
   int nloops;           /* how many pumping rounds have been made */
   int maxflips;         /* maximum number of flips, if a 1-cycle is found (depending on heurdata->minflips) */ 
   int maxloops;         /* maximum number of pumping rounds */
   int nstallloops;      /* number of loops without reducing the current best number of factional variables */
   int maxstallloops;    /* maximal number of allowed stalling loops */
   int bestnfracs;       /* best number of fractional variables */
   int i;
   int j;

   SCIP_Bool success;         
   SCIP_Bool lperror; 
   SCIP_Bool* cycles;           /* are there short cycles */

#ifdef NDEBUG
   SCIP_RETCODE retstat;
#endif
   
   assert(heur != NULL);
   assert(strcmp(SCIPheurGetName(heur), HEUR_NAME) == 0);
   assert(scip != NULL);
   assert(result != NULL);
   assert(SCIPhasCurrentNodeLP(scip));

   *result = SCIP_DELAYED;

   /* only call heuristic, if an optimal LP solution is at hand */
   if( SCIPgetLPSolstat(scip) != SCIP_LPSOLSTAT_OPTIMAL )
      return SCIP_OKAY;

   /* only call heuristic, if the LP solution is basic (which allows fast resolve in diving) */
   if( !SCIPisLPSolBasic(scip) )
      return SCIP_OKAY;

   /* don't dive two times at the same node */
   if( SCIPgetLastDivenode(scip) == SCIPgetNNodes(scip) && SCIPgetDepth(scip) > 0 )
      return SCIP_OKAY;

   *result = SCIP_DIDNOTRUN;

   /* only call feaspump once at the root */
   if( SCIPgetDepth(scip) == 0 && SCIPheurGetNCalls(heur) > 0 )
      return SCIP_OKAY;

   /* reset the timing mask to its default value (at the root node it could be different) */
   SCIPheurSetTimingmask(heur, HEUR_TIMING);

   /* only call the heuristic before cutting planes if we do not have an incumbent and no pricer exists */
   if( heurtiming == SCIP_HEURTIMING_DURINGLPLOOP && SCIPgetNSolsFound(scip) > 0 && SCIPgetNPricers(scip) == 0 )
      return SCIP_OKAY;

   /* get heuristic's data */
   heurdata = SCIPheurGetData(heur);
   assert(heurdata != NULL);

   /* only apply heuristic, if only a few solutions have been found and no pricer exists */
   if( heurdata->maxsols >= 0 && SCIPgetNSolsFound(scip) > heurdata->maxsols && SCIPgetNPricers(scip) == 0 )
      return SCIP_OKAY;

   /* get all variables of LP and number of fractional variables in LP solution that should be integral */
   SCIP_CALL( SCIPgetVarsData(scip, &vars, &nvars, &nbinvars, &nintvars, NULL, NULL) );
   nfracs = SCIPgetNLPBranchCands(scip);
   assert(0 <= nfracs && nfracs <= nbinvars + nintvars);
   if( nfracs == 0 )
      return SCIP_OKAY;

   /* calculate the maximal number of LP iterations until heuristic is aborted */
   nlpiterations = SCIPgetNLPIterations(scip);
   ncalls = SCIPheurGetNCalls(heur);
   nsolsfound = 10*SCIPheurGetNBestSolsFound(heur) + heurdata->nsuccess;
   maxnlpiterations = (SCIP_Longint)((1.0 + 10.0*(nsolsfound+1.0)/(ncalls+1.0)) * heurdata->maxlpiterquot * nlpiterations);
   maxnlpiterations += heurdata->maxlpiterofs;
  
   /* don't try to dive, if we took too many LP iterations during diving */
   if( heurdata->nlpiterations >= maxnlpiterations )
      return SCIP_OKAY;
   
   /* at the first root call, allow more iterations if there is no feasible solution yet */
   if( SCIPheurGetNCalls(heur) == 0 && SCIPgetNSolsFound(scip) == 0 && SCIPgetDepth(scip) == 0 )
      maxnlpiterations += nlpiterations;

   /* allow at least a certain number of LP iterations in this dive */
   maxnlpiterations = MAX(maxnlpiterations, heurdata->nlpiterations + MINLPITER);
   
   /* calculate maximal number of flips and loops */
   maxflips = 3*heurdata->minflips;
   maxloops = (heurdata->maxloops == -1 ? INT_MAX : heurdata->maxloops);
   maxstallloops = (heurdata->maxstallloops == -1 ? INT_MAX : heurdata->maxstallloops);
   
   SCIPdebugMessage("executing feasibility pump heuristic, nlpiters=%"SCIP_LONGINT_FORMAT", maxnlpit:%"SCIP_LONGINT_FORMAT", maxflips:%d \n",
      nlpiterations, maxnlpiterations, maxflips);

   *result = SCIP_DIDNOTFIND;

   /* memory allocation */
   SCIP_CALL( SCIPallocBufferArray(scip, &mostfracvars, maxflips) );
   SCIP_CALL( SCIPallocBufferArray(scip, &mostfracvals, maxflips) );
   SCIP_CALL( SCIPallocBufferArray(scip, &lastroundedsols, heurdata->cyclelength) );
   SCIP_CALL( SCIPallocBufferArray(scip, &lastalphas, heurdata->cyclelength) );
   SCIP_CALL( SCIPallocBufferArray(scip, &cycles, heurdata->cyclelength) );

   for( j = 0; j < heurdata->cyclelength; j++ )
   {
      SCIP_CALL( SCIPcreateSol(scip, &lastroundedsols[j], heur) ); 
   }

   closestsol = NULL;
   if( heurdata->stage3 )
   {
      SCIP_CALL( SCIPcreateSol(scip, &closestsol, heur) );
   }

   mindistance = SCIPinfinity(scip);
   
   probingscip = NULL;
   varmapfw = NULL;

   if( heurdata->usefp20 )
   {
      SCIP_CALL( setupProbingSCIP(scip, &probingscip, &varmapfw, &success) );
      
      if( success )
      {
         SCIP_CALL( SCIPsetIntParam(probingscip, "heuristics/feaspump2/freq", -1) );

         /* do not abort subproblem on CTRL-C */
         SCIP_CALL( SCIPsetBoolParam(probingscip, "misc/catchctrlc", FALSE) );

#ifndef SCIP_DEBUG
         /* disable output to console */
         SCIP_CALL( SCIPsetIntParam(probingscip, "display/verblevel", 0) );
#endif

         /* do presolve and initialize solving */
         SCIP_CALL( SCIPsetLongintParam(probingscip, "limits/nodes", 1LL) ); 
         SCIP_CALL( SCIPsetIntParam(probingscip, "lp/solvefreq", -1) );

         /* disable expensive presolving */
         SCIP_CALL( SCIPsetPresolving(probingscip, SCIP_PARAMSETTING_FAST, TRUE) );
         SCIP_CALL( SCIPsolve(probingscip) );
         SCIP_CALL( SCIPsetLongintParam(probingscip, "limits/nodes", 2LL) ); 
         
         /* set SCIP into probing mode and create root node of the probing tree */
         SCIP_CALL( SCIPstartProbing(probingscip) );
         SCIP_CALL( SCIPnewProbingNode(probingscip) );

         SCIPdebugMessage("successfully copied SCIP instance -> feasibility pump 2.0 can be used.\n");       
      }
   }

   /* start diving */
   SCIP_CALL( SCIPstartDive(scip) );
 
   /* lp was solved optimal */
   lperror = FALSE;
   lpsolstat = SCIP_LPSOLSTAT_OPTIMAL;

   /* pumping rounds */
   nsolsfound = SCIPgetNBestSolsFound(scip);
   if( heurdata->objfactor == 1.0 )
      objfactor = MIN(1.0 - 0.1 / (SCIP_Real)(1 + nsolsfound), 0.999);
   else  
      objfactor = heurdata->objfactor;

   /* scale distance function and original objective to the same norm */
   objnorm = SCIPgetObjNorm(scip);
   objnorm = MAX(objnorm, 1.0); 
   scalingfactor = SQRT((SCIP_Real)(nbinvars + nintvars)) / objnorm;

   /* data initialization */
   alpha = 1.0;
   nloops = 0;
   nstallloops = 0;
   nbestsolsfound = SCIPgetNBestSolsFound(scip);
   bestnfracs = INT_MAX;

   /* pumping loop */
   while( nfracs > 0
      && heurdata->nlpiterations < adjustedMaxNLPIterations(maxnlpiterations, nsolsfound, nstallloops)
      && nloops < maxloops && nstallloops < maxstallloops 
      && !SCIPisStopped(scip) )
   {
      int minimum;
      SCIP_Real* pseudocandsfrac;
      SCIP_Longint nlpiterationsleft;
      int iterlimit;

      /* decrease convex combination scalar */
      nloops++;
      alpha *= objfactor;

      SCIPdebugMessage("feasibility pump loop %d: %d fractional variables (alpha: %.4f, stall: %d/%d)\n", 
         nloops, nfracs, alpha, nstallloops, maxstallloops);

      success = FALSE;

      /* create solution from diving LP and try to round it */
      SCIP_CALL( SCIPlinkLPSol(scip, heurdata->sol) );
      SCIP_CALL( SCIProundSol(scip, heurdata->sol, &success) );  
         
      /* if the rounded solution is feasible and better, add it to SCIP */ 
      if( success )
      {
         SCIP_CALL( SCIPtrySol(scip, heurdata->sol, FALSE, FALSE, FALSE, FALSE, &success) );
         if( success )
            *result = SCIP_FOUNDSOL; 
      }
      
      SCIP_CALL( SCIPlinkLPSol(scip, heurdata->roundedsol) );

      /* randomly choose maximum number of variables to flip in current pumping round in case of a 1-cycle */
      maxnflipcands = SCIPgetRandomInt(MIN(nfracs/2+1, heurdata->minflips), MIN(nfracs, maxflips), &heurdata->randseed);
      nflipcands = 0;

      /* get all unfixed integer variables */
      SCIP_CALL( SCIPgetPseudoBranchCands(scip, &tmppseudocands, &npseudocands, NULL) );
      SCIP_CALL( SCIPduplicateBufferArray(scip, &pseudocands, tmppseudocands, npseudocands) );

      /* get array of all fractional variables and sort it w.r.t. their fractionalities */
      if( heurdata->usefp20 )
      {
         SCIP_CALL( SCIPallocBufferArray(scip, &pseudocandsfrac, npseudocands) );

         for( i = 0; i < npseudocands; i++ )
         {
            frac = SCIPfeasFrac(scip, SCIPvarGetLPSol(pseudocands[i]));
            pseudocandsfrac[i] = MIN(frac, 1.0-frac); /* always a number between 0 and 0.5 */
            if( SCIPvarGetType(pseudocands[i]) == SCIP_VARTYPE_BINARY )
               pseudocandsfrac[i] -= 10.0; /* binaries always come first */
         }
         SCIPsortRealPtr(pseudocandsfrac, (void*)pseudocands, npseudocands);
         SCIPfreeBufferArray(scip, &pseudocandsfrac);

         SCIPdebugMessage("iteratively fix and propagate variables\n");
      }

      for( i = 0; i < npseudocands; i++ )
      {
         SCIP_VAR* probingvar;
         SCIP_Bool infeasible;
         SCIP_Longint ndomreds;

         var = pseudocands[i];
         orgobjcoeff = SCIPvarGetObj(var);

         /* round the LP solution */
         solval = SCIPvarGetLPSol(var);            
         frac = SCIPfeasFrac(scip, solval);

         solval = SCIPfloor(scip, solval+0.5);

         /* ensure, that the fixing value is inside the local domains */
         if( heurdata->usefp20 )
         {
            SCIP_Real lb;
            SCIP_Real ub;

            probingvar = (SCIP_VAR*) SCIPhashmapGetImage(varmapfw, var);
            lb = SCIPvarGetLbLocal(probingvar);
            ub = SCIPvarGetUbLocal(probingvar);

            solval = MAX(solval, lb);
            solval = MIN(solval, ub);

            /* fix the variable and propagate the domain change */
            if( !SCIPisFeasEQ(probingscip, lb, ub) )
            {            
               assert(SCIPisFeasLE(probingscip, lb, ub));
               SCIP_CALL( SCIPnewProbingNode(probingscip) );
               
               SCIP_CALL( SCIPfixVarProbing(probingscip, probingvar, solval) );
               SCIPdebugMessage("try to fix variable <%s> (domain [%f,%f] to %f\n",SCIPvarGetName(probingvar), lb, ub,
                  solval);
               SCIP_CALL( SCIPpropagateProbing(probingscip, 3, &infeasible, &ndomreds) );
               SCIPdebugMessage("  -> reduced %"SCIP_LONGINT_FORMAT" domains\n", ndomreds);
               
               if( infeasible )
               {
                  SCIPdebugMessage("  -> infeasible!\n");
                  SCIP_CALL( SCIPbacktrackProbing(probingscip, SCIPgetProbingDepth(probingscip)-1) );
               }
            }
            else
            {
               SCIPdebugMessage("variable <%s> is already fixed to %f\n",SCIPvarGetName(probingvar), solval);
            }
         }            
         
         assert(SCIPisFeasLE(scip, SCIPvarGetLbLocal(var), solval) && SCIPisFeasLE(scip, solval, SCIPvarGetUbLocal(var))); 
         SCIP_CALL( SCIPsetSolVal(scip, heurdata->roundedsol, var, solval) );

         /* variables which are already integral, are treated separately */
         if( SCIPisFeasZero(scip, frac) )
         {
            SCIP_Real lb;
            SCIP_Real ub;
            
            /* variables at their bounds should be kept there */
            lb = SCIPvarGetLbLocal(var);
            ub = SCIPvarGetUbLocal(var);
            if( SCIPisFeasEQ(scip, solval, lb) )
               newobjcoeff = (1.0 - alpha)/scalingfactor + alpha * orgobjcoeff;
            else if( SCIPisFeasEQ(scip, solval, ub) )
               newobjcoeff = - (1.0 - alpha)/scalingfactor + alpha * orgobjcoeff;
            else
               newobjcoeff = alpha * orgobjcoeff;
         }
         else 
         {
            /* check whether the variable is one of the most fractionals and label if so */
            insertFlipCand(mostfracvars, mostfracvals, &nflipcands, maxnflipcands, var, frac);
               
            if( frac > 0.5 )
               newobjcoeff = - (1.0 - alpha)/scalingfactor + alpha * orgobjcoeff;
            else
               newobjcoeff = (1.0 - alpha)/scalingfactor + alpha * orgobjcoeff;
         }

         /* change one coefficient of the objective */
         SCIP_CALL( SCIPchgVarObjDive(scip, var, newobjcoeff) );                
         
      }

      if( heurdata->usefp20 )
      {          
         SCIP_CALL( SCIPbacktrackProbing(probingscip, 1) );
      }
      
      /* change objective coefficients for continuous variables */
      for( i = nbinvars+nintvars; i < nvars; i++ )
      {
         SCIP_CALL( SCIPchgVarObjDive(scip, vars[i], alpha *  SCIPvarGetObj(vars[i])) );         
      }

      SCIPfreeBufferArray(scip, &pseudocands);
      
      /* initialize cycle check */
      minimum = MIN(heurdata->cyclelength, nloops-1);
      for( j = 0; j < heurdata->cyclelength; j++ ) 
         cycles[j] = (nloops > j+1) && (REALABS(lastalphas[j] - alpha) < heurdata->alphadiff);
      
      /* check for j-cycles */
      for( i = 0; i < nbinvars+nintvars; i++ )
      {
         solval = SCIPgetSolVal(scip, heurdata->roundedsol, vars[i]);

         /* cycles exist, iff all solution values are equal */
         for( j = 0; j < minimum; j++ ) 
         {
            oldsolval = SCIPgetSolVal(scip, lastroundedsols[j], vars[i]);
            cycles[j] = cycles[j] && SCIPisFeasEQ(scip, solval, oldsolval);
         }
      }
  
      /* force to flip variables at random after a couple of pumping rounds, 
       * or if a new best solution in the current region has been found
       */
      assert(heurdata->perturbfreq > 0);
      if( nloops % heurdata->perturbfreq == 0 || (heurdata->pertsolfound && SCIPgetNBestSolsFound(scip) > nbestsolsfound) )
      {
         SCIPdebugMessage(" -> random perturbation\n");
         SCIP_CALL( handleCycle(scip, heurdata, vars, nintvars+nbinvars, alpha) );
         nbestsolsfound = SCIPgetNBestSolsFound(scip);
      }
      else 
      {
         minimum = MIN(heurdata->cyclelength, nloops-1);
         
         for( j = 0; j < minimum; j++ ) 
         {
            /* if we got the same rounded solution as in some step before, we have to flip some variables */
            if( cycles[j] )
            {
               /* 1-cycles have a special flipping rule (flip most fractional variables) */
               if( j == 0 )
               {
                  SCIPdebugMessage(" -> avoiding 1-cycle: flipping %d candidates\n", nflipcands);
                  SCIP_CALL( handle1Cycle(scip, heurdata, mostfracvars, nflipcands, alpha) );
               }
               else 
               {
                  SCIPdebugMessage(" -> avoiding %d-cycle by random flip\n", j+1);
                  SCIP_CALL( handleCycle(scip, heurdata, vars, nintvars+nbinvars, alpha) );
               }
               break;
            }
         }
      }
      
      /* the LP with the new (distance) objective is solved */
      nlpiterations = SCIPgetNLPIterations(scip);
      nlpiterationsleft = adjustedMaxNLPIterations(maxnlpiterations, nsolsfound, nstallloops) - heurdata->nlpiterations;
      iterlimit = MAX((int)nlpiterationsleft, MINLPITER);
      SCIPdebugMessage(" -> solve LP with iteration limit %d\n", iterlimit);

      /* Errors in the LP solver should not kill the overall solving process, if the LP is just needed for a heuristic.
       * Hence in optimized mode, the return code is catched and a warning is printed, only in debug mode, SCIP will stop.
       */
#ifdef NDEBUG
      retstat = SCIPsolveDiveLP(scip, iterlimit, &lperror);
      if( retstat != SCIP_OKAY )
      { 
         SCIPwarningMessage("Error while solving LP in Feaspump heuristic; LP solve terminated with code <%d>\n",retstat);
      }
#else
      SCIP_CALL( SCIPsolveDiveLP(scip, iterlimit, &lperror) );
#endif

      lpsolstat = SCIPgetLPSolstat(scip);

      /* update iteration count */
      heurdata->nlpiterations += SCIPgetNLPIterations(scip) - nlpiterations;
      SCIPdebugMessage(" -> number of iterations: %"SCIP_LONGINT_FORMAT"/%"SCIP_LONGINT_FORMAT", lperror=%u, lpsolstat=%d\n", 
         heurdata->nlpiterations, adjustedMaxNLPIterations(maxnlpiterations, nsolsfound, nstallloops), lperror, lpsolstat);

      /* check whether LP was solved optimal */
      if( lperror || lpsolstat != SCIP_LPSOLSTAT_OPTIMAL )
         break; 
      
      if( heurdata->stage3 )
      {
         SCIP_Real distance;        /* distance of the current rounded solution from the LP solution */

         assert(closestsol != NULL);

         /* calculate distance */
         distance = 0.0;
         for( i = 0; i < nbinvars+nintvars; i++ )
         {
            SCIP_Real roundedval;
            SCIP_Real lpval;

            roundedval = SCIPgetSolVal(scip, heurdata->roundedsol, vars[i]);
            lpval = SCIPvarGetLPSol(vars[i]);
            distance += REALABS(roundedval - lpval);
         }
      
         /* copy solution and update minimum distance */
         if( SCIPisLT(scip, distance, mindistance) )
         {
            for( i = 0; i < nbinvars+nintvars; i++ )
            {
               SCIP_CALL( SCIPsetSolVal(scip, closestsol, vars[i], SCIPgetSolVal(scip, heurdata->roundedsol, vars[i])) );
            }
            mindistance = distance;
         }
      }
      
      /* swap the last solutions */
      tmpsol = lastroundedsols[heurdata->cyclelength-1];
      for( j = heurdata->cyclelength-1; j > 0; j-- )
      {
         lastroundedsols[j] = lastroundedsols[j-1];
         lastalphas[j] = lastalphas[j-1];
      }
      lastroundedsols[0] = heurdata->roundedsol;
      lastalphas[0] = alpha;
      heurdata->roundedsol = tmpsol;
      
      /* check for improvement in number of fractionals */
      nfracs = SCIPgetNLPBranchCands(scip);
      if( nfracs < bestnfracs )
      {
         bestnfracs = nfracs;
         nstallloops = 0;
      }
      else
         nstallloops++;
      
      SCIPdebugMessage(" -> loop finished: %d fractional variables (stall: %d/%d, iterations: %"SCIP_LONGINT_FORMAT"/%"SCIP_LONGINT_FORMAT")\n", 
         nfracs, nstallloops, maxstallloops, heurdata->nlpiterations, adjustedMaxNLPIterations(maxnlpiterations, nsolsfound, nstallloops));
   }   
   /* try final solution, if no more fractional variables are left */
   if( nfracs == 0 && !lperror && lpsolstat == SCIP_LPSOLSTAT_OPTIMAL )
   {
      success = FALSE;
      
      SCIP_CALL( SCIPlinkLPSol(scip, heurdata->sol) );
      SCIP_CALL( SCIPtrySol(scip, heurdata->sol, FALSE, FALSE, FALSE, FALSE, &success) );
      if( success )
         *result = SCIP_FOUNDSOL;  
   }

   /* end diving */
   SCIP_CALL( SCIPendDive(scip) );

   /* end probing in order to be able to apply stage 3 */
   if( heurdata->usefp20 )
   {
      SCIP_CALL( SCIPendProbing(probingscip) ); 
   }
   
   /* only do stage 3 if we have not found a solution yet */
   /* only do stage 3 if the distance of the closest infeasible solution to the polyhedron is below a certain treshold */
   if( heurdata->stage3 && (*result != SCIP_FOUNDSOL) && SCIPisLE(scip, mindistance, (SCIP_Real) heurdata->neighborhoodsize) )
   {
      /* setup some parameters for the submip */
      SCIP_Real timelimit;
      SCIP_Real memorylimit;

      assert(closestsol != NULL);
      assert(!SCIPisInfinity(scip, mindistance) || nloops == 0);

      /* if we do not use feasibility pump 2.0, we have not created a copied SCIP instance yet */
      if( heurdata->usefp20 )
      {
         assert(probingscip != NULL);        
         SCIP_CALL( SCIPrestartSolve(probingscip) );
      }
      else
      {
         assert(probingscip == NULL);
         SCIP_CALL( setupProbingSCIP(scip, &probingscip, &varmapfw, &success) );
      }

      /* check whether there is enough time and memory left */
      SCIP_CALL( SCIPgetRealParam(scip, "limits/time", &timelimit) );
      if( !SCIPisInfinity(scip, timelimit) )
         timelimit -= SCIPgetSolvingTime(scip);
      SCIP_CALL( SCIPgetRealParam(scip, "limits/memory", &memorylimit) );
      if( !SCIPisInfinity(scip, memorylimit) )   
         memorylimit -= SCIPgetMemUsed(scip)/1048576.0;
      if( timelimit > 10.0 && memorylimit >= 0.0 )
      {
         /* do not abort subproblem on CTRL-C */
         SCIP_CALL( SCIPsetBoolParam(probingscip, "misc/catchctrlc", FALSE) );

#ifndef SCIP_DEBUG
         /* disable output to console */
         SCIP_CALL( SCIPsetIntParam(probingscip, "display/verblevel", 0) );
#endif
         /* set limits for the subproblem */
         SCIP_CALL( SCIPsetLongintParam(probingscip, "limits/nodes", 1000LL) ); 
         SCIP_CALL( SCIPsetLongintParam(probingscip, "limits/stallnodes", 100LL) ); 
         SCIP_CALL( SCIPsetRealParam(probingscip, "limits/time", timelimit) );
         SCIP_CALL( SCIPsetRealParam(probingscip, "limits/memory", memorylimit) );

         /* forbid recursive call of heuristics and separators solving subMIPs */
         SCIP_CALL( SCIPsetSubscipsOff(probingscip, TRUE) );

         /* disable heuristics which aim to feasibility instead of optimality */
         SCIP_CALL( SCIPsetIntParam(probingscip, "heuristics/feaspump/freq", -1) );
         SCIP_CALL( SCIPsetIntParam(probingscip, "heuristics/octane/freq", -1) );
         SCIP_CALL( SCIPsetIntParam(probingscip, "heuristics/objpscostdiving/freq", -1) );
         SCIP_CALL( SCIPsetIntParam(probingscip, "heuristics/rootsoldiving/freq", -1) );

         /* disable cutting plane separation */
         SCIP_CALL( SCIPsetSeparating(probingscip, SCIP_PARAMSETTING_OFF, TRUE) );

         /* disable expensive presolving */
         SCIP_CALL( SCIPsetPresolving(probingscip, SCIP_PARAMSETTING_FAST, TRUE) );
   
         /* use inference branching */
         SCIP_CALL( SCIPsetIntParam(probingscip, "branching/inference/priority", INT_MAX/4) );

         /* use best estimate node selection */
         SCIP_CALL( SCIPsetIntParam(probingscip, "nodeselection/estimate/stdpriority", INT_MAX/4) ); 

         /* disable conflict analysis */
         SCIP_CALL( SCIPsetBoolParam(probingscip, "conflict/useprop", FALSE) ); 
         SCIP_CALL( SCIPsetBoolParam(probingscip, "conflict/useinflp", FALSE) );
         SCIP_CALL( SCIPsetBoolParam(probingscip, "conflict/useboundlp", FALSE) );
         SCIP_CALL( SCIPsetBoolParam(probingscip, "conflict/usesb", FALSE) ); 
         SCIP_CALL( SCIPsetBoolParam(probingscip, "conflict/usepseudo", FALSE) );
               
         /* the neighborhood size is double the distance plus another ten percent */
         mindistance = SCIPceil(scip, 2.2*mindistance);
         
         SCIP_CALL( addLocalBranchingConstraint(scip, probingscip, varmapfw, closestsol, mindistance) );

         /* Errors in the LP solver should not kill the overall solving process, if the LP is just needed for a heuristic.
          * Hence in optimized mode, the return code is catched and a warning is printed, only in debug mode, SCIP will stop.
          */
#ifdef NDEBUG
         retstat = SCIPsolve(probingscip);
         if( retstat != SCIP_OKAY )
         { 
            SCIPwarningMessage("Error while solving subMIP in stage 3 of feasibility pump heuristic; subSCIP terminated with code <%d>\n",
               retstat);
         }
#else
         SCIP_CALL( SCIPsolve(probingscip) );
#endif
         /* check, whether a solution was found */
         if( SCIPgetNSols(probingscip) > 0 )
         {
            SCIP_SOL** subsols;
            SCIP_VAR** subvars;
            int nsubsols;

            /* for copying a solution we need an explicit mapping */
            SCIP_CALL( SCIPallocBufferArray(scip, &subvars, nvars) ); 
            for( i = 0; i < nvars; i++ )
               subvars[i] = (SCIP_VAR*) SCIPhashmapGetImage(varmapfw, vars[i]);
            
            nsubsols = SCIPgetNSols(probingscip);
            subsols = SCIPgetSols(probingscip);
            success = FALSE;

            /* check, whether a solution was found;
             * due to numerics, it might happen that not all solutions are feasible -> try all solutions until one was accepted
             */
            for( i = 0; i < nsubsols && !success; ++i )
            {
               SCIP_CALL( createNewSol(scip, probingscip, subvars, heur, subsols[i], &success) );
            }
            if( success )
               *result = SCIP_FOUNDSOL;

            SCIPfreeBufferArray(scip, &subvars);   
         }

      }
   }
   
   if( *result == SCIP_FOUNDSOL )
      heurdata->nsuccess++;

   /* free hash map and copied SCIP */
   if( varmapfw != NULL )      
      SCIPhashmapFree(&varmapfw);   

   if( probingscip != NULL )      
   {
      SCIP_CALL( SCIPfree(&probingscip) );
   }
   
   if( heurdata->stage3 )
   {
      SCIP_CALL( SCIPfreeSol(scip, &closestsol) );
   }
   
   /* free memory */
   for( j = 0; j < heurdata->cyclelength; j++ )
   {
      SCIP_CALL( SCIPfreeSol(scip, &lastroundedsols[j]) );
   }

   SCIPfreeBufferArray(scip, &cycles);
   SCIPfreeBufferArray(scip, &lastalphas);
   SCIPfreeBufferArray(scip, &lastroundedsols);
   SCIPfreeBufferArray(scip, &mostfracvals);
   SCIPfreeBufferArray(scip, &mostfracvars);

   SCIPdebugMessage("feasibility pump finished [%d iterations done].\n", nloops);

   return SCIP_OKAY;
}


/*
 * primal heuristic specific interface methods
 */

/** creates the feaspump primal heuristic and includes it in SCIP */
SCIP_RETCODE SCIPincludeHeurFeaspump(
   SCIP*                 scip                /**< SCIP data structure */
   )
{
   SCIP_HEURDATA* heurdata;

   /* create feaspump primal heuristic data */
   SCIP_CALL( SCIPallocMemory(scip, &heurdata) );

   /* include primal heuristic */
   SCIP_CALL( SCIPincludeHeur(scip, HEUR_NAME, HEUR_DESC, HEUR_DISPCHAR, HEUR_PRIORITY, HEUR_FREQ, HEUR_FREQOFS,
         HEUR_MAXDEPTH, HEUR_TIMING, HEUR_USESSUBSCIP,
         heurCopyFeaspump,
         heurFreeFeaspump, heurInitFeaspump, heurExitFeaspump, 
         heurInitsolFeaspump, heurExitsolFeaspump, heurExecFeaspump,
         heurdata) );

   /* add feaspump primal heuristic parameters */
   SCIP_CALL( SCIPaddRealParam(scip,
         "heuristics/feaspump/maxlpiterquot", 
         "maximal fraction of diving LP iterations compared to node LP iterations",
         &heurdata->maxlpiterquot, FALSE, DEFAULT_MAXLPITERQUOT, 0.0, SCIP_REAL_MAX, NULL, NULL) );
   SCIP_CALL( SCIPaddRealParam(scip,
         "heuristics/feaspump/objfactor", 
         "factor by which the regard of the objective is decreased in each round, 1.0 for dynamic",
         &heurdata->objfactor, FALSE, DEFAULT_OBJFACTOR, 0.0, 1.0, NULL, NULL) );
   SCIP_CALL( SCIPaddRealParam(scip,
         "heuristics/feaspump/alphadiff", 
         "threshold difference for the convex parameter to perform perturbation",
         &heurdata->alphadiff, FALSE, DEFAULT_ALPHADIFF, 0.0, 1.0, NULL, NULL) );

   SCIP_CALL( SCIPaddIntParam(scip,
         "heuristics/feaspump/maxlpiterofs", 
         "additional number of allowed LP iterations",
         &heurdata->maxlpiterofs, FALSE, DEFAULT_MAXLPITEROFS, 0, INT_MAX, NULL, NULL) );
   SCIP_CALL( SCIPaddIntParam(scip,
         "heuristics/feaspump/maxsols", 
         "total number of feasible solutions found up to which heuristic is called (-1: no limit)",
         &heurdata->maxsols, TRUE, DEFAULT_MAXSOLS, -1, INT_MAX, NULL, NULL) );
   SCIP_CALL( SCIPaddIntParam(scip,
         "heuristics/feaspump/maxloops",
         "maximal number of pumping loops (-1: no limit)",
         &heurdata->maxloops, TRUE, DEFAULT_MAXLOOPS, -1, INT_MAX, NULL, NULL) );
   SCIP_CALL( SCIPaddIntParam(scip,
         "heuristics/feaspump/maxstallloops",
         "maximal number of pumping rounds without fractionality improvement (-1: no limit)",
         &heurdata->maxstallloops, TRUE, DEFAULT_MAXSTALLLOOPS, -1, INT_MAX, NULL, NULL) );
   SCIP_CALL( SCIPaddIntParam(scip,
         "heuristics/feaspump/minflips", 
         "minimum number of random variables to flip, if a 1-cycle is encountered",
         &heurdata->minflips, TRUE, DEFAULT_MINFLIPS, 1, INT_MAX, NULL, NULL) );
   SCIP_CALL( SCIPaddIntParam(scip,
         "heuristics/feaspump/cyclelength", 
         "maximum length of cycles to be checked explicitly in each round",
         &heurdata->cyclelength, TRUE, DEFAULT_CYCLELENGTH, 1, 100, NULL, NULL) );
   SCIP_CALL( SCIPaddIntParam(scip,
         "heuristics/feaspump/perturbfreq", 
         "number of iterations until a random perturbation is forced",
         &heurdata->perturbfreq, TRUE, DEFAULT_PERTURBFREQ, 1, INT_MAX, NULL, NULL) );
   SCIP_CALL( SCIPaddIntParam(scip, "heuristics/feaspump/neighborhoodsize",
         "radius (using Manhattan metric) of the neighborhood to be searched in stage 3",
         &heurdata->neighborhoodsize, FALSE, DEFAULT_NEIGHBORHOODSIZE, 1, INT_MAX, NULL, NULL) );

   SCIP_CALL( SCIPaddBoolParam(scip,
         "heuristics/feaspump/beforecuts", 
         "should the feasibility pump be called at root node before cut separation?",
         &heurdata->beforecuts, FALSE, DEFAULT_BEFORECUTS, NULL, NULL) );
   SCIP_CALL( SCIPaddBoolParam(scip,
         "heuristics/feaspump2/usefp20", 
         "should an iterative round-and-propagate scheme be used to find the integral points?",
         &heurdata->usefp20, FALSE, DEFAULT_USEFP20, NULL, NULL) );
   SCIP_CALL( SCIPaddBoolParam(scip,
         "heuristics/feaspump2/pertsolfound", 
         "should a random perturbation be performed if a feasible solution was found?",
         &heurdata->pertsolfound, FALSE, DEFAULT_PERTSOLFOUND, NULL, NULL) );
   SCIP_CALL( SCIPaddBoolParam(scip,
         "heuristics/feaspump2/stage3", 
         "should we solve a local branching sub-MIP if no solution could be found?",
         &heurdata->stage3, FALSE, DEFAULT_STAGE3, NULL, NULL) );

   return SCIP_OKAY;
}
