// Author(s): Thomas Neele, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_UTILITIES_H
#define MCRL2_SMT_UTILITIES_H

#include <queue>

#include "mcrl2/smt/native_translation.h"
#include "mcrl2/smt/translation_error.h"

namespace mcrl2::smt
{

inline
std::string translate_identifier(const std::string& id)
{
  std::string result = id;
  for (char& i : result)
  {
    if (i == '\'')
    {
      i = '!';
    }
  }
  return result;
}

inline
std::string translate_identifier(const core::identifier_string& id)
{
  return translate_identifier(core::pp(id));
}

inline
std::string translate_symbol(const data::function_symbol& f, const native_translations& nt)
{
  auto find_result = nt.symbols.find(f);
  return find_result != nt.symbols.end() ? find_result->second : translate_identifier(f.name());
}

inline
std::string make_projection_name(const data::function_symbol& cons, std::size_t i, const native_translations& nt)
{
  // Projections for natively defined sort List
  if(data::pp(cons) == "|>")
  {
    return i == 0 ? "head" : "tail";
  }
  // Projection for all constructors that are mapped to @id, such as @cNat and @cInt
  if(translate_symbol(cons, nt) == "@id")
  {
    return "@id";
  }
  return "@proj-" + translate_symbol(cons, nt) + "-" + std::to_string(i);
}

inline
data::function_symbol make_projection_func(const data::function_symbol& cons, const data::sort_expression& arg_sort, std::size_t i, const native_translations& nt)
{
  data::function_sort sort(data::sort_expression_list({ cons.sort().target_sort() }), arg_sort);
  return data::function_symbol(make_projection_name(cons, i, nt), sort);
}

inline
std::string make_recogniser_name(const data::function_symbol& cons, const native_translations& nt)
{
  auto find_result = nt.special_recogniser.find(cons);
  if(find_result != nt.special_recogniser.end())
  {
    return find_result->second;
  }
  // Z3 automatically generates recognisers "is-constructorname"
  return "is-" + translate_symbol(cons, nt);
}

inline
data::function_symbol make_recogniser_func(const data::function_symbol& cons, const native_translations& nt)
{
  data::function_sort sort(data::sort_expression_list({ cons.sort().target_sort() }), data::sort_bool::bool_());
  return data::function_symbol(make_recogniser_name(cons, nt), sort);
}

template<class T>
std::vector<T> topological_sort(std::map<T, std::set<T>> dependencies)
{
  std::queue<T> vertices_without_dependencies;
  std::map<T, std::set<T> > reverse_dependencies;
  for (const auto& p: dependencies)
  {
    const T& node = p.first;
    const std::set<T>& successors = p.second;
    for (const T& succ: successors)
    {
      reverse_dependencies[succ].insert(node);
    }

    if (successors.empty())
    {
      vertices_without_dependencies.push(node);
    }
  }

  std::vector<T> output;
  output.reserve(dependencies.size());

  while (!vertices_without_dependencies.empty())
  {
    T vertex = vertices_without_dependencies.front();
    vertices_without_dependencies.pop();
    output.push_back(vertex);

    const std::set<T>& successors = reverse_dependencies[vertex];
    for (const T& succ: successors)
    {
      dependencies[succ].erase(vertex);
      if (dependencies[succ].empty())
      {
        vertices_without_dependencies.push(succ);
      }
    }
  }

  if (output.size() != dependencies.size())
  {
    for (const T& node: output)
    {
      dependencies.erase(node);
    }
    assert(!dependencies.empty());
    //TODO: SMT2.5 format supports mutually recursive data types
    std::ostringstream out;
    for(const auto& p: dependencies)
    {
      out << p.first << ", ";
    }
    throw translation_error("Dependency loop trying to resolve dependencies: {" + out.str() + "}");
  }
  else
  {
    return output;
  }
}

template <typename OutStream>
class stack_outstream
{
protected:
  std::stack<typename OutStream::pos_type> m_stack;
  std::ostringstream buf;
  OutStream& m_out;

public:
  stack_outstream(OutStream& out)
  : m_out(out)
  {}

  ~stack_outstream()
  {
    m_out << buf.str();
  }

  void push()
  {
    m_stack.push(buf.tellp());
  }

  void pop()
  {
    m_stack.pop();
  }

  typename OutStream::pos_type top_size()
  {
    assert(!m_stack.empty());
    return buf.tellp() - m_stack.top();
  }

  void copy_top(std::string& dest)
  {
    dest = buf.str().substr(m_stack.top(), std::string::npos);
  }

  template <typename T>
  stack_outstream& operator<<(const T& s)
  {
    buf << s;
    return *this;
  }
};

} // namespace mcrl2::smt

#endif
