// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/input_output_tool.h"

#include "mcrl2/lps/symbolic_lts_io.h"
#include "mcrl2/lps/symbolic_lts_bisim.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/lts/lts_builder.h"
#include "mcrl2/lts/state_space_generator.h"
#include "mcrl2/symbolic/print.h"
#include "mcrl2/utilities/logger.h"

#include "convert_concrete_lts.h"

using namespace mcrl2;
using namespace mcrl2::lts;
using namespace mcrl2::lts::detail;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::log;
using namespace sylvan::ldds;

using mcrl2::utilities::tools::input_output_tool;

enum class symbolic_lts_equivalence
{
  none,
  bisim
};

// \overload
inline
std::istream& operator>>(std::istream& is, symbolic_lts_equivalence& eq)
{
  try
  {
    std::string s;
    is >> s;

    if (s == "none") {
      eq = symbolic_lts_equivalence::none;
    }
    else if (s == "bisim") {
      eq = symbolic_lts_equivalence::bisim;
    }
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

// \overload
inline
std::ostream& operator<<(std::ostream& os, const symbolic_lts_equivalence eq)
{
  switch (eq)
  {
    case symbolic_lts_equivalence::none:
    {
      os << "none";
      break;
    }
    case symbolic_lts_equivalence::bisim:
    {
      os << "bisim";
      break;
    }
  }

  return os;
}

inline std::string description(const symbolic_lts_equivalence eq)
{
  switch(eq)
  {
    case symbolic_lts_equivalence::none:
      return "identity equivalence to concrete LTS";
    case symbolic_lts_equivalence::bisim:
      return "naive symbolic strong bisimilarity algorithm requiring O(n^2) symbolic operations.";
  }
}

class ltsconvert_tool : public input_output_tool
{
    typedef input_output_tool super;

    // Lace options
    std::size_t lace_n_workers = 1;
    std::size_t lace_dqsize = 1024*1024*4; // set large default
    std::size_t lace_stacksize = 0; // use default

    // Sylvan options
    std::size_t memory_limit = 3;
    std::size_t initial_ratio = 16;
    std::size_t table_ratio = 1;

  public:
    ltsconvert_tool() :
      input_output_tool("ltsconvertsymbolic", "Maurice Laveaux",
                      "applies various conversions to symbolic LTSs",
                      "")
    {}
    
    void add_options(utilities::interface_description& desc) override
    {
      super::add_options(desc);

      desc.add_option("equivalence", 
        make_enum_argument<symbolic_lts_equivalence>("NAME")
          .add_value(symbolic_lts_equivalence::none, true)
          .add_value(symbolic_lts_equivalence::bisim),
          "generate an equivalent LTS, preserving equivalence NAME:",
          'e');
      desc.add_option("lace-workers", utilities::make_optional_argument("NUM", "1"), "set number of Lace workers (threads for parallelization), (0=autodetect, default 1)");
      desc.add_option("lace-dqsize", utilities::make_optional_argument("NUM", "4194304"), "set length of Lace task queue (default 1024*1024*4)");
      desc.add_option("lace-stacksize", utilities::make_optional_argument("NUM", "0"), "set size of program stack in kilobytes (0=default stack size)");
      desc.add_option("memory-limit", utilities::make_optional_argument("NUM", "3"), "Sylvan memory limit in gigabytes (default 3)", 'm');

      desc.add_option("out", utilities::make_mandatory_argument("FORMAT"), "use FORMAT as the output format.", 'o');
    }

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);

      if (parser.has_option("lace-workers"))
      {
        lace_n_workers = parser.option_argument_as<int>("lace-workers");
      }
      if (parser.has_option("lace-dqsize"))
      {
        lace_dqsize = parser.option_argument_as<int>("lace-dqsize");
      }
      if (parser.has_option("lace-stacksize"))
      {
        lace_stacksize = parser.option_argument_as<int>("lace-stacksize");
      }
      if (parser.has_option("memory-limit"))
      {
        memory_limit = parser.option_argument_as<std::size_t>("memory-limit");
      }
      if (parser.has_option("initial-ratio"))
      {
        initial_ratio = parser.option_argument_as<std::size_t>("initial-ratio");
        if (!utilities::is_power_of_two(initial_ratio))
        {
          throw mcrl2::runtime_error("The initial-ratio should be a power of two.");
        }
      }
      if (parser.has_option("table-ratio"))
      {
        table_ratio = parser.option_argument_as<std::size_t>("table-ratio");
        if (!utilities::is_power_of_two(table_ratio))
        {
          throw mcrl2::runtime_error("The table-ratio should be a power of two.");
        }
      }
      if (parser.options.count("out"))
      {
        if (1 < parser.options.count("out"))
        {
          mCRL2log(warning) << "multiple output formats specified; can only use one\n";
        }

        outtype = mcrl2::lts::detail::parse_format(parser.option_argument("out"));

        if (outtype == lts_none)
        {
          mCRL2log(warning) << "format '" << parser.option_argument("out") <<
                    "' is not recognised; option ignored" << std::endl;
        }
      }

      if (2 < parser.arguments.size())
      {
        parser.error("too many file arguments");
      }
      
      m_equivalence = parser.option_argument_as<symbolic_lts_equivalence>("equivalence");
    }
    
    bool run() override
    {      
      lace_start(lace_n_workers, lace_dqsize);
      lace_set_stacksize(lace_stacksize);
      sylvan::sylvan_set_limits(memory_limit * 1024 * 1024 * 1024, std::log2(table_ratio), std::log2(initial_ratio));
      sylvan::sylvan_init_package();
      sylvan::sylvan_init_ldd();

      if (input_filename().empty() || input_filename() == "-")
      {
        std::cin >> m_input;
      }
      else
      {
        std::ifstream ifs(input_filename(), std::ifstream::in | std::ios_base::binary);
        if (!ifs.good())
        {
          throw mcrl2::runtime_error("Could not open file " + input_filename() + ".");
        }
        ifs >> m_input;
      }

      if (m_equivalence == symbolic_lts_equivalence::none)
      {
        // Convert into a concrete LTS.
        lps::specification lpsspec;
        lps::explorer_options options;
        options.save_at_end = false;
        
        if (outtype == lts_none)
        {
          mCRL2log(verbose) << "Trying to detect output format by extension..." << std::endl;

          outtype = mcrl2::lts::detail::guess_format(output_filename(), true);
        }
        
        std::unique_ptr<lts_builder> builder = create_lts_builder(lpsspec, options, outtype, output_filename());

        convert_concrete_lts algorithm(m_input, std::move(builder));
        algorithm.run();
        algorithm.save(output_filename());
      }
      else
      {
        bisim(m_input);
      }

      sylvan::sylvan_quit();
      lace_stop();
      return true;
    }

  private:
    lts_type outtype = lts_none;
    symbolic_lts_equivalence m_equivalence = symbolic_lts_equivalence::none;

    lps::symbolic_lts m_input;
};

int main(int argc, char* argv[])
{
  return ltsconvert_tool().execute(argc, argv);
}