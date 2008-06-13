// Author(s): Aad Mathijssen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/aterm_io.h
/// \brief Load a file containing an ATerm.

#ifndef MCRL2_CORE_DETAIL_ATERM_IO_H
#define MCRL2_CORE_DETAIL_ATERM_IO_H

#include <cstdio>
#include <cerrno>
#include <string>
#include "mcrl2/exception.h"
#include "aterm2.h"

namespace mcrl2 {

namespace core {

namespace detail {

    /// Loads an ATerm from the given file, of from stdin if filename is the empty string.
    /// If reading fails an exception is thrown.
    inline
    ATerm load_aterm(const std::string& filename)
    {
      //open filename for reading as spec_stream
      FILE *spec_stream = NULL;
      if (filename.empty()) {
        spec_stream = stdin;
      } else {
        spec_stream = fopen(filename.c_str(), "rb");
      }
      if (spec_stream == NULL) {
        std::string err_msg(strerror(errno)); 
        if (err_msg.length() > 0 && err_msg[err_msg.length()-1] == '\n') {
          err_msg.replace(err_msg.length()-1, 1, "");
        }
        throw mcrl2::runtime_error("could not open input file '" + filename + "' for reading (" + err_msg + ")");
      }
      //read term from spec_stream
      ATerm spec_term = ATreadFromFile(spec_stream);
      if (spec_stream != stdin) {
        fclose(spec_stream);
      }
      if (spec_term == NULL) {
        throw mcrl2::runtime_error("could not read a valid ATerm from " + ((spec_stream == stdin)?"stdin":("'" + filename + "'")));
      }
      return spec_term;
    }

    /// Saves an ATerm to the given file, or to stdout if filename is the empty string.
    /// If writing fails an exception is thrown.
    inline
    void save_aterm(ATerm term, const std::string& filename, bool binary = true)
    {
      //open filename for writing as spec_stream
      FILE *spec_stream = NULL;
      if (filename.empty()) {
        spec_stream = stdout;
      } else {
        spec_stream = fopen(filename.c_str(), binary?"wb":"w");
      }
      if (spec_stream == NULL) {
        std::string err_msg(strerror(errno)); 
        if (err_msg.length() > 0 && err_msg[err_msg.length()-1] == '\n') {
          err_msg.replace(err_msg.length()-1, 1, "");
        }
        throw mcrl2::runtime_error("could not open output file '" + filename + "' for writing (" + err_msg + ")");
      }
      //write specification to spec_stream
      ATbool result;
      if (binary) {
        result = ATwriteToSAFFile(term, spec_stream);
      } else {
        result = ATwriteToTextFile(term, spec_stream);
      }
      if (spec_stream != stdout) {
        fclose(spec_stream);
      }
      if (result == ATfalse) {
        throw mcrl2::runtime_error("could not write ATerm to " + ((spec_stream == stdout)?"stdout":("'" + filename + "'")));
      }
    }

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_ATERM_IO_H
