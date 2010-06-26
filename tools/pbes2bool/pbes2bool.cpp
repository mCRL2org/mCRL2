// Author(s): Jan Friso Groote // Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bool.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

// ======================================================================
//
// file          : pbes2bool
// date          : 15-04-2007
// version       : 0.1.3
//
// author(s)     : Alexander van Dam <avandam@damdonk.nl>
//                 Jan Friso Groote <J.F.Groote@tue.nl>
//
// ======================================================================


#define NAME "pbes2bool"
#define AUTHOR "Jan Friso Groote"

//C++
#include <iostream>
#include <string>
#include <utility>

#include <sstream>

//Tool framework
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/pbes_rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

//Data Framework
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/data/data_equation.h" // for debug std::cerr

//Boolean equation systems
#include "mcrl2/pbes/utility.h"
#include "mcrl2/pbes/bes_deprecated.h"
#include "mcrl2/pbes/pbesrewr.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace std;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using bes::bes_expression;
using namespace ::bes;

// using atermpp::make_substitution;

//Function declarations used by main program
//------------------------------------------

using namespace mcrl2;
using utilities::tools::input_tool;
using utilities::tools::rewriter_tool;
using utilities::tools::pbes_rewriter_tool;
using namespace mcrl2::utilities::tools;

class pbes2bool_tool: public squadt_tool< pbes_rewriter_tool<rewriter_tool<input_tool> > >
{
  protected:
    // Tool options.
    /// The output file name
    std::string m_output_filename;
    std::string opt_outputformat;              // The output format
    ::bes::transformation_strategy opt_strategy; // The strategy
    bool opt_use_hashtables;                   // The hashtable option
    bool opt_construct_counter_example;        // The counter example option
    bool opt_store_as_tree;                    // The tree storage option
    bool opt_data_elm;                         // The data elimination option
    std::string opt_counter_example_file;      // The counter example file name

    typedef squadt_tool< pbes_rewriter_tool<rewriter_tool<input_tool> > > super;

    std::string default_rewriter() const
    { return "quantifier-all";
    }

    /// \brief Checks if the number of positional options is OK.
    /// \param parser A command line parser
    void check_positional_options(const command_line_parser& parser)
    {
      if (2 < parser.arguments.size())
      {
        parser.error("too many file arguments");
      }
    }

    /// \brief Returns a message about the output filename
    std::string output_file_message() const
    {
      std::ostringstream out;
      out << "Output written to " << ((m_output_filename.empty())? "standard output" : ("'" + m_output_filename + "'"));
      return out.str();
    }

    /// \brief Adds a message about input and output files to the given description.
    std::string make_tool_description(const std::string& description) const
    {
      // return core::word_wrap_text(description + " If INFILE is not present, standard input is used. If OUTFILE is not present, standard output is used.");
      return description + " If INFILE is not present, standard input is used. If OUTFILE is not present, standard output is used.";
    }

    // Overload synopsis to cope with optional OUTFILE
    std::string synopsis() const
    {
      return "[OPTION]...[INFILE [OUTFILE]]\n";
    }

  public:
    pbes2bool_tool()
      : super(
          NAME,
          AUTHOR,
          "Generate a BES from a PBES and solve it, unless indicated otherwise",
          "Solves PBES from INFILE, or if so indicated writes an equivalent BES to OUTFILE. "
          "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used."),
        opt_outputformat("none"),
        opt_strategy(::bes::lazy),
        opt_use_hashtables(false),
        opt_construct_counter_example(false),
        opt_store_as_tree(false),
        opt_data_elm(true),
        opt_counter_example_file("")
    {}

    /// \brief Returns a const reference to the output filename.
    const std::string& output_filename() const
    {
      return m_output_filename;
    }

    /// \brief Returns a reference to the output filename.
    std::string& output_filename()
    {
      return m_output_filename;
    }

  protected:
    void parse_options(const command_line_parser& parser)
    { super::parse_options(parser);

      input_tool::parse_options(parser);
      if (1 < parser.arguments.size())
      {
        m_output_filename = parser.arguments[1];
      }

      opt_use_hashtables            = 0 < parser.options.count("hashtables");
      opt_construct_counter_example = 0 < parser.options.count("counter");
      opt_store_as_tree             = 0 < parser.options.count("tree");
      opt_data_elm                  = parser.options.count("unused-data") == 0;
      opt_outputformat              = "none";
      opt_strategy                  = lazy;

      if (parser.options.count("output")) { // Output format
        std::string format = parser.option_argument("output");

        if (!((format == "none") || (format == "vasy") || (format == "cwi") || (format == "pbes"))) {
          parser.error("unknown output format specified (got `" + format + "')");
        }

        opt_outputformat = format;
      }

      if (parser.options.count("strategy")) { // Bes solving strategy (currently only one available)
        int strategy = parser.option_argument_as< int >("strategy");

        switch (strategy) {
          case 0:
           opt_strategy = lazy;
           break;
          case 1:
           opt_strategy = optimize;
           break;
          case 2:
           opt_strategy = on_the_fly;
           break;
          case 3:
           opt_strategy = on_the_fly_with_fixed_points;
           break;
          default:
            parser.error("unknown strategy specified: available strategies are '0', '1', '2', and '3'");
        }
      }
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.
        add_option("strategy", make_mandatory_argument("STRAT"),
          "use strategy STRAT (default '0');\n"
          " 0) Compute all boolean equations which can be reached"
          " from the initial state, without optimization"
          " (default). This is is the most data efficient"
          " option per generated equation.\n"
          " 1) Optimize by immediately substituting the right"
          " hand sides for already investigated variables"
          " that are true or false when generating an"
          " expression. This is as memory efficient as 0.\n"
          " 2) In addition to 1, also substitute variables that"
          " are true or false into an already generated right"
          " hand side. This can mean that certain variables"
          " become unreachable (e.g. X0 in X0 && X1, when X1"
          " becomes false, assuming X0 does not occur"
          " elsewhere. It will be maintained which variables"
          " have become unreachable as these do not have to be"
          " investigated. Depending on the PBES, this can"
          " reduce the size of the generated BES substantially"
          " but requires a larger memory footprint.\n"
          " 3) In addition to 2, investigate for generated"
          " variables whether they occur on a loop, such that"
          " they can be set to true or false, depending on the"
          " fixed point symbol. This can increase the time"
          " needed to generate an equation substantially",
          's').
        add_option("counter",
          "print at the end a tree labelled with instantiations "
          "of the left hand side of equations; this tree is an "
          "indication of how pbes2bool came to the validity or "
          "invalidity of the PBES",
          'c').
        /* add_option("precompile",
          "precompile the pbes for faster rewriting; Deprecated. Does not "
          "work anymore. Only present for compatibility reasons",
          'p'). */
        add_option("hashtables",
          "use hashtables when substituting in bes equations, "
          "and translate internal expressions to binary decision "
          "diagrams (discouraged, due to performance)",
          'H').
        add_option("output",
          make_mandatory_argument("FORMAT"),
          "use output format FORMAT:\n"
          " 'none' (default),\n"
          " 'vasy',\n"
          " 'pbes' (save as a PBES in internal format),\n"
          " 'cwi'",
          'o').
        add_option("tree",
          "store state in a tree (for memory efficiency)",
          't').
        add_option("unused_data",
          "do not remove unused parts of the data specification",
          'u');
    }

  public:
    bool run()
    {
      using namespace pbes_system;
      using namespace utilities;

      if (core::gsVerbose)
      {
        std::cerr << "pbes2bool parameters:" << std::endl;
        std::cerr << "  input file:         " << m_input_filename << std::endl;
        std::cerr << "  output file:        " << m_output_filename << std::endl;
        std::cerr << "  data rewriter:      " << m_rewrite_strategy << std::endl;
        std::cerr << "  pbes rewriter:      " << m_pbes_rewriter_type << std::endl;
      }

      // load the pbes
      mcrl2::pbes_system::pbes<> p;
      p.load(m_input_filename);
      p.normalize();
      p.instantiate_global_variables();
      // data rewriter
      /* for (std::set <mcrl2::data::data_equation >::const_iterator i=p.data().equations();
               i!=p.data().equations(); ++i)
      { std::cerr << "AKAKAKAKAKAK " << *i << "\n";
      } */
      data::rewriter datar= (opt_data_elm) ?
            data::rewriter(p.data(), mcrl2::data::used_data_equation_selector(p.data(), pbes_to_aterm(p)), rewrite_strategy()) :
            data::rewriter(p.data(), rewrite_strategy());

      ::bes::boolean_equation_system bes_equations=
                    ::bes::boolean_equation_system(
                            p,
                            datar,
                            opt_strategy,
                            opt_store_as_tree,
                            opt_construct_counter_example,
                            opt_use_hashtables);

      
      // pbes rewriter
      /* The code below can be reactivated, once the pbes_rewriters deliver acceptable performance.
         As it stands their performance is so bad, that they cannot be used.

      switch (rewriter_type())
      {
        case simplify:
        {
          simplifying_rewriter<pbes_expression, data::rewriter> pbesr(datar);
          pbesrewr(p,pbesr); // Simplify p such that it does not have to be done
                             // repeatedly.
          bes_equations=::bes::boolean_equation_system(
                            p,
                            pbesr,
                            opt_strategy,
                            opt_store_as_tree,
                            opt_construct_counter_example,
                            opt_use_hashtables);
          break;
        }
        case quantifier_finite:
        {
          data::number_postfix_generator generator("UNIQUE_PREFIX");
          data::data_enumerator<> datae(p.data(), datar, generator);
          data::rewriter_with_variables datarv(datar);
          bool enumerate_infinite_sorts = false;
          enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables,
                                                             data::data_enumerator<> >
                          pbesr(datarv, datae, enumerate_infinite_sorts);
          pbesrewr(p,pbesr);  // Simplify p such that this does not need to be done
                              // repeatedly. 
          bes_equations=::bes::boolean_equation_system(
                            p,
                            pbesr,
                            opt_strategy,
                            opt_store_as_tree,
                            opt_construct_counter_example,
                            opt_use_hashtables);
          break;
        }
        case quantifier_all:
        {
          data::number_postfix_generator generator("UNIQUE_PREFIX");
          data::data_enumerator<> datae(p.data(), datar, generator);
          data::rewriter_with_variables datarv(datar);
          const bool enumerate_infinite_sorts1 = false;
          enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables,
                                                             data::data_enumerator<> >
                          pbesr1(datarv, datae, enumerate_infinite_sorts1);
          pbesrewr(p,pbesr1);  // Simplify p such that this does not need to be done
                               // repeatedly, without expanding quantifiers over infinite
                               // domains. 
          const bool enumerate_infinite_sorts2 = true;
          enumerate_quantifiers_rewriter<pbes_expression, data::rewriter_with_variables,
                                                             data::data_enumerator<> >
                          pbesr2(datarv, datae, enumerate_infinite_sorts2);
          bes_equations=::bes::boolean_equation_system(
                            p,
                            pbesr2,
                            opt_strategy,
                            opt_store_as_tree,
                            opt_construct_counter_example,
                            opt_use_hashtables);
          break;
        }
        case pfnf:
        {
          throw mcrl2::runtime_error("The pfnf boolean equation rewriter cannot be used\n");
        }
        case prover:
        {
          throw mcrl2::runtime_error("The prover based rewriter cannot be used\n");
        }
      } */

      if (opt_outputformat == "cwi")
      { // in CWI format only if the result is a BES, otherwise Binary
        save_bes_in_cwi_format(m_output_filename,bes_equations);
        return true;
      }
      if (opt_outputformat == "vasy")
      { //Save resulting bes if necessary.
        save_bes_in_vasy_format(m_output_filename,bes_equations);
        return true;
      }
      if (opt_outputformat == "pbes")
      { //Save resulting bes if necessary.
        save_bes_in_pbes_format(m_output_filename,bes_equations,p);
        return true;
      }

      assert(opt_outputformat=="none");
      bool result=solve_bes(bes_equations,
                            opt_use_hashtables,
                            opt_construct_counter_example);
      core::gsMessage("The solution for the initial variable of the pbes is %s\n",
                       ( result ? "true" : "false"));

      if (opt_construct_counter_example)
#ifdef NDEBUG // Precompile the data terms to compiler format.
      { bes_equations.print_counter_example(true,datar.get_internal_rewriter(),opt_store_as_tree,opt_counter_example_file);
      }
#else // Use data terms in standard format, and do not allow a tree format to store arguments, as this is not accepted
      // by the internal consistency checks.
      { bes_equations.print_counter_example(false,datar.get_internal_rewriter(),false,opt_counter_example_file);
      }
#endif
      return true;
    }

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY

# define counter_example_file_for_output "counter_example"
# define option_transformation_strategy  "transformation_strategy"
# define option_selected_output_format   "selected_output_format"
// const char* option_precompile            = "precompile";
# define option_counter                  "counter"
# define option_hash_table               "hash_table"
# define option_tree                     "tree"
# define option_unused_data              "unused_data"

  private:

    enum bes_output_format
    {
      none,
      vasy,
      cwi,
      pbes
    };

    static bool initialise_types()
    {
      tipi::datatype::enumeration< transformation_strategy > transformation_strategy_enumeration;

      transformation_strategy_enumeration.
        add(lazy, "lazy").
        add(optimize, "optimize").
        add(on_the_fly, "on-the-fly").
        add(on_the_fly_with_fixed_points, "on-the-fly-with-fixed-points");

      tipi::datatype::enumeration< bes_output_format > output_format_enumeration;

      output_format_enumeration.
        add(none, "none").
        add(vasy, "vasy").
        add(cwi, "cwi").
        add(cwi, "pbes");

      return true;
    }

  public:

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& c) const
    {
      static bool initialised = initialise_types();

      static_cast< void > (initialised); // harmless, and prevents unused variable warnings

      c.add_input_configuration("main-input",
                 tipi::mime_type("pbes", tipi::mime_type::application), tipi::tool::category::transformation);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration& c)
    {
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;
      using namespace mcrl2;

      // Let squadt_tool update configuration for rewriter and add output file configuration
      if (c.output_exists("main-output")) {
        m_output_filename = c.get_output("main-output").location();
      } 
      synchronise_with_configuration(c);

      /* if (!c.option_exists(option_precompile)) {
        c.add_option(option_precompile).set_argument_value< 0 >(false);
      } */
      if (!c.option_exists(option_counter)) {
        c.add_option(option_counter).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_hash_table)) {
        c.add_option(option_hash_table).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_tree)) {
        c.add_option(option_tree).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_unused_data)) {
        c.add_option(option_unused_data).set_argument_value< 0 >(true);
      }
      if (!c.option_exists(option_transformation_strategy)) {
        c.add_option(option_transformation_strategy).set_argument_value< 0 >(lazy);
      }
      if (!c.option_exists(option_selected_output_format)) {
        c.add_option(option_selected_output_format).set_argument_value< 0 >(none);
      }

      /* Create display */
      tipi::tool_display d;

      // Helper for format selection
      mcrl2::utilities::squadt::radio_button_helper < bes_output_format > format_selector(d);

      // Helper for strategy selection
      mcrl2::utilities::squadt::radio_button_helper < transformation_strategy > strategy_selector(d);

      layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0,5,0,5));

      // checkbox& precompile(d.create< checkbox >().set_status(c.get_option_argument< bool >(option_precompile)));
      checkbox& counter(d.create< checkbox >().set_status(c.get_option_argument< bool >(option_counter)));
      checkbox& hash_table(d.create< checkbox >().set_status(c.get_option_argument< bool >(option_hash_table)));
      checkbox& tree(d.create< checkbox >().set_status(c.get_option_argument< bool >(option_tree)));
      checkbox& unused_data(d.create< checkbox >().set_status(c.get_option_argument< bool >(option_unused_data)));

      add_rewrite_option(d, m);
      add_pbes_rewrite_option(d, m);

      m.append(d.create< label >().set_text("Output format : ")).
        append(d.create< horizontal_box >().
                    append(format_selector.associate(none, "none")).
                    append(format_selector.associate(vasy, "vasy")).
                    append(format_selector.associate(pbes, "pbes")).
                    append(format_selector.associate(cwi, "cwi")),
              margins(0,5,0,5)).
        append(d.create< label >().set_text("Strategy to generate a BES from a PBES: "), margins(8,5,0,5)).
        append(d.create< vertical_box >().
            append(strategy_selector.associate(lazy, "0: without optimisation")).
            append(strategy_selector.associate(optimize, "1: forward substitution of true/false")).
            append(strategy_selector.associate(on_the_fly, "2: full substitution of true/false")).
            append(strategy_selector.associate(on_the_fly_with_fixed_points, "3: full substitution and cycle detection")),
              margins(0,5,8,5)).
        append(d.create< horizontal_box >().
            append(d.create< vertical_box >().
                // append(precompile.set_label("precompile for faster rewriting")).
                append(counter.set_label("produce a counter example")).
                append(unused_data.set_label("remove unused data"))).
            append(d.create< vertical_box >().
                append(hash_table.set_label("use hash tables and translation to BDDs")).
                append(tree.set_label("store state in a tree (memory efficiency)"))));

      button& okay_button = d.create< button >().set_label("OK");

      m.append(d.create< label >().set_text(" ")).
        append(okay_button, layout::right);

      /// Copy values from options specified in the configuration
      if (c.option_exists(option_transformation_strategy)) {
        strategy_selector.set_selection(
            c.get_option_argument< transformation_strategy >(option_transformation_strategy, 0));
      }
      if (c.option_exists(option_selected_output_format)) {
        format_selector.set_selection(
            c.get_option_argument< bes_output_format >(option_selected_output_format, 0));
      }

      send_display_layout(d.manager(m));

      /* Wait until the ok button was pressed */
      okay_button.await_change();

      c.get_option(option_transformation_strategy).set_argument_value< 0 >(strategy_selector.get_selection());
      c.get_option(option_selected_output_format).set_argument_value< 0 >(format_selector.get_selection());

      if (c.get_option_argument< bes_output_format >(option_selected_output_format) != none)
      {
        /* Add output file to the configuration */
        if (c.output_exists("main-output")) {
          tipi::configuration::object& output_file = c.get_output("main-output");

          output_file.location(c.get_output_name(".txt"));
        }
        else {
          c.add_output("main-output", tipi::mime_type("txt", tipi::mime_type::application),
                       c.get_output_name(".txt"));
        }
      }

      // c.get_option(option_precompile).set_argument_value< 0, boolean >(precompile.get_status());
      c.get_option(option_counter).set_argument_value< 0, boolean >(counter.get_status());
      c.get_option(option_hash_table).set_argument_value< 0, boolean >(hash_table.get_status());
      c.get_option(option_tree).set_argument_value< 0, boolean >(tree.get_status());
      c.get_option(option_unused_data).set_argument_value< 0, boolean >(unused_data.get_status());

      send_clear_display();

      if(c.output_exists("main-output"))
      {
        m_output_filename = c.get_output("main-output").location();
      }

      update_configuration(c);
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& c) const
    {
      return c.input_exists("main-input") &&
             c.option_exists(option_transformation_strategy) &&
             c.option_exists(option_selected_output_format) &&
            ((c.get_option_argument< bes_output_format >(option_selected_output_format)==none)
                             || c.output_exists("main-output"));
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& c)
    {
      // static std::string strategies[] = { "lazy", "fly" };
      static std::string formats[]    = { "none", "vasy", "cwi", "pbes" };

      // Let squadt_tool update configuration for rewriter and add output file
      // configuration
      if (c.output_exists("main-output")) {
          m_output_filename = c.get_output("main-output").location();
      }

      synchronise_with_configuration(c);

      opt_construct_counter_example = c.get_option_argument< bool >(option_counter);
      opt_store_as_tree             = c.get_option_argument< bool >(option_tree);
      opt_data_elm                  = c.get_option_argument< bool >(option_unused_data);;
      opt_use_hashtables            = c.get_option_argument< bool >(option_hash_table);;

      if (opt_construct_counter_example && !c.output_exists(counter_example_file_for_output)) {
        opt_counter_example_file = c.get_output_name(".txt").c_str();

        c.add_output(counter_example_file_for_output, tipi::mime_type("txt", tipi::mime_type::text),
                     opt_counter_example_file);
      }

      opt_outputformat = formats[c.get_option_argument< bes_output_format >(option_selected_output_format)];

      opt_strategy = c.get_option_argument< transformation_strategy >(option_transformation_strategy, 0);

      bool result=run();

      send_clear_display();

      return result;
    }
#endif
};

class pbes2bool_gui_tool: public mcrl2_gui_tool<pbes2bool_tool> {
public:
	pbes2bool_gui_tool() {

		std::vector<std::string> values;

		m_gui_options["counter"] = create_checkbox_widget();
		m_gui_options["hashtables"] = create_checkbox_widget();

		values.clear();
		values.push_back("none");
		values.push_back("vasy");
		values.push_back("pbes");
		values.push_back("cwi");
		m_gui_options["output"] = create_radiobox_widget(values);

		values.clear();
		values.push_back("simplify");
		values.push_back("quantifier-all");
		values.push_back("quantifier-finite");
		values.push_back("pfnf");
		m_gui_options["pbes-rewriter"] = create_radiobox_widget(values);

		values.clear();
		values.push_back("jitty");
		values.push_back("jittyc");
		values.push_back("jittyc");
		values.push_back("inner");
		values.push_back("innep");
		values.push_back("innec");
		m_gui_options["rewriter"] = create_radiobox_widget(values);

		values.clear();
		values.push_back("0");
		values.push_back("1");
		values.push_back("2");
		values.push_back("3");
		m_gui_options["strategy"] = create_radiobox_widget(values);

		m_gui_options["tree"] = create_checkbox_widget();
		m_gui_options["unused_data"] = create_checkbox_widget();
	}
};

int main(int argc, char* argv[])
{
   MCRL2_ATERMPP_INIT(argc, argv)

   return pbes2bool_gui_tool().execute(argc, argv);
}
