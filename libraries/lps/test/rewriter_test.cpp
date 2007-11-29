// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewriter_test.cpp
/// \brief Add your file description here.

#include <cstdlib>
#include <string>
#include <algorithm>
#include <boost/test/minimal.hpp>
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/data/sort_identifier.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/modal_formula/detail/read_text.h"

using namespace std;
using namespace atermpp;
using namespace lps;
using namespace lps::detail;

///////////////////////////////////////////////////////////////////////////////
// rewriter
/// \brief rewriter.
class rewriter
{
  private:
    Rewriter* m_rewriter;

  public:
    /// Constructs a rewriter from data specification d.
    ///
    rewriter(data_specification d)
    {
      m_rewriter = createRewriter(d);
    }

    ~rewriter()
    {
      delete m_rewriter;
    }
  
		/// \brief Rewrites a data expression.
		/// \param d The term to be rewritten.
		/// \return The normal form of d.
		///
		data_expression operator()(const data_expression& d) const
		{
		  ATerm t = m_rewriter->toRewriteFormat(d);
		  return m_rewriter->rewrite((ATermAppl) t);
		}
};

struct sort_has_name
{
  std::string m_name;
    
  sort_has_name(std::string name)
    : m_name(name)
  {}
  
  bool operator()(sort_expression s) const
  {
    return is_sort_identifier(s) && std::string(sort_identifier(s).name()) == s;
  }
};

struct data_operation_has_name
{
  std::string m_name;
    
  data_operation_has_name(std::string name)
    : m_name(name)
  {}
  
  bool operator()(data_operation c) const
  {
    return std::string(c.name()) == m_name;
  }
};

/// Find the mapping named s.
data_operation find_mapping(data_specification data, std::string s)
{
  data_operation_list::iterator i = std::find_if(data.mappings().begin(), data.mappings().end(), data_operation_has_name(s));
  if (i == data.mappings().end())
  {
    return data_operation();
  }
  return *i;
}

/// Find the constructor named s.
data_operation find_constructor(data_specification data, std::string s)
{
  data_operation_list::iterator i = std::find_if(data.constructors().begin(), data.constructors().end(), data_operation_has_name(s));
  if (i == data.constructors().end())
  {
    return data_operation();
  }
  return *i;
}

/// Find the sort named s.
sort_expression find_sort(data_specification data, std::string s)
{
  sort_expression_list::iterator i = std::find_if(data.sorts().begin(), data.sorts().end(), sort_has_name(s));
  if (i == data.sorts().end())
  {
    return sort_expression();
  }
  return *i;
}

void test1()
{
  specification spec = mcrl22lps(read_text("natural.mcrl2"));
  rewriter r(spec.data());

  data_expression n1    = find_mapping(spec.data(), "_1");
  data_expression n2    = find_mapping(spec.data(), "_2");
  data_expression n3    = find_mapping(spec.data(), "_3");
  data_expression n4    = find_mapping(spec.data(), "_4");
  data_expression n5    = find_mapping(spec.data(), "_5");
  data_expression n6    = find_mapping(spec.data(), "_6");
  data_expression n7    = find_mapping(spec.data(), "_7");
  data_expression n8    = find_mapping(spec.data(), "_8");
  data_expression n9    = find_mapping(spec.data(), "_9");
  data_expression plus  = find_mapping(spec.data(), "plus");
  
  cout << pp(data_application(plus, make_list(n4, n5))) << endl;
  BOOST_CHECK(r(data_application(plus, make_list(n4, n5))) == r(data_application(plus, make_list(n2, n7))));
}

int test_main(int, char*[])
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  test1();

  return 0;
}
