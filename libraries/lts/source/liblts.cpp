// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts.cpp

#include <string>
#include <set>
#include <stack>
#include <bitset>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/lts/lts_io.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::log;

#ifdef USE_BCG
// #include <bcg_user.h>
#endif

using namespace std;

namespace mcrl2
{
namespace lts
{
namespace detail
{

std::vector < atermpp::function_symbol > state_function_symbols;

lts_type guess_format(string const& s, const bool be_verbose/*=true*/)
{
  string::size_type pos = s.find_last_of('.');

  if (pos != string::npos)
  {
    string ext = s.substr(pos+1);

    if (ext == "aut")
    {
      if (be_verbose) 
      {
        mCRL2log(verbose) << "Detected Aldebaran extension.\n";
      }
      return lts_aut;
    }
    else if (ext == "lts")
    {
      if (be_verbose) 
      {
        mCRL2log(verbose) << "Detected mCRL2 extension.\n";
      }
      return lts_lts;
    }
    else if (ext == "fsm")
    {
      if (be_verbose) 
      {
        mCRL2log(verbose) << "Detected Finite State Machine extension.\n";
      }
      return lts_fsm;
    }
    else if (ext == "dot")
    {
      if (be_verbose) 
      {
        mCRL2log(verbose) << "Detected GraphViz extension.\n";
      }
      return lts_dot;
#ifdef USE_BCG
    }
    else if (ext == "bcg")
    {
      if (be_verbose) 
      {
        mCRL2log(verbose) << "Detected Binary Coded Graph extension.\n";
      }
      return lts_bcg;
#endif
    }
  }

  return lts_none;
}

static std::string type_strings[] = { "unknown", "lts", "aut", "fsm", "dot", "bcg" };

static std::string extension_strings[] = { "", "lts", "aut", "fsm", "dot", "bcg" };

static std::string type_desc_strings[] = { "unknown LTS format",
    "mCRL2 LTS format",
    "Aldebaran format (CADP)",
    "Finite State Machine format",
    "GraphViz format",
    "SVC format",
    "Binary Coded Graph format (CADP)"
                                         };


static std::string mime_type_strings[] = { "",
    "application/lts",
    "text/aut",
    "text/fsm",
    "application/bcg",
    "text/dot",
                                         };

lts_type parse_format(std::string const& s)
{
  if (s == "lts")
  {
    return lts_lts;
  }
  else if (s == "aut")
  {
    return lts_aut;
  }
  else if (s == "fsm")
  {
    return lts_fsm;
  }
#ifdef USE_BCG
  else if (s == "bcg")
  {
    return lts_bcg;
  }
#endif
  else if (s == "dot")
  {
    return lts_dot;
  }
  return lts_none;
}

std::string string_for_type(const lts_type type)
{
  return (type_strings[type]);
}

std::string extension_for_type(const lts_type type)
{
  return (extension_strings[type]);
}

std::string mime_type_for_type(const lts_type type)
{
  return (mime_type_strings[type]);
}

static const std::set<lts_type> &initialise_supported_lts_formats()
{
  static std::set<lts_type> s;
  for (size_t i = lts_type_min; i<1+(size_t)lts_type_max; ++i)
  {
    if (lts_none != (lts_type) i)
    {
      s.insert((lts_type) i);
    }
  }
  return s;
}
const std::set<lts_type> &supported_lts_formats()
{
  static const std::set<lts_type> &s = initialise_supported_lts_formats();
  return s;
}

std::string supported_lts_formats_text(lts_type default_format, const std::set<lts_type> &supported)
{
  vector<lts_type> types(supported.begin(),supported.end());
  std::sort(types.begin(),types.end(),boost::bind(lts_named_cmp<lts_type>,type_strings,_1,_2));

  string r;
  for (vector<lts_type>::iterator i=types.begin(); i!=types.end(); ++i)
  {
    r += "  '" + type_strings[*i] + "' for the " + type_desc_strings[*i];

    if (*i == default_format)
    {
      r += " (default)";
    }

    // Still unsafe if types.size() < 2
    assert(types.size() >= 2);
    if (i == types.end() - 2)
    {
      r += ", or\n";
    }
    else if (i != types.end() - 1)
    {
      r += ",\n";
    }
  }

  return r;
}

std::string supported_lts_formats_text(const std::set<lts_type> &supported)
{
  return supported_lts_formats_text(lts_none,supported);
}

std::string lts_extensions_as_string(const std::string& sep, const std::set<lts_type> &supported)
{
  vector<lts_type> types(supported.begin(),supported.end());
  std::sort(types.begin(),types.end(),boost::bind(lts_named_cmp<lts_type>,extension_strings,_1,_2));

  string r, prev;
  bool first = true;
  for (vector<lts_type>::iterator i=types.begin(); i!=types.end(); i++)
  {
    if (extension_strings[*i] == prev)   // avoid mentioning extensions more than once
    {
      continue;
    }
    if (first)
    {
      first = false;
    }
    else
    {
      r += sep;
    }
    r += "*." + extension_strings[*i];
    prev = extension_strings[*i];
  }

  return r;
}

std::string lts_extensions_as_string(const std::set<lts_type> &supported)
{
  return lts_extensions_as_string(",",supported);
}

} // namespace detail
} //lts
} //data
