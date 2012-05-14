// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/builder_msvc.inc.h
/// \brief add your file description here.

// aterm traversal
template <typename T>
T operator()(const T& x,
             typename boost::enable_if<typename boost::is_base_of< aterm::aterm, T>::type>::type* = 0
            )
{
  core::msg("aterm traversal");
  throw mcrl2::runtime_error("unknown type encountered in builder function!");
  return x;
}

// aterm list traversal
template <typename T>
atermpp::term_list<T> operator()(const atermpp::term_list<T>& x)
{
  core::msg("aterm list traversal");
  std::vector<T> result;
  for (typename atermpp::term_list<T>::const_iterator i = x.begin(); i != x.end(); ++i)
  {
    result.push_back(update_copy(*i));
  }
  return atermpp::convert<atermpp::term_list<T> >(result);
}

// Container traversal
template <typename T>
void operator()(T& x,
                typename boost::disable_if<typename boost::is_base_of< aterm::aterm, T>::type>::type* = 0,
                typename atermpp::detail::enable_if_container<T>::type* = 0
               )
{
  core::msg("container traversal");
  for (typename T::iterator i = x.begin(); i != x.end(); ++i)
  {
    update(*i);
  }
}

// aterm set traversal
template <typename T>
void operator()(std::set<T>& x)
{
  core::msg("aterm set traversal");
  std::set<T> result;
  for (typename std::set<T>::const_iterator i = x.begin(); i != x.end(); ++i)
  {
    result.insert(update_copy(*i));
  }
  std::swap(x, result);
}
