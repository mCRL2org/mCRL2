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
             typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value>::type* = 0
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
  for (auto i = x.begin(); i != x.end(); ++i)
  {
    result.push_back(atermpp::vertical_cast<T>(update_copy(*i)));
  }
  return atermpp::term_list<T>(result.begin(), result.end());
}

// Container traversal
template <typename T>
void operator()(T& x,
                typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value>::type* = 0,
                typename atermpp::enable_if_container<T>::type* = 0
               )
{
  core::msg("container traversal");
  for (auto i = x.begin(); i != x.end(); ++i)
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
  for (auto i = x.begin(); i != x.end(); ++i)
  {
    result.insert(update_copy(*i));
  }
  using std::swap;
  swap(x, result);
}
