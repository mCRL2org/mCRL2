// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_TRANSLATE_SPECIFICATION_H
#define MCRL2_SMT_TRANSLATE_SPECIFICATION_H

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/find.h"
#include "mcrl2/smt/translate_expression.h"

namespace mcrl2::smt
{
namespace detail
{

template <typename OutputStream>
inline
void translate_sort_definition(const std::string& sort_name,
                               const data::sort_expression& s,
                               const data::data_specification& dataspec,
                               OutputStream& out,
                               const native_translations& nt,
                               std::map<data::structured_sort, std::string>& struct_name_map)
{
  auto find_result = nt.sorts.find(s);
  if(find_result != nt.sorts.end())
  {
    // Do not output anything for natively defined sorts
    return;
  }

  out << "(declare-datatypes () ((" << translate_identifier(sort_name) << " ";
  for(const data::function_symbol& cons: dataspec.constructors(s))
  {
    out << "(" << translate_identifier(cons.name()) << " ";
    if(data::is_function_sort(cons.sort()))
    {
      const data::function_sort& cs = atermpp::down_cast<data::function_sort>(cons.sort());
      std::size_t index = 0;
      for(const data::sort_expression& arg: cs.domain())
      {
        out << "(" << make_projection_name(cons, index, nt) << " ";
        translate_sort_expression(arg, out, nt, struct_name_map);
        out << ") ";
        index++;
      }
    }
    out << ") ";
  }
  out << ")))\n";
}

template <typename OutputStream>
inline
void translate_sort_definition(const data::basic_sort& s,
                               const data::data_specification& dataspec,
                               OutputStream& out,
                               const native_translations& nt,
                               std::map<data::structured_sort, std::string>& struct_name_map)
{
  translate_sort_definition(pp(s.name()), s, dataspec, out, nt, struct_name_map);
}

// Find the dependencies in the definition of a sort
static inline
std::set<data::sort_expression> find_dependencies(const data::data_specification& dataspec, const data::sort_expression& sort)
{
  std::set<data::sort_expression> dependencies;
  for(const data::function_symbol& cons: dataspec.constructors(sort))
  {
    find_sort_expressions(cons, std::inserter(dependencies, dependencies.end()));
  }

  std::set<data::sort_expression> result;
  for(const data::sort_expression& s: dependencies)
  {
    if((data::is_basic_sort(s) || data::is_structured_sort(s)) && s != sort)
    {
      result.insert(s);
    }
  }
  return result;
}

// Find all sorts that need to be translated and the dependencies in their definitions
inline
std::map<data::sort_expression, std::set<data::sort_expression>> find_sorts_and_dependencies(const data::data_specification& dataspec,
                                                                                             std::map<data::structured_sort, std::string>& struct_name_map)
{
  std::map<data::sort_expression, std::set<data::sort_expression>> result;
  for(const data::sort_expression& s: dataspec.sorts())
  {
    if(data::is_function_sort(s))
    {
      // SMT-LIB2 does not support function sorts
      // Hope & pray that nothing breaks later...
      continue;
    }
    auto find_alias = dataspec.sort_alias_map().find(s);
    if(find_alias != dataspec.sort_alias_map().end() && find_alias->second != s)
    {
      if(data::is_structured_sort(s))
      {
        struct_name_map[atermpp::down_cast<data::structured_sort>(s)] = pp(find_alias->second);
      }
      // translate only the unique representation of a sort
      continue;
    }

    result[s] = find_dependencies(dataspec, s);
  }
  return result;
}

template <typename OutputStream>
inline
void translate_sort_definitions(const data::data_specification& dataspec,
                               OutputStream& out,
                               const native_translations& nt,
                               data::set_identifier_generator& id_gen,
                               std::map<data::structured_sort, std::string>& struct_name_map)
{
  auto sort_dependencies = find_sorts_and_dependencies(dataspec, struct_name_map);
  // for(const auto& p: sort_dependencies)
  // {
  //   std::cout << p.first << " := " << core::detail::print_set(p.second) << std::endl;
  // }
  auto sorts = topological_sort(sort_dependencies);
  for(const data::sort_expression& s: sorts)
  {
    std::string name(pp(s));
    if(data::is_structured_sort(s))
    {
      // The structured sort is anonymous. Can happen in a specification such as
      // sort StateList = List(struct s1 | s2);
      name = pp(id_gen("@struct"));
      struct_name_map[atermpp::down_cast<data::structured_sort>(s)] = name;
    }
    translate_sort_definition(name, s, dataspec, out, nt, struct_name_map);
  }
}



template <typename OutputStream>
inline
void translate_alias(const data::alias& s,
                     OutputStream& out, const native_translations& nt,
                     const std::map<data::structured_sort, std::string>& struct_name_map)
{
  out << "(define-sort " << translate_identifier(s.name().name()) << " () ";
  translate_sort_expression(s.reference(), out, nt, struct_name_map);
  out << ")\n";
}

inline
bool is_higher_order(const data::function_symbol& f)
{
  const data::sort_expression& s = f.sort();
  if(data::is_function_sort(s))
  {
    const data::function_sort& fs = atermpp::down_cast<data::function_sort>(s);
    for(const data::sort_expression& arg: fs.domain())
    {
      if(data::is_function_sort(arg))
      {
        return true;
      }
    }
  }
  return false;
}

inline
bool is_higher_order(const data::application& a)
{
  if(data::is_function_symbol(a.head()))
  {
    auto& f = atermpp::down_cast<data::function_symbol>(a.head());
    return is_higher_order(f);
  }
  return true;
}

inline
bool is_higher_order(const data::data_equation& eq)
{
  const data::data_expression& lhs = eq.lhs();
  if(data::is_function_symbol(lhs))
  {
    return data::is_function_sort(lhs.sort());
  }
  else if(data::is_application(lhs) && data::is_function_symbol(atermpp::down_cast<data::application>(lhs).head()))
  {
    return is_higher_order(atermpp::down_cast<data::function_symbol>(atermpp::down_cast<data::application>(lhs).head()));
  }
  else
  {
    return true;
  }
}

template <typename OutputStream>
inline
void translate_mapping(const data::function_symbol& f, OutputStream& out, const native_translations& nt,
                       const std::map<data::structured_sort, std::string>& snm, bool check_native = true)
{
  if(is_higher_order(f) || (check_native && nt.has_native_definition(f)))
  {
    return;
  }

  out << "(declare-fun " << translate_symbol(f, nt) << " ";
  if(data::is_function_sort(f.sort()))
  {
    out << "(";
    data::function_sort fs(atermpp::down_cast<data::function_sort>(f.sort()));
    for(const data::sort_expression& s: fs.domain())
    {
      translate_sort_expression(s, out, nt, snm);
      out << " ";
    }
    out << ") ";
    translate_sort_expression(fs.codomain(), out, nt, snm);
  }
  else
  {
    out << "() ";
    translate_sort_expression(f.sort(), out, nt, snm);
  }
  out << ")\n";
}

template <typename OutputStream>
inline
void translate_native_mappings(OutputStream& out,
                               std::unordered_map<data::data_expression, std::string>& c,
                               const native_translations& nt,
                               const std::map<data::structured_sort, std::string>& snm)
{
  out << "(define-funs-rec (\n";
  for(const auto& [mapping, eqn]: nt.mappings)
  {
    if(is_higher_order(mapping))
    {
      continue;
    }
   mCRL2log(log::verbose) << "Outputting " << mapping << ": " << mapping.sort() << " with eqn " << eqn << std::endl;

    out << "(" << translate_symbol(mapping, nt) << " ";
    data::data_expression condition = declare_variables_binder(eqn.variables(), out, nt);
    out << " ";
    translate_sort_expression(mapping.sort().target_sort(), out, nt, snm);
    out << ")\n";
  }
  out << ")\n";

  out << "(\n";
  for(const auto& [mapping, eqn]: nt.mappings)
  {
    if(is_higher_order(mapping))
    {
      continue;
    }

    translate_data_expression(eqn.rhs(), out, c, nt);
    out << "\n";
  }
  out << "))\n";
}

template <typename OutputStream>
inline
void translate_equation(const data::data_equation& eq,
                        OutputStream& out,
                        std::unordered_map<data::data_expression, std::string>& c,
                        const native_translations& nt)
{
  if(is_higher_order(eq) || nt.has_native_definition(eq))
  {
    return;
  }

  const data::variable_list& vars = eq.variables();
  data::data_expression body(data::lazy::implies(eq.condition(), data::equal_to(eq.lhs(), eq.rhs())));
  data::data_expression definition(vars.empty() ? body : data::forall(vars, body));

  translate_assertion(definition, out, c, nt);
}

} // namespace detail

template <typename OutputStream>
void translate_data_specification(const data::data_specification& dataspec,
                                  OutputStream& o,
                                  std::unordered_map<data::data_expression, std::string>& c,
                                  const native_translations& nt)
{
  // Generator for new names of projection functions and structs
  data::set_identifier_generator id_gen;
  // Inline struct definitions are anonymous, we keep track of a newly-generate name for each
  std::map<data::structured_sort, std::string> struct_name_map;

  // Translate sorts
  detail::translate_sort_definitions(dataspec, o, nt, id_gen, struct_name_map);
  for(const auto& s: dataspec.sort_alias_map())
  {
    if(data::is_basic_sort(s.first))
    {
      detail::translate_alias(data::alias(atermpp::down_cast<data::basic_sort>(s.first), s.second), o, nt, struct_name_map);
    }
  }

  // Translate mappings
  for(const data::function_symbol& f: dataspec.mappings())
  {
    detail::translate_mapping(f, o, nt, struct_name_map);
  }
  detail::translate_native_mappings(o, c, nt, struct_name_map);

  // Translate remaining equations
  for(const data::data_equation& eq: dataspec.equations())
  {
    detail::translate_equation(eq, o, c, nt);
  }
}

} // namespace mcrl2::smt

#endif
