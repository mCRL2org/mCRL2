// Author(s): Jan Friso Groote, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesrewr.cpp

#include "boost.hpp" // precompiled headers

#include <iostream>
#include <string>
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/pbes_rewriter_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using utilities::tools::input_output_tool;
using utilities::tools::rewriter_tool;
using utilities::tools::pbes_rewriter_tool;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;

class pbes_rewriter : public pbes_rewriter_tool<rewriter_tool<input_output_tool> >
{
  protected:
    typedef pbes_rewriter_tool<rewriter_tool<input_output_tool> > super;

  public:
    pbes_rewriter()
      : super(
        "pbesrewr",
        "Jan Friso Groote and Wieger Wesselink",
        "rewrite and simplify a PBES",
        "Rewrite the PBES in INFILE, remove quantified variables and write the resulting PBES to OUTFILE. "
        "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used."
      )
    {}

    bool run()
    {
      using namespace pbes_system;
      using namespace utilities;

      if (core::gsVerbose)
      {
        std::clog << "pbesrewr parameters:" << std::endl;
        std::clog << "  input file:         " << m_input_filename << std::endl;
        std::clog << "  output file:        " << m_output_filename << std::endl;
        std::clog << "  pbes rewriter:      " << m_pbes_rewriter_type << std::endl;
      }

      // load the pbes
      pbes<> p;
      p.load(m_input_filename);

      // data rewriter
      data::rewriter datar = create_rewriter(p.data());

      // pbes rewriter
      switch (rewriter_type())
      {
        case simplify:
        {
          simplifying_rewriter<pbes_expression, data::rewriter> pbesr(datar);
          pbes_rewrite(p, pbesr);
          break;
        }
        case quantifier_all:
        {
          data::number_postfix_generator generator("UNIQUE_PREFIX");
          data::data_enumerator<> datae(p.data(), datar, generator);
          data::rewriter_with_variables datarv(datar);
          bool enumerate_infinite_sorts = true;
          enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > pbesr(datarv, datae, enumerate_infinite_sorts);
          pbes_rewrite(p, pbesr);
          break;
        }
        case quantifier_finite:
        {
          data::number_postfix_generator generator("UNIQUE_PREFIX");
          data::data_enumerator<> datae(p.data(), datar, generator);
          data::rewriter_with_variables datarv(datar);
          bool enumerate_infinite_sorts = false;
          enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > pbesr(datarv, datae, enumerate_infinite_sorts);
          pbes_rewrite(p, pbesr);
          break;
        }
        case pfnf:
        {
          pfnf_rewriter pbesr;
          pbes_rewrite(p, pbesr);
          break;
        }
        case prover:
        default:
        {
          // Just ignore.
          assert(0);  // The PBES rewriter cannot be activated through
          // the commandline. So, we cannot end up here.
          break;
        }
      }

      // save the result
      p.save(m_output_filename);

      return true;
    }

};

class pbes_rewriter_gui: public mcrl2_gui_tool<pbes_rewriter>
{
  public:
    pbes_rewriter_gui()
    {

      std::vector<std::string> values;

      values.clear();
      values.push_back("simplify");
      values.push_back("quantifier-all");
      values.push_back("quantifier-finite");
      values.push_back("pfnf");
      m_gui_options["pbes-rewriter"] = create_radiobox_widget(values);

      add_rewriter_widget();
    }
};
int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return pbes_rewriter_gui().execute(argc, argv);
}
