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
#include "mcrl2/utilities/pbes_rewriter_tool.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbesrewr.h"
#include "mcrl2/pbes/rewriter.h"

using namespace mcrl2;
using utilities::pbes_rewriter_tool;

class pbes_rewr_tool: public pbes_rewriter_tool
{
  public:
    pbes_rewr_tool()
      : pbes_rewriter_tool(
          "pbesrewr",
          "Jan friso Groote, Wieger Wesselink",
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
        std::cout << "pbesrewr parameters:" << std::endl;
        std::cout << "  input file:         " << m_input_filename << std::endl;
        std::cout << "  output file:        " << m_output_filename << std::endl;
        std::cout << "  pbes rewriter:      " << m_pbes_rewriter_type << std::endl;
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
        case utilities::prover:
        { // Just ignore.
          break;
        }
      }
      
      // save the result
      p.save(m_output_filename);
      
      return true;
    }
};

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Squadt interactor begin %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/mcrl2_squadt_interface.h>

using namespace utilities;

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface, public pbes_rewr_tool
{

  private:

    static const char*  pbes_file_for_input;  ///< file containing an LPS
    static const char*  pbes_file_for_output; ///< file used to write the output to
    const char* option_rewrite_strategy;
    const char* option_pbes_rewrite_strategy;

    static const char* option_transformation_strategy;

    static bool initialise_types() 
    {
      tipi::datatype::enumeration< pbes_rewriter_type > storage_types;

      storage_types.
          add(simplify, "simplify").
          add(quantifier_all, "quantifier_all").
          add(quantifier_finite, "quantifier_finite");

      return true;
    }

  public:

    /** \brief constructor */
    squadt_interactor() {
      static bool initialised = initialise_types();

      static_cast< void > (initialised); // harmless, and prevents unused variable warnings
    }

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration&);

    int execute(int argc, char** argv)
    { if (utilities::squadt::free_activation(*this, argc, argv))
      { return EXIT_SUCCESS;
      }
      return pbes_rewr_tool::execute(argc,argv);
    }
};

const char* squadt_interactor::pbes_file_for_input  = "pbes_in";
const char* squadt_interactor::pbes_file_for_output = "pbes_out";

const char* squadt_interactor::option_transformation_strategy = "transformation_strategy";

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(pbes_file_for_input, tipi::mime_type("pbes", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;
  using namespace utilities;

  if (!c.option_exists(option_rewrite_strategy)) 
  { c.add_option(option_rewrite_strategy).set_argument_value< 0 >(GS_REWR_JITTY);
  }

  if (!c.option_exists(option_pbes_rewrite_strategy)) 
  { c.add_option(option_pbes_rewrite_strategy).set_argument_value< 0 >(utilities::simplify); 
  }


  /* Create display */
  tipi::tool_display d;


  // Helper for rewrite strategy selection
  mcrl2::utilities::squadt::radio_button_helper < RewriteStrategy > rewrite_strategy_selector(d);

  // Helper for pbes rewrite strategy selection, aka quantifier elimination
  mcrl2::utilities::squadt::radio_button_helper < pbes_rewriter_type > pbes_rewrite_strategy_selector(d);

  layout::vertical_box& m = d.create< vertical_box >();

  m.append(d.create< label >().set_text("Rewrite strategy")).
    append(d.create< horizontal_box >().
                append(rewrite_strategy_selector.associate(GS_REWR_INNER, "Inner")).
#ifdef MCRL2_INNERC_AVAILABLE
                append(rewrite_strategy_selector.associate(GS_REWR_INNERC, "Innerc")).
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
                append(rewrite_strategy_selector.associate(GS_REWR_JITTY, "Jitty")).
                append(rewrite_strategy_selector.associate(GS_REWR_JITTYC, "Jittyc")),
#else
                append(rewrite_strategy_selector.associate(GS_REWR_JITTY, "Jitty")),
#endif
          margins(0,5,0,5)).

  append(d.create< label >().set_text("Quantifier elimination : ")).
    append(d.create< horizontal_box >().
                append(pbes_rewrite_strategy_selector.associate(simplify, "none")).
                append(pbes_rewrite_strategy_selector.associate(quantifier_all, "all")).
                append(pbes_rewrite_strategy_selector.associate(quantifier_finite, "only finite")),
          margins(0,5,0,5));

  button& okay_button = d.create< button >().set_label("OK");

  m.append(d.create< label >().set_text(" ")).
    append(okay_button, layout::right);

  /// Copy values from options specified in the configuration

  if (c.option_exists(option_rewrite_strategy)) 
  { rewrite_strategy_selector.set_selection(
        c.get_option_argument< RewriteStrategy >(option_rewrite_strategy, 0));
  }

  if (c.option_exists(option_pbes_rewrite_strategy)) 
  { pbes_rewrite_strategy_selector.set_selection(
        c.get_option_argument< pbes_rewriter_type >(option_pbes_rewrite_strategy, 0));
  }

  send_display_layout(d.manager(m));

  /* Wait until the ok button was pressed */
  okay_button.await_change();

  c.get_option(option_rewrite_strategy).set_argument_value< 0 >(rewrite_strategy_selector.get_selection());
  c.get_option(option_pbes_rewrite_strategy).set_argument_value< 0 >(pbes_rewrite_strategy_selector.get_selection());
  /* Add output file to the configuration */
  if (c.output_exists(pbes_file_for_output)) {
    tipi::configuration::object& output_file = c.get_output(pbes_file_for_output);

    output_file.location(c.get_output_name(".pbes"));
  }
  else {
    c.add_output(pbes_file_for_output, tipi::mime_type("pbes", tipi::mime_type::application), c.get_output_name(".pbes"));
  }

  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(pbes_file_for_input);
  result &= c.output_exists(pbes_file_for_output);
  result &= c.option_exists(option_rewrite_strategy);
  result &= c.option_exists(option_pbes_rewrite_strategy);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {

  using namespace utilities;
  set_input_filename(c.get_input(pbes_file_for_input).location());
  set_output_filename(c.get_output(pbes_file_for_output).location());
  m_rewrite_strategy = c.get_option_argument< RewriteStrategy >(option_rewrite_strategy, 0);
  m_pbes_rewriter_type = c.get_option_argument< pbes_rewriter_type >(option_pbes_rewrite_strategy, 0);
  bool result = run();

  send_clear_display();

  return result;
}
#endif


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Squadt interactor end   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  try 
  {
#ifdef ENABLE_SQUADT_CONNECTIVITY
     squadt_interactor tool;
#else 
     pbes_rewr_tool tool;
#endif
    return tool.execute(argc, argv);
  }
  catch (std::exception& e) 
  {
    std::cerr << e.what() << std::endl;
  }
  return EXIT_FAILURE;
}
