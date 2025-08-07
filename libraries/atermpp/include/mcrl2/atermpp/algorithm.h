// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/algorithm.h
/// \brief Algorithms for ATerms.

#ifndef MCRL2_ATERMPP_ALGORITHM_H
#define MCRL2_ATERMPP_ALGORITHM_H

#include <unordered_map>

#include "mcrl2/atermpp/builder.h"
#include "mcrl2/atermpp/detail/algorithm_impl.h"

namespace atermpp
{

namespace detail
{

template <template <class> class Builder, class ReplaceFunction>
struct replace_aterm_builder: public Builder<replace_aterm_builder<Builder, ReplaceFunction> >
{
  using super = Builder<replace_aterm_builder<Builder, ReplaceFunction>>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::derived;

  ReplaceFunction f;

  replace_aterm_builder(ReplaceFunction f_)
    : f(f_)
  {}

  template <class T>
  void apply(T& result, const aterm& x)
  {
    const T fx(f(x));
    if (x == fx) 
    {
      super::apply(result, x); 
    }
    else 
    {
      result = fx;
    }
  }
};

template <template <class> class Builder, class ReplaceFunction>
replace_aterm_builder<Builder, ReplaceFunction>
make_replace_aterm_builder(ReplaceFunction f)
{
  return replace_aterm_builder<Builder, ReplaceFunction>(f);
}

template <template <class> class Builder, class ReplaceFunction>
struct partial_replace_aterm_builder: public Builder<partial_replace_aterm_builder<Builder, ReplaceFunction> >
{
  using super = Builder<partial_replace_aterm_builder<Builder, ReplaceFunction>>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::derived;

  ReplaceFunction f;

  partial_replace_aterm_builder(ReplaceFunction f_)
    : f(f_)
  {}

  template <class T>
  void apply(T& result, const aterm& x)
  {
    std::pair<aterm, bool> p = f(x);
    if (p.second)
    { 
      super::apply(result, x);
    } 
    else 
    {
      result = p.first;
    }
  }
};

template <template <class> class Builder, class ReplaceFunction>
partial_replace_aterm_builder<Builder, ReplaceFunction>
make_partial_replace_aterm_builder(ReplaceFunction f)
{
  return partial_replace_aterm_builder<Builder, ReplaceFunction>(f);
}

template <template <class> class Builder, class ReplaceFunction>
struct bottom_up_replace_aterm_builder: public Builder<bottom_up_replace_aterm_builder<Builder, ReplaceFunction> >
{
  using super = Builder<bottom_up_replace_aterm_builder<Builder, ReplaceFunction>>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::derived;

  ReplaceFunction f;

  bottom_up_replace_aterm_builder(ReplaceFunction f_)
    : f(f_)
  {}

  template <class T>
  void apply(T& result, const aterm& x)
  {
    aterm t;
    super::apply(t, x);
    result = static_cast<T>(f(t));
  }
};

template <template <class> class Builder, class ReplaceFunction>
bottom_up_replace_aterm_builder<Builder, ReplaceFunction>
make_bottom_up_replace_aterm_builder(ReplaceFunction f)
{
  return bottom_up_replace_aterm_builder<Builder, ReplaceFunction>(f);
}

template <template <class> class Builder, class ReplaceFunction>
struct cached_bottom_up_replace_aterm_builder: public Builder<cached_bottom_up_replace_aterm_builder<Builder, ReplaceFunction> >
{
  using super = Builder<cached_bottom_up_replace_aterm_builder<Builder, ReplaceFunction>>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::derived;

  ReplaceFunction f;
  std::unordered_map<aterm, aterm>& cache;

  cached_bottom_up_replace_aterm_builder(ReplaceFunction f_, std::unordered_map<aterm, aterm>& cache_)
    : f(f_), cache(cache_)
  {}

  template <class T>
  void apply(T& result, const aterm& x)
  {
    auto i = cache.find(x);
    if (i != cache.end())
    {
      result = i->second;
      return;
    }
    aterm t;
    super::apply(t, x);
    result = f(t);
    cache[x] = result;
  }
};

template <template <class> class Builder, class ReplaceFunction>
cached_bottom_up_replace_aterm_builder<Builder, ReplaceFunction>
make_cached_bottom_up_replace_aterm_builder(ReplaceFunction f, std::unordered_map<aterm, aterm>& cache)
{
  return cached_bottom_up_replace_aterm_builder<Builder, ReplaceFunction>(f, cache);
}

} // namespace detail

/// \brief Calls op(elem) for subterms of the term t.
/// \param t A term
/// \param op The operation that is applied to subterms
/// \return a copy of the (internally modified) op.
/// The function op must have the signature bool op(aterm t).
/// When op(t) is false, the children of t are skipped.
template <typename UnaryFunction, typename Term>
UnaryFunction for_each(Term t, UnaryFunction op)
{
  return detail::for_each_impl<std::add_lvalue_reference_t<UnaryFunction>>(t, op);
}

/// \brief Finds a subterm of t that matches a given predicate.
/// \param t A term
/// \param match The predicate that determines if a subterm is a match
/// \return A subterm that matches the given predicate, or aterm() if none was found.
template <typename Term, typename MatchPredicate>
aterm find_if(const Term& t, MatchPredicate match)
{
  aterm output;
  detail::find_if_impl<std::add_lvalue_reference_t<MatchPredicate>>(t, match, output);
  return output;
}

/// \brief Finds a subterm of t that matches a given predicate.
/// The term is only partially traversed. If the stop predicate
/// returns true in a subterm, the recursion is not continued.
/// \param t A term
/// \param match The predicate that determines if a subterm is a match
/// \param stop The predicate that determines if the recursion should not be continued in a subterm
/// \return A subterm that matches the given predicate, or aterm() if none was found.
template <typename Term, typename MatchPredicate, typename StopPredicate>
aterm partial_find_if(Term t, MatchPredicate match, StopPredicate stop)
{
  return detail::partial_find_if_impl<std::add_lvalue_reference_t<MatchPredicate>>(t, match, stop);
}

/// \brief Finds all subterms of t that match a given predicate, and writes the found terms
/// to the destination range starting with destBegin.
/// \param t A term
/// \param match The predicate that determines if a subterm is a match
/// \param destBegin The iterator range to which output is written.
template <typename Term, typename MatchPredicate, typename OutputIterator>
void find_all_if(const Term& t, MatchPredicate match, OutputIterator destBegin)
{
  OutputIterator i = destBegin; // we make a copy, since a reference to an iterator is needed
  detail::find_all_if_impl<std::add_lvalue_reference_t<MatchPredicate>>(t, match, i);
}

/// \brief Finds all subterms of t that match a given predicate, and writes the found terms
/// to the destination range starting with destBegin.
/// The term is only partially traversed. If the stop predicate
/// returns true in a subterm, the recursion is not continued.
/// \param t A term
/// \param match The predicate that determines if a subterm is a match
/// \param stop The predicate that determines if the recursion should not be continued in a subterm
/// \param destBegin The iterator range to which output is written.
template <typename Term, typename MatchPredicate, typename StopPredicate, typename OutputIterator>
void partial_find_all_if(Term t, MatchPredicate match, StopPredicate stop, OutputIterator destBegin)
{
  OutputIterator i = destBegin; // we make a copy, since a reference to an iterator is needed
  detail::partial_find_all_if_impl<std::add_lvalue_reference_t<MatchPredicate>,
      std::add_lvalue_reference_t<StopPredicate>>(t, match, stop, i);
}

/// \brief Replaces each subterm x of t by r(x). The ReplaceFunction r has
/// the following signature:
/// aterm x;
/// aterm result = r(x);
/// The replacements are performed in top down order.
/// \param t A term
/// \param r The replace function that is applied to subterms.
/// \return The result of the replacement.
template <typename Term, typename ReplaceFunction>
Term replace(const Term& t, ReplaceFunction r)
{
  Term result;
  detail::make_replace_aterm_builder<atermpp::builder>(r).apply(result,t);
  return result;
}

/// \brief Replaces each subterm in t that is equal to old_value with new_value.
/// The replacements are performed in top down order. For example,
/// replace(f(f(x)), f(x), x) returns f(x) and not x.
/// \param t A term
/// \param old_value The subterm that will be replaced.
/// \param new_value The value that will be substituted.
/// \return The result of the replacement.
template <typename Term>
Term replace(const Term& t, const aterm& old_value, const aterm& new_value)
{
  return replace(t, [&](const aterm& t) { return t == old_value ? new_value : t; });
}

/// \brief Replaces each subterm x of t by r(x). The ReplaceFunction r has
/// the following signature:
/// aterm x;
/// aterm result = r(x);
/// The replacements are performed in bottom up order. For example,
/// replace(f(f(x)), f(x), x) returns x.
/// \param t A term
/// \param r The replace function that is applied to subterms.
/// \return The result of the replacement.
template <typename Term, typename ReplaceFunction>
Term bottom_up_replace(Term t, ReplaceFunction r)
{
  Term result;
  detail::make_bottom_up_replace_aterm_builder<atermpp::builder>(r).apply(result, t);
  return result;
}

/// \brief Replaces each subterm in t that is equal to old_value with new_value.
/// The replacements are performed in top down order. For example,
/// replace(f(f(x)), f(x), x) returns f(x) and not x.
/// \param t A term
/// \param old_value The value of the subterm that is replaced.
/// \param new_value The value that is substituted.
/// \return The result of the replacement.
template <typename Term>
Term bottom_up_replace(Term t, const aterm& old_value, const aterm& new_value)
{
  return bottom_up_replace(t, [&](const aterm& t) { return t == old_value ? new_value : t; });
}

/// \brief Replaces subterms x of t by r(x). The replace function r returns an
/// additional boolean value. This value is used to prevent further recursion.
/// The ReplaceFunction r has the following signature:
/// aterm x;
/// std::pair<aterm, bool> result = r(x);
/// result.first  is the result r(x) of the replacement
/// result.second denotes if the recursion should be continued
/// The replacements are performed in top down order.
/// \param t A term
/// \param r The replace function that is applied to subterms.
/// \return The result of the replacement.
template <typename Term, typename ReplaceFunction>
Term partial_replace(Term t, ReplaceFunction r)
{
  Term result;
  detail::make_partial_replace_aterm_builder<atermpp::builder>(r).apply(result, t);
  return result;
}

/// \brief Replaces each subterm x of t by r(x). The ReplaceFunction r has
/// the following signature:
/// aterm x;
/// aterm result = r(x);
/// The replacements are performed in bottom up order. For example,
/// replace(f(f(x)), f(x), x) returns x.
/// \param t A term
/// \param r The replace function that is applied to subterms.
/// \param cache A cache for the result of aterm terms.
/// \return The result of the replacement.
template <typename Term, typename ReplaceFunction>
Term bottom_up_replace(Term t, ReplaceFunction r, std::unordered_map<aterm, aterm>& cache)
{
  Term result;
  detail::make_cached_bottom_up_replace_aterm_builder<atermpp::builder>(r, cache).apply(result, t);
  return result;
}

} // namespace atermpp

#endif // MCRL2_ATERMPP_ALGORITHM_H
