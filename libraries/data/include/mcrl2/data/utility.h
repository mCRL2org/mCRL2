// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/utility.h
/// \brief Provides utilities for working with lists.

#ifndef MCRL2_DATA_UTILITY_H
#define MCRL2_DATA_UTILITY_H

#include "mcrl2/atermpp/vector.h"

namespace mcrl2 {

  namespace data {

      /// \brief Constructs a vector with element type T of one argument.
      ///
      /// \param[in] t1 The first element of the vector.
      template <typename T>
      atermpp::vector<T> make_vector(const T& t1)
      {
        atermpp::vector<T> v;
        v.push_back(t1);
        return v;
      }

      /// \brief Constructs a vector with element type T of two arguments.
      ///
      /// \param[in] t1 The first element of the vector.
      /// \param[in] t2 The second element of the vector.
      template <typename T>
      atermpp::vector<T> make_vector(const T& t1, const T& t2)
      {
        atermpp::vector<T> v;
        v.push_back(t1);
        v.push_back(t2);
        return v;
      } 

      /// \brief Constructs a vector with element type T of three arguments.
      ///
      /// \param[in] t1 The first element of the vector.
      /// \param[in] t2 The second element of the vector.
      /// \param[in] t3 The third element of the vector.
      template <typename T>
      atermpp::vector<T> make_vector(const T& t1, const T& t2, const T& t3)
      {
        atermpp::vector<T> v;
        v.push_back(t1);
        v.push_back(t2);
        v.push_back(t3);
        return v;
      } 

      /// \brief Constructs a vector with element type T of four arguments.
      ///
      /// \param[in] t1 The first element of the vector.
      /// \param[in] t2 The second element of the vector.
      /// \param[in] t3 The third element of the vector.
      /// \param[in] t4 The fourth element of the vector.
      template <typename T>
      atermpp::vector<T> make_vector(const T& t1, const T& t2, const T& t3, const T& t4)
      {
        atermpp::vector<T> v;
        v.push_back(t1);
        v.push_back(t2);
        v.push_back(t3);
        v.push_back(t4);
        return v;
      }

  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_UTILITY_H

