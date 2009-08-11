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

//mCRL2
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/atermpp/aterm_init.h"


//LPS framework
#include "mcrl2/lps/specification.h"

//DATA
#include "mcrl2/data/data_specification.h"

//LPSPARUNFOLDLIB
#include "lpsparunfoldlib.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"

using namespace mcrl2::utilities;
using namespace mcrl2::data;

using namespace mcrl2;
using namespace mcrl2::utilities::tools;

//class parunfold_tool: public squadt_tool< rewriter_tool<input_output_tool> >
class parunfold_tool: public  rewriter_tool<input_output_tool>
{
  protected:

    //typedef squadt_tool< rewriter_tool<input_output_tool> > super;
    typedef rewriter_tool<input_output_tool> super;

    std::set< int > m_set_index; ///< Options of the algorithm
    std::string m_unfoldsort;    

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("index", make_mandatory_argument("[NUM]"), 
           "unfolds process parameters for comma separated indices", 'i');
      desc.add_option("sort", make_mandatory_argument("NAME"),
          "unfolds all process parameters of sort NAME", 's');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      if ((0 == parser.options.count("index") and 0 == parser.options.count("sort")) or (0 < parser.options.count("index") and 0 < parser.options.count("sort")))
      {
        parser.error("Use either --sort or --index to unfold process parameters.");
      }

      // Parse string argument to [NUM] (a set of indices)
      if (0 < parser.options.count("index"))
      {
        std::string  m_string_index;
 
        m_string_index = parser.option_argument_as< std::string  >("index");

        int s_index = m_string_index.find_first_of(",");
        int s_old_index=0;     
 
        while( s_index != std::string::npos ) {

          m_set_index.insert( atoi(m_string_index.substr( s_old_index, s_index - (s_old_index ) ).c_str() ) );
          s_old_index = s_index+1;
          s_index = m_string_index.find_first_of(",",s_old_index);
        }
        m_set_index.insert( atoi(m_string_index.substr( s_old_index, s_index - (s_old_index ) ).c_str() ) );
      }

      // Parse string argument NAME for sort argument
      if (0 < parser.options.count("sort"))
      {
        m_unfoldsort = parser.option_argument_as< std::string  >("sort");
      }

    }

  public:

    parunfold_tool()
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
      lps::specification lps_specification;

      lps_specification.load(m_input_filename);

      if (!m_unfoldsort.empty() and m_set_index.empty() )
      {
        mcrl2::data::basic_sort b_sort( m_unfoldsort );

        if (!search_basic_sort( lps_specification.data(), b_sort ))
        {
          std::cerr << "No sorts found of name " << m_unfoldsort << std::endl;
          return false;
        } 

        mcrl2::data::assignment_list assignments = lps_specification.initial_process().assignments();
        for(mcrl2::data::assignment_list::iterator k = assignments.begin()
                                                 ; k != assignments.end()
                                                 ; ++k)
        {
          if( k ->lhs().sort() == b_sort )
            {  
              m_set_index.insert (std::distance(assignments.begin(),k ) );
            }
        }      

        if ( m_set_index.empty() )
        {
          std::cerr << "No process parameters found of sort " << m_unfoldsort << std::endl;
          return false;
        }
      }

      while (!m_set_index.empty())
      {
        lpsparunfold lpsparunfold( lps_specification );
        int index = *(max_element( m_set_index.begin(), m_set_index.end() ) );
        lps_specification = lpsparunfold.algorithm( index );
        m_set_index.erase( index );
      }

      lps_specification.save(m_output_filename);

      return true;
    }

};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return parunfold_tool().execute(argc, argv);
}
