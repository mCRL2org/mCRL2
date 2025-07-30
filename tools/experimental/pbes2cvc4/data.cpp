// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdio>

#include "pbes2cvc4.h"

#include "mcrl2/utilities/exception.h"

#include "mcrl2/data/bool.h"
#ifdef MCRL2_ENABLE_MACHINENUMBERS
#include "mcrl2/data/int64.h"
#include "mcrl2/data/nat64.h"
#include "mcrl2/data/pos64.h"
#include "mcrl2/data/real64.h"
#include "mcrl2/data/list64.h"
#else
#include "mcrl2/data/int.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/list.h"
#endif
#include "mcrl2/data/replace.h"
#include "mcrl2/data/representative_generator.h"

using namespace mcrl2;
using namespace mcrl2::data;

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
      if (dependencies[*i].erase(vertex) && dependencies[*i].empty()) {
        output.push_back(*i);
      }
    }
  }
  
  for (typename std::map<T, std::set<T> >::const_iterator i = dependencies.begin(); i != dependencies.end(); i++) {
    for (typename std::set<T>::const_iterator j = i->second.begin(); j != i->second.end(); j++) {
      std::cerr << i->first << " depends on " << *j << "\n";
    }
  }
  for (typename std::map<T, std::set<T> >::const_iterator i = dependencies.begin(); i != dependencies.end(); i++) {
    if (!i->second.empty()) {
      throw mcrl2::runtime_error("Dependency loop");
    }
  }
  
  return output;
}

// Find all sorts that need to be defined in yices, ordered by dependency structure.
static std::vector<mcrl2::data::sort_expression> order_sorts(const data_specification &data, const std::set<sort_expression> &used_sorts)
{
  std::map<sort_expression, std::set<sort_expression> > sort_dependencies;
  for (std::set<sort_expression>::const_iterator i = used_sorts.begin(); i != used_sorts.end(); i++) {
    if (*i == sort_bool::bool_() ||
        *i == sort_pos::pos() ||
        *i == sort_nat::nat() ||
        *i == sort_int::int_() ||
        *i == sort_real::real_())
    {
      sort_dependencies[*i] = std::set<sort_expression>();
    } else if (is_container_sort(*i)) {
      std::set<sort_expression> dependencies;
      sort_expression base_sort = container_sort(*i).element_sort();
      dependencies.insert(base_sort);
      sort_dependencies[*i] = dependencies;
    } else if (is_basic_sort(*i)) {
      std::set<sort_expression> dependencies;
      function_symbol_vector constructors = data.constructors(*i);
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
    return "(- " + result + ")";
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

std::string define_variable(const translated_data_specification &translation, std::string name, mcrl2::data::sort_expression sort)
{
  if (sort == sort_pos::pos()) {
    return "(declare-fun " + name + " () Int)\n"
           "(assert (>= " + name + " 1))\n";
  } else if (sort == sort_nat::nat()) {
    return "(declare-fun " + name + " () Int)\n"
           "(assert (>= " + name + " 0))\n";
  } else {
    assert(translation.sort_names.count(sort) > 0);
    return "(declare-fun " + name + " () " + translation.sort_names.at(sort) + ")\n";
  }
}

std::string sanitize_term(std::string term)
{
  if (term == "-")
  {
    return "minus";
  }
  if (term == "[]")
  {
    return "emptylist";
  }
  if (term == "|>")
  {
    return "cons";
  }
  if (term == "<|")
  {
    return "snoc";
  }
  if (term == "#")
  {
    return "count";
  }
  if (term == "++")
  {
    return "concat_";
  }
  if (term == ".")
  {
    return "at";
  }
  for (char& i : term)
  {
    if (i == '\'')
    {
      i = '_';
    }
    if (i == '@')
    {
      i = '_';
    }
  }
  return term;
}

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

static bool is_structured_sort(const mcrl2::data::data_specification &data, mcrl2::data::sort_expression sort)
{
#ifdef MCRL2_ENABLE_MACHINENUMBERS
  if (sort == sort_nat::nnpair()) {
    return true;
  }
#else
  if (sort == sort_nat::natpair()) {
    return true;
  }
#endif
  if (sort_list::is_list(sort)) {
    return true;
  }
  for (const mcrl2::data::function_symbol& mapping : data.mappings())
  {
    if (std::string(mapping.name()) == "@to_pos" && mapping.sort() == make_function_sort_(sort, sort_pos::pos()))
    {
      return true;
    }
  }
  return false;
}

static void translate_sort_definition(const mcrl2::data::data_specification &data, mcrl2::data::sort_expression sort, set_identifier_generator &function_name_generator, translated_data_specification &translation)
{
  assert(is_basic_sort(sort) || is_container_sort(sort));
  
  if (sort == sort_bool::bool_()) {
    translation.sort_names[sort] = "Bool";
    translation.function_names[sort_bool::true_()] = "true";
    translation.function_names[sort_bool::false_()] = "false";
  } else if (sort == sort_pos::pos()) {
    // HACK
    translation.sort_names[sort] = "Int";
  } else if (sort == sort_nat::nat()) {
    // HACK
    translation.sort_names[sort] = "Int";
  } else if (sort == sort_int::int_()) {
    translation.sort_names[sort] = "Int";
  } else if (sort == sort_real::real_()) {
    translation.sort_names[sort] = "Real";
  } else {
    std::string name = mangle_sort_name(sort);
    function_symbol_vector constructors = data.constructors(sort);
    
    if (constructors.size() == 0) {
      translation.sort_names[sort] = name;
      translation.definition += "(declare-sort " + name + " 0)\n";
    } else if (!is_structured_sort(data, sort)) {
      throw mcrl2::runtime_error("Non-structured constructed sort " + data::pp(sort) + " found, giving up.");
    } else {
      translation.sort_names[sort] = name;
      translation.definition += "(declare-datatypes () ( (" + name + "\n";
      
      int index = 0;
      for (function_symbol_vector::iterator i = constructors.begin(); i != constructors.end(); i++) {
        std::string constructor_name = sanitize_term(i->name());
        if (function_name_generator.has_identifier(constructor_name)) {
          constructor_name = name + "-" + constructor_name;
        }
        constructor_name = function_name_generator(constructor_name);
        
        translation.function_names[*i] = constructor_name;
        translation.constructor_field_names[*i] = std::vector<std::string>();
        
        if (is_function_sort(i->sort())) {
          translation.definition += "  ( " + constructor_name;
          sort_expression_list domain = function_sort(i->sort()).domain();
          for (const mcrl2::data::sort_expression& j : domain)
          {
            assert(translation.sort_names.count(j) > 0);
            std::string sort_name = translation.sort_names[j];
            std::string parameter_name = function_name_generator(name + itoa(index++));
            
            translation.definition += " (" + parameter_name + " " + sort_name + ")";
            translation.constructor_field_names[*i].push_back(parameter_name);
          }
          translation.definition += ")\n";
        } else {
          translation.definition += "  ( " + constructor_name + ")\n";
        }
      }
      
      translation.definition += ")))\n";
    }
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
static std::map<mcrl2::data::function_symbol, function_definition> builtin_function_map()
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
#ifndef MCRL2_ENABLE_MACHINENUMBERS
  output[sort_pos::add_with_carry()] = data_equation(variable_list({b, p, p2}), sort_pos::add_with_carry(b, p, p2), if_(b, sort_pos::succ(sort_pos::plus(p, p2)), sort_pos::plus(p, p2)));
#endif
  
  
  function_symbol nat_zero("zero", nat);
  output[nat_zero] = "0";
  function_symbol nat_one("one", nat);
  output[nat_one] = "1";
  function_symbol nat_two("two", nat);
  output[nat_two] = "2";
  function_symbol nat_minus("-", function_sort(sort_expression_list({nat, nat}), nat));
  output[nat_minus] = "-";
  //function_symbol nat_do_exp("exp", function_sort(make_list(pos, nat), pos));
  //output[nat_do_exp] = data_equation(atermpp::make_vector(p, n), nat_do_exp(p, n), if_(equal_to(n, nat_one), p, if_(equal_to(sort_nat::mod(n, nat_two), nat_one), sort_pos::times(p, nat_do_exp(sort_pos::times(p, p), sort_nat::div(n, nat_two))), nat_do_exp(sort_pos::times(p, p), sort_nat::div(n, nat_two)))));
  
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
  output[sort_nat::exp(nat, nat)] = function_definition::unavailable();
  output[sort_nat::exp(pos, nat)] = function_definition::unavailable();
  //output[sort_nat::exp(nat, nat)] = data_equation(atermpp::make_vector(n, n2), sort_nat::exp(n, n2), if_(equal_to(n2, nat_zero), n, if_(less(n, nat_two), n, sort_nat::pos2nat(nat_do_exp(sort_nat::nat2pos(n), n2)))));
  //output[sort_nat::exp(pos, nat)] = data_equation(atermpp::make_vector(p, n2), sort_nat::exp(p, n2), if_(equal_to(n2, nat_zero), p, if_(equal_to(p, pos_one), p, nat_do_exp(p, n2))));
  
  
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
  output[sort_int::maximum(int_, pos)] = data_equation(variable_list({p, i}), sort_int::maximum(i, p), sort_int::int2pos(sort_int::maximum(i, sort_int::pos2int(p))));
  output[sort_int::maximum(nat, int_)] = data_equation(variable_list({n, i}), sort_int::maximum(n, i), sort_int::int2nat(sort_int::maximum(sort_int::nat2int(n), i)));
  output[sort_int::maximum(int_, nat)] = data_equation(variable_list({n, i}), sort_int::maximum(i, n), sort_int::int2nat(sort_int::maximum(i, sort_int::nat2int(n))));
  output[sort_int::abs()] = "abs";
  output[sort_int::negate(pos)] = "-";
  output[sort_int::negate(nat)] = "-";
  output[sort_int::negate(int_)] = "-";
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
  output[sort_int::exp(int_, nat)] = function_definition::unavailable();
  //output[sort_int::exp(int_, nat)] = data_equation(atermpp::make_vector(i, n), sort_int::exp(i, n), if_(equal_to(sort_nat::mod(n, nat_two), nat_zero), sort_int::nat2int(sort_nat::exp(sort_int::abs(i), n)), sort_int::minus(int_zero, sort_int::nat2int(sort_nat::exp(sort_int::abs(i), n)))));
  
  
  function_symbol real_zero("zero", real);
  output[real_zero] = "0";
  function_symbol real_one("one", real);
  output[real_one] = "1";
  function_symbol real_half("half", real);
  output[real_half] = "(/ 1 2)";
  function_symbol real_div("div", function_sort(sort_expression_list({real, pos}), int_));
  output[real_div] = "div";
  //function_symbol real_do_exp("exp", function_sort(make_list(real, nat), real));
  //output[real_do_exp] = data_equation(atermpp::make_vector(r, n), real_do_exp(r, n), if_(equal_to(n, nat_one), r, if_(equal_to(sort_nat::mod(n, nat_two), nat_one), sort_real::times(r, real_do_exp(sort_pos::times(r, r), sort_nat::div(n, nat_two))), real_do_exp(sort_real::times(r, r), sort_nat::div(n, nat_two)))));
  
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
  output[sort_real::abs(real)] = "abs";
  output[sort_real::negate(real)] = "-";
  output[sort_real::succ(real)] = data_equation(variable_list({r}), sort_real::succ(r), sort_real::plus(r, real_one));
  output[sort_real::pred(real)] = data_equation(variable_list({r}), sort_real::pred(r), sort_real::minus(r, real_one));
  output[sort_real::plus(real, real)] = "+";
  output[sort_real::minus(real, real)] = "-";
  output[sort_real::times(real, real)] = "*";
  output[sort_real::divides(pos, pos)] = "/";
  output[sort_real::divides(nat, nat)] = "/";
  output[sort_real::divides(int_, int_)] = "/";
  output[sort_real::divides(real, real)] = "/";
  output[sort_real::exp(real, int_)] = function_definition::unavailable();
  //output[sort_real::exp(real, int_)] = data_equation(atermpp::make_vector(r, i), sort_real::exp(r, i), if_(equal_to(i, int_zero), real_one, if_(less(i, int_zero), real_do_exp(sort_real::divides(real_one, r), sort_int::abs(i)), real_do_exp(r, sort_int::int2nat(i)))));
  output[sort_real::floor()] = "to_int";
  output[sort_real::ceil()] = data_equation(variable_list({r}), sort_real::ceil(r), sort_real::negate(sort_real::floor(sort_real::negate(r))));
  output[sort_real::round()] = data_equation(variable_list({r}), sort_real::round(r), sort_real::floor(sort_real::plus(r, real_half)));
  
  
  add_undefined_functions(output, sort_bool::bool_generate_functions_code());
  add_undefined_functions(output, sort_pos::pos_generate_functions_code());
  add_undefined_functions(output, sort_nat::nat_generate_functions_code());
  add_undefined_functions(output, sort_int::int_generate_functions_code());
  add_undefined_functions(output, sort_real::real_generate_functions_code());
  
  
  return output;
}



void translate_data_specification(const mcrl2::pbes_system::pbes &pbes, translated_data_specification &translation)
{
  const data_specification &data = pbes.data();
  
  
  std::map<mcrl2::data::function_symbol, function_definition> definitions = builtin_function_map();

        const std::set<data_equation>& equations = pbes.data().equations();
        data_equation_vector effective_equations(equations.begin(), equations.end());
  for (size_t i = 0; i < effective_equations.size(); ) {
    function_symbol symbol;
    if (is_function_symbol(effective_equations[i].lhs())) {
      symbol = function_symbol(effective_equations[i].lhs());
    } else if (is_application(effective_equations[i].lhs())) {
      application lhs(effective_equations[i].lhs());
      if (is_function_symbol(lhs.head())) {
        symbol = function_symbol(lhs.head());
      } else {
        i++;
        continue;
      }
    } else {
      i++;
      continue;
    }
    
    if (is_equal_to_function_symbol(symbol) ||
        is_not_equal_to_function_symbol(symbol) ||
        is_if_function_symbol(symbol) ||
        is_greater_function_symbol(symbol) ||
        is_greater_equal_function_symbol(symbol) ||
        definitions.count(symbol) > 0)
    {
      effective_equations[i] = effective_equations.back();
      effective_equations.pop_back();
    } else {
      i++;
    }
  }
  for (std::map<mcrl2::data::function_symbol, function_definition>::iterator i = definitions.begin(); i != definitions.end(); i++) {
    if (i->second.is_equations()) {
      effective_equations.insert(effective_equations.end(), i->second.equations().begin(), i->second.equations().end());
    }
  }
  
  
  std::multimap<mcrl2::data::function_symbol, mcrl2::data::data_equation> function_equations;
  std::multimap<mcrl2::data::data_equation, mcrl2::data::function_symbol> equation_functions;
  for (data_equation_vector::const_iterator i = effective_equations.begin(); i != effective_equations.end(); i++) {
    std::set<mcrl2::data::function_symbol> rhs_functions = find_function_symbols(i->rhs());
    std::set<mcrl2::data::function_symbol> lhs_functions = find_function_symbols(i->lhs());
    rhs_functions.insert(lhs_functions.begin(), lhs_functions.end());
    std::set<mcrl2::data::function_symbol> condition_functions = find_function_symbols(i->condition());
    rhs_functions.insert(condition_functions.begin(), condition_functions.end());
    
    for (std::set<mcrl2::data::function_symbol>::iterator j = lhs_functions.begin(); j != lhs_functions.end(); j++) {
      function_equations.insert(std::pair<mcrl2::data::function_symbol, mcrl2::data::data_equation>(*j, *i));
    }
    for (std::set<mcrl2::data::function_symbol>::iterator j = rhs_functions.begin(); j != rhs_functions.end(); j++) {
      equation_functions.insert(std::pair<mcrl2::data::data_equation, mcrl2::data::function_symbol>(*i, *j));
    }
  }
  
  
  std::set<function_symbol> defined_functions;
  std::set<data_equation> defined_equations;
  std::set<function_symbol> function_queue = find_function_symbols(pbes);
  std::set<function_symbol> constructors(data.constructors().begin(), data.constructors().end());
  for (std::set<function_symbol>::const_iterator i = constructors.begin(); i != constructors.end(); i++) {
    function_queue.erase(*i);
  }
  
  while (!function_queue.empty()) {
    function_symbol function = *function_queue.begin();
    function_queue.erase(function);
    
    defined_functions.insert(function);
    
    std::multimap<mcrl2::data::function_symbol, mcrl2::data::data_equation>::const_iterator f_begin = function_equations.lower_bound(function);
    std::multimap<mcrl2::data::function_symbol, mcrl2::data::data_equation>::const_iterator f_end = function_equations.upper_bound(function);
    for (std::multimap<mcrl2::data::function_symbol, mcrl2::data::data_equation>::const_iterator i = f_begin; i != f_end; i++) {
      data_equation equation = i->second;
      defined_equations.insert(equation);
      
      std::multimap<mcrl2::data::data_equation, mcrl2::data::function_symbol>::const_iterator e_begin = equation_functions.lower_bound(equation);
      std::multimap<mcrl2::data::data_equation, mcrl2::data::function_symbol>::const_iterator e_end = equation_functions.upper_bound(equation);
      for (std::multimap<mcrl2::data::data_equation, mcrl2::data::function_symbol>::const_iterator j = e_begin; j != e_end; j++) {
        if (constructors.count(j->second) > 0) {
          continue;
        }
        
        if (defined_functions.count(j->second) == 0) {
          function_queue.insert(j->second);
        }
      }
    }
  }
  
  
  set_identifier_generator function_name_generator;
  
  
  
  std::set<sort_expression> used_sorts = find_sort_expressions(defined_equations);
  for (std::vector<pbes_system::pbes_equation>::const_iterator i = pbes.equations().begin(); i != pbes.equations().end(); i++) {
    std::set<pbes_system::propositional_variable_instantiation> instantiations = pbes_system::find_propositional_variable_instantiations(i->formula());
    for (std::set<pbes_system::propositional_variable_instantiation>::iterator j = instantiations.begin(); j != instantiations.end(); j++) {
      std::set<sort_expression> sorts = find_sort_expressions(j->parameters());
      used_sorts.insert(sorts.begin(), sorts.end());
    }
    //std::set<sort_expression> equation_sorts = pbes_system::find_sort_expressions(i->formula());
    //used_sorts.insert(equation_sorts.begin(), equation_sorts.end());
    //equation_sorts = pbes_system::find_sort_expressions(i->variable());
    //used_sorts.insert(equation_sorts.begin(), equation_sorts.end());
  }
  
  
  
  // Translate all sorts to yices sort definitions.
  std::vector<sort_expression> sorts = order_sorts(data, used_sorts);
  for (std::vector<sort_expression>::iterator i = sorts.begin(); i != sorts.end(); i++) {
    translate_sort_definition(data, *i, function_name_generator, translation);
    
    // Equality and inequality must be defined for all sorts.
    translation.function_names[equal_to(*i)] = "=";
    translation.function_names[not_equal_to(*i)] = "distinct";
    
    variable v1("v1", *i);
    variable v2("v2", *i);
    definitions[equal_to(*i)] = "=";
    definitions[not_equal_to(*i)] = "distinct";
    definitions[if_(*i)] = "ite";
    if (!definitions.count(greater(*i))) {
      definitions[greater(*i)] = data_equation(variable_list({v1, v2}), greater(v1, v2), sort_bool::not_(less_equal(v1, v2)));
    }
    if (!definitions.count(greater_equal(*i))) {
      definitions[greater_equal(*i)] = data_equation(variable_list({v1, v2}), greater_equal(v1, v2), sort_bool::not_(less(v1, v2)));
    }
  }
  // The unrolling code may generate invocations of those functions.
  translation.function_names[sort_bool::and_()] = "and";
  translation.function_names[sort_bool::or_()] = "or";
  translation.function_names[sort_bool::not_()] = "not";
  
  
  
  for (std::set<function_symbol>::const_iterator i = defined_functions.begin(); i != defined_functions.end(); i++) {
    if (definitions.count(*i) > 0 && definitions[*i].is_builtin()) {
      translation.function_names[*i] = definitions[*i].name();
    } else if (definitions.count(*i) > 0 && definitions[*i].is_identity()) {
      translation.function_names[*i] = "";
    } else if (definitions.count(*i) > 0 && definitions[*i].is_unavailable()) {
      throw mcrl2::runtime_error("Function " + data::pp(*i) + " not available in pbes2cvc4, giving up.");
    } else {
      std::string name = sanitize_term(i->name());
      if (function_name_generator.has_identifier(name)) {
        if (!function_sort(i->sort()).domain().empty()) {
          name = mangle_sort_name(*function_sort(i->sort()).domain().begin()) + "-" + name;
        }
      }
      name = function_name_generator(name);
      
      translation.definition += "(declare-fun " + name + " (";
      if (is_function_sort(i->sort())) {
        sort_expression_list domain = function_sort(i->sort()).domain();
        for (const mcrl2::data::sort_expression& j : domain)
        {
          assert(translation.sort_names.count(j) > 0);
          translation.definition += translation.sort_names[j] + " ";
        }
      }
      assert(translation.sort_names.count(i->sort().target_sort()));
      translation.definition += ") " + translation.sort_names[i->sort().target_sort()] + ")\n";
      
      translation.function_names[*i] = name;
    }
  }
  
  
  for (std::set<data_equation>::const_iterator i = defined_equations.begin(); i != defined_equations.end(); i++) {
    std::string quantification_domain;
    std::map<variable, std::string> context;
    for (variable_list::const_iterator j = i->variables().begin(); j != i->variables().end(); j++) {
      if (find_free_variables(i->lhs()).count(*j) == 0 &&
          find_free_variables(i->rhs()).count(*j) == 0 &&
          find_free_variables(i->condition()).count(*j) == 0)
      {
        continue;
      }
      
      std::string name = sanitize_term(j->name());
      assert(translation.sort_names.count(j->sort()));
      quantification_domain += "(" + name + " " + translation.sort_names[j->sort()] + ")";
      context[*j] = name;
    }
    
    std::string pattern = translate_expression(i->lhs(), context, translation);
    std::string valuation = translate_expression(i->rhs(), context, translation);
    std::string condition = translate_expression(i->condition(), context, translation);
    
    if (context.empty()) {
      translation.definition += "(assert ";
      if (i->condition() != sort_bool::true_()) {
        translation.definition += "(=> " + condition + " ";
      }
      translation.definition += "(= " + pattern + " " + valuation + ")";
      if (i->condition() != sort_bool::true_()) {
        translation.definition += ")";
      }
      translation.definition += ")\n";
    } else {
      translation.definition += "(assert (forall (" + quantification_domain + ") (! ";
      if (i->condition() != sort_bool::true_()) {
        translation.definition += "(=> " + condition + " ";
      }
      translation.definition += "(= " + pattern + " " + valuation + ")";
      if (i->condition() != sort_bool::true_()) {
        translation.definition += ")";
      }
      translation.definition += " :rewrite-rule) ))\n";
    }
  }
}
