// Author(s): F.P.M. (Frank) Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./lpsparunfold.cpp

#define NAME "lpsparunfold"
#define AUTHOR "Frank Stappers"

// C++
#include <iostream>
#include <string>
#include <algorithm>
#include <climits>

//mCRL2
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/stochastic_specification.h"

//LPS framework
#include "mcrl2/lps/io.h"

//DATA
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/parse.h"

//LPSPARUNFOLDLIB
#include "mcrl2/lps/lpsparunfoldlib.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"

using namespace mcrl2::utilities;
using namespace mcrl2::data;

using namespace mcrl2;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::log;

using mcrl2::data::tools::rewriter_tool;

class lpsparunfold_tool: public  rewriter_tool<input_output_tool>
{
  protected:

    typedef rewriter_tool<input_output_tool> super;

    std::set< std::size_t > m_set_index; ///< Options of the algorithm
    std::string m_unfoldsort;
    std::size_t m_repeat_unfold;
    bool m_add_distribution_laws;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("index", make_mandatory_argument("[NUM]"),
                      "unfolds process parameters for comma separated indices", 'i');
      desc.add_option("sort", make_mandatory_argument("NAME"),
                      "unfolds all process parameters of sort NAME", 's');
      desc.add_option("repeat", make_mandatory_argument("NUM"),
                      "repeat unfold NUM times", 'n');
      desc.add_option("laws",
                      "generates additional distribution laws for projection and determine functions", 'l');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

//      if (((0 == parser.options.count("index")) && (0 == parser.options.count("sort"))) ||
//          ((0 < parser.options.count("index") && (0 < parser.options.count("sort")))))
//      {
//        parser.error("Use either --sort or --index to unfold process parameters.");
//      }

      // Parse string argument to [NUM] (a set of indices)
      if (0 < parser.options.count("index"))
      {
        std::string indices_str(parser.option_argument_as<std::string>("index"));
        std::stringstream indices(indices_str);

        std::size_t index;
        char comma;
        while (indices.good())
        {
          indices >> index;
          if (indices.bad())
          {
            throw mcrl2::runtime_error("Not a valid comma-separated list of indices: " + indices_str);
          }
          m_set_index.insert(index);
          if (indices.good())
          {
            indices >> comma;
            if (comma != ',')
            {
              throw mcrl2::runtime_error("Not a valid comma-separated list of indices: " + indices_str);
            }
          }
        }
        if (!indices.eof())
        {
          throw mcrl2::runtime_error("Not a valid comma-separated list of indices: " + indices_str);
        }
      }

      // Parse string argument NAME for sort argument
      if (0 < parser.options.count("sort"))
      {
        m_unfoldsort = parser.option_argument_as< std::string  >("sort");
      }

      m_repeat_unfold = 1;
      if (0 < parser.options.count("repeat"))
      {
        m_repeat_unfold = parser.option_argument_as< std::size_t  >("repeat");
      }

      m_add_distribution_laws = false;
      if (0 < parser.options.count("laws"))
      {
        m_add_distribution_laws = true;
      }
    }

  public:

    lpsparunfold_tool()
      : super(
        "lpsparunfold",
        "Frank Stappers",
        "unfolds process parameter of an LPS",
        "Unfolds a set of given process parameters of the linear process specification (LPS) "
        "in INFILE and writes the result to OUTFILE. If INFILE is not present, stdin is "
        "used. If OUTFILE is not present, stdout is used."
      )
    {}

    ///Reads a specification from input_file,
    ///applies instantiation of sums to it and writes the result to output_file.
    bool run()
    {
      if ((m_set_index.empty() && m_unfoldsort.empty()) ||
          (!m_set_index.empty() && !m_unfoldsort.empty())
         )
      {
        std::cout << "Specify either --sort=SORT or --index=NUM to unfold process parameters." << std::endl;
        return false;
      }

      lps::stochastic_specification spec;
      load_lps(spec, input_filename());

      /* lpsparunfold-cache is used to avoid the introduction of equations for already unfolded sorts */
      std::map< mcrl2::data::sort_expression , lspparunfold::unfold_cache_element  >  unfold_cache;

      for (std::size_t i =0; i != m_repeat_unfold; ++i)
      {
        mCRL2log(verbose) << "Pass: " << i+1 << " of " << m_repeat_unfold << std::endl;

        //Calculate process parameters indices where m_unfoldsort occurs
        if (!m_unfoldsort.empty())
        {
          m_set_index.clear();

          data::sort_expression b_sort = mcrl2::data::parse_sort_expression( m_unfoldsort, spec.data() );
          mcrl2::data::sort_expression sort = normalize_sorts(b_sort, spec.data());


          if (!search_sort_expression(spec.data().sorts(), sort))
          {
            mCRL2log(warning) << "No sorts found of name " << m_unfoldsort << std::endl;
            break;
          }
          mcrl2::data::assignment_list assignments = spec.initial_process().assignments();
          for (mcrl2::data::assignment_list::iterator k = assignments.begin()
               ; k != assignments.end()
               ; ++k)
          {
            if (k ->lhs().sort() == sort)
            {
              m_set_index.insert(std::distance(assignments.begin(),k));
            }

          }

          if (m_set_index.empty())
          {
            mCRL2log(warning) << "No process parameters found of sort " << m_unfoldsort << std::endl;
            break;
          }
        }

        //Unfold process parameters for calculated indices
        std::set< std::size_t > h_set_index = m_set_index;

        while (!h_set_index.empty())
        {
          lpsparunfold lpsparunfold(spec, &unfold_cache, m_add_distribution_laws);
          std::size_t index = *(max_element(h_set_index.begin(), h_set_index.end()));
          spec = lpsparunfold.algorithm(index);
          h_set_index.erase(index);
        }
      }
      save_lps(spec, output_filename());

      return true;
    }

};

int main(int argc, char** argv)
{
  return lpsparunfold_tool().execute(argc, argv);
}
