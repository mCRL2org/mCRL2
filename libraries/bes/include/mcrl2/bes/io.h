// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/io.h
/// \brief add your file description here.

#ifndef MCRL2_BES_IO_H
#define MCRL2_BES_IO_H

#include <iostream>
#include <map>
#include <utility>
#include <iostream>
#include <string>
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/exception.h"

namespace mcrl2 {

namespace bes {

  /// \brief Convert a BES expression to cwi format.
  template <typename Expression, typename VariableMap>
  std::string bes_expression2cwi(const Expression& p, const VariableMap& variables)
  {
    typedef typename core::term_traits<Expression> tr;
    
    std::string result;
    if (tr::is_true(p))
    {
      result = "T";
    }
    else if (tr::is_false(p))
    {
      result = "F";
    }
    else if (tr::is_and(p))
    {
      std::string left = bes_expression2cwi(tr::left(p), variables);
      std::string right = bes_expression2cwi(tr::right(p), variables);
      result = "(" + left + " & " + right + ")";
    }
    else if (tr::is_or(p))
    {
      std::string left = bes_expression2cwi(tr::left(p), variables);
      std::string right = bes_expression2cwi(tr::right(p), variables);
      result = "(" + left + " | " + right + ")";
    }
    else if (tr::is_prop_var(p))
    {
      typename VariableMap::const_iterator i = variables.find(tr::name(p));
      if (i == variables.end())
      {
        throw mcrl2::runtime_error("Found undeclared variable in bes_expression2cwi: " + tr::pp(p));
      }
      std::stringstream out;
      out << i->second;
      result = out.str();
    }
    else
    {
      throw mcrl2::runtime_error("Unknown expression encountered in bes_expression2cwi: " + tr::pp(p));
    }
    return result;
  }
  
  /// \brief Save a sequence of BES equations in CWI format to a stream.
  template <typename Iter>
  void bes2cwi(Iter first, Iter last, std::ostream& out)
  {
    typedef typename std::iterator_traits<Iter>::value_type equation_type;
    typedef typename equation_type::term_type term_type;
    typedef typename core::term_traits<term_type> tr;
    typedef typename tr::string_type string_type;
  
    // Number the variables of the equations 0, 1, ... and put them in the map variables.
    std::map<string_type, int> variables;
    int index = 0;
    for (Iter i = first; i != last; ++i)
    {
      variables[i->variable().name()] = index++;
    }
  
    for (Iter i = first; i != last; ++i)
    {
      out << (i->symbol().is_mu() ? "min" : "max")
          << " "
          << variables[i->variable().name()]
          << " = "
          << bes_expression2cwi(i->formula(), variables)
          << "\n";
    }
  }
  
  /// \brief Save a sequence of BES equations in CWI format to a file.
  template <typename Iter>
  void bes2cwi(Iter first, Iter last, const std::string& outfilename)
  {
    if (outfilename == "-")
    {
      bes2cwi(first, last, std::cout);
    }
    else
    {
      std::ofstream out(outfilename.c_str());
      if (out)
      {
        bes2cwi(first, last, out);
      }
      else
      {
        throw mcrl2::runtime_error("could not open file " + outfilename);
      }
    }
  }

  /// \brief Supported BES output formats.
  enum bes_output_format
  {
    bes_output_bes,
    bes_output_cwi
  };
  
  /// \brief Save a PBES in the format specified.
  inline
  void save_bes(const boolean_equation_system<>& bes_spec, std::string outfilename, bes_output_format output_format, bool aterm_ascii = false)
  {
    switch(output_format)
    {
      case bes_output_bes:
      {
        if (aterm_ascii)
        {
          core::gsVerboseMsg("Saving result in ATerm ascii format...\n");
          bes_spec.save(outfilename, false);
        }
        else
        {
          core::gsVerboseMsg("Saving result in ATerm binary format...\n");
          bes_spec.save(outfilename, true);
        }
      }
      case bes_output_cwi:
      {
        core::gsVerboseMsg("Saving result in CWI format...\n");
        bes::bes2cwi(bes_spec.equations().begin(), bes_spec.equations().end(), outfilename);
        break;
      }
      default:
      {
        throw mcrl2::runtime_error("unknown output format encountered in save_bes");
      }
    }
  }

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_IO_H
