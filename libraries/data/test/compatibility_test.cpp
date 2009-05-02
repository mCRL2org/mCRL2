// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file compatibility_test.cpp
/// \brief Regression test for the transformations between new and old data
///       format.

#include <iostream>
#include <sstream>
#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/exception.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/alpha.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/data_specification.h"

using namespace mcrl2;
using namespace atermpp;

namespace mcrl2 {
  namespace data {
    namespace detail {
      inline 
      ATermAppl parse_specification(std::istream& from) 
      { 
        ATermAppl result = core::parse_proc_spec(from); 
        if (result == NULL) 
          throw mcrl2::runtime_error("parse error"); 
        return result; 
      } 
     
      inline 
      ATermAppl type_check_specification(ATermAppl spec) 
      { 
        ATermAppl result = core::type_check_proc_spec(spec); 
        if (result == NULL) 
          throw mcrl2::runtime_error("type check error"); 
        return result; 
      } 
     
      inline 
      ATermAppl alpha_reduce(ATermAppl spec) 
      { 
        ATermAppl result = core::gsAlpha(spec); 
        if (result == NULL) 
          throw mcrl2::runtime_error("alpha reduction error"); 
        return result; 
      } 
    }

    // \brief Parses a data specification. 
    // \param[in] text a textual description of the data specification 
    // \return the data specification corresponding to text. 
    inline 
    data_specification parse_data_specification(const std::string& text) 
    { 
      // TODO: This is only a temporary solution. A decent standalone parser needs 
      // to be made for data specifications. 

      // make a fake linear process 
      std::stringstream lps_stream; 
      lps_stream << text; 
      lps_stream << "init delta;\n"; 

      ATermAppl result = data::detail::parse_specification(lps_stream); 
      result           = data::detail::type_check_specification(result); 
      result           = data::detail::alpha_reduce(result); 

      return data_specification(atermpp::arg1(result)); 
    } 
  }  
}

void compatibility_test()
{
  const std::string text(
    "sort S;\n"
    "cons s:S;\n"
    "map f:S -> List(S);\n"
  );

  // Create fake lps stream
  std::stringstream data_stream;
  data_stream << text;
  data_stream << "init delta;\n";

  aterm_appl lps_spec(data::detail::parse_specification(data_stream));
  lps_spec = data::detail::type_check_specification(lps_spec);
  lps_spec = data::detail::alpha_reduce(lps_spec);

  aterm_appl spec_old_format = atermpp::arg1(lps_spec);
  data::data_specification spec_new_format(spec_old_format);
  aterm_appl spec_old_format1 = data::detail::data_specification_to_aterm_data_spec(remove_all_system_defined(spec_new_format));

  BOOST_CHECK(spec_old_format == spec_old_format1);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  compatibility_test();

  return EXIT_SUCCESS;
}
