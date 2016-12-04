// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file regfrmtrans.cpp

#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/xyz_identifier_generator.h"
#include "mcrl2/modal_formula/translate_regular_formulas.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::state_formulas;
using namespace mcrl2::regular_formulas;

namespace mcrl2
{
namespace regular_formulas
{
namespace detail
{

//local declarations
//------------------

static state_formula translate_reg_frms_appl(state_formula part, xyz_identifier_generator &xyz_generator);
/*Pre: part represents a part of a state formula
 *     after the data implementation phase
 *Ret: part in which all regular formulas are translated in terms of state and
 *     action formulas
 */

//implementation
//--------------

state_formula translate_reg_frms(const state_formula &state_frm)
{
  xyz_identifier_generator xyz_generator(find_identifiers(state_frm));
  return translate_reg_frms_appl(state_frm, xyz_generator);
}

static state_formula translate_reg_frms_appl(state_formula part, xyz_identifier_generator& xyz_generator)
{
  if (data::is_data_expression(part) ||
      lps::is_multi_action(part) ||
      state_formulas::is_variable(part) ||
      data::is_assignment(part) ||
      state_formulas::is_true(part) ||
      state_formulas::is_false(part) ||
      state_formulas::is_yaled(part) ||
      state_formulas::is_yaled_timed(part) ||
      state_formulas::is_delay(part) ||
      state_formulas::is_delay_timed(part)
     )
  {
    //part is a data expression, a multiaction, a state variable or a data
    //variable declaration (with or without initialisation); return part
  }
  else if (state_formulas::is_must(part))
  {
    //part is the must operator; return equivalent non-regular formula
    const regular_formula reg_frm = must(part).formula();
    const state_formula phi = must(part).operand();
    if (regular_formulas::is_seq(reg_frm))
    {
      const regular_formula R1 = seq(reg_frm).left();
      const regular_formula R2 = seq(reg_frm).right();
      //red([R1.R2]phi) -> red([R1][R2]phi)
      part = translate_reg_frms_appl(must(R1, must(R2, phi)),xyz_generator);
    }
    else if (regular_formulas::is_alt(reg_frm))
    {
      const regular_formula R1 = alt(reg_frm).left();
      const regular_formula R2 = alt(reg_frm).right();
      //red([R1+R2]phi) -> red([R1]phi) && red([R2]phi)
      part = and_(
               translate_reg_frms_appl(must(R1,phi),xyz_generator),
               translate_reg_frms_appl(must(R2,phi),xyz_generator)
             );
    }
    else if (regular_formulas::is_trans(reg_frm))
    {
      const regular_formula R = trans(atermpp::aterm_appl(reg_frm)).operand();
      //red([R+]phi) -> red([R.R*]phi)
      part = translate_reg_frms_appl(must(seq(R,trans_or_nil(R)),phi),xyz_generator);
    }
    else if (regular_formulas::is_trans_or_nil(reg_frm))
    {
      const regular_formula R = trans_or_nil(atermpp::aterm_appl(reg_frm)).operand();
      //red([R*]phi) -> nu X. red(phi) && red([R]X),
      //where X does not occur free in phi and R
      const identifier_string X = xyz_generator("X");
      part = nu(X, assignment_list(), and_(
                     translate_reg_frms_appl(phi,xyz_generator),
                     translate_reg_frms_appl(must(R,mcrl2::state_formulas::variable(X, data_expression_list())),xyz_generator)
               ));
    }
    else
    {
      //reg_frm is an action formula; reduce phi
      part = must(reg_frm, translate_reg_frms_appl(phi,xyz_generator));
    }
  }
  else if (state_formulas::is_may(part))
  {
    //part is the may operator; return equivalent non-regular formula
    regular_formula reg_frm = may(part).formula();
    state_formula phi = may(part).operand();
    if (regular_formulas::is_seq(reg_frm))
    {
      const regular_formula R1 = seq(reg_frm).left();
      const regular_formula R2 = seq(reg_frm).right();
      //red(<R1.R2>phi) -> red(<R1><R2>phi)
      part = translate_reg_frms_appl(may(R1, may(R2, phi)),xyz_generator);
    }
    else if (regular_formulas::is_alt(reg_frm))
    {
      const regular_formula R1 = alt(reg_frm).left();
      const regular_formula R2 = alt(reg_frm).right();
      //red(<R1+R2>phi) -> red(<R1>phi) || red(<R2>phi)
      part = or_(
               translate_reg_frms_appl(may(R1,phi),xyz_generator),
               translate_reg_frms_appl(may(R2,phi),xyz_generator)
             );
    }
    else if (regular_formulas::is_trans(reg_frm))
    {
      const regular_formula R = trans(atermpp::aterm_appl(reg_frm)).operand();
      //red(<R+>phi) -> red(<R.R*>phi)
      part = translate_reg_frms_appl(may(seq(R,trans_or_nil(R)),phi),xyz_generator);
    }
    else if (regular_formulas::is_trans_or_nil(reg_frm))
    {
      const regular_formula R = trans_or_nil(atermpp::aterm_appl(reg_frm)).operand();
      //red(<R*>phi) -> mu X. red(phi) || red(<R>X),
      //where X does not occur free in phi and R
      const identifier_string X =xyz_generator("X");
      part = mu(X, assignment_list(), or_(
                     translate_reg_frms_appl(phi,xyz_generator),
                     translate_reg_frms_appl(may(R,mcrl2::state_formulas::variable(X, data_expression_list())),xyz_generator)
               ));
    }
    else
    {
      //reg_frm is an action formula; reduce phi
      part = may(reg_frm, translate_reg_frms_appl(phi,xyz_generator));
    }
  }
  else if (state_formulas::is_not(part))
  {
    const not_& not_part = atermpp::down_cast<not_>(part);
    part = not_(translate_reg_frms_appl(not_part.operand(),xyz_generator));
  }
  else if (state_formulas::is_and(part))
  {
    part = and_(translate_reg_frms_appl(and_(part).left(),xyz_generator),
                translate_reg_frms_appl(and_(part).right(),xyz_generator));
  }
  else if (state_formulas::is_or(part))
  {
    part = or_(translate_reg_frms_appl(or_(part).left(),xyz_generator),
               translate_reg_frms_appl(or_(part).right(),xyz_generator));
  }
  else if (state_formulas::is_imp(part))
  {
    part = imp(translate_reg_frms_appl(imp(part).left(),xyz_generator),
                translate_reg_frms_appl(imp(part).right(),xyz_generator));
  }
  else if (state_formulas::is_forall(part))
  {
    part = mcrl2::state_formulas::forall(mcrl2::state_formulas::forall(part).variables(),
                  translate_reg_frms_appl(mcrl2::state_formulas::forall(part).body(),xyz_generator));
  }
  else if (state_formulas::is_exists(part))
  {
    part = mcrl2::state_formulas::exists(mcrl2::state_formulas::exists(part).variables(),
                  translate_reg_frms_appl(mcrl2::state_formulas::exists(part).body(),xyz_generator));
  }
  else if (state_formulas::is_nu(part))
  {
    const nu nu_part(part);
    part = nu(nu_part.name(),nu_part.assignments(),
                  translate_reg_frms_appl(nu_part.operand(),xyz_generator));
  }
  else if (state_formulas::is_mu(part))
  {
    const mu mu_part(part);
    part = mu(mu_part.name(),mu_part.assignments(),
                  translate_reg_frms_appl(mu_part.operand(),xyz_generator));
  }
  else
  {
    assert(0); //Unexpected state_formula.
  }

  return part;
}

}   // namespace detail
}   // namespace regular_formulas
}     // namespace mcrl2
