#ifndef __LIBGSREWRITE_H
#define __LIBGSREWRITE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>

/** \brief Rewrite strategies. */
typedef enum { GS_REWR_INNER     /** \brief Innermost */
	     , GS_REWR_INNERC    /** \brief Compiling innermost */
	     , GS_REWR_JITTY     /** \brief JITty */
	     , GS_REWR_JITTYC    /** \brief Compiling JITty */
	     , GS_REWR_INNER_P   /** \brief Innermost + Prover */
	     , GS_REWR_INNERC_P  /** \brief Compiling innermost + Prover*/
	     , GS_REWR_JITTY_P   /** \brief JITty + Prover */
	     , GS_REWR_JITTYC_P  /** \brief Compiling JITty + Prover*/
	     , GS_REWR_INVALID   /** \brief Invalid strategy */
	     } RewriteStrategy;

/** \deprecated */
void gsRewriteInit(ATermAppl DataEqnSpec, RewriteStrategy Strategy);
/** \deprecated */
void gsRewriteFinalise();

/** \deprecated */
bool gsRewriteIsInitialised();

/** \deprecated */
void gsRewriteAddEqn(ATermAppl Eqn);
/** \deprecated */
void gsRewriteRemoveEqn(ATermAppl Eqn);

/** \deprecated */
ATermAppl gsRewriteTerm(ATermAppl Term);
/** \deprecated */
ATermList gsRewriteTerms(ATermList Terms);

/** \deprecated */
ATerm gsToRewriteFormat(ATermAppl Term);
/** \deprecated */
ATermAppl gsFromRewriteFormat(ATerm Term);
/** \deprecated */
ATerm gsRewriteInternal(ATerm Term);
/** \deprecated */
ATermList gsRewriteInternals(ATermList Terms);

/**
 * \brief Print a string representation of a rewrite strategy.
 * \param stream File stream to print to.
 * \param strat  Rewrite strategy to print.
 **/
void PrintRewriteStrategy(FILE *stream, RewriteStrategy strat);
/**
 * \brief Get rewrite strategy from its string representation.
 * \param s String representation of strategy.
 * \return Rewrite strategy represented by s. If s is not a valid rewrite
 *         strategy, ::GS_REWR_INVALID is returned.
 **/
RewriteStrategy RewriteStrategyFromString(const char *s);

/** \deprecated */
void RWsetVariable(ATerm v, ATerm t);
/** \deprecated */
void RWclearVariable(ATerm v);
/** \deprecated */
void RWclearAllVariables();
/** \deprecated */
ATerm RWapplySubstitution(ATerm v);

#ifdef __cplusplus
}
#endif

#endif
