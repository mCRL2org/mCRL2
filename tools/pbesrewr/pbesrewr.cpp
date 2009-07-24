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
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbesrewr.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using utilities::tools::input_output_tool;
using utilities::tools::rewriter_tool;
using utilities::tools::pbes_rewriter_tool;
using namespace mcrl2::utilities::tools;

class pbes_rewriter : public squadt_tool< pbes_rewriter_tool<rewriter_tool<input_output_tool> > >
{
  protected:
    typedef squadt_tool< pbes_rewriter_tool<rewriter_tool<input_output_tool> > > super;

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
          pbesrewr(p, pbesr);
          break;
        }
        case quantifier_all:
        {
          data::number_postfix_generator generator("UNIQUE_PREFIX");
          data::data_enumerator<> datae(p.data(), datar, generator);
          data::rewriter_with_variables datarv(datar);
          bool enumerate_infinite_sorts = true;
          enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > pbesr(datarv, datae, enumerate_infinite_sorts);
          pbesrewr(p, pbesr);
          break;
        }
        case quantifier_finite:
        {
          data::number_postfix_generator generator("UNIQUE_PREFIX");
          data::data_enumerator<> datae(p.data(), datar, generator);
          data::rewriter_with_variables datarv(datar);
          bool enumerate_infinite_sorts = false;
          enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > pbesr(datarv, datae, enumerate_infinite_sorts);
          pbesrewr(p, pbesr);
          break;
        }
        case prover:
        default:
        { // Just ignore.
          assert(0);  // The PBES rewriter cannot be activated through
                      // the commandline or squadt. So, we cannot end up here.
          break;
        }
      }

      // save the result
      p.save(m_output_filename);

      return true;
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY
    void set_capabilities(tipi::tool::capabilities& c) const {
      c.add_input_configuration("main-input", tipi::mime_type("pbes", tipi::mime_type::application), tipi::tool::category::transformation);
    }

    void user_interactive_configuration(tipi::configuration& c) {
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;
      using namespace utilities;

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      /* Create display */
      tipi::tool_display d;

      layout::vertical_box& m = d.create< vertical_box >();

      add_rewrite_option(d, m);
      add_pbes_rewrite_option(d, m);

      button& okay_button = d.create< button >().set_label("OK");

      m.append(d.create< label >().set_text(" ")).
        append(okay_button, layout::right);

      send_display_layout(d.manager(m));

      /* Wait until the ok button was pressed */
      okay_button.await_change();

      /* Add output file to the configuration */
      if (!c.output_exists("main-output")) {
        c.add_output("main-output", tipi::mime_type("pbes", tipi::mime_type::application), c.get_output_name(".pbes"));
      }

      send_clear_display();

      // let squadt_tool update configuration for rewriter and input/output files
      update_configuration(c);
    }

    bool check_configuration(tipi::configuration const& c) const {
      return c.input_exists("main-input") &&
             c.output_exists("main-output") &&
             c.option_exists("rewrite-strategy") &&
             c.option_exists("pbes-rewriter-type");
    }

    bool perform_task(tipi::configuration& c) {
      using namespace utilities;

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      bool result = run();

      send_clear_display();

      return result;
    }
#endif
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return pbes_rewriter().execute(argc, argv);
}
