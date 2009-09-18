// Author(s): Alexander van Dam, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesinfo.cpp
/// \brief Tool that displays information about a PBES.

// ======================================================================
//
// file          : pbesinfo
// date          : 11-04-2007
// version       : 0.1.0
//
// author(s)     : Alexander van Dam <avandam@damdonk.nl>
//
// ======================================================================

//C++
#include <cstdio>
#include <exception>
#include <iostream>
#include <string>

//MCRL2-specific
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/input_tool.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities::tools;

class pbesinfo_tool: public input_tool
{
  protected:
    typedef input_tool super;

    bool opt_full;

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      opt_full = parser.options.count("full") > 0;
    }
    
    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("full",
                      "display the predicate variables and their signature",
                      'f'
                     )
      ;
    }

  public:
    pbesinfo_tool()
      : super(
          "pbesinfo",
          "Alexander van Dam, Wieger Wesselink",
          "display basic information about a PBES",
          super::make_tool_description("Print basic information about the PBES in INFILE.")
        ),
        opt_full(false)
     {}

    /// If PBES can be loaded from file_name, then
    /// - Show if PBES is closed and if it is well formed
    ///       - Show number of equations
    /// - Show number of mu's / nu's.
    /// - Show which predicate variables have mu's and which predicate variables have nu's
    /// - Show predicate variables and their type
    /// else
    /// - Give error
    bool run()
    {
      pbes<> p;
      p.load(input_filename());

      // Get PBES equations. Makes a lot of function calls more readable.
      atermpp::vector<pbes_equation> eqsys;
      eqsys = p.equations();

      bool pbes_well_formed = p.is_well_typed();
      bool pbes_closed = p.is_closed();

      // Vectors for storing intermediate results
      vector<identifier_string> predvar_mu;
      vector<identifier_string> predvar_nu;
      vector<propositional_variable> predvar_data;

      // Integers for showing totals
      int mu = 0;
      int nu = 0;
      int fp_errors = 0;

      for (atermpp::vector<pbes_equation>::iterator fp_i = eqsys.begin(); fp_i != eqsys.end(); fp_i++)
      {
         // - Store variables
         predvar_data.push_back(fp_i->variable());

         // Check on mu or nu
         if (fp_i->symbol().is_mu())
         {
           // If fp is mu:
           // - Increase #mu's
           // - Store predicate variable in mu-list and common list
           // - Store variables
           mu++;
           if (opt_full)
             predvar_mu.push_back(fp_i->variable().name());
         }
         else if (fp_i->symbol().is_nu())
         {
         // If fp is nu:
         // - Increase #nu's
         // - Store predicate variable in nu-list and common list
           nu++;
           if (opt_full)
             predvar_nu.push_back(fp_i->variable().name());
         }
         else
         {
           fp_errors++;
         }
      }

      // Show file from which PBES was read
      std::cout << input_file_message() << "\n\n";

      // Check if errors occurred in reading PBEs
      if (fp_errors != 0)
      {
        cerr << "WARNING: Reading number of mu's and nu's had errors. Results may be incorrect" << endl;
      }

      // Show if PBES is closed and well formed
      cout << "The PBES is " << (pbes_closed ? "" : "not ") << "closed and " << (pbes_well_formed ? "" : "not ") << "well formed" << endl;

      // Show number of equations
      cout << "Number of equations: " << eqsys.size() << endl;

      // Show number of mu's with the predicate variables from the mu's
      cout << "Number of mu's:      " << mu;
      int size_mu = predvar_mu.size();
      int mu_done = 1;
      if (size_mu > 0)
        cout << "   (";
      for (vector<identifier_string>::iterator i = predvar_mu.begin(); i != predvar_mu.end(); i++)
      {
        cout << *i << ((mu_done == size_mu) ? ")" : ", ");
        mu_done++;
      }
      cout << endl;

      // Show number of nu's with the predicate variables from the nu's
      cout << "Number of nu's:      " << nu;
      int size_nu = predvar_nu.size();
      int nu_done = 1;
      if (size_nu > 0)
        cout << "   (";
      for (vector<identifier_string>::iterator i = predvar_nu.begin(); i != predvar_nu.end(); i++)
      {
        cout << *i << ((mu_done == size_mu) ? ")" : ", ");
        nu_done++;
      }
      cout << endl;

     // Show binding variables with their signature
      if (opt_full)
      {
        std::cout << "Predicate variables:\n";
        for (atermpp::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          std::cout << core::pp(i->symbol()) << "." << core::pp(i->variable()) << std::endl;
        }
/*        
        int nr_predvar = 1;
        for (vector<propositional_variable>::iterator pv_i = predvar_data.begin(); pv_i != predvar_data.end(); pv_i++)
        {
          int bv_size = pv_i->parameters().size();
          int nr_sorts = 1;
          if (nr_predvar == 1)
            cout << "Predicate variables: " << pv_i->name() << " :: ";
          else
            cout << "                     " << pv_i->name() << " :: ";
          for (atermpp::term_list<variable>::iterator dv_i = pv_i->parameters().begin(); dv_i != pv_i->parameters().end(); dv_i++)
          {
            cout << core::pp(dv_i->sort());
            if (nr_sorts < bv_size)
            {
              cout << " x ";
              nr_sorts++;
            }
            else
              cout << " -> " << core::pp(sort_bool::bool_());
            }
          cout << endl;
          nr_predvar++;
        }
*/        
      }
      return true;
    }
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return pbesinfo_tool().execute(argc, argv);
}
