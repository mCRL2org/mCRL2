// Author(s): Wieger Wesselink, Alexander van Dam
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/io.h
/// \brief IO routines for boolean equation systems.

#ifndef MCRL2_PBES_IO_H
#define MCRL2_PBES_IO_H

#include <utility>
#include <iostream>
#include <string>
#include "mcrl2/bes/io.h"
#include "mcrl2/data/io.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/file_formats.h"
#include "mcrl2/pbes/index_traits.h"
#include "mcrl2/pbes/pbesinstconversion.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

// transforms DataVarId to DataVarIdNoIndex
// transforms OpId to OpIdNoIndex
// transforms PropVarInst to PropVarInstNoIndex
struct index_remover
{
  atermpp::aterm_appl operator()(const atermpp::aterm_appl& x) const
  {
    if (x.function() == core::detail::function_symbol_DataVarId())
    {
      return atermpp::aterm_appl(core::detail::function_symbol_DataVarIdNoIndex(), x.begin(), --x.end());
    }
    else if (x.function() == core::detail::function_symbol_OpId())
    {
      return atermpp::aterm_appl(core::detail::function_symbol_OpIdNoIndex(), x.begin(), --x.end());
    }
    else if (x.function() == core::detail::function_symbol_PropVarInst())
    {
      return atermpp::aterm_appl(core::detail::function_symbol_PropVarInstNoIndex(), x.begin(), --x.end());
    }
    return x;
  }
};

// transforms DataVarIdNoIndex to DataVarId
// transforms OpIdNoIndex to OpId
// transforms PropVarInstNoIndex to PropVarInst
struct index_adder
{
  atermpp::aterm_appl operator()(const atermpp::aterm_appl& x) const
  {
    if (x.function() == core::detail::function_symbol_DataVarIdNoIndex())
    {
      const data::variable& y = atermpp::aterm_cast<const data::variable>(x);
      std::size_t index = core::index_traits<data::variable, data::variable_key_type>::index(y);
      return atermpp::aterm_appl(core::detail::function_symbol_DataVarId(), x[0], x[1], atermpp::aterm_int(index));
    }
    else if (x.function() == core::detail::function_symbol_OpIdNoIndex())
    {
      const data::function_symbol& y = atermpp::aterm_cast<const data::function_symbol>(x);
      std::size_t index = core::index_traits<data::function_symbol, data::function_symbol_key_type>::index(y);
      return atermpp::aterm_appl(core::detail::function_symbol_OpId(), x[0], x[1], atermpp::aterm_int(index));
    }
    else if (x.function() == core::detail::function_symbol_PropVarInstNoIndex())
    {
      const pbes_system::propositional_variable_instantiation& y = atermpp::aterm_cast<const pbes_system::propositional_variable_instantiation>(x);
      std::size_t index = core::index_traits<propositional_variable_instantiation, propositional_variable_key_type>::index(y);
      return atermpp::aterm_appl(core::detail::function_symbol_PropVarInst(), x[0], x[1], atermpp::aterm_int(index));
    }
    return x;
  }
};

} // namespace detail

inline
atermpp::aterm add_index(const atermpp::aterm& x)
{
  return atermpp::replace(x, detail::index_adder());
}

inline
atermpp::aterm remove_index(const atermpp::aterm& x)
{
  return atermpp::replace(x, detail::index_remover());
}

/// \brief Save a PBES in the format specified.
/// \param pbes_spec The pbes to be stored
/// \param outfilename The name of the file to which the output is stored.
/// \param output_format Determines the format in which the result is written.
/// \param aterm_ascii Determines, if output_format is pbes, whether the file
///        is written is ascii format.
inline
void save_pbes(const pbes& pbes_spec,
               const std::string& outfilename,
               pbes_file_format output_format,
               bool aterm_ascii = false)
{
  switch (output_format)
  {
    case pbes_file_pbes:
    {
      if (aterm_ascii)
      {
        mCRL2log(log::verbose) << "Saving result in aterm ascii format..." << std::endl;
        pbes_spec.save(outfilename, false);
      }
      else
      {
        mCRL2log(log::verbose) << "Saving result in aterm binary format..." << std::endl;
        pbes_spec.save(outfilename, true);
      }
      break;
    }
    case pbes_file_bes:
    case pbes_file_cwi:
    case pbes_file_pgsolver:
    {
      if (!pbes_system::algorithms::is_bes(pbes_spec))
      {
        throw mcrl2::runtime_error("the PBES cannot be saved as a BES");
      }
      bes::boolean_equation_system bes_spec = pbesinstconversion(pbes_spec);
      bes::save_bes(bes_spec, outfilename, output_format);
      break;
    }
    default:
    {
      throw mcrl2::runtime_error("unknown output format encountered in save_pbes");
    }
  }
}

/// \brief Load pbes from file.
/// \param p The pbes to which the result is loaded.
/// \param infilename The file from which to load the PBES.
/// \param f The format that should be assumed for the file in infilename.
inline
void load_pbes(pbes& p,
              const std::string& infilename,
              const pbes_file_format f)
{
  switch(f)
  {
    case pbes_file_pbes:
    {
      p.load(infilename);
      break;
    }
    case pbes_file_bes:
    case pbes_file_cwi:
    case pbes_file_pgsolver:
    {
      bes::boolean_equation_system b;
      bes::load_bes(b, infilename, f);
      p = bes2pbes(b);
      break;
    }
    default:
    {
      throw mcrl2::runtime_error("unknown file format encountered in load_pbes");
    }
  }
}

/// \brief Load pbes from file.
/// \param p The pbes to which the result is loaded.
/// \param infilename The file from which to load the PBES.
///
/// The format of the file in infilename is guessed.
inline
void load_pbes(pbes& p,
              const std::string& infilename)
{
  pbes_file_format f = guess_format(infilename);
  load_pbes(p, infilename, f);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_IO_H
