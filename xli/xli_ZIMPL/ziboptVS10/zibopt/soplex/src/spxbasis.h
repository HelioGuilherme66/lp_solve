/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the class library                   */
/*       SoPlex --- the Sequential object-oriented simPlex.                  */
/*                                                                           */
/*    Copyright (C) 1996      Roland Wunderling                              */
/*                  1996-2010 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SoPlex is distributed under the terms of the ZIB Academic Licence.       */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SoPlex; see the file COPYING. If not email to soplex@zib.de.  */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: spxbasis.h,v 1.59 2010/09/16 17:45:03 bzfgleix Exp $"

/**@file  spxbasis.h
 * @brief Simplex basis.
 */
#ifndef _SPXBASIS_H_
#define _SPXBASIS_H_

#include <assert.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#include "spxdefines.h"
#include "spxlp.h"
#include "svector.h"
#include "ssvector.h"
#include "dataarray.h"
#include "slinsolver.h"
#include "nameset.h"
#include "spxout.h"

namespace soplex
{
class SPxSolver;

/**@class SPxBasis
   @brief   Simplex basis.
   @ingroup Algo

   Consider the linear program as provided from class SPxLP:
   \f[
   \begin{array}{rl}
     \hbox{max}  & c^T x                 \\
     \hbox{s.t.} & l_r \le Ax \le u_r    \\
                 & l_c \le  x \le u_c
     \end{array}
   \f]
   where \f$c, l_c, u_c, x \in {\bf R}^n\f$, \f$l_r, u_r \in {\bf R}^m\f$ and
   \f$A \in {\bf R}^{m \times n}\f$. Solving this LP with the simplex algorithm
   requires the definition of a \em basis. Such can be defined as a set of
   column vectors or a set of row vectors building a nonsingular matrix. We
   will refer to the first case as the \em columnwise \em representation and
   the latter case will be called the \em rowwise \em representation. In both
   cases, a \em basis is a set of vectors forming a nonsigular matrix. The
   dimension of the vectors is refered to as the basis' \em dimension,
   whereas the number of vectors belonging to the LP is called the basis'
   \em codimension.
   
   Class SPxBasis is designed to represent a generic simplex basis, suitable
   for both representations. At any time the representation can be changed by
   calling method setRep().
   
   Class SPxBasis provides methods for solving linear systems with the basis
   matrix. However, SPxBasis does not provide a linear solver by its own.
   Instead, a SLinSolver object must be #load%ed to a SPxBasis which will
   be called for solving linear systems.
*/
class SPxBasis
{
public:

   /// basis status.
   /** Each SPxBasis is assigned a status flag, which can take on of the
       above values.
   */
   enum SPxStatus
   {
      NO_PROBLEM = -2,  ///< No Problem has been loaded to the basis.
      SINGULAR   = -1,  ///< Basis is singular.
      REGULAR    = 0,   ///< Basis is not known to be dual nor primal feasible.
      DUAL       = 1,   ///< Basis is dual feasible.
      PRIMAL     = 2,   ///< Basis is primal feasible.
      OPTIMAL    = 3,   ///< Basis is optimal, i.e. dual and primal feasible.
      UNBOUNDED  = 4,   ///< LP has been proven to be primal unbounded.
      INFEASIBLE = 5    ///< LP has been proven to be primal infeasible.
   };


   /// Basis descriptor.
   class Desc
   {
   public:

      //------------------------------------
      //**@name Status */
      //@{
      /// Status of a variable.
      /** A basis is described by assigning a Status to all of the LP
          variables and covariables. This assignment is maintained by the
          basis #Desc%riptor.
          
          Variables and covariables may have a primal or dual Status. The
          first type specifies that a variable is set on a primal bound, while
          the latter type indicates a dual variable to be set on a bound.
          If a row variable has a primal status, say #P_ON_UPPER, this means
          that the upper bound of the inequality is set to be tight. Hence,
          in this case the upper bound must not be infinity.

          Equivalently, if the status of a variable is dual, say #D_ON_UPPER,
          it means that the dual variable corresponding to the upper bound
          inequality of this variable is set to 0.

          For a column basis, primal #Status%es correspond to nonbasic
          variables, while dual ones are basic. This is reversed for a row
          basis. We will now reveal in more detail the significance of
          variable #Status%es.

          <b>Primal Variables</b>

          Consider a range inequality \f$l_r \le a^T x \le u_r\f$ or bounds on
          a variable \f$l_c \le x_c \le u_c\f$. The following table reveals
          what is implied if the corresponding variable or covariable is
          assigned to a primal #Status:

          \f[
          \begin{array}{lcl}
 l_c \le x_c \le u_c   & \mbox{Status}(x_i)  & l_r \le a^T x \le u_r \\
 \hline
 x_c = u_c < \infty    & \mbox{P\_ON\_UPPER} & a^T x = u_r < \infty  \\
 x_c = l_c > -\infty   & \mbox{P\_ON\_LOWER} & a^T x = l_r > -\infty \\
 -\infty < l_c = x_c = u_c < \infty
                       & \mbox{P\_FIXED}     &
                               -\infty < l_r = a^T x = u_r < \infty  \\
 -\infty = l_i < x_i=0 < u_i = \infty
                       & \mbox{P\_FREE}      &
                           -\infty = l_r < a^T x = 0 < u_r = \infty  \\
          \end{array}
          \f]

          Note that to determine whether a variable with #Status stat is set to
          its upper bound, one can compute the test (-stat | -#P_ON_UPPER).
          This will yield true even if the variable is fixed, i.e., sitting on
          both bounds at the same time.

          <b>Dual Variables</b>

          In principle for implementing the Simplex algorithm it would suffice
          to use only one dual #Status. However, for performance reasons it
          is advisable to introduce various dual status types, reflecting
          the structure of the bounds. Given an upper bound \f$u\f$ and a lower
          bound \f$l\f$ of a constraint or variable, the following table 
          indicates the setting of the dual Status of this variable.

          \f[
          \begin{array}{cl}
              l \le ... \le u               & \mbox{Status}        \\
          \hline
              -\infty < l \ne u < \infty    & \mbox{D\_ON\_BOTH}   \\
              -\infty < l \ne u = \infty    & \mbox{D\_ON\_UPPER}  \\
              -\infty = l \ne u < \infty    & \mbox{D\_ON\_LOWER}  \\
              -\infty < l  =  u < \infty    & \mbox{D\_FREE}       \\
              -\infty = l \ne u = \infty    & \mbox{D\_UNDEFINED}  \\
          \end{array}
          \f]

          Note that unbounded primal variables are reflected by an #D_UNDEFINED
          dual variable, since no dual variables exist for them. To facilitate
          the assignment of dual #Status%es, class SPxBasis provides methods
          #dualStatus(), #dualColStatus() and #dualRowStatus)().
      */
      enum Status
      {
         P_ON_LOWER  = -4,  ///< primal variable is set to its lower bound
         P_ON_UPPER  = -2,  ///< primal variable is set to its upper bound
         P_FREE      = -1,  ///< primal variable is left free, but unset
         P_FIXED     = P_ON_UPPER + P_ON_LOWER,  ///< primal variable is fixed to both bounds
         D_FREE      = 1,   ///< dual variable is left free, but unset
         D_ON_UPPER  = 2,   ///< dual variable is set to its upper bound
         D_ON_LOWER  = 4,   ///< dual variable is set to its lower bound
         D_ON_BOTH   = D_ON_LOWER + D_ON_UPPER,  ///< dual variable has two bounds
         D_UNDEFINED = 8    ///< primal or dual variable has no status
      };
      //@}

      friend class SPxBasis;
      friend std::ostream& operator<<(std::ostream& os, const Status& stat);

private:

      //------------------------------------
      //**@name Data */
      //@{
      DataArray < Status > rowstat;   ///< status of rows.
      DataArray < Status > colstat;   ///< status of columns.
      DataArray < Status > * stat;    ///< basis' status.
      DataArray < Status > * costat;  ///< cobasis' status.
      //@}

public:

      //------------------------------------
      //**@name Access / modification */
      //@{
      /// returns number of columns.
      int nCols() const
      {
         return colstat.size();
      }
      /// returns number of rows.
      int nRows() const
      {
         return rowstat.size();
      }
      /// returns dimension.
      int dim() const
      {
         return stat->size();
      }
      /// returns codimension.
      int coDim() const
      {
         return costat->size();
      }
      ///
      Status& rowStatus(int i)
      {
         return rowstat[i];
      }
      /// returns status of row \p i.
      Status rowStatus(int i) const
      {
         return rowstat[i];
      }
      /// returns the array of row  \ref soplex::SPxBasis::Desc::Status "Status"es.
      const Status* rowStatus(void) const
      {
         return rowstat.get_const_ptr();
      }
      ///
      Status& colStatus(int i)
      {
         return colstat[i];
      }
      /// returns status of column \p i.
      Status colStatus(int i) const
      {
         return colstat[i];
      }
      /// returns the array of column \ref soplex::SPxBasis::Desc::Status "Status"es.
      const Status* colStatus(void) const
      {
         return colstat.get_const_ptr();
      }
      ///
      Status& status(int i)
      {
         return (*stat)[i];
      }
      /// returns status of variable \p i.
      Status status(int i) const
      {
         return (*stat)[i];
      }
      /// returns the array of variable \ref soplex::SPxBasis::Desc::Status "Status"es.
      const Status* status(void) const
      {
         return stat->get_const_ptr();
      }
      ///
      Status& coStatus(int i)
      {
         return (*costat)[i];
      }
      /// returns status of covariable \p i.
      Status coStatus(int i) const
      {
         return (*costat)[i];
      }
      /// returns the array of covariable \ref soplex::SPxBasis::Desc::Status "Status"es.
      const Status* coStatus(void) const
      {
         return costat->get_const_ptr();
      }
      /// resets dimensions.
      void reSize(int rowDim, int colDim);
      //@}

      //------------------------------------
      //**@name Debugging */
      //@{
      /// Prints out status.
      void dump() const;
#ifndef NO_CONSISTENCY_CHECKS
      /// consistency check.
      bool isConsistent() const;
#endif
      //@}

      //------------------------------------
      //**@name Construction / destruction */
      //@{
      /// default constructor
      Desc()
         : stat(0)
         , costat(0)
      {}
      explicit Desc(const SPxSolver& base);

      /// copy constructor
      Desc(const Desc& old);
      /// assignment operator
      Desc& operator=(const Desc& rhs);
      //@}
   };

protected:

   //------------------------------------
   //**@name Protected data 
   /**
      For storing the basis matrix we keep two arrays: Array #theBaseId
      contains the SPxId%s of the basis vectors, and #matrix the pointers to
      the vectors themselfes. Method #loadMatrixVecs() serves for loading
      #matrix according to the SPxId%s stored in #theBaseId. This method must
      be called whenever the vector pointers may have
      changed due to manipulations of the LP.
   */
   //@{
   /// the LP
   SPxSolver* theLP;
   /// SPxId%s of basic vectors.
   DataArray < SPxId > theBaseId;
   /// pointers to the vectors of the basis matrix.
   DataArray < const SVector* > matrix;
   /// \c true iff the pointers in \ref soplex::SPxBasis::matrix "matrix" are set up correctly.
   bool matrixIsSetup;

   /* @brief LU factorization of basis matrix
      The factorization of the matrix is stored in #factor if #factorized != 0.
      Otherwise #factor is undefined.
   */
   SLinSolver* factor;
   /// \c true iff \ref soplex::SPxBasis::factor "factor" = \ref soplex::SPxBasis::matrix "matrix" \f$^{-1}\f$.
   bool factorized;

   /// number of updates before refactorization.
   /** When a vector of the basis matrix is exchanged by a call to method
       #change(), the LU factorization of the matrix is updated
       accordingly. However, after atmost #maxUpdates updates of the
       factorization, it is recomputed in order to regain numerical
       stability and reduce fill in.
   */
   int   maxUpdates;

   /// allowed increase of nonzeros before refactorization.
   /** When the number of nonzeros in LU factorization exceeds
       #nonzeroFactor times the number of nonzeros in B, the
       basis matrix is refactorized.
   */
   Real   nonzeroFactor;

   /// allowed increase in realtive fill before refactorization
   /** When the real relative fill is bigger than fillFactor times lastFill
    *  the Basis will be refactorized.
    */ 
   Real   fillFactor;
   /* Rank-1-updates to the basis may be performed via method #change(). In
      this case, the factorization is updated, and the following members are
      reset.
   */
   int    iterCount;     ///< number of calls to change() since last manipulation
   int    updateCount;   ///< number of calls to change() since last factorize()
   int    nzCount;       ///< number of nonzeros in basis matrix
   int    lastMem;       ///< memory needed after last fresh factorization
   Real   lastFill;      ///< fill ratio that occured during last factorization
   int    lastNzCount;   ///< number of nonzeros in basis matrix after last fresh factorization

   SPxId  lastin;        ///< lastEntered(): variable entered the base last
   SPxId  lastout;       ///< lastLeft(): variable left the base last
   int    lastidx;       ///< lastIndex(): basis index where last update was done
   Real   minStab;       ///< minimum stability
   //@}

private:

   //------------------------------------
   //**@name Private data */
   //@{
   SPxStatus thestatus;      ///< current status of the basis.
   Desc      thedesc;        ///< the basis' Descriptor
   bool      freeSlinSolver; ///< true iff factor should be freed inside of this object
   //@}

public:

   //------------------------------------------------
   /**@name Status and Descriptor related Methods */
   //@{
   /// returns current SPxStatus.
   SPxStatus status() const
   {
      return thestatus;
   }

   /// sets basis SPxStatus to \p stat.
   void setStatus(SPxStatus stat)
   {
      METHOD( "SPxBasis::setStatus()" );
      MSG_DEBUG( spxout << "DBSTAT01 SPxBasis::setStatus(): status: "
                 << int(thestatus) << " (" << thestatus << ") -> "
                 << int(stat) << " (" << stat << ")" << std::endl; )
      thestatus = stat;
      if( stat == NO_PROBLEM )
         invalidate();
   }

   ///
   const Desc& desc() const
   {
      return thedesc;
   }
   /// returns current basis Descriptor.
   Desc& desc()
   {
      return thedesc;
   }

   /// dual Status for the \p i'th column variable of the loaded LP.
   Desc::Status dualColStatus(int i) const;

   /// dual Status for the column variable with ID \p id of the loaded LP.
   Desc::Status dualStatus(const SPxColId& id) const;

   /// dual Status for the \p i'th row variable of the loaded LP.
   Desc::Status dualRowStatus(int i) const;

   /// dual Status for the row variable with ID \p id of the loaded LP.
   Desc::Status dualStatus(const SPxRowId& id) const;

   /// dual Status for the variable with ID \p id of the loaded LP.
   /** It is automatically detected, whether the \p id is one of a 
       row or a column variable, and the correct row or column status
       is returned.
   */
   Desc::Status dualStatus(const SPxId& id) const
   {
      return id.isSPxRowId()
         ? dualStatus(SPxRowId(id))
         : dualStatus(SPxColId(id));
   }
   //@}


   //-----------------------------------
   /**@name Inquiry Methods */
   //@{
   /// 
   inline SPxId& baseId(int i)
   {
      return theBaseId[i];
   }
   /// returns the Id of the \p i'th basis vector.
   inline SPxId baseId(int i) const
   {
      return theBaseId[i];
   }

   /// returns the \p i'th basic vector.
   const SVector& baseVec(int i) const
   {
      assert( matrixIsSetup );
      return *matrix[i];
   }

   /// returns SPxId of last vector included to the basis.
   inline SPxId lastEntered() const
   {
      return lastin;
   }

   /// returns SPxId of last vector that left the basis.
   inline SPxId lastLeft() const
   {
      return lastout;
   }

   /// returns index in basis where last update was done.
   inline int lastIndex() const
   {
      return lastidx;
   }

   /// returns number of basis changes since last refactorization.
   inline int lastUpdate() const
   {
      return updateCount;
   }

   /// returns number of basis changes since last \ref soplex::SPxBasis::load() "load()".
   inline int iteration() const
   {
      return iterCount;
   }
   /// returns loaded solver.
   inline SPxSolver* solver() const
   {
      return theLP;
   }
   //@}

   //-----------------------------------
   /**@name Linear Algebra */
   //@{
   /// Basis-vector product.
   /** Depending on the representation, for an SPxBasis B,
       B.multBaseWith(x) computes
       - \f$x \leftarrow Bx\f$    in the columnwise case, and
       - \f$x \leftarrow x^TB\f$  in the rowwise case.

       Both can be seen uniformly as multiplying the basis matrix \p B with
       a vector \p x aligned the same way as the \em vectors of \p B.
    */
   Vector& multBaseWith(Vector& x) const;

   /// Vector-basis product.
   /** Depending on the representation, for a #SPxBasis B,
       B.multWithBase(x) computes
       - \f$x \leftarrow x^TB\f$  in the columnwise case and
       - \f$x \leftarrow Bx\f$    in the rowwise case.

       Both can be seen uniformly as multiplying the basis matrix \p B with
       a vector \p x aligned the same way as the \em covectors of \p B.
    */
   Vector& multWithBase(Vector& x) const;

   /// returns the stability of the basis matrix.
   Real stability() const
   {
      return factor->stability();
   }
   ///
   void solve(Vector& x, const Vector& rhs)
   {
      METHOD( "SPxBasis::solve()" );
      if (!factorized) 
         factorize();
      factor->solveRight(x, rhs);
   }
   ///
   void solve(SSVector& x, const SVector& rhs)
   {
      METHOD( "SPxBasis::solve()" );
      if (!factorized) 
         factorize();
      factor->solveRight(x, rhs);
   }
   /// solves linear system with basis matrix.
   /** Depending on the representation, for a SPxBasis B,
       B.solve(x) computes
       - \f$x \leftarrow B^{-1}rhs\f$       in the columnwise case and
       - \f$x \leftarrow rhs^TB^{-1}\f$     in the rowwise case.

       Both can be seen uniformly as solving a linear system with the basis
       matrix \p B and a right handside vector \p x aligned the same way as
       the \em vectors of \p B.
    */
   void solve4update(SSVector& x, const SVector& rhs)
   {
      METHOD( "SPxBasis::solve4update()" );
      if (!factorized) 
         factorize();
      factor->solveRight4update(x, rhs);
   }
   /// solves two systems in one call.
   void solve4update(SSVector& x, Vector& y, const SVector& rhsx, SSVector& rhsy)
   {
      METHOD( "SPxBasis::solve4update()" );
      if (!factorized) 
         factorize();
      factor->solve2right4update(x, y, rhsx, rhsy);
   }

   /// Cosolves linear system with basis matrix.
   /** Depending on the representation, for a SPxBasis B,
       B.coSolve(x) computes
       - \f$x \leftarrow rhs^TB^{-1}\f$     in the columnwise case and
       - \f$x \leftarrow B^{-1}rhs\f$       in the rowwise case.

       Both can be seen uniformly as solving a linear system with the basis
       matrix \p B and a right handside vector \p x aligned the same way as
       the \em covectors of \p B.
    */
   void coSolve(Vector& x, const Vector& rhs)
   {
      METHOD( "SPxBasis::coSolve()" );
      if (!factorized) 
         factorize();
      factor->solveLeft(x, rhs);
   }
   ///
   void coSolve(SSVector& x, const SVector& rhs)
   {
      METHOD( "SPxBasis::coSolve()" );
      if (!factorized) 
         factorize();
      factor->solveLeft(x, rhs);
   }
   /// solves two systems in one call.
   void coSolve(SSVector& x, Vector& y, const SVector& rhsx, SSVector& rhsy)
   {
      METHOD( "SPxBasis::coSolve()" );
      if (!factorized) 
         factorize();
      factor->solveLeft(x, y, rhsx, rhsy);
   }
   //@}


   //------------------------------------
   /**@name Modification notification.
       These methods must be called after the loaded LP has been modified.
    */
   //@{
   /// inform SPxBasis, that \p n new rows had been added.
   void addedRows(int n);
   /// inform SPxBasis that row \p i had been removed.
   void removedRow(int i);
   /// inform SPxBasis that rows in \p perm with negative entry were removed.
   void removedRows(const int perm[]);
   /// inform SPxBasis that \p n new columns had been added.
   void addedCols(int n);
   /// inform SPxBasis that column \p i had been removed.
   void removedCol(int i);
   /// inform SPxBasis that columns in \p perm with negative entry were removed.
   void removedCols(const int perm[]);
   /// inform SPxBasis that a row had been changed.
   void changedRow(int);
   /// inform SPxBasis that a column had been changed.
   void changedCol(int);
   /// inform SPxBasis that a matrix entry had been changed.
   void changedElement(int, int);
   //@}


   //--------------------------------
   /**@name Miscellaneous */
   //@{
   /// performs basis update.
   /** Changes the \p i 'th vector of the basis with the vector associated to
       \p id. This includes:
       - updating the factorization, or recomputing it from scratch by
         calling   \ref soplex::SPxSolver::factorize()   "factorize()",
       - resetting \ref soplex::SPxSolver::lastEntered() "lastEntered()",
       - resetting \ref soplex::SPxSolver::lastIndex()   "lastIndex()",
       - resetting \ref soplex::SPxSolver::lastLeft()    "lastLeft()",
       - resetting \ref soplex::SPxSolver::lastUpdate()  "lastUpdate()",
       - resetting \ref soplex::SPxSolver::iterations()  "iterations()".

       The basis descriptor is \em not \em modified, since #factor()
       cannot know about how to set up the status of the involved variables
       correctly.

       A vector \p enterVec may be passed for a fast ETA update of the LU
       factorization associated to the basis. It must be initialized with
       the solution vector \f$x\f$ of the right linear system \f$Bx = b\f$
       with the entering vector as right-hand side vector \f$b\f$, where \f$B\f$
       denotes the basis matrix. This can be computed using method #solve().
       When using FAST updates, a vector \p eta may be passed for
       improved performance. It must be initialized by a call to
       factor->solveRightUpdate() as described in SLinSolver. The 
       implementation hidden behind FAST updates depends on the
       SLinSolver implementation class.
    */
   virtual void change(int i, SPxId& id,
      const SVector* enterVec, const SSVector* eta = 0);

   /** Load basis from \p in in MPS format. If \p rowNames and \p colNames 
    *  are \c NULL, default names are used for the constraints and variables.
    */
   virtual bool readBasis(std::istream& in,
      const NameSet* rowNames, const NameSet* colNames);

   /** Write basis to \p os in MPS format. If \p rowNames and \p colNames are
    *  \c NULL, default names are used for the constraints and variables.
    */
   virtual void writeBasis(std::ostream& os, 
      const NameSet* rownames, const NameSet* colnames) const;

   /// checks if a Descriptor is valid for the current LP w.r.t. its bounds
   virtual bool isDescValid(const Desc& ds);

   /// sets up basis.
   /** Loads a Descriptor to the basis and sets up the basis matrix and
       all vectors accordingly. The Descriptor must have the same number of
       rows and columns as the currently loaded LP.
   */
   virtual void loadDesc(const Desc&);

   /// sets up linear solver to use.
   /** If destroy is true, solver will be freed inside this object, e.g. in the destructor.
   */
   virtual void loadSolver(SLinSolver* solver, const bool destroy = false);

   /// loads the LP \p lp to the basis.
   /** This involves resetting all counters to 0 and setting up a regular
       default basis consisting of slacks, artificial variables or bounds.
   */
   virtual void load(SPxSolver* lp);

   /// unloads the LP from the basis.
   virtual void unLoad()
   {
      METHOD( "SPxBasis::unLoad()" );
      theLP = 0;
      setStatus(NO_PROBLEM);
   }

   /// invalidates actual basis.
   /** This method makes the basis matrix and vectors invalid. The basis will
       be reinitialized if needed.
   */
   void invalidate();

   /// Restores initial basis.
   /** This method changes the basis to that present just after loading the LP
       (see addedRows() and addedCols()). This may be necessary if a row or a 
       column is changed, since then the current basis may become singular.
   */
   void restoreInitialBasis();

   /// output basis entries.
   void dump();
   
#ifndef NO_CONSISTENCY_CHECKS
   /// consistency check.
   bool isConsistent() const;
#endif

   /// returns statistical information in form of a string.
   std::string statistics() const
   {
      return factor->statistics();
   }
   //@}

   //--------------------------------------
   /**@name Constructors / Destructors */
   //@{
   /// default constructor.
   SPxBasis();
   /// copy constructor
   SPxBasis(const SPxBasis& old);
   /// assignment operator
   SPxBasis& operator=(const SPxBasis& rhs);
   /// destructor.
   virtual ~SPxBasis();
   //@}
   

protected:

   //--------------------------------------
   /**@name Protected helpers */
   //@{
   /// loads \ref soplex::SPxBasis::matrix "matrix" according to the SPxId%s stored in \ref soplex::SPxBasis::theBaseId "theBaseId".
   /** This method must  be called whenever there is a chance, that the vector
       pointers may have changed due to manipulations of the LP.
    */
   void loadMatrixVecs();

   /// resizes internal arrays.
   /** When a new LP is loaded, the basis matrix and vectors become invalid
       and possibly also of the wrong dimension. Hence, after loading an
       LP, #reDim() is called to reset all arrays etc. accoriding to the
       dimensions of the loaded LP.
   */
   void reDim();

   /// factorizes the basis matrix.
   virtual void factorize();

   /// sets descriptor representation according to loaded LP.
   void setRep();
   //@}

};


//
// Auxiliary functions.
//

/// Pretty-printing of basis status.
std::ostream& operator<<( std::ostream& os,
                          const SPxBasis::SPxStatus& status );


} // namespace soplex
#endif // _SPXBASIS_H_

//-----------------------------------------------------------------------------
//Emacs Local Variables:
//Emacs mode:c++
//Emacs c-basic-offset:3
//Emacs tab-width:8
//Emacs indent-tabs-mode:nil
//Emacs End:
//-----------------------------------------------------------------------------
