// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdio>

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/sort_expression.h"
#include "pbes2yices.h"

#include "mcrl2/utilities/exception.h"

#include "mcrl2/data/bool.h"
#ifdef MCRL2_ENABLE_MACHINENUMBERS
#include "mcrl2/data/int64.h"
#include "mcrl2/data/nat64.h"
#include "mcrl2/data/pos64.h"
#include "mcrl2/data/real64.h"
#else
#include "mcrl2/data/int.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/real.h"
#endif
#include "mcrl2/data/replace.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/data/substitutions/data_expression_assignment.h"

using namespace mcrl2;
using namespace mcrl2::data;

static std::string sanitize_term(std::string term)
{
  if (term == "-") {
    return "minus";
  }
  return term;
}

// Topological sort.
template<class T> std::vector<T> tsort(std::map<T, std::set<T> > dependencies)
{
  std::map<T, std::set<T> > reverse_dependencies;
  for (typename std::map<T, std::set<T> >::const_iterator i = dependencies.begin(); i != dependencies.end(); i++) {
    reverse_dependencies[i->first] = std::set<T>();
  }
  for (typename std::map<T, std::set<T> >::const_iterator i = dependencies.begin(); i != dependencies.end(); i++) {
    for (typename std::set<T>::const_iterator j = i->second.begin(); j != i->second.end(); j++) {
      reverse_dependencies[*j].insert(i->first);
    }
  }
  
  std::vector<T> output;
  for (typename std::map<T, std::set<T> >::const_iterator i = dependencies.begin(); i != dependencies.end(); i++) {
    if (i->second.empty()) {
      output.push_back(i->first);
    }
  }
  
  size_t index = 0;
  while (index < output.size()) {
    T vertex = output[index];
    index++;
    
    const std::set<T> &edges = reverse_dependencies.at(vertex);
    for (typename std::set<T>::const_iterator i = edges.begin(); i != edges.end(); i++) {
      dependencies[*i].erase(vertex);
      if (dependencies[*i].empty()) {
        output.push_back(*i);
      }
    }
  }
  
  for (typename std::map<T, std::set<T> >::const_iterator i = dependencies.begin(); i != dependencies.end(); i++) {
    if (!i->second.empty()) {
      throw mcrl2::runtime_error("Dependency loop");
    }
  }
  
  return output;
}

static std::set<data::function_symbol> get_builtin_constructors()
{
  std::set<data::function_symbol> output;
  output.insert(sort_pos::c1());
#ifndef MCRL2_ENABLE_MACHINENUMBERS
  output.insert(sort_pos::cdub());
  output.insert(sort_nat::cnat());
#endif
  output.insert(sort_nat::c0());
  output.insert(sort_int::cint());
  output.insert(sort_int::cneg());
  output.insert(sort_real::creal());
  return output;
}

std::string translate_expression(data_expression expression, const std::map<variable, std::string> &bound_variables, const translated_data_specification &translation)
{
  if (sort_real::is_creal_application(expression)) {
    application a(expression);
    return "(/ " + translate_expression(*a.begin(), bound_variables, translation) + " " + translate_expression(*++a.begin(), bound_variables, translation) + ")";
  } else if (sort_int::is_cint_application(expression)) {
    application a(expression);
    return translate_expression(*a.begin(), bound_variables, translation);
  } else if (sort_int::is_cneg_application(expression)) {
    application a(expression);
    std::string result = translate_expression(*a.begin(), bound_variables, translation);
    if (result[0] == '(') {
      return "(- 0 " + result + ")";
    } else {
      return "-" + result;
    }
  } else if (sort_nat::is_c0_function_symbol(expression)) {
    return "0";
#ifndef MCRL2_ENABLE_MACHINENUMBERS
  } else if (sort_nat::is_cnat_application(expression)) {
    application a(expression);
    return translate_expression(*a.begin(), bound_variables, translation);
  } else if (sort_pos::is_cdub_application(expression)) {
    return data::pp(expression);
#endif
  } else if (sort_pos::is_c1_function_symbol(expression)) {
    return data::pp(expression);
  }
  
  
  
  if (is_function_symbol(expression)) {
    function_symbol s(expression);
    assert(translation.function_names.count(s));
    return translation.function_names.at(s);
  } else if (is_variable(expression)) {
    assert(bound_variables.count(variable(expression)));
    return bound_variables.at(variable(expression));
  } else if (is_application(expression)) {
    application a(expression);
    assert(is_function_symbol(a.head()));
    
    assert(translation.function_names.count(function_symbol(a.head())));
    if (translation.function_names.at(function_symbol(a.head())) == "") {
      assert(a.size() == 1);
      return translate_expression(*a.begin(), bound_variables, translation);
    }
    
    std::string output = "(" + translation.function_names.at(function_symbol(a.head()));
    for (const mcrl2::data::data_expression& i : a)
    {
      output += " " + translate_expression(i, bound_variables, translation);
    }
    output += ")";
    return output;
  } else {
    throw mcrl2::runtime_error("Unable to translate expression " + data::pp(expression) + ", giving up.");
  }
}

// Find all sorts that need to be defined in yices, ordered by dependency structure.
static std::vector<mcrl2::data::sort_expression> required_sorts(const mcrl2::pbes_system::pbes &pbes)
{
  std::set<sort_expression> all_sorts(pbes.data().sorts().begin(), pbes.data().sorts().end());
  
  std::map<sort_expression, std::set<sort_expression> > sort_dependencies;
  for (std::set<sort_expression>::const_iterator i = all_sorts.begin(); i != all_sorts.end(); i++) {
    if (is_container_sort(*i)) {
      std::set<sort_expression> dependencies;
      sort_expression base_sort = container_sort(*i).element_sort();
      dependencies.insert(base_sort);
      sort_dependencies[*i] = dependencies;
    } else if (is_basic_sort(*i)) {
      std::set<sort_expression> dependencies;
      function_symbol_vector constructors = pbes.data().constructors(*i);
      for (function_symbol_vector::iterator j = constructors.begin(); j != constructors.end(); j++) {
        if (is_function_sort(j->sort())) {
          sort_expression_list domain = function_sort(j->sort()).domain();
          for (const mcrl2::data::sort_expression& k : domain)
          {
            if (sort_expression(k) != *i)
            {
              dependencies.insert(sort_expression(k));
            }
          }
        }
      }
      sort_dependencies[*i] = dependencies;
    }
  }
  
  try 
  {
    return tsort(sort_dependencies);
  } 
  catch(mcrl2::runtime_error& ) 
  {
    throw mcrl2::runtime_error("Mutually recursive types detected, giving up.");
  }
}

// Flattens a sort name to a yices-compatible form.
static std::string mangle_sort_name(sort_expression sort)
{
  sort_expression base_sort = sort;
  std::string name_prefix = "";
  while (is_container_sort(base_sort)) {
    container_sort container(base_sort);
    name_prefix += container.container_name().function().name();
    name_prefix += "-";
    base_sort = container.element_sort();
  }
  assert(is_basic_sort(base_sort));
  
  std::string base_name = sanitize_term(basic_sort(base_sort).name());
  return name_prefix + base_name;
}

static void translate_sort_definition(const mcrl2::data::data_specification &data, mcrl2::data::sort_expression sort, set_identifier_generator &function_name_generator, translated_data_specification &translation)
{
  assert(is_basic_sort(sort) || is_container_sort(sort));
  
  if (sort == sort_bool::bool_()) {
    translation.sort_names[sort] = "bool";
    translation.function_names[sort_bool::true_()] = "true";
    translation.function_names[sort_bool::false_()] = "false";
  } else if (sort == sort_nat::nat()) {
    translation.sort_names[sort] = "nat";
  } else if (sort == sort_int::int_()) {
    translation.sort_names[sort] = "int";
  } else if (sort == sort_real::real_()) {
    translation.sort_names[sort] = "real";
  } else if (sort == sort_pos::pos()) {
    translation.definition += "(define-type pos (subtype (n::nat) (> n 0 )))\n";
    translation.sort_names[sort] = "pos";
  } else {
    std::string name = mangle_sort_name(sort);
    function_symbol_vector constructors = data.constructors(sort);
    
    std::string definition;
    if (constructors.size() == 0) {
      definition = "(define-type " + name + ")";
    } else {
      definition += "(define-type " + name + " (datatype";
      int index = 0;
      for (function_symbol_vector::iterator i = constructors.begin(); i != constructors.end(); i++) {
        std::string constructor_name = sanitize_term(i->name());
        if (function_name_generator.has_identifier(constructor_name)) {
          constructor_name = name + "-" + constructor_name;
        }
        translation.function_names[*i] = function_name_generator(constructor_name);
        translation.constructor_field_names[*i] = std::vector<std::string>();
        
        definition += " ";
        if (is_function_sort(i->sort())) {
          definition += "(" + constructor_name;
          
          sort_expression_list domain = function_sort(i->sort()).domain();
          for (const mcrl2::data::sort_expression& j : domain)
          {
            std::string sort_name;
            if (j == sort)
            {
              sort_name = name;
            }
            else
            {
              assert(translation.sort_names.count(j) > 0);
              sort_name = translation.sort_names[j];
            }

            std::string field_name = "@@" + name + itoa(index++);
            translation.constructor_field_names[*i].push_back(field_name);
            definition += " " + field_name + "::" + sort_name;
          }

          definition += ")";
        } else {
          definition += constructor_name;
        }
      }
      definition += "))";
    }
    
    translation.definition += definition + "\n";
    translation.sort_names[sort] = name;
  }
}

class function_definition
{
  public:
  enum function_definition_type { builtin_type, equations_type, identity_type, unavailable_type };
  
  protected:
  function_definition_type m_type;
  std::string m_name;
  std::vector<data_equation> m_equations;
  
  public:
  function_definition(): m_type(unavailable_type) {}
  function_definition(const char *name): m_type(builtin_type), m_name(name) {}
  function_definition(std::string name): m_type(builtin_type), m_name(name) {}
  function_definition(const std::vector<data_equation> equations): m_type(equations_type), m_equations(equations) {}
  function_definition(data_equation definition): m_type(equations_type)
  {
    m_equations.push_back(definition);
  }
  
  protected:
  function_definition(function_definition_type type, std::string name, const std::vector<data_equation> equations):
    m_type(type),
    m_name(name),
    m_equations(equations)
  {}
  
  public:
  static function_definition unavailable() { return function_definition(unavailable_type, "", std::vector<data_equation>()); }
  static function_definition identity() { return function_definition(identity_type, "", std::vector<data_equation>()); }
  
  bool is_builtin() const { return m_type == builtin_type; }
  bool is_equations() const { return m_type == equations_type; }
  bool is_identity() const { return m_type == identity_type; }
  bool is_unavailable() const { return m_type == unavailable_type; }
  const std::string &name() const { return m_name; }
  const std::vector<data_equation> &equations() const { return m_equations; }
};

static void add_undefined_functions(std::map<mcrl2::data::function_symbol, function_definition> &function_definitions, const function_symbol_vector &functions)
{
  for (function_symbol_vector::const_iterator i = functions.begin(); i != functions.end(); i++) {
    std::string name = i->name();
    if (name[0] == '@' && function_definitions.count(*i) == 0) {
      function_definitions[*i] = function_definition::unavailable();
    }
  }
}

// Constructs a list of all mcrl2 types and functions that map to yices builtins.
static std::map<mcrl2::data::function_symbol, function_definition> builtin_function_map(const std::vector<sort_expression> &sorts)
{
  std::map<function_symbol, function_definition> output;
  
  basic_sort bool_ = sort_bool::bool_();
  basic_sort pos = sort_pos::pos();
  basic_sort nat = sort_nat::nat();
  basic_sort int_ = sort_int::int_();
  basic_sort real = sort_real::real_();
  
  
  variable b("b", bool_);
  variable p("p", pos);
  variable p2("p2", pos);
  variable n("n", nat);
  variable n2("n2", nat);
  variable i("i", int_);
  variable i2("i2", int_);
  variable r("r", real);
  variable r2("r2", real);
  
  
  output[sort_bool::not_()] = "not";
  output[sort_bool::and_()] = "and";
  output[sort_bool::or_()] = "or";
  output[sort_bool::implies()] = "=>";
  
  
  function_symbol pos_one("one", pos);
  output[pos_one] = "1";
  
  output[less(pos)] = "<";
  output[less_equal(pos)] = "<=";
  output[greater(pos)] = ">";
  output[greater_equal(pos)] = ">=";
  output[sort_pos::plus()] = "+";
  output[sort_pos::times()] = "*";
  output[sort_pos::succ()] = data_equation(variable_list({p}), sort_pos::succ(p), sort_pos::plus(p, pos_one));
  output[sort_pos::add_with_carry()] = "(lambda (c::bool p1::pos p2::pos) (if c (+ 1 p1 p2) (+ p1 p2)))";
  
  
  function_symbol nat_zero("zero", nat);
  output[nat_zero] = "0";
  function_symbol nat_one("one", nat);
  output[nat_one] = "1";
  function_symbol nat_two("two", nat);
  output[nat_two] = "2";
  function_symbol nat_minus("-", function_sort(sort_expression_list({nat, nat}), nat));
  output[nat_minus] = "-";
  function_symbol nat_do_exp("exp", function_sort(sort_expression_list({pos, nat}), pos));
  output[nat_do_exp] = data_equation(variable_list({p, n}), nat_do_exp(p, n), if_(equal_to(n, nat_one), p, if_(equal_to(sort_nat::mod(n, nat_two), nat_one), sort_pos::times(p, nat_do_exp(sort_pos::times(p, p), sort_nat::div(n, nat_two))), nat_do_exp(sort_pos::times(p, p), sort_nat::div(n, nat_two)))));
  
  output[less(nat)] = "<";
  output[less_equal(nat)] = "<=";
  output[greater(nat)] = ">";
  output[greater_equal(nat)] = ">=";
  output[sort_nat::pos2nat()] = function_definition::identity();
  output[sort_nat::nat2pos()] = function_definition::identity();
  output[sort_nat::maximum(pos, nat)] = data_equation(variable_list({p, n}), sort_nat::maximum(p, n), sort_nat::nat2pos(sort_nat::maximum(sort_nat::pos2nat(p), n)));
  output[sort_nat::maximum(nat, pos)] = data_equation(variable_list({n, p}), sort_nat::maximum(n, p), sort_nat::nat2pos(sort_nat::maximum(sort_nat::pos2nat(p), n)));
  output[sort_nat::succ(nat)] = data_equation(variable_list({n}), sort_nat::succ(n), sort_nat::plus(n, nat_one));
  output[sort_nat::pred()] = data_equation(variable_list({p}), sort_nat::pred(p), nat_minus(sort_nat::pos2nat(p), nat_one));
  output[sort_nat::plus(pos, nat)] = "+";
  output[sort_nat::plus(nat, pos)] = "+";
  output[sort_nat::plus(nat, nat)] = "+";
  output[sort_nat::times(nat, nat)] = "*";
  output[sort_nat::div()] = "div";
  output[sort_nat::mod()] = "mod";
  output[sort_nat::exp(nat, nat)] = data_equation(variable_list({n, n2}), sort_nat::exp(n, n2), if_(equal_to(n2, nat_zero), n, if_(less(n, nat_two), n, sort_nat::pos2nat(nat_do_exp(sort_nat::nat2pos(n), n2)))));
  output[sort_nat::exp(pos, nat)] = data_equation(variable_list({p, n2}), sort_nat::exp(p, n2), if_(equal_to(n2, nat_zero), p, if_(equal_to(p, pos_one), p, nat_do_exp(p, n2))));
  
  
  function_symbol int_zero("zero", int_);
  output[int_zero] = "0";
  function_symbol int_one("one", int_);
  output[int_one] = "1";
  
  output[less(int_)] = "<";
  output[less_equal(int_)] = "<=";
  output[greater(int_)] = ">";
  output[greater_equal(int_)] = ">=";
  output[sort_int::pos2int()] = function_definition::identity();
  output[sort_int::int2pos()] = function_definition::identity();
  output[sort_int::nat2int()] = function_definition::identity();
  output[sort_int::int2nat()] = function_definition::identity();
  output[sort_int::maximum(pos, int_)] = data_equation(variable_list({p, i}), sort_int::maximum(p, i), sort_int::int2pos(sort_int::maximum(sort_int::pos2int(p), i)));
  output[sort_int::maximum(int_, pos)] = data_equation(variable_list({i, p}), sort_int::maximum(i, p), sort_int::int2pos(sort_int::maximum(i, sort_int::pos2int(p))));
  output[sort_int::maximum(nat, int_)] = data_equation(variable_list({n, i}), sort_int::maximum(n, i), sort_int::int2nat(sort_int::maximum(sort_int::nat2int(n), i)));
  output[sort_int::maximum(int_, nat)] = data_equation(variable_list({i, n}), sort_int::maximum(i, n), sort_int::int2nat(sort_int::maximum(i, sort_int::nat2int(n))));
  output[sort_int::abs()] = data_equation(variable_list({i}), sort_int::abs(i), if_(less(i, int_zero), sort_int::minus(int_zero, i), i));
  output[sort_int::negate(pos)] = data_equation(variable_list({p}), sort_int::negate(p), sort_int::minus(int_zero, sort_int::pos2int(p)));
  output[sort_int::negate(nat)] = data_equation(variable_list({n}), sort_int::negate(n), sort_int::minus(int_zero, sort_int::nat2int(n)));
  output[sort_int::negate(int_)] = data_equation(variable_list({i}), sort_int::negate(i), sort_int::minus(int_zero, i));
  output[sort_int::succ(int_)] = data_equation(variable_list({i}), sort_int::succ(i), sort_int::plus(i, int_one));
  output[sort_int::pred(nat)] = data_equation(variable_list({n}), sort_int::pred(n), sort_int::minus(sort_int::nat2int(n), int_one));
  output[sort_int::pred(int_)] = data_equation(variable_list({i}), sort_int::pred(i), sort_int::minus(i, int_one));
  output[sort_int::plus(int_, int_)] = "+";
  output[sort_int::minus(pos, pos)] = "-";
  output[sort_int::minus(nat, nat)] = "-";
  output[sort_int::minus(int_, int_)] = "-";
  output[sort_int::times(int_, int_)] = "*";
  output[sort_int::div(int_, pos)] = "div";
  output[sort_int::mod(int_, pos)] = "mod";
  output[sort_int::exp(int_, nat)] = data_equation(variable_list({i, n}), sort_int::exp(i, n), if_(equal_to(sort_nat::mod(n, nat_two), nat_zero), sort_int::nat2int(sort_nat::exp(sort_int::abs(i), n)), sort_int::minus(int_zero, sort_int::nat2int(sort_nat::exp(sort_int::abs(i), n)))));
  
  
  function_symbol real_zero("zero", real);
  output[real_zero] = "0";
  function_symbol real_one("one", real);
  output[real_one] = "1";
  function_symbol real_half("half", real);
  output[real_half] = "(/ 1 2)";
  function_symbol real_div("div", function_sort(sort_expression_list({real, pos}), int_));
  output[real_div] = "div";
  function_symbol real_do_exp("exp", function_sort(sort_expression_list({real, nat}), real));
  output[real_do_exp] = data_equation(variable_list({r, n}), real_do_exp(r, n), if_(equal_to(n, nat_one), r, if_(equal_to(sort_nat::mod(n, nat_two), nat_one), sort_real::times(r, real_do_exp(sort_pos::times(r, r), sort_nat::div(n, nat_two))), real_do_exp(sort_real::times(r, r), sort_nat::div(n, nat_two)))));
  
  output[less(real)] = "<";
  output[less_equal(real)] = "<=";
  output[greater(real)] = ">";
  output[greater_equal(real)] = ">=";
  output[sort_real::pos2real()] = function_definition::identity();
  output[sort_real::real2pos()] = function_definition::identity();
  output[sort_real::nat2real()] = function_definition::identity();
  output[sort_real::real2nat()] = function_definition::identity();
  output[sort_real::int2real()] = function_definition::identity();
  output[sort_real::real2int()] = function_definition::identity();
  output[sort_real::creal()] = "/";
  output[sort_real::abs(real)] = data_equation(variable_list({r}), sort_real::abs(r), if_(less(r, real_zero), sort_real::minus(real_zero, r), r));
  output[sort_real::negate(real)] = data_equation(variable_list({r}), sort_real::negate(r), sort_real::minus(real_zero, r));
  output[sort_real::succ(real)] = data_equation(variable_list({r}), sort_real::succ(r), sort_real::plus(r, real_one));
  output[sort_real::pred(real)] = data_equation(variable_list({r}), sort_real::pred(r), sort_real::minus(r, real_one));
  output[sort_real::plus(real, real)] = "+";
  output[sort_real::minus(real, real)] = "-";
  output[sort_real::times(real, real)] = "*";
  output[sort_real::divides(pos, pos)] = "/";
  output[sort_real::divides(nat, nat)] = "/";
  output[sort_real::divides(int_, int_)] = "/";
  output[sort_real::divides(real, real)] = "/";
  output[sort_real::exp(real, int_)] = data_equation(variable_list({r, i}), sort_real::exp(r, i), if_(equal_to(i, int_zero), real_one, if_(less(i, int_zero), real_do_exp(sort_real::divides(real_one, r), sort_int::abs(i)), real_do_exp(r, sort_int::int2nat(i)))));
  output[sort_real::floor()] = data_equation(variable_list({r}), sort_real::floor(r), real_div(r, pos_one));
  output[sort_real::ceil()] = data_equation(variable_list({r}), sort_real::ceil(r), sort_real::negate(real_div(sort_real::negate(r), pos_one)));
  output[sort_real::round()] = data_equation(variable_list({r}), sort_real::round(r), sort_real::floor(sort_real::plus(r, real_half)));
  
  
  add_undefined_functions(output, sort_bool::bool_generate_functions_code());
  add_undefined_functions(output, sort_pos::pos_generate_functions_code());
  add_undefined_functions(output, sort_nat::nat_generate_functions_code());
  add_undefined_functions(output, sort_int::int_generate_functions_code());
  add_undefined_functions(output, sort_real::real_generate_functions_code());
  
  
  for (std::vector<sort_expression>::const_iterator i = sorts.begin(); i != sorts.end(); ++i) {
    variable v1("v1", *i);
    variable v2("v2", *i);
    
    output[if_(*i)] = "if";
    if (!output.count(greater(*i))) {
      output[greater(*i)] = data_equation(variable_list({v1, v2}), greater(v1, v2), sort_bool::not_(less_equal(v1, v2)));
    }
    if (!output.count(greater_equal(*i))) {
      output[greater_equal(*i)] = data_equation(variable_list({v1, v2}), greater_equal(v1, v2), sort_bool::not_(less(v1, v2)));
    }
  }
  
  
  return output;
}

// A rule is a data equation in a given matching context.
// Besides an rhs and condition, it has data expressions corresponding to matching terms,
// and instantiations for variables that no longer exist in the matching contest.
// For example, the rewrite rule "foo(x, y, []) = z" in the matching context "foo(v2 |> v3, v0, v1)"
//   with arguments = [a, b, head(L), tail(L)]
// has parameters { 0 -> y, 1 -> [] }:
//   matching term v0 corresponds to expression y,
//   matching term v1 corresponds to expression [],
//   matching terms v2 and v3 don't correspond to anything in the rewrite rule
// and bound_variables { x -> "L" }.
struct rule
{
  data_expression rhs;
  data_expression condition;
  std::map<size_t, data_expression> parameters;
  std::map<variable, std::string> bound_variables;
};

static std::string match_pattern(
  const data_specification &specification,
  const translated_data_specification &translation,
  sort_expression sort,
  data_expression pattern,
  set_identifier_generator &argument_variable_generator,
  std::vector<variable> argument_variables,
  std::vector<std::string> arguments,
  data_expression condition,
  std::vector<rule> rules)
{
  if (rules.empty()) {
    representative_generator generator(specification);
    data_expression expression = generator(sort);
    mCRL2log(log::warning) << "Completing function definition with rule " << data::pp(data_equation(variable_list(), condition, pattern, expression)) << "\n";
    return translate_expression(expression, std::map<variable, std::string>(), translation);
  }
  
  rule first_rule = rules[0];
  for (size_t i = 0; i < arguments.size(); ++i) {
    if (first_rule.parameters.count(i) && !is_variable(first_rule.parameters[i])) {
      assert(is_function_symbol(first_rule.parameters[i]) || is_application(first_rule.parameters[i]));
      
      function_symbol_vector constructors = specification.constructors(first_rule.parameters[i].sort().target_sort());
      
      std::map<function_symbol, size_t> constructor_indices;
      for (size_t j = 0; j < constructors.size(); ++j) {
        constructor_indices[constructors[j]] = j;
      }
      
      std::vector<std::vector<rule> > split_rules;
      split_rules.resize(constructors.size());
      for (std::vector<rule>::iterator j = rules.begin(); j != rules.end(); ++j) {
        rule new_rule;
        new_rule.rhs = j->rhs;
        new_rule.condition = j->condition;
        new_rule.bound_variables = j->bound_variables;
        for (size_t k = 0; k < i; ++k) {
          if (j->parameters.count(k)) {
            new_rule.parameters[k] = j->parameters[k];
          }
        }
        for (size_t k = i + 1; k < arguments.size(); ++k) {
          if (j->parameters.count(k)) {
            new_rule.parameters[k - 1] = j->parameters[k];
          }
        }
        
        if (!j->parameters.count(i) || is_variable(j->parameters[i])) {
          if (j->parameters.count(i)) {
            new_rule.bound_variables[variable(j->parameters[i])] = arguments[i];
          }
          
          for (std::vector<std::vector<rule> >::iterator k = split_rules.begin(); k != split_rules.end(); ++k) {
            k->push_back(new_rule);
          }
        } else {
          function_symbol constructor;
          if (is_function_symbol(j->parameters[i])) {
            constructor = function_symbol(j->parameters[i]);
          } else {
            assert(is_application(j->parameters[i]));
            application a(j->parameters[i]);
            constructor = function_symbol(a.head());
            
            size_t index = arguments.size() - 1;
            for (const mcrl2::data::data_expression& k : a)
            {
              new_rule.parameters[index++] = k;
            }
          }
          assert(constructor_indices.count(constructor));
          split_rules[constructor_indices[constructor]].push_back(new_rule);
        }
      }
      
      std::vector<std::string> base_arguments;
      base_arguments.insert(base_arguments.end(), arguments.begin(), arguments.begin() + i);
      base_arguments.insert(base_arguments.end(), arguments.begin() + i + 1, arguments.end());
      
      std::vector<variable> base_argument_variables;
      base_argument_variables.insert(base_argument_variables.end(), argument_variables.begin(), argument_variables.begin() + i);
      base_argument_variables.insert(base_argument_variables.end(), argument_variables.begin() + i + 1, argument_variables.end());
      
      std::string output;
      for (size_t j = 0; j < constructors.size(); ++j) {
        assert(translation.constructor_field_names.count(constructors[j]));
        std::vector<std::string> constructor_field_names = translation.constructor_field_names.at(constructors[j]);
        assert(constructor_field_names.size() == (is_function_sort(constructors[j].sort()) ? function_sort(constructors[j].sort()).domain().size() : 0));
        
        std::vector<std::string> case_arguments = base_arguments;
        std::vector<variable> term_variables;
        for (const std::string& constructor_field_name : constructor_field_names)
        {
          case_arguments.push_back("(" + constructor_field_name + " " + arguments[i] + ")");
          term_variables.emplace_back(argument_variable_generator("v"), sort_bool::bool_());
        }

        std::vector<variable> case_argument_variables = base_argument_variables;
        case_argument_variables.insert(case_argument_variables.end(), term_variables.begin(), term_variables.end());
        data_expression term;
        if (constructor_field_names.size() > 0) {
          term = application(constructors[j], term_variables.begin(), term_variables.end());
        } else {
          term = constructors[j];
        }
        data_expression_assignment assignment(argument_variables[i], term);
        data_expression new_pattern = replace_free_variables(pattern, assignment);
        
        std::string result = match_pattern(specification, translation, sort, new_pattern, argument_variable_generator, case_argument_variables, case_arguments, condition, split_rules[j]);
        if (j == 0) {
          output = result;
        } else {
          assert(translation.function_names.count(constructors[j]));
          output = "(if (" + translation.function_names.at(constructors[j]) + "? " + arguments[i] + ") " + result + " " + output + ")";
        }
      }
      
      return output;
    }
  }
  
  std::map<variable, std::string> bound_variables = first_rule.bound_variables;
  for (size_t i = 0; i < arguments.size(); ++i) {
    if (first_rule.parameters.count(i)) {
      assert(is_variable(first_rule.parameters[i]));
      bound_variables[variable(first_rule.parameters[i])] = arguments[i];
    }
  }
  
  if (first_rule.condition != sort_bool::true_()) {
    data_expression case_condition = first_rule.condition;
    std::string condition_result = translate_expression(case_condition, bound_variables, translation);
    std::string true_result = translate_expression(first_rule.rhs, bound_variables, translation);
    rules.erase(rules.begin());
    std::string false_result = match_pattern(specification, translation, sort, pattern, argument_variable_generator, argument_variables, arguments, sort_bool::and_(condition, sort_bool::not_(case_condition)), rules);
    
    return "(if " + condition_result + " " + true_result + " " + false_result + ")";
  }
  
  return translate_expression(first_rule.rhs, bound_variables, translation);
}


struct remove_duplicate_variables_builder: public data_expression_builder<remove_duplicate_variables_builder>
{
  using super = data_expression_builder<remove_duplicate_variables_builder>;
  using super::enter;
  using super::leave;
  using super::apply;

  set_identifier_generator generator;
  std::set<variable> seen;
  std::map<variable, variable> replacements;

  remove_duplicate_variables_builder(set_identifier_generator &generator_): generator(generator_) {}

  variable apply(const variable &v)
  {
    if (seen.count(v))
    {
      core::identifier_string name = generator(v.name());
      variable new_variable(name, v.sort());
      replacements[new_variable] = v;
      return new_variable;
    }
    else
    {
      seen.insert(v);
      return v;
    }
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};



// Converts a set of equations together defining a function into a function definition in yices format.
static std::string construct_function_definition(data::function_symbol function, const std::vector<data_equation> &equations, const data_specification &specification, const translated_data_specification &translation)
{
  if (!is_function_sort(function.sort())) {
    assert(equations.size() == 1);
    assert(equations[0].condition() == sort_bool::true_());
    std::string output;
    output += "(define ";
    output += translation.function_names.at(function);
    output += "::";
    output += translation.sort_names.at(function.sort());
    output += " ";
    output += translate_expression(equations[0].rhs(), std::map<variable, std::string>(), translation);
    output += ")";
    return output;
  }
  
  set_identifier_generator argument_variable_generator;
  std::vector<variable> argument_variables;
  std::vector<std::string> arguments;
  
  function_sort sort(function.sort());
  sort_expression_list domain = sort.domain();
  for (const sort_expression& i : domain)
  {
    core::identifier_string name = argument_variable_generator("v");
    arguments.push_back(name);
    argument_variables.emplace_back(name, i);
  }
  data_expression pattern = application(function, argument_variables.begin(), argument_variables.end());
  
  std::set<function_symbol> all_constructors(specification.constructors().begin(), specification.constructors().end());
  
  std::vector<rule> rules;
  for (std::vector<data_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i) {
    // The lhs may contain duplicate variables.
    // Replace them with fresh ones, and add equality conditions.
    set_identifier_generator generator;
    for (const variable& j : i->variables())
    {
      generator.add_identifier(j.name());
    }
    remove_duplicate_variables_builder remover(generator);
    data_expression lhs;
    remover.apply(lhs, i->lhs());
    std::vector<data_expression> conditions;
    if (i->condition() != sort_bool::true_()) {
      conditions.push_back(i->condition());
    }
    for (std::map<variable, variable>::iterator j = remover.replacements.begin(); j != remover.replacements.end(); ++j) {
      conditions.push_back(equal_to(j->first, j->second));
    }
    
    
    
    rule new_rule;
    new_rule.rhs = i->rhs();
    new_rule.condition = lazy::join_and(conditions.begin(), conditions.end());
    
    assert(is_application(lhs));
    application a(lhs);
    assert(a.head() == function);
    
    int index = 0;
    bool invalid = false;
    for (const data_expression& j : a)
    {
      new_rule.parameters[index++] = j;

      std::set<function_symbol> functions_used = find_function_symbols(j);
      for (std::set<function_symbol>::iterator k = functions_used.begin(); k != functions_used.end(); ++k) {
        if (!all_constructors.count(*k)) {
          invalid = true;
          break;
        }
      }
    }

    if (!invalid) {
      rules.push_back(new_rule);
    }
  }
  
  std::string rhs = match_pattern(specification, translation, sort.codomain(), pattern, argument_variable_generator, argument_variables, arguments, sort_bool::true_(), rules);
  
  assert(translation.function_names.count(function));
  assert(translation.sort_names.count(sort.codomain()));
  
  std::string output;
  output += "(define ";
  output += translation.function_names.at(function);
  output += "::(-> ";
  for (const sort_expression& i : domain)
  {
    assert(translation.sort_names.count(i));
    output += translation.sort_names.at(i);
    output += " ";
  }
  output += translation.sort_names.at(sort.codomain());
  output += ") (lambda (";
  size_t index = 0;
  for (const sort_expression& i : domain)
  {
    if (index != 0) {
      output += " ";
    }
    output += arguments[index++];
    output += "::";
    output += translation.sort_names.at(i);
  }
  output += ") ";
  output += rhs;
  output += "))";
  
  return output;
}

void translate_data_specification(const mcrl2::pbes_system::pbes &pbes, translated_data_specification &translation)
{
  const data_specification &data = pbes.data();
  
  set_identifier_generator function_name_generator;
  
  // Translate all sorts to yices sort definitions.
  std::vector<sort_expression> sorts = required_sorts(pbes);
  for (std::vector<sort_expression>::iterator i = sorts.begin(); i != sorts.end(); i++) {
    translate_sort_definition(data, *i, function_name_generator, translation);
    
    // Equality and inequality must be defined for all sorts.
    translation.function_names[equal_to(*i)] = "=";
    translation.function_names[not_equal_to(*i)] = "/=";
  }
  // The unrolling code may generate invocations of those functions.
  translation.function_names[sort_bool::and_()] = "and";
  translation.function_names[sort_bool::or_()] = "or";
  translation.function_names[sort_bool::not_()] = "not";
  
  std::set<data::function_symbol> builtin_constructors = get_builtin_constructors();
  std::map<mcrl2::data::function_symbol, function_definition> definitions = builtin_function_map(sorts);
  std::map<mcrl2::data::function_symbol, std::vector<data_equation> > equations;
  for (std::set<data_equation>::const_iterator i = pbes.data().equations().begin(); i != pbes.data().equations().end(); i++) {
    if (is_application(i->lhs())) {
      application lhs(i->lhs());
      if (is_function_symbol(lhs.head())) {
        function_symbol function(lhs.head());
        equations[function].push_back(*i);
      }
    } else if (is_function_symbol(i->lhs())) {
      function_symbol lhs(i->lhs());
      equations[lhs].push_back(*i);
    }
  }
  for (std::map<mcrl2::data::function_symbol, std::vector<data_equation> >::iterator i = equations.begin(); i != equations.end(); ++i) {
    if (definitions.count(i->first) == 0) {
      definitions[i->first] = i->second;
    }
  }
  
  std::map<mcrl2::data::function_symbol, std::set<mcrl2::data::function_symbol> > definition_dependencies;
  std::set<data::function_symbol> queue = find_function_symbols(pbes);
  while (!queue.empty()) {
    data::function_symbol function = *queue.begin();
    queue.erase(function);
    
    std::set<mcrl2::data::function_symbol> dependencies;
    if (translation.function_names.count(function) == 0 && builtin_constructors.count(function) == 0) {
      assert(definitions.count(function) > 0);
      
      function_definition definition = definitions[function];
      if (definition.is_unavailable()) {
        throw mcrl2::runtime_error("Function " + data::pp(function) + " not available in pbes2yices, giving up.");
      }
      
      if (definition.is_equations()) {
        const std::vector<data_equation> &function_equations = definition.equations();
        for (std::vector<data_equation>::const_iterator i = function_equations.begin(); i != function_equations.end(); ++i) {
          std::set<mcrl2::data::function_symbol> equation_dependencies = find_function_symbols(i->rhs());
          dependencies.insert(equation_dependencies.begin(), equation_dependencies.end());
          equation_dependencies = find_function_symbols(i->condition());
          dependencies.insert(equation_dependencies.begin(), equation_dependencies.end());
        }
        dependencies.erase(function);
        
        for (std::set<mcrl2::data::function_symbol>::iterator i = dependencies.begin(); i != dependencies.end(); ++i) {
          if (definition_dependencies.count(*i) == 0) {
            queue.insert(*i);
          }
        }
      }
    }
    definition_dependencies[function] = dependencies;
  }
  
  std::vector<data::function_symbol> function_order;
  try 
  {
    function_order = tsort(definition_dependencies);
  } 
  catch(mcrl2::runtime_error& ) 
  {
    throw mcrl2::runtime_error("Mutually recursive function definitions detected, giving up.");
  }
  
  for (std::vector<data::function_symbol>::iterator i = function_order.begin(); i != function_order.end(); ++i) {
    if (builtin_constructors.count(*i)) {
      continue;
    }
    if (translation.function_names.count(*i)) {
      continue;
    }
    assert(definitions.count(*i));
    
    function_definition definition = definitions[*i];
    assert(!definition.is_unavailable());
    
    if (definition.is_builtin()) 
    {
      translation.function_names[*i] = definition.name();
    } 
    else if (definition.is_identity()) 
    {
      translation.function_names[*i] = "";
    } 
    else 
    {
      assert(definition.is_equations());
      
      std::string name = sanitize_term(i->name());
      if (function_name_generator.has_identifier(name)) {
        if (!function_sort(i->sort()).domain().empty()) {
          name = mangle_sort_name(*function_sort(i->sort()).domain().begin()) + "-" + name;
        }
      }
      name = function_name_generator(name);
      
      translation.function_names[*i] = name;
      translation.definition += construct_function_definition(*i, definition.equations(), data, translation) + "\n";
    }
  }
}
