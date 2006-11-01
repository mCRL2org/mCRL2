#ifndef _LIBENUM_H
#define _LIBENUM_H

#include <aterm2.h>
#include "librewrite.h"

/** \brief Enumeration strategies. **/
typedef enum { ENUM_STANDARD } EnumerateStrategy;

typedef void (*FindSolutionsCallBack)(ATermList);

/**
 * \brief Iterator-like class to retreive valuations that satisfy a certain
 *        condition.
 *
 * EnumeratorSolutions objects are returned by Enumerator objects to enumerate
 * all the solutions to a given boolean expression. Typical use is as follows
 * (with e a Enumerator object):
 *
 * \code
 *   EnumeratorSolutions *es = e->findSolutions(vars,expr);
 *   ATermList l;
 *   while ( es->next(&l) )
 *   {
 *     // l is a valuation for vars that makes expr true
 *     ...
 *   }
 * \endcode
 **/
class EnumeratorSolutions
{
	public:
		/** \brief Destructor. **/
		virtual ~EnumeratorSolutions();

		/**
		 * \brief Get next solutions (if available).
		 * \param solution Place to store the solutions.
		 * \return Whether or not a solution was found and stored in
		 *         solution. If so, *solution is a substitution list
		 *         from libstruct, mapping mCRL2 data variables to terms
		 *         in the internal rewriter format.
		 *
		 * Once this function returns false, it will continue to return
		 * false. That is, when false is returned all solutions have
		 * been enumerated.
		 **/
		virtual bool next(ATermList *solution) = 0;
		/**
		 * \brief Check whether or not an error occurred during
		 *        enumeration.
		 * \return Whether or not an error occurred during enumeration.
		 **/
		virtual bool errorOccurred() = 0;
};

/**
 * \brief Interface class for finding solutions to boolean expressions.
 *
 * This is the interface class for enumerations of the solutions of boolean
 * expressions. Actual Enumerator objects need to be created with
 * createEnumerator().
 *
 * Note that EnumeratorSolutions objects that are returned by a Enumerator
 * object are only to be used with this same Enumerator object.
 **/
class Enumerator
{
	public:
		/** \brief Destructor. **/
		virtual ~Enumerator();

		/** \deprecated **/
		virtual ATermList FindSolutions(ATermList Vars, ATerm Expr, FindSolutionsCallBack f = NULL) = 0;

		/**
		 * \brief Enumerate all valuations that (might) make an
		 *        expression true.
		 * \param Vars      A list of the variables to be instantiated.
		 * \param Expr      The expression for which to find the
		 *                  valuations.
		 * \param true_only Boolean to indicate whether only those
		 *                  valuations that make expr true should be
		 *                  returned or those that do not make expr
		 *                  false.
		 * \param old       An EnumeratorSolutions object which can be
		 *                  used to retreive the satisfying valuations.
		 *                  If NULL, a new object is created.
		 * \return An EnumeratorSolutions object which can be used to
		 *         retreive all valuations of the variables in Var such
		 *         that expr becomes true. If old is not NULL, old
		 *         itself is returned (reinitialised for the new state).
		 *
		 * The optional argument old allows one to reuse
		 * EnumeratorSolutions objects (avoiding superfluous allocation
		 * and deallocation). Typical use is as follows (where e is a
		 * Enumerator object):
		 *
		 * \code
		 *   EnumeratorSolutions *es = NULL;
		 *   while ( c )
		 *   {
		 *     es = e->findSolutions(vars,expr,b,es);
		 *     ...
		 *   }
		 * \endcode
		 **/
		virtual EnumeratorSolutions *findSolutions(ATermList Vars, ATerm Expr, bool true_only, EnumeratorSolutions *old = NULL) = 0;
		/**
		 * \brief Enumerate all valuations that make an expression true.
		 * \param Vars A list of the variables to be instantiated.
		 * \param Expr The expression for which to find all solutions.
		 * \param old An EnumeratorSolutions object which can be used to
		 *            retreive the satisfying valuations. If NULL, a new
		 *            object is created.
		 * \return An EnumeratorSolutions object which can be used to
		 *         retreive all valuations of the variables in Var such
		 *         that expr becomes true. If old is not NULL, old
		 *         itself is returned (reinitialised for the new state).
		 *
		 * The optional argument old allows one to reuse
		 * EnumeratorSolutions objects (avoiding superfluous allocation
		 * and deallocation). Typical use is as follows (where e is a
		 * Enumerator object):
		 *
		 * \code
		 *   EnumeratorSolutions *es = NULL;
		 *   while ( c )
		 *   {
		 *     es = e->findSolutions(vars,expr,es);
		 *     ...
		 *   }
		 * \endcode
		 **/
		virtual EnumeratorSolutions *findSolutions(ATermList Vars, ATerm Expr, EnumeratorSolutions *old = NULL) = 0;
		
		/**
		 * \brief Get rewriter used by this object.
		 * \return Rewriter object used by this Enumerator object.
		 **/
		virtual Rewriter *getRewriter() = 0;
};

/**
 * \brief Create a Enumerator object.
 * \param spec              A mCRL2 LPE containing the data specification
 *                          related to the expressions which solutions need
 *                          to be found.
 * \param r                 Rewriter to use for rewriting terms.
 * \param clean_up_rewriter Whether or not to delete the Rewriter object on
 *                          destruction on the destruction of the returned
 *                          Enumerator object.
 * \param strategy          The strategy to use for solution finding.
 * \return A Enumerator object with the given parameters.
 **/
Enumerator *createEnumerator(ATermAppl spec, Rewriter *r, bool clean_up_rewriter = false, EnumerateStrategy strategy = ENUM_STANDARD);

#endif
