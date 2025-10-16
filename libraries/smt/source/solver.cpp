// Author(s): Ruud Koolen, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file solver.cpp

#include "mcrl2/data/list.h"
#include "mcrl2/smt/translate_specification.h"
#include "mcrl2/smt/solver.h"
#include "mcrl2/smt/unfold_pattern_matching.h"


namespace mcrl2::smt
{

answer smt_solver::execute_and_check(const std::string& s, const std::chrono::microseconds& timeout) const
{
  z3.write(s);

  std::string result = timeout == std::chrono::microseconds::zero() ? z3.read() : z3.read(timeout);
  if (result.starts_with("sat"))
  {
    return answer::SAT;
  }
  else if (result.starts_with("unsat"))
  {
    return answer::UNSAT;
  }
  else if (result.starts_with("unknown"))
  {
    return answer::UNKNOWN;
  }
  else
  {
    mCRL2log(log::error) << "Error when checking satisfiability of \n" << s.substr(0, 500) << "...." << std::endl;
    throw mcrl2::runtime_error("Got unexpected response from SMT-solver:\n" + result);
  }
}

smt_solver::smt_solver(const data::data_specification& dataspec)
: m_native(initialise_native_translation(dataspec))
, z3("Z3")
{
  std::ostringstream out;
  translate_data_specification(dataspec, out, m_cache, m_native);
  z3.write(out.str());
}

answer smt_solver::solve(const data::variable_list& vars, const data::data_expression& expr, const std::chrono::microseconds& timeout)
{
  z3.write("(push)\n");
  std::ostringstream out;
  translate_variable_declaration(vars, out, m_cache, m_native);
  translate_assertion(expr, out, m_cache, m_native);
  out << "(check-sat)\n";
  answer result = execute_and_check(out.str(), timeout);
  z3.write("(pop)\n");
  return result;
}


namespace detail {

struct fixed_string_translation: public native_translation_t
{
  std::string translation;

  fixed_string_translation(const std::string& s)
  : translation(s)
  {}

  void operator()(const data::data_expression& /*e*/,
                         const std::function<void(std::string)>& output_func,
                         const std::function<void(data::data_expression)>& /*translate_func*/) const
  {
    output_func(translation);
  }
};

static const native_translation_t pp_translation = [](const data::data_expression& e,
                                                      const std::function<void(std::string)>& output_func,
                                                      const std::function<void(data::data_expression)>& /*translate_func*/)
{
  output_func(data::pp(e));
};
static const native_translation_t pp_real_translation = [](const data::data_expression& e,
                                                           const std::function<void(std::string)>& output_func,
                                                           const std::function<void(data::data_expression)>& /*translate_func*/)
{
  assert(data::sort_real::is_creal_application(e));
  const data::application& a = atermpp::down_cast<data::application>(e);
  output_func("(/ " +  data::pp(a[0]) + ".0  " + data::pp(a[1]) + ".0)");
};
static const native_translation_t reconstruct_divmod = [](const data::data_expression& e,
                                                          [[maybe_unused]]
                                                          const std::function<void(std::string)>& output_func,
                                                          [[maybe_unused]]
                                                          const std::function<void(data::data_expression)>& translate_func)
{
#ifndef MCRL2_ENABLE_MACHINENUMBERS
  assert(data::sort_nat::is_first_application(e) || data::sort_nat::is_last_application(e));
  const data::application& a = atermpp::down_cast<data::application>(e);
  if(data::sort_nat::is_divmod_application(a[0]))
  {
    const data::application& a2 = atermpp::down_cast<data::application>(a[0]);
    output_func("(" + std::string(data::sort_nat::is_first_application(a2) ? "div" : "mod") + " ");
    translate_func(a2[0]);
    output_func(" ");
    translate_func(a2[1]);
    output_func(")");
  }
  else
#endif
  {
    throw translation_error("Cannot perform native translation of " + pp(e));
  }
};

} // namespace detail

native_translations initialise_native_translation(const data::data_specification& dataspec)
{
  using namespace detail;
  using namespace data;
  native_translations nt;

  nt.sorts[sort_bool::bool_()] = "Bool";
  nt.sorts[sort_pos::pos()] = "Int";
  nt.sorts[sort_nat::nat()] = "Int";
  nt.sorts[sort_int::int_()] = "Int";
  nt.sorts[sort_real::real_()] = "Real";
#ifdef MCRL2_ENABLE_MACHINENUMBERS
  nt.sorts[sort_machine_word::machine_word()] = "Int";
#endif // MCRL2_ENABLE_MACHINENUMBERS

  std::vector<sort_expression> number_sorts({ sort_pos::pos(), sort_nat::nat(), sort_int::int_(), sort_real::real_() });
  std::set<sort_expression> sorts = dataspec.sorts();
  // Add native definitions for the left-hand side of every alias
  for(const auto& alias: dataspec.sort_alias_map())
  {
    sorts.insert(alias.first);
  }
  for(const sort_expression& sort: sorts)
  {
    if(is_basic_sort(sort) || is_container_sort(sort))
    {
      nt.set_native_definition(equal_to(sort), "=");
      nt.set_native_definition(not_equal_to(sort), "distinct");
      nt.set_native_definition(if_(sort), "ite");

      if(std::find(number_sorts.begin(), number_sorts.end(), sort) != number_sorts.end())
      {
        nt.set_native_definition(less(sort));
        nt.set_native_definition(less_equal(sort));
        nt.set_native_definition(greater(sort));
        nt.set_native_definition(greater_equal(sort));
        nt.set_alternative_name(sort_real::exp(sort, sort == sort_real::real_() ? sort_int::int_() : sort_nat::nat()), "^");
        nt.set_alternative_name(sort_real::minus(sort, sort), "-");
        nt.set_alternative_name(sort_real::negate(sort), "-");
      }
      else if(sort == basic_sort("@NatPair") || sort == basic_sort("@NatNatPair") || sort == basic_sort("@word"))
      {
        // NatPair is not used and its equations upset Z3
        nt.set_native_definition(less(sort));
        nt.set_native_definition(less_equal(sort));
        nt.set_native_definition(greater(sort));
        nt.set_native_definition(greater_equal(sort));
      }
      else
      {
        // Functions <, <=, >, >= are already defined on Int/Real and cannot be overloaded
        nt.set_alternative_name(less(sort), "@less");
        nt.set_alternative_name(less_equal(sort), "@less_equal");
        nt.set_alternative_name(greater(sort), "@greater");
        nt.set_alternative_name(greater_equal(sort), "@greater_equal");
      }

      std::string printed_sort_name = nt.sorts.find(sort) != nt.sorts.end() ? nt.sorts.find(sort)->second : translate_identifier(pp(sort));
      std::string list_sort_name = "(List " + printed_sort_name + ")";
      nt.set_alternative_name(sort_list::count(sort), "@list_count");
      nt.set_alternative_name(sort_list::snoc(sort), "@snoc");
      nt.set_native_definition(sort_list::empty(sort), "nil");
      nt.set_ambiguous(sort_list::empty(sort));
      nt.set_native_definition(sort_list::cons_(sort), "insert");
      nt.set_native_definition(sort_list::head(sort));
      nt.set_native_definition(sort_list::tail(sort));
      nt.sorts[sort_list::list(sort)] = list_sort_name;
    }
  }

  std::vector<function_symbol_vector> fs_numbers(
    {
      sort_pos::pos_generate_functions_code(),
      sort_nat::nat_generate_functions_code(),
      sort_int::int_generate_functions_code(),
      sort_real::real_generate_functions_code()
    }
  );
  for(const auto& fsv: fs_numbers)
  {
    for(const auto& fs: fsv)
    {
      nt.set_native_definition(fs);
    }
  }

  auto one = [](const sort_expression& s)
  {
    return s == sort_int::int_() ? sort_int::int_(1) : sort_real::real_(1);
  };
  for(const sort_expression& s: { sort_int::int_(), sort_real::real_() })
  {
    variable v1("v1", s);
    variable v2("v2", s);
    variable_list l1({v1});
    variable_list l2({v1,v2});
    nt.mappings[sort_real::minimum(s,s)] = data_equation(l2, sort_real::minimum(v1, v2), if_(less(v1,v2), v1, v2));
    nt.mappings[sort_real::maximum(s,s)] = data_equation(l2, sort_real::maximum(v1, v2), if_(less(v1,v2), v2, v1));
    nt.mappings[sort_real::succ(s)] = data_equation(l1, sort_real::succ(v1), sort_real::plus(v1, one(s)));
    nt.mappings[sort_real::pred(s)] = data_equation(l1, sort_real::pred(v1), sort_real::minus(v1, one(s)));
    nt.set_ambiguous(sort_real::minimum(s,s));
    nt.set_ambiguous(sort_real::maximum(s,s));
    nt.set_ambiguous(sort_real::succ(s));
    nt.set_ambiguous(sort_real::pred(s));
  }
  nt.expressions[sort_nat::first()] = reconstruct_divmod;
  nt.expressions[sort_nat::last()] = reconstruct_divmod;

  // TODO come up with equations for these
  // nt.mappings[sort_real::floor(s)]
  // nt.mappings[sort_real::ceil(s)]
  // nt.mappings[sort_real::round(s)]

  nt.set_native_definition(sort_bool::not_(), "not");
  nt.set_native_definition(sort_bool::and_(), "and");
  nt.set_native_definition(sort_bool::or_(), "or");
  nt.set_native_definition(sort_bool::implies());

nt.set_native_definition(sort_nat::c0(), pp(sort_nat::c0()));
  nt.set_native_definition(sort_pos::c1(), pp(sort_pos::c1()));
#ifdef MCRL2_ENABLE_MACHINENUMBERS
  nt.set_native_definition(sort_pos::succpos(), "+ 1");
  nt.set_native_definition(sort_nat::succ_nat(), "+ 1");
  variable v1("v1", sort_nat::nat());
  nt.mappings[sort_nat::natpred()] = data_equation(variable_list({v1}), sort_nat::natpred(v1), sort_int::minus(v1, sort_nat::nat(1)));
  nt.set_native_definition(sort_machine_word::zero_word(), "0");
  nt.set_native_definition(sort_machine_word::one_word(), "1");
  nt.expressions[sort_pos::most_significant_digit()] = pp_translation;
  nt.expressions[sort_pos::concat_digit()] = pp_translation;
  nt.expressions[sort_nat::most_significant_digit_nat()] = pp_translation;
  nt.expressions[sort_nat::concat_digit(sort_nat::nat(), sort_machine_word::machine_word())] = pp_translation;
  // nt.set_ambiguous(sort_nat::concat_digit())
#else
  nt.expressions[sort_pos::cdub()] = pp_translation;
  nt.set_native_definition(sort_nat::cnat(), "@id");
#endif
  nt.set_native_definition(sort_int::cneg(), "-");
  nt.set_native_definition(sort_int::cint(), "@id");
  nt.expressions[sort_real::creal()] = pp_real_translation;
  nt.set_native_definition(sort_real::creal());

  nt.special_recogniser[data::sort_bool::true_()] = "@id";
  nt.special_recogniser[data::sort_bool::false_()] = "not";
  nt.special_recogniser[data::sort_pos::c1()] = "= 1";
nt.special_recogniser[data::sort_nat::c0()] = "= 0";
#ifdef MCRL2_ENABLE_MACHINENUMBERS
nt.special_recogniser[sort_pos::succpos()] = ">= 2";
  nt.special_recogniser[sort_nat::succ_nat()] = ">= 1";
#else
  nt.special_recogniser[data::sort_pos::cdub()] = ">= 2";
  nt.special_recogniser[data::sort_nat::cnat()] = ">= 1";
#endif
  nt.special_recogniser[data::sort_int::cneg()] = "< 0";
  nt.special_recogniser[data::sort_int::cint()] = ">= 0";

  nt.set_native_definition(sort_nat::pos2nat(), "@id");
  nt.set_native_definition(sort_nat::nat2pos(), "@id");
  nt.set_native_definition(sort_int::pos2int(), "@id");
  nt.set_native_definition(sort_int::int2pos(), "@id");
  nt.set_native_definition(sort_int::nat2int(), "@id");
  nt.set_native_definition(sort_int::int2nat(), "@id");
  nt.set_native_definition(sort_real::pos2real(), "to_real");
  nt.set_native_definition(sort_real::real2pos(), "to_int");
  nt.set_native_definition(sort_real::nat2real(), "to_real");
  nt.set_native_definition(sort_real::real2nat(), "to_int");
  nt.set_native_definition(sort_real::int2real(), "to_real");
  nt.set_native_definition(sort_real::real2int(), "to_int");

  function_symbol id_bool("@id", function_sort({sort_bool::bool_()}, sort_bool::bool_()));
  function_symbol id_int("@id", function_sort({sort_int::int_()}, sort_int::int_()));
  function_symbol id_real("@id", function_sort({sort_real::real_()}, sort_real::real_()));
  variable vb("b", sort_bool::bool_());
  variable vi("i", sort_int::int_());
  variable vr("r", sort_real::real_());
  nt.mappings[id_bool] = data_equation(variable_list({vb}), id_bool(vb), vb);
  nt.mappings[id_int] = data_equation(variable_list({vi}), id_int(vi), vi);
  nt.mappings[id_real] = data_equation(variable_list({vr}), id_real(vr), vr);
  // necessary for translating the equations above
  nt.set_native_definition(equal_to(sort_bool::bool_()), "=");
  nt.set_native_definition(equal_to(sort_int::int_()), "=");
  nt.set_native_definition(equal_to(sort_real::real_()), "=");

  unfold_pattern_matching(dataspec, nt);

  return nt;
}


} // namespace mcrl2::smt

