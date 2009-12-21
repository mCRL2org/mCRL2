// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/parse.h
///
/// \brief Parse mCRL2 specifications and expressions.

#ifndef MCRL2_PARSE_H
#define MCRL2_PARSE_H

#include <iostream>
#include <aterm2.h>

namespace mcrl2 {
  namespace core {

/** \brief  Parse an mCRL2 identifier.
  * \param[in] se_stream An input stream from which can be read.
  * \post   The content of se_stream is parsed as an mCRL2 identifier.
  * \return The parsed identifier, if everything went well;
  *         NULL, otherwise.
**/
ATermAppl parse_identifier(std::istream &se_stream);


/** \brief  Parse an mCRL2 sort expression.
  * \param[in] se_stream An input stream from which can be read.
  * \post   The content of se_stream is parsed as an mCRL2 sort expression.
  * \return The parsed sort expression, if everything went well;
  *         NULL, otherwise.
**/
ATermAppl parse_sort_expr(std::istream &se_stream);


/** \brief  Parse an mCRL2 data expression.
  * \param[in] de_stream An input stream from which can be read.
  * \post   The content of de_stream is parsed as an mCRL2 data expression.
  * \return The parsed data expression, if everything went well;
  *         NULL, otherwise.
**/
ATermAppl parse_data_expr(std::istream &de_stream);


/** \brief  Parse an mCRL2 data specification.
  * \param[in] ps_stream An input stream from which can be read.
  * \post   The content of ps_stream is parsed as an mCRL2 data specification.
  * \return The parsed data specification, if everything went well;
  *         NULL, otherwise.
**/
ATermAppl parse_data_spec(std::istream &ps_stream);


/** \brief  Parse an mCRL2 multiaction.
  * \param[in] ma_stream An input stream from which can be read.
  * \post   The content of ma_stream is parsed as an mCRL2 multiaction.
  * \return The parsed multiaction, if everything went well;
  *         NULL, otherwise.
**/
ATermAppl parse_mult_act(std::istream &ma_stream);


/** \brief  Parse an mCRL2 process expression.
  * \param[in] pe_stream An input stream from which can be read.
  * \post   The content of pe_stream is parsed as an mCRL2 process expression.
  * \return The parsed process expression, if everything went well;
  *         NULL, otherwise.
**/
ATermAppl parse_proc_expr(std::istream &pe_stream);


/** \brief  Parse an mCRL2 process specification.
  * \param[in] ps_stream An input stream from which can be read.
  * \post   The content of ps_stream is parsed as an mCRL2 process
  *         specification.
  * \return the parsed process specification, if everything went well;
  *         NULL, otherwise.
**/
ATermAppl parse_proc_spec(std::istream &ps_stream);


/** \brief  Parse an mCRL2 state formula.
  * \param[in] sf_stream An input stream from which can be read.
  * \post   The content of sf_stream is parsed as an mCRL2 state formula.
  * \return The parsed state formula, if everything went well;
  *         NULL, otherwise.
**/
ATermAppl parse_state_frm(std::istream &sf_stream);


/** \brief  Parse an mCRL2 action rename specification.
  * \param[in] sf_stream An input stream from which can be read.
  * \post   The content of sf_stream is parsed as an mCRL2 action rename
  *         specification.
  * \return The parsed rename rule file, if everything went well;
  *         NULL, otherwise.
**/
ATermAppl parse_action_rename_spec(std::istream &sf_stream);


/** \brief  Parse an mCRL2 parameterised boolean equation system (PBES)
  *  specification.
  * \param[in] pbes_spec_stream An input stream from which can be read.
  * \post   The content of pbes_spec_stream is parsed as an mCRL2
  *         PBES specification.
  * \return The parsed PBES specification, if everything went well;
  *         NULL, otherwise.
**/
ATermAppl parse_pbes_spec(std::istream &pbes_spec_stream);


/** \brief  Parse mCRL2 data variables, according to the syntax of
  *         data variable declarations following the var keyword
  * \param[in] sf_stream An input stream from which can be read.
  * \post   The content of sf_stream is parsed as a list of mCRL2 data
  *         variables
  * \return The parsed list of data variables, if everything went well;
  *         NULL, otherwise.
**/
ATermList parse_data_vars(std::istream &sf_stream);

/** \brief  Returns wheter s is a valid user identifier
 *  \param[in] s An input string
 *  \return true iff s is a user identifier, i.e. if s is not a reserved
 *          identifer.
**/
bool is_user_identifier(std::string const& s);

  }
}

#endif // MCRL2_PARSE_H
