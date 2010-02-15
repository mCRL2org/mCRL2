// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/pbes_rewriter_tool.h
/// \brief Base class for tools that use a pbes rewriter.

#ifndef MCRL2_UTILITIES_PBES_REWRITER_TOOL_H
#define MCRL2_UTILITIES_PBES_REWRITER_TOOL_H

#include <set>
#include <string>
#include <iostream>
#include <stdexcept>
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/utilities/rewriter_tool.h"

namespace mcrl2 {

namespace utilities {

namespace tools {

  /// \brief Base class for filter tools that use a pbes rewriter.
  template <typename Tool>
  class pbes_rewriter_tool: public Tool
  {
    public:
       /// An enumerated type for the available pbes rewriters
       enum pbes_rewriter_type
       {
         simplify,
         quantifier_all,
         quantifier_finite,
         prover,
         pfnf
       };

       /// \brief Returns a description of a pbes rewriter
       /// \param type A rewriter type
       /// \return A description of the rewriter type
       static
       std::string rewriter_description(pbes_rewriter_type type)
       {
         switch(type)
         {
           case simplify          : return "  'simplify' for simplification";
           case quantifier_all    : return "  'quantifier-all' for eliminating all quantifiers";
           case quantifier_finite : return "  'quantifier-finite' for eliminating finite quantifier variables";
           case prover            : return "  'prover' for rewriting using a prover";
           case pfnf              : return "  'pfnf' for rewriting into PFNF normal form";
         }
         return "  unknown pbes rewriter";
       }

       /// \brief Returns the string corresponding to a pbes rewriter type
       /// \param type A string
       /// \return A string corresponding to the pbes rewriter type
       static
       pbes_rewriter_type parse_pbes_rewriter_type(std::string type)
       {
         if (type == "simplify"         ) { return simplify         ; }
         if (type == "quantifier-all"   ) { return quantifier_all   ; }
         if (type == "quantifier-finite") { return quantifier_finite; }
         if (type == "prover"           ) { return prover           ; }
         if (type == "pfnf"             ) { return pfnf             ; }
         throw std::runtime_error("Error: unknown pbes rewriter option " + type);
       }

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
        result.insert(pfnf);
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
      void add_options(interface_description& desc)
      {
      	Tool::add_options(desc);
        std::string text = "use pbes rewrite strategy NAME:\n";
        std::set<pbes_rewriter_type> types = available_rewriters();
        for (typename std::set<pbes_rewriter_type>::iterator i = types.begin(); i != types.end(); ++i)
        {
          text = text + (i == types.begin() ? "" : "\n") + rewriter_description(*i);
        }
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
  	    Tool::parse_options(parser);
        m_pbes_rewriter_type = parse_pbes_rewriter_type(parser.option_argument("pbes-rewriter"));

        // The following alternative doesn't work, it is not clear why.
        // m_pbes_rewriter_type = parser.option_argument_as<pbes_rewriter_tool<Tool>::pbes_rewriter_type>("pbes-rewriter");
      }


    public:
      /// \brief Constructor.
      /// \param name The name of the tool
      /// \param author The author(s) of the tool
      /// \param what_is One-line "what is" description of the tool
      /// \param tool_description The description of the tool
      pbes_rewriter_tool(const std::string& name,
                         const std::string& author,
                         const std::string& what_is,
                         const std::string& tool_description,
                         std::string known_issues = ""
                        )
        : Tool(name, author, what_is, tool_description, known_issues)
      {}

      /// \brief Returns the rewriter type
      /// \return The rewriter type
      pbes_rewriter_type rewriter_type() const
      {
        return m_pbes_rewriter_type;
      }
  };

  /// \brief Stream operator for rewriter type
  /// \param is An input stream
  /// \param t A rewriter type
  /// \return The input stream
  template <typename Tool>
  std::istream& operator>>(std::istream& is, typename pbes_rewriter_tool<Tool>::pbes_rewriter_type& t)
  {
    std::string s;
    is >> s;
    try
    {
      t = pbes_rewriter_tool<Tool>::parse_pbes_rewriter_type(s);
    }
    catch (std::runtime_error)
    {
      is.setstate(std::ios_base::failbit);
    }
    return is;
  }

} // namespace tools

} // namespace utilities

} // namespace mcrl2

#ifdef ENABLE_SQUADT_CONNECTIVITY
# include "mcrl2/utilities/squadt_pbes_rewriter_tool.h"
#endif

#endif // MCRL2_UTILITIES_PBES_REWRITER_TOOL_H
