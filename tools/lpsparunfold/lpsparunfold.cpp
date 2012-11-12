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
#include <limits.h>

//mCRL2
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/atermpp/aterm_init.h"


//LPS framework
#include "mcrl2/lps/specification.h"

//DATA
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/parse.h"

//LPSPARUNFOLDLIB
#include "lpsparunfoldlib.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"

using namespace mcrl2::utilities;
using namespace mcrl2::data;

using namespace mcrl2;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::log;

class lpsparunfold_tool: public  rewriter_tool<input_output_tool>
{
  protected:

    typedef rewriter_tool<input_output_tool> super;

    std::set< size_t > m_set_index; ///< Options of the algorithm
    std::string m_unfoldsort;
    size_t m_repeat_unfold;
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
        std::string  m_string_index;

        m_string_index = parser.option_argument_as< std::string  >("index");

        size_t s_index = m_string_index.find_first_of(",");
        size_t s_old_index=0;
        char* p;

        while (s_index != std::string::npos)
        {
          errno = 0;
          long val = strtol(m_string_index.substr(s_old_index, s_index - s_old_index).c_str(), &p ,10);
          /* Check for various possible errors */
          if (errno != 0 || *p != '\0')
          {
            throw parser.error("Parsed an invalid index value");
          }
          m_set_index.insert(val);
          s_old_index = s_index+1;
          s_index = m_string_index.find_first_of(",",s_old_index);
        }

        errno =0;
        long val = strtol(m_string_index.substr(s_old_index, s_index - s_old_index).c_str(), &p ,10);
        /* Check for various possible errors */
        if (errno != 0 || *p != '\0')
        {
          throw parser.error("Parsed an invalid index value");
        }
        m_set_index.insert(val);
      }

      // Parse string argument NAME for sort argument
      if (0 < parser.options.count("sort"))
      {
        m_unfoldsort = parser.option_argument_as< std::string  >("sort");
      }

      m_repeat_unfold = 1;
      if (0 < parser.options.count("repeat"))
      {
        m_repeat_unfold = parser.option_argument_as< size_t  >("repeat");
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

      lps::specification lps_specification;

      lps_specification.load(m_input_filename);

      /* lpsparunfold-cache is used to avoid the introduction of equations for already unfolded sorts */
      atermpp::map< mcrl2::data::sort_expression , lspparunfold::unfold_cache_element  >  unfold_cache;

      for (size_t i =0; i != m_repeat_unfold; ++i)
      {
        mCRL2log(verbose) << "Pass: " << i+1 << " of " << m_repeat_unfold << std::endl;

        //Calculate process parameters indices where m_unfoldsort occurs
        if (!m_unfoldsort.empty())
        {
          m_set_index.clear();

          data::sort_expression b_sort = mcrl2::data::parse_sort_expression( m_unfoldsort, lps_specification.data() );
          mcrl2::data::sort_expression sort = normalize_sorts(b_sort, lps_specification.data());


          if (!search_sort_expression(lps_specification.data().sorts(), sort))
          {
            mCRL2log(warning) << "No sorts found of name " << m_unfoldsort << std::endl;
            break;
          }
          mcrl2::data::assignment_list assignments = lps_specification.initial_process().assignments();
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
        std::set< size_t > h_set_index = m_set_index;

        while (!h_set_index.empty())
        {
          lpsparunfold lpsparunfold(lps_specification, &unfold_cache, m_add_distribution_laws);
          size_t index = *(max_element(h_set_index.begin(), h_set_index.end()));
          lps_specification = lpsparunfold.algorithm(index);
          h_set_index.erase(index);
        }
      }
      lps_specification.save(m_output_filename);



      return true;
    }

};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return lpsparunfold_tool().execute(argc, argv);
}
