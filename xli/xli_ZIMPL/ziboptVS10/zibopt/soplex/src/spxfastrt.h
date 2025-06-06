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
#pragma ident "@(#) $Id: spxfastrt.h,v 1.25 2010/09/16 17:45:03 bzfgleix Exp $"

/**@file  spxfastrt.h
 * @brief Fast shifting ratio test.
 */
#ifndef _SPXFASTRT_H_
#define _SPXFASTRT_H_

#include <assert.h>

#include "spxdefines.h"
#include "spxratiotester.h"

namespace soplex
{

/**@brief   Fast shifting ratio test.
   @ingroup Algo

   Class SPxFastRT is an implementation class of SPxRatioTester providing
   fast and stable ratio test. Stability is achieved by allowing some
   infeasibility to ensure numerical stability such as the Harris procedure.
   Performance is achieved by skipping the second phase if the first phase
   already shows a stable enough pivot.

   See SPxRatioTester for a class documentation.
*/
class SPxFastRT : public SPxRatioTester
{
private:
   //-------------------------------------
   /**@name Data */
   //@{
   /// parameter for computing minimum stability requirement
   Real minStab;
   /// |value| < epsilon is considered 0.
   Real epsilon;
   /// currently allowed infeasibility.
   Real delta;
   /// initially allowed infeasibility.
   Real delta0;
   /// flag used in methods minSelect/maxSelect to retrieve correct basis status
   bool iscoid;
   //@}

   //-------------------------------------
   /**@name Private helpers */
   //@{
   /// resets tolerances (epsilon).
   void resetTols();
   /// relaxes stability requirements.
   void relax();
   /// tightens stability requirements.
   void tighten();

   /// Max phase 1 value.
   /** Computes the maximum value \p val that could be used for updating \p update
       such that it would still fulfill the upper and lower bounds \p upBound and
       \p lowBound, respectively, within #delta. Return value is the index where the
       maximum value is encountered. At the same time the maximum absolute value
       of \p update.delta() is computed and returned in \p maxabs. Internally all
       loops are started at \p start and incremented by \p incr.
    */
   int maxDelta(Real& val, Real& maxabs, UpdateVector& update,
      const Vector& lowBound, const Vector& upBound, int start, int incr) const;

   ///
   int maxDelta(Real& val, Real& maxabs);

   ///
   SPxId maxDelta(int& nr, Real& val, Real& maxabs);

   /// Min phase 1 value.
   /** Computes the minimum value \p val that could be used for updating \p update
       such that it would still fulfill the upper and lower bounds \p upBound and
       \p lowBound, respectively, within #delta. Return value is the index where the
       minimum value is encountered. At the same time the maximum absolute value
       of \p update.delta() is computed and returned in \p maxabs. Internally all
       loops are started at \p start and incremented by \p incr.
   */
   int minDelta(Real& val, Real& maxabs, UpdateVector& update,
      const Vector& lowBound, const Vector& upBound, int start, int incr) const;

   ///
   int minDelta(Real& val, Real& maxabs);

   ///
   SPxId minDelta(int& nr, Real& val, Real& maxabs);

   /// selects stable index for maximizing ratio test.
   /** Selects from all update values \p val < \p max the one with the largest
       value of \p upd.delta() which must be greater than \p stab and is
       returned in \p stab. The index is returned as well as the corresponding
       update value \p val. Internally all loops are started at \p start and
       incremented by \p incr.
   */
   int maxSelect(Real& val, Real& stab, Real& best, Real& bestDelta,
      Real max, const UpdateVector& upd, const Vector& low,
      const Vector& up, int start = 0, int incr = 1) const;
   ///
   int maxSelect(Real& val, Real& stab, Real& bestDelta, Real max);
   ///
   SPxId maxSelect(int& nr, Real& val, Real& stab,
      Real& bestDelta, Real max);

   /// selects stable index for minimizing ratio test.
   /** Select from all update values \p val > \p max the one with the largest
       value of \p upd.delta() which must be greater than \p stab and is
       returned in \p stab. The index is returned as well as the corresponding
       update value \p val. Internally all loops are started at \p start and
       incremented by \p incr.
   */
   int minSelect(Real& val, Real& stab, Real& best, Real& bestDelta,
      Real max, const UpdateVector& upd, const Vector& low,
      const Vector& up, int start = 0, int incr = 1) const;
   ///
   int minSelect(Real& val, Real& stab,
      Real& bestDelta, Real max);
   ///
   SPxId minSelect(int& nr, Real& val, Real& stab,
      Real& bestDelta, Real max);

   /// tests for stop after phase 1.
   /** Tests whether a shortcut after phase 1 is feasible for the
       selected leave pivot. In this case return the update value in \p sel.
   */
   bool minShortLeave(Real& sel, int leave, Real maxabs);
   ///
   bool maxShortLeave(Real& sel, int leave, Real maxabs);

   /// numerical stability tests.
   /** Tests whether the selected leave index needs to be discarded (and do so)
       and the ratio test is to be recomputed.
   */
   bool minReLeave(Real& sel, int leave, Real maxabs);
   ///
   bool maxReLeave(Real& sel, int leave, Real maxabs);

   /// numerical stability check.
   /** Tests whether the selected enter \p id needs to be discarded (and do so)
       and the ratio test is to be recomputed.
   */
   bool minReEnter(Real& sel, Real maxabs, const SPxId& id, int nr);
   ///
   bool maxReEnter(Real& sel, Real maxabs, const SPxId& id, int nr);

   /// Tests and returns whether a shortcut after phase 1 is feasible for the
   /// selected enter pivot.
   bool shortEnter(const SPxId& enterId, int nr, Real max, Real maxabs) const;
   //@}

public:

   //-------------------------------------
   /**@name Construction / destruction */
   //@{
   /// default constructor
   SPxFastRT()
      : SPxRatioTester("Fast")
      , iscoid(false)
   {}
   /// copy constructor
   SPxFastRT(const SPxFastRT& old)
      : SPxRatioTester(old)
      , minStab(old.minStab)
      , epsilon(old.epsilon)
      , delta(old.delta)
      , delta0(old.delta0)
      , iscoid(false)
   {}
   /// assignment operator
   SPxFastRT& operator=( const SPxFastRT& rhs)
   {
      if(this != &rhs)
      {
         SPxRatioTester::operator=(rhs);
         minStab = rhs.minStab;
         epsilon = rhs.epsilon;
         delta = rhs.delta;
         delta0 = rhs.delta0;
         iscoid = false;
      }

      return *this;
   }
   /// destructor
   virtual ~SPxFastRT()
   {}
   /// clone function for polymorphism
   inline virtual SPxRatioTester* clone() const
   {
      return new SPxFastRT(*this);
   }
   //@}

   //-------------------------------------
   /**@name Access / modification */
   //@{
   ///
   virtual void load(SPxSolver* solver);
   ///
   virtual int selectLeave(Real& val);
   ///
   virtual SPxId selectEnter(Real& val);
   ///
   virtual void setType(SPxSolver::Type type);
   //@}
};
} // namespace soplex
#endif // _SPXFASTRT_H_

//-----------------------------------------------------------------------------
//Emacs Local Variables:
//Emacs mode:c++
//Emacs c-basic-offset:3
//Emacs tab-width:8
//Emacs indent-tabs-mode:nil
//Emacs End:
//-----------------------------------------------------------------------------
