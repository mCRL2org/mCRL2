// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl22lps.cpp
/// \brief This tool linearises mcrl2 specifications into linear
///         form.

#include "boost.hpp" // precompiled headers

#define TOOLNAME "mcrl22lps"
#define AUTHOR "Jan Friso Groote"

#include <fstream>
#include <iostream>
#include <string>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/atermpp/aterm_init.h"

// #include "gc.h"  Required for ad hoc garbage collection. This is possible with ATcollect,
// useful to find garbage collection problems.

using namespace mcrl2::utilities;
using namespace mcrl2::data::detail;
using namespace mcrl2::process;
using namespace mcrl2::log;

using mcrl2::utilities::tools::input_output_tool;
using mcrl2::utilities::tools::rewriter_tool;
using mcrl2::lps::t_lin_method;
using mcrl2::lps::lmStack;
using mcrl2::lps::lmRegular;
using mcrl2::lps::lmRegular2;
using mcrl2::lps::t_lin_options;
using mcrl2::lps::linearise;

class mcrl22lps_tool : public rewriter_tool< input_output_tool >
{
    typedef rewriter_tool< input_output_tool > super;

  private:
    t_lin_options m_linearisation_options;
    bool noalpha;   // indicates whether alpha reduction is needed.
    bool opt_check_only;

  protected:

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("lin-method", make_mandatory_argument("NAME"),
                      "use linearisation method NAME:\n"
                      "  'regular' for generating an LPS in regular form\n"
                      "  (specification should be regular, default),\n"
                      "  'regular2' for a variant of 'regular' that uses more data variables\n"
                      "  (useful when 'regular' does not work), or\n"
                      "  'stack' for using stack data types\n"
                      "  (useful when 'regular' and 'regular2' do not work)"
                      , 'l');
      desc.add_option("cluster",
                      "all actions in the final LPS are clustered. "
                      "Clustering means that summands with the same action labels are "
                      "grouped together. For instance, a(f1) . P(g1) + a(f2) . P(g2) "
                      "is replaced by sum b: Bool . a(if(b, f1, f2)) . P(if(b, f2, g2)). "
                      "The advantage is that the number of summands can be reduced "
                      "subtantially in this way. The disadvantage is that sum operators "
                      "are introduced and new data sorts with auxiliary functions are generated. "
                      "In order to avoid the generation of new sorts, the option -b/--binary "
                      "can be used.", 'c');
      desc.add_option("no-cluster",
                      "the actions in intermediate LPSs are not clustered before "
                      "they are put in parallel. By default these processes are "
                      "clustered to avoid a blow-up in the number of summands when "
                      "transforming two parallel linear processes into a single linear "
                      "process. If a linear process with M summands is put in parallel "
                      "with a linear process with N summands the resulting process has "
                      "M×N + M + N summands. Both M and N can be substantially reduced "
                      "by clustering at the cost of introducing new sorts and functions. "
                      "See -c/--cluster, esp. for a short explanation of the clustering "
                      "process.", 'n');                      
      desc.add_option("no-alpha",
                      "alphabet reductions are not applied."
                      "By default mcrl22lps attempts to distribute communication, hiding "
                      "and allow operators over the parallel composition operator as "
                      "this reduces the size of intermediate linear processes. By using "
                      "this option, this step can be avoided. The name stems from the "
                      "alphabet axioms in process algebra.", 'z');                      
      desc.add_option("newstate",
                      "state variables are encoded using enumerated types instead "
                      "of positive natural numbers (Pos). By using this option new "
                      "finite sorts named Enumk are generated where k is the size of "
                      "the domain. Also, auxiliary case functions and equalities are "
                      "defined. In combination with the option --binary the finite "
                      "sorts are encoded by booleans. "
                      "(requires linearisation method 'regular' or 'regular2').", 'w');
      desc.add_option("binary",
                      "when clustering use binary case functions instead of "
                      "n-ary; in the presence of -w/--newstate, state variables are "
                      "encoded by a vector of boolean variables", 'b');
      desc.add_option("statenames",
                      "the names of generated data parameters are extended with the "
                      "name of the process in which they occur. This makes it "
                      "easier to determine where the parameter comes from.", 'a');
      desc.add_option("no-rewrite",
                      "do not rewrite data terms while linearising; useful when the rewrite "
                      "system does not terminate. This option also switches off the application of "
                      "constant elimination.", 'o');
      desc.add_option("no-globvars",
                      "instantiate don't care values with arbitrary constants, "
                      "instead of modelling them by global variables. This has no effect"
                      "on global variables that are declared in the specification.", 'f');
      desc.add_option("no-sumelm",
                      "avoid applying sum elimination in parallel composition", 'm');
      desc.add_option("no-deltaelm",
                      "avoid removing spurious delta summands. "
                      "Due to the existence of time, delta summands cannot be omitted. "
                      "Due to the presence of multi-actions the number of summands can "
                      "be huge. The algorithm for removing delta summands simply works "
                      "by comparing each delta summand with each other summand to see "
                      "whether the condition of the one implies the condition of the "
                      "other. Clearly, this has quadratic complexity, and can take a "
                      "long time.", 'g');
      desc.add_option("delta",
                      "add a true->delta summands to each state in each process; "
                      "these delta's subsume all other conditional timed delta's, "
                      "effectively reducing the number of delta summands drastically "
                      "in the resulting linear process; speeds up linearisation. This is the default, "
                      "but it does not deal correctly with time.", 'D');
      desc.add_option("timed",
                      "Translate the process to linear form preserving all timed information. In parallel processes the number "
                      "of possible time constraints can be large, slowing down linearisation. Confer the --delta option "
                      "which yiels a much faster translation that does not preserve timing correctly", 'T');
      desc.add_option("no-constelm",
                      "do not try to apply constant elimination when generating a linear "
                      "process.");
      desc.add_option("check-only",
                      "check syntax and static semantics; do not linearise", 'e');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      opt_check_only                                  = 0 < parser.options.count("check-only");
      noalpha                                         = 0 < parser.options.count("no-alpha");
      m_linearisation_options.final_cluster           = 0 < parser.options.count("cluster");
      m_linearisation_options.no_intermediate_cluster = 0 < parser.options.count("no-cluster");
      m_linearisation_options.newstate                = 0 < parser.options.count("newstate");
      m_linearisation_options.binary                  = 0 < parser.options.count("binary");
      m_linearisation_options.statenames              = 0 < parser.options.count("statenames");
      m_linearisation_options.norewrite               = 0 < parser.options.count("no-rewrite");
      m_linearisation_options.noglobalvars            = 0 < parser.options.count("no-globvars");
      m_linearisation_options.nosumelm                = 0 < parser.options.count("no-sumelm");
      m_linearisation_options.nodeltaelimination      = 0 < parser.options.count("no-deltaelm");
      m_linearisation_options.add_delta               = 0 == parser.options.count("timed");
      m_linearisation_options.do_not_apply_constelm   = 0 < parser.options.count("no-constelm") ||
                                                        0 < parser.options.count("no-rewrite");
      m_linearisation_options.lin_method = lmRegular;

      if (0 < parser.options.count("lin-method"))
      {
        if (1 < parser.options.count("lin-method"))
        {
          parser.error("multiple use of option -l/--lin-method; only one occurrence is allowed");
        }
        std::string lin_method_str(parser.option_argument("lin-method"));
        if (lin_method_str == "stack")
        {
          m_linearisation_options.lin_method = lmStack;
        }
        else if (lin_method_str == "regular")
        {
          m_linearisation_options.lin_method = lmRegular;
        }
        else if (lin_method_str == "regular2")
        {
          m_linearisation_options.lin_method = lmRegular2;
        }
        else
        {
          parser.error("option -l/--lin-method has illegal argument '" + lin_method_str + "'");
        }
      }

      //check for dangerous and illegal option combinations
      if (m_linearisation_options.newstate && m_linearisation_options.lin_method == lmStack)
      {
        parser.error("option -w/--newstate cannot be used with -lstack/--lin-method=stack");
      }

      m_linearisation_options.infilename       = input_filename();
      m_linearisation_options.outfilename      = output_filename();
      m_linearisation_options.rewrite_strategy = rewrite_strategy();
    }

  public:

    mcrl22lps_tool() : super(
        TOOLNAME,
        AUTHOR,
        "translate an mCRL2 specification to an LPS",
        "Linearises the mCRL2 specification in INFILE and writes the resulting LPS to "
        "OUTFILE. If OUTFILE is not present, stdout is used. If INFILE is not present, "
        "stdin is used."), noalpha(false), opt_check_only(false)
    {}

    bool run()
    {
      //linearise infilename with options
      process_specification spec;
      if (m_linearisation_options.infilename.empty())
      {
        //parse specification from stdin
        mCRL2log(verbose) << "reading input from stdin..." << std::endl;
        spec = parse_process_specification(std::cin,!noalpha);
      }
      else
      {
        //parse specification from infilename
        mCRL2log(verbose) << "reading input from file '" <<  m_linearisation_options.infilename << "'..." << std::endl;
        std::ifstream instream(m_linearisation_options.infilename.c_str(), std::ifstream::in|std::ifstream::binary);
        if (!instream.is_open())
        {
          throw mcrl2::runtime_error("cannot open input file: " + m_linearisation_options.infilename);
        }
        spec = parse_process_specification(instream,!noalpha);
        instream.close();
      }
      //report on well-formedness (if needed)
      if (opt_check_only)
      {
        if (m_linearisation_options.infilename.empty())
        {
          mCRL2log(info) << "stdin contains a well-formed mCRL2 specification" << std::endl;
        }
        else
        {
          mCRL2log(info) << "the file '" << m_linearisation_options.infilename << "' contains a well-formed mCRL2 specification" << std::endl;
        }
        return true;
      }
      //store the result
      mcrl2::lps::specification linear_spec(linearise(spec,m_linearisation_options));
      if (m_linearisation_options.outfilename.empty())
      {
        mCRL2log(verbose) << "writing LPS to stdout..." << std::endl;
      }
      else
      {
        mCRL2log(verbose) << "writing LPS to file '" <<  m_linearisation_options.outfilename << "'..." << std::endl;
      }
      linear_spec.save(m_linearisation_options.outfilename);
      return true;
    }
};

class mcrl22lps_gui_tool: public mcrl2_gui_tool<mcrl22lps_tool>
{
  public:
    mcrl22lps_gui_tool()
    {

      m_gui_options["statenames"] = create_checkbox_widget();
      m_gui_options["binary"] = create_checkbox_widget();
      m_gui_options["cluster"] = create_checkbox_widget();
      m_gui_options["delta"] = create_checkbox_widget();
      m_gui_options["check-only"] = create_checkbox_widget();
      m_gui_options["no-globvars"] = create_checkbox_widget();
      m_gui_options["no-deltaelm"] = create_checkbox_widget();
      m_gui_options["check-only"] = create_checkbox_widget();
      m_gui_options["check-only"] = create_checkbox_widget();
      m_gui_options["no-constelm"] = create_checkbox_widget();

      std::vector<std::string> values;
      values.clear();
      values.push_back("regular");
      values.push_back("regular2");
      values.push_back("stack");
      m_gui_options["lin-method"] = create_radiobox_widget(values);
      m_gui_options["no-cluster"] = create_checkbox_widget();
      m_gui_options["no-rewrite"] = create_checkbox_widget();

      add_rewriter_widget();
      m_gui_options["newstate"] = create_checkbox_widget();
      m_gui_options["no-alpha"] = create_checkbox_widget();
    }
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return mcrl22lps_gui_tool().execute(argc, argv);
}

