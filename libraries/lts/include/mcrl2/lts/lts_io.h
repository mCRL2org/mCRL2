// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file
 *
 * \brief This include file contains routines to read and write
 *        labelled transitions from and to files and from streams.
 * \details This is the LTS library's main header file. It declares all publicly
 * accessible data structures that form the interface of the library.
 * \author Muck van Weerdenburg
 */

#ifndef MCRL2_LTS_LTS_IO_H
#define MCRL2_LTS_LTS_IO_H

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "boost/iterator/transform_iterator.hpp"

#include "mcrl2/atermpp/container_utility.h"

#include "mcrl2/lts/transition.h"
#include "mcrl2/exception.h"

#include "mcrl2/lts/detail/lts_convert.h"


namespace mcrl2
{
namespace lts
{
namespace detail
{

/** \brief Determines the LTS format from a filename by its extension.
 * \param[in] s The name of the file of which the format will be
 * determined.
 * \return The LTS format based on the extension of \a s.
 * If no supported format can be determined from the extension then \a
 * lts_none is returned.  */
lts_type guess_format(std::string const& s);

/** \brief Determines the LTS format from a format specification string.
 * \details This can be any of the following strings:
 * \li "aut" for the Ald&eacute;baran format;
 * \li "fsm" for the FSM format;
 * \li "dot" for the GraphViz format;
 * \li "bcg" for the BCG format (only available if the LTS library is built
 * with BCG support).
 *
 * \param[in] s The format specification string.
 * \return The LTS format based on the value of \a s.
 * If no supported format can be determined then \a lts_none is returned.
 */
lts_type parse_format(std::string const& s);

/** \brief Gives a string representation of an LTS format.
 * \details This is the "inverse" of \ref parse_format.
 * \param[in] type The LTS format.
 * \return The name of the LTS format specified by \a type. */
std::string string_for_type(const lts_type type);

/** \brief Gives the filename extension associated with an LTS format.
 * \param[in] type The LTS format.
 * \return The filename extension of the LTS format specified by \a type.
 */
std::string extension_for_type(const lts_type type);

/** \brief Gives the MIME type associated with an LTS format.
 * \param[in] type The LTS format.
 * \return The MIME type of the LTS format specified by \a type.
 */
std::string mime_type_for_type(const lts_type type);

/** \brief Gives the set of all supported LTS formats.
 * \return The set of all supported LTS formats. */
const std::set<lts_type> &supported_lts_formats();

/** \brief Gives a textual list describing supported LTS formats.
 * \param[in] default_format The format that should be marked as default
 *                           (or \a lts_none for no default).
 * \param[in] supported      The formats that should be considered
 *                           supported.
 * \return                   A string containing lines of the form
 *                           "  'name' for the ... format". Every line
 *                           except the last is terminated with '\\n'. */
std::string supported_lts_formats_text(lts_type default_format = lts_none, const std::set<lts_type> &supported = supported_lts_formats());

/** \brief Gives a textual list describing supported LTS formats.
 * \param[in] supported      The formats that should be considered
 *                           supported.
 * \return                   A string containing lines of the form
 *                           "  'name' for the ... format". Every line
 *                           except the last is terminated with '\\n'. */
std::string supported_lts_formats_text(const std::set<lts_type> &supported);

/** \brief Gives a list of extensions for supported LTS formats.
 * \param[in] sep       The separator to use between each extension.
 * \param[in] supported The formats that should be considered supported.
 * \return              A string containing a list of extensions of the
 *                      formats in \a supported, separated by \a sep.
 *                      E.g. "*.aut,*.lts" */
std::string lts_extensions_as_string(const std::string& sep = ",", const std::set<lts_type> &supported = supported_lts_formats());

/** \brief Gives a list of extensions for supported LTS formats.
 * \param[in] supported The formats that should be considered supported.
 * \return              A string containing a list of extensions of the
 *                      formats in \a supported, separated by \a ','.
 *                      E.g. "*.aut,*.lts" */
std::string lts_extensions_as_string(const std::set<lts_type> &supported);

}


/** \brief Read a labelled transition system and return it in fsm format.
 *  \details The file can refer to any file in lts, aut, fsm, bcg, or dot 
 *           format. After reading it is is attempted to translate it into
 *           fsm format.
 *  \param[in] path A string with the name of the file.
 *  \param[out] l The lts in which the transition system is put. */
inline void load_lts_as_fsm_file(const std::string& path, lts_fsm_t& l)
{
  const lts_type intype = mcrl2::lts::detail::guess_format(path);
  switch (intype)
  {
    case lts_lts:
    {
      lts_lts_t l1;
      l1.load(path);
      detail::lts_convert(l1,l);
      return;
    }
    case lts_none:
      mCRL2log(log::warning) << "Cannot determine type of input. Assuming .aut.\n";
    case lts_aut:
    {
      lts_aut_t l1;
      l1.load(path);
      detail::lts_convert(l1,l);
      return;
    }
    case lts_fsm:
    {
      l.load(path);
      return;
    }
#ifdef USE_BCG
    case lts_bcg:
    {
      lts_bcg_t l1;
      l1.load(path);
      detail::lts_convert(l1,l);
      return;
    }
#endif
    case lts_dot:
    {
      lts_dot_t l1;
      l1.load(path);
      detail::lts_convert(l1,l);
      return;
    }
  }
}

}
}

#endif // MCRL2_LTS_LTS_IO_H
