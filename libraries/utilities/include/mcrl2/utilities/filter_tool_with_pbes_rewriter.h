// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/filter_tool_with_pbes_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_FILTER_TOOL_WITH_PBES_REWRITER_H
#define MCRL2_UTILITIES_FILTER_TOOL_WITH_PBES_REWRITER_H

#include <iostream>
#include <stdexcept>
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/utilities/filter_tool_with_rewriter.h"

namespace mcrl2 {

namespace utilities {

  /// An enumerated type for the available pbes rewriters
  enum pbes_rewriter_type
  {
    simplify,
    quantifier_all,
    quantifier_finite,
    prover
  };

  /// \brief Returns a description of a pbes rewriter
  /// \param type A rewriter type
  /// \return A description of the rewriter type
  std::string pbes_rewriter_description(pbes_rewriter_type type)
  {
    switch(type)
    {
      case simplify          : return "  'simplify' for simplification";
      case quantifier_all    : return "  'quantifier-all' for eliminating all quantifiers";
      case quantifier_finite : return "  'quantifier-finite' for eliminating finite quantifier variables";
      case prover            : return "  'prover' for rewriting using a prover";
    }
  }

  /// \brief Returns the string corresponding to a pbes rewriter type
  /// \param type A pbes rewriter type
  /// \return A string corresponding to the pbes rewriter type
  inline
  pbes_rewriter_type parse_pbes_rewriter_type(std::string type)
  {
    if (type == "simplify"         ) { return simplify         ; }
    if (type == "quantifier_all"   ) { return quantifier_all   ; }
    if (type == "quantifier_finite") { return quantifier_finite; }
    if (type == "prover"           ) { return prover           ; }
    throw std::runtime_error("Error: unknown pbes rewriter option " + type);
  }

  /// \brief Stream operator for pbes_rewriter_type
  inline
  std::istream& operator>>(std::istream& is, pbes_rewriter_type& t)
  {
    std::string s;
    is >> s;
    try
    {
      t = parse_pbes_rewriter_type(s);
    }
    catch (std::runtime_error)
    {
      is.setstate(std::ios_base::failbit);
    }
    return is;
  }

  /// \brief Base class for filter tools that use a pbes rewriter.
  class filter_tool_with_pbes_rewriter: public filter_tool_with_rewriter
  {
    protected:
      /// \brief The type of the pbes rewriter
      pbes_rewriter_type m_pbes_rewriter_type;

      /// \brief Returns the types of rewriters that are available for this tool.
      /// Override this method to change the standard behavior.
      /// \return The set { simplify, quantifier_all, quantifier_finite }
      virtual std::set<pbes_rewriter_type> available_rewriters() const
      {
        std::set<pbes_rewriter_type> result;
        result.insert(simplify);
        result.insert(quantifier_all);
        result.insert(quantifier_finite);
        return result;
      }

      /// \brief Returns the default pbes rewriter.
      /// Override this method to change the standard behavior.
      /// \return The string "simplify"
      virtual std::string default_rewriter() const
      {
        return "simplify";
      }

      /// \brief Add options to an interface description. Also includes
      /// rewriter options.
      /// \param desc An interface description
      void add_options(utilities::interface_description& desc)
      {
        std::string text = "use pbes rewrite strategy NAME:\n";
        std::set<pbes_rewriter_type> types = available_rewriters();
        for (std::set<pbes_rewriter_type>::iterator i = types.begin(); i != types.end(); ++i)
        {
          text = text + (i == types.begin() ? "" : "\n") + pbes_rewriter_description(*i);
        }
        filter_tool_with_rewriter::add_options(desc);
        desc.add_option(
          "pbes-rewriter",
          make_optional_argument("NAME", default_rewriter()),
          text,
          'p'
        );
      }

      /// \brief Parse non-standard options
      /// \param parser A command line parser
      void parse_options(const command_line_parser& parser)
      {
        m_pbes_rewriter_type = parser.option_argument_as<pbes_rewriter_type>("pbes-rewriter");
      }

    public:
      /// \brief Constructor.
      /// \param name The name of the tool
      /// \param author The author(s) of the tool
      /// \param tool_description The description of the tool
      filter_tool_with_pbes_rewriter(const std::string& name,
                                     const std::string& author,
                                     const std::string& tool_description
                                    )
        : filter_tool_with_rewriter(name, author, tool_description)
      {}
      
      /// \brief Returns the rewriter type
      /// \return The rewriter type
      pbes_rewriter_type rewriter_type() const
      {
        return m_pbes_rewriter_type;
      }
  };

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_FILTER_TOOL_WITH_PBES_REWRITER_H
