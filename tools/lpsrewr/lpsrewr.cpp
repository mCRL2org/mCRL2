// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsrewr.cpp
/// \brief A tool to rewrite linear process specifications

#include "boost.hpp" // precompiled headers

#define TOOLNAME "lpsrewr"
#define AUTHOR "Aad Mathijssen"

#include "mcrl2/lps/lps_rewrite.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"

using namespace mcrl2::data;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;

using mcrl2::utilities::tools::input_output_tool;
using mcrl2::utilities::tools::rewriter_tool;

class lps_rewriter_tool : public rewriter_tool<input_output_tool>
{ 
  protected: 
    typedef rewriter_tool<input_output_tool> super;
    
    static Rewriter *rewr;

    bool m_benchmark;
    unsigned long m_bench_times;
  
  //std::string     infilename;
  //std::string     outfilename;
  //RewriteStrategy strategy;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("benchmark", make_mandatory_argument("NUM"),
              "rewrite data expressions NUM times; do not save output", 'b');
    }

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    { super::parse_options(parser);

      m_benchmark = (parser.options.count("benchmark")>0);

      if (m_benchmark) 
      { m_bench_times = parser.option_argument_as< unsigned long >("benchmark");
      }
    }

  public: 
    lps_rewriter_tool()
      : super(
          TOOLNAME,
          AUTHOR,
          "Rewrite data expressions of the LPS in INFILE and save the result to OUTFILE."
          "If OUTFILE is not present, stdout is used. If INFILE is not present, stdin is"
          "used."
        ),
        m_benchmark(false),
        m_bench_times(1)
    {}

    bool run()
    { specification spec;
      spec.load(m_input_filename);
      rewriter rewr=create_rewriter(spec.data());

      if (gsVerbose)
      {
        std::cerr << "lpsrewr parameters:" << std::endl;
        std::cerr << "  input file:         " << m_input_filename << std::endl;
        std::cerr << "  output file:        " << m_output_filename << std::endl;
        std::cerr << "  benchmark:          " << (m_benchmark?"YES":"NO") << std::endl;
        std::cerr << "  number of times:    " << m_bench_times << std::endl;
      }

      if (m_benchmark)
      { std::cerr << "rewriting LPS " << m_bench_times << " times...\n";
      }
      for (unsigned long i=0; i < m_bench_times; i++) 
      { spec=rewrite_lps(spec,rewr);
      }
      spec.save(m_output_filename);
      return true;
    }
};


// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include "mcrl2/utilities/mcrl2_squadt_interface.h"

// Strings containing tags used for communication between lpsrewr and squadt
const char* lps_file_for_input    = "lps_in";
const char* lps_file_for_output   = "lps_out";

class squadt_interactor 
              : public mcrl2::utilities::squadt::mcrl2_tool_interface,
                public lps_rewriter_tool
{
  private:
    static bool initialise_types()
    {
      return true;
    }

  public:

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration&);

    int execute(int argc, char** argv)
    { if (squadt::free_activation(*this, argc, argv))
      { return EXIT_SUCCESS;
      }
      return lps_rewriter_tool::execute(argc,argv);
    }
};

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(lps_file_for_input, tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) 
{ // lpsrewr does not require interaction with squadt.
}

/*
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;


  / * Create display * /
  tipi::tool_display d;

  layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0,5,0,5));

  / * Create and add the top layout manager * /
  button&     okay_button       = d.create< button >().set_label("OK");

  m.append(d.create< label >().set_text(" ")).
    append(okay_button, layout::right);

  send_display_layout(d.manager(m));

  / * Wait until the ok button was pressed * /
  okay_button.await_change();

  / * Add output file to the configuration * /
  if (c.output_exists(lps_file_for_output)) {
    tipi::configuration::object& output_file = c.get_output(lps_file_for_output);

    output_file.location(c.get_output_name(".lps"));
  }
  else {
    c.add_output(lps_file_for_output, tipi::mime_type("lps", tipi::mime_type::application), c.get_output_name(".lps"));
  }

  send_clear_display();
} */

bool squadt_interactor::check_configuration(tipi::configuration const& c) const 
{
  bool result = true;

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) 
{
  input_filename() = c.get_input(lps_file_for_input).location();
  output_filename() = c.get_output(lps_file_for_output).location();

  bool result = run();

  if (result) {
    send_clear_display();
  }

  return (result);
}
#endif //ENABLE_SQUADT_CONNECTIVITY


//Main program
//------------

int main(int argc, char **argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  try
  {
#ifdef ENABLE_SQUADT_CONNECTIVITY
    squadt_interactor tool;
#else
    lps_rewriter_tool tool;
#endif
    return tool.execute(argc, argv);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return EXIT_FAILURE;
}

