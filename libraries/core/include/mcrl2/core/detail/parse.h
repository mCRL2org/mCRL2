// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parse.h
///
/// \brief A library for parsing stream into the internal mCRL2 format

#ifndef MCRL2_PARSE_H
#define MCRL2_PARSE_H

#include <iostream>
#include <vector>
#include <aterm2.h>

namespace mcrl2 {
  namespace core {
    namespace detail {
  
//Global precondition: the ATerm library has been initialised

/** \pre     se_stream is an input stream from which can be read       
  * \post    the content of se_stream is parsed as an mCRL2 sort expression
  * \return  the parsed sort expression, if everything went well
  *          NULL, otherwise
**/ 
ATermAppl parse_sort_expr(std::istream &se_stream);


/** \pre    de_stream is an input stream from which can be read       
  * \post   the content of de_stream is parsed as an mCRL2 data expression
  * \return the parsed data expression, if everything went well
  *         NULL, otherwise
**/ 
ATermAppl parse_data_expr(std::istream &de_stream);


/** \pre    ma_stream is an input stream from which can be read       
  * \post   the content of ma_stream is parsed as an mCRL2 multi-action
  * \return the parsed multi-action, if everything went well
  *         NULL, otherwise
**/ 
ATermAppl parse_mult_act(std::istream &ma_stream);


/** \pre    pe_stream is an input stream from which can be read       
  * \post   the content of pe_stream is parsed as an mCRL2 process expression
  * \return the parsed process expression, if everything went well
  *         NULL, otherwise
**/ 
ATermAppl parse_proc_expr(std::istream &pe_stream);


/** \pre    ps_stream is an input stream from which can be read       
  * \post   the content of ps_stream is parsed as an mCRL2 process
  *         specification
  * \return the parsed process specification, if everything went well
  *         NULL, otherwise
**/ 
ATermAppl parse_proc_spec(std::istream &ps_stream);


/** \pre    sf_stream is an input stream from which can be read       
  * \post   the content of sf_stream is parsed as an mCRL2 state formula
  * \return the parsed state formula, if everything went well
  *         NULL, otherwise
**/ 
ATermAppl parse_state_frm(std::istream &sf_stream);


/** \pre    sf_stream is an input stream from which can be read       
  * \post   the content of sf_stream is parsed as an action rename specification
  * \return the parsed rename rule file, if everything went well
  *         NULL, otherwise
**/ 
ATermAppl parse_action_rename_spec(std::istream &sf_stream);


/** \pre    pbes_spec_stream is an input stream from which can be read       
  * \post   the content of pbes_spec_stream is parsed as an mCRL2
  *         parameterised boolean equation system specification
  * \return the parsed PBES specification, if everything went well
  *         NULL, otherwise
**/ 
ATermAppl parse_pbes_spec(std::istream &pbes_spec_stream);

    }
  }
}

#endif // MCRL2_PARSE_H
