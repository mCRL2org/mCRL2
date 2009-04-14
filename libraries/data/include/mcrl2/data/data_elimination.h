// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_elimination.h
/// \brief The files provides the class data_elimination.

#ifndef MCRL2_DATA_DATA_ELIMINATION_H
#define MCRL2_DATA_DATA_ELIMINATION_H

#include <cassert>
#include <mcrl2/data/data_specification.h>
#include <mcrl2/data/sort.h>
#include <mcrl2/data/data_expression.h>
#include <mcrl2/data/data_operation.h>

namespace mcrl2 {

namespace data {

  /// \brief   This class facilitates the elmination of unused or unnecessary
  ///          parts of a data specification.
  class data_elimination {
    public:
      /// \brief Default constructor.
      data_elimination();

      /// \brief Destructor.
      ~data_elimination();


      /// \brief       Ensure that data elemination does not remove sort s.
      /// \param[in] s An mCRL2 data sort expression.
      void keep_sort(sort_expression s);

      /// \brief        Ensure that data elemination does not remove mapping op.
      /// \param[in] op An mCRL2 data operation.
      void keep_function(data_operation op);

      /// \brief Ensure that data elimination does not remove the standard data
      ///        sorts (such as Bool and Pos) and mappings.
      void keep_standard_data();

      /// \brief          Ensure that data elimination does not remove the sorts
      ///                 and mappings used in expr.
      /// \param[in] expr An mCRL2 data expression.
      void keep_data_from_expr(data_expression expr);


      /// \brief          Apply data elimination to spec
      /// \param[in] spec An mCRL2 data specification.
      /// \return         A data specification containing only those sorts and
      ///                 mappings from spec that where previously indicated to
      ///                 keep (via the keep_* methods) and those that are used
      ///                 (directly or indirectly) in the specification of
      ///                 theses sorts and mappings.
      data_specification apply(data_specification spec);


    private:
      ATermTable used_data;
      bool keep_basis;
  };

}

}

#endif
