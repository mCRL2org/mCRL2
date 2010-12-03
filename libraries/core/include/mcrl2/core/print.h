// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/print.h
/// \brief Functions for pretty printing ATerms.

#ifndef MCRL2_PRINT_H
#define MCRL2_PRINT_H

#include <cstdio>
#include <cctype>
#include <cassert>
#include <cstdlib>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>
#include "mcrl2/exception.h"
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/core/traverser.h"

namespace mcrl2 {
  namespace core {

/// \brief t_pp_format represents the available pretty print formats
typedef enum { ppDefault, ppDebug, ppInternal, ppInternalDebug} t_pp_format;

/// \brief Print string representation of pretty print format
/// \param pp_format a pretty print format
/// \return string representation of the pretty print format
/// \throws mcrl2::runtime error if an unknown pretty print format
///         is passed into the function.
inline
std::string pp_format_to_string(const t_pp_format pp_format)
{
  switch(pp_format)
  {
    case ppDefault:
      return "default";
    case ppDebug:
      return "debug";
    case ppInternal:
      return "internal";
    case ppInternalDebug:
      return "internal_debug";
    default:
      throw mcrl2::runtime_error("Unknown pretty print format");
  }
}

/** \brief Extensions of the printf function. The following new
 *         conversion formats are supported:
 *         - '%P' for pretty printing ATerm's
 *         - '%T' for normal printing of ATerm's
 *         - '%F' for printing of AFun's
**/
int gsprintf(const char *format, ...);

/** \brief Extensions of the fprintf function. The following new
 *         conversion formats are supported:
 *         - '%P' for pretty printing ATerm's
 *         - '%T' for normal printing of ATerm's
 *         - '%F' for printing of AFun's
**/
int gsfprintf(FILE *stream, const char *format, ...);

/** \brief Extensions of the vfprintf functions. The following new
 *         conversion formats are supported:
 *         - '%P' for the pretty printing ATerm's
 *         - '%T' for the normal printing of ATerm's
 *         - '%F' for the printing of AFun's
**/
int gsvfprintf(FILE *stream, const char *format, va_list args);

/** \brief Print a textual description of an ATerm representation of an
 *         mCRL2 specification or expression to an output stream.
 *  \param[in] out_stream A pointer to a stream to which can be written.
 *  \param[in] part An ATerm representation of a part of an mCRL2
 *             specification or expression.
 *  \param[in] pp_format A pretty print format.
 *  \post A textual representation of part is written to out_stream using
 *        method pp_format.
**/
void PrintPart_C(FILE *out_stream, const ATerm part, t_pp_format pp_format = ppDefault);

/** \brief Print a textual description of an ATerm representation of an
 *         mCRL2 specification or expression to an output stream.
 *  \param[in] out_stream A stream to which can be written.
 *  \param[in] part An ATerm representation of a part of an mCRL2
 *             specification or expression.
 *  \param[in] pp_format A pretty print format.
 *  \post A textual representation of part is written to out_stream using
 *        method pp_format.
**/
void PrintPart_CXX(std::ostream &out_stream, const ATerm part,
  t_pp_format pp_format = ppDefault);

/** \brief Return a textual description of an ATerm representation of an
 *         mCRL2 specification or expression.
 *  \param[in] part An ATerm representation of a part of an mCRL2
 *             specification or expression.
 *  \param[in] pp_format A pretty print format.
 *  \return A textual representation of part according to method pp_format.
**/
std::string PrintPart_CXX(const ATerm part, t_pp_format pp_format = ppDefault);

/** \brief Return a textual description of an ATerm representation of an
 *         mCRL2 specification or expression.
 *  \param[in] part An ATerm representation of a part of an mCRL2
 *             specification or expression.
 *  \param[in] pp_format A pretty print format.
 *  \return A textual representation of part according to method pp_format.
**/
template <typename Term>
std::string pp(Term part, t_pp_format pp_format = ppDefault)
{
  return PrintPart_CXX(atermpp::aterm_traits<Term>::term(part), pp_format);
}

    namespace detail {
      
      template <typename Derived>
      class print_traverser: public traverser<Derived>
      {
        protected:
          std::ostream& out;

#ifdef MCRL2_PRINT_DEBUG
          std::ostringstream debug;
          std::vector<std::string> debug_strings;
          std::vector<std::size_t> debug_positions;
#endif
        public:
          typedef traverser<Derived> super;
      
#ifndef MCRL2_PRINT_DEBUG
          using super::enter;
          using super::leave;
#endif
          using super::operator();

          print_traverser(std::ostream& o)
            : out(o)
          {}

          void print(const std::string& s)
          {
            out << s;
#ifdef MCRL2_PRINT_DEBUG
            debug << s;
#endif
          }
            
          void operator()(const core::identifier_string& x)
          {
            static_cast<Derived&>(*this).enter(x);
            print(std::string(x));
            static_cast<Derived&>(*this).leave(x);
          }

#ifdef MCRL2_PRINT_DEBUG
          template <typename T>
          std::string print_debug(const T& t)
          {
            return pp(t);
          }

          // Enter object
          template <typename Expression>
          void enter(const Expression& x)
          {
            debug_strings.push_back(static_cast<Derived&>(*this).print_debug(x));
            debug_positions.push_back(debug.str().size());
          }
          
          // Leave object
          template <typename Expression>
          void leave(const Expression& x)
          {
            std::string expected = debug_strings.back();
            debug_strings.pop_back();
            std::size_t begin_pos = debug_positions.back();
            debug_positions.pop_back();
            std::string result = debug.str().substr(begin_pos);
            if (expected != result) {
              std::cerr << "--- Error in print ---\n"
                        << "  expected: " << expected << "\n"
                        << "       got: " << result << std::endl;
              BOOST_CHECK(expected == result);
            }
          }
#endif
      };

      // apply a traverser with one additional template argument
      template <template <class> class Traverser, class OutputStream>
      class apply_print_traverser: public Traverser<apply_print_traverser<Traverser, OutputStream> >
      {
        typedef Traverser<apply_print_traverser<Traverser, OutputStream> > super;
        
        public:
          using super::enter;
          using super::leave;
          using super::operator();

#if BOOST_MSVC
    template <typename T>
    void operator()(const T& x)
    {
      super::operator()(x);
    }
#endif
      
          apply_print_traverser(std::ostream& out):
            super(out)
          {}
      };

    } // namespace detail

    /// \brief Prints the object t to a stream.
    template <typename T>
    void print(const T& t, std::ostream& out)
    {
      detail::apply_print_traverser<detail::print_traverser, std::ostringstream> printer(out);
      printer(t);
    }

    /// \brief Returns a string representation of the object t.
    template <typename T>
    std::string print(const T& t)
    {
      std::ostringstream out;
      print(t, out);
      return out.str();
    }

  }
}

#endif //MCRL2_PRINT_H
