// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/convert.h
/// \brief Conversion utilities for converting between the old aterm format
///        and the new c++ implementation.

#ifndef MCRL2_DATA_DETAIL_CONVERT_H
#define MCRL2_DATA_DETAIL_CONVERT_H

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/vector.h"

namespace mcrl2 {

  namespace data {

    namespace detail {

      template <typename Term>
      atermpp::vector<Term> term_list_to_vector(const atermpp::term_list<Term>& l)
      {
        atermpp::vector<Term> v;

        for (typename atermpp::term_list<Term>::const_iterator i = l.begin(); i != l.end(); ++i)
        {
          v.push_back(*i);
        }

        return v;
      }

      template <typename Term>
      atermpp::term_list<Term> vector_to_term_list(const atermpp::vector<Term>& v)
      {
        atermpp::term_list<Term> l;

        for (typename atermpp::vector<Term>::const_reverse_iterator i = v.rbegin(); i != v.rend(); ++i)
        {
          l = push_front(l, *i);
        }

        return l;
      }

    } // namespace detail
    
  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_DETAIL_CONVERT_H

