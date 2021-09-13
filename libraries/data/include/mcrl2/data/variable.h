// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/variable.h
/// \brief The class variable.

#ifndef MCRL2_DATA_VARIABLE_H
#define MCRL2_DATA_VARIABLE_H

#include "mcrl2/core/index_traits.h"
#include "mcrl2/data/data_expression.h"

namespace mcrl2
{

namespace data
{

typedef std::pair<atermpp::aterm, atermpp::aterm> variable_key_type;

//--- start generated class variable ---//
/// \brief A data variable
class variable: public data_expression
{
  public:


    /// Move semantics
    variable(const variable&) noexcept = default;
    variable(variable&&) noexcept = default;
    variable& operator=(const variable&) noexcept = default;
    variable& operator=(variable&&) noexcept = default;

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }

    const sort_expression& sort() const
    {
      return atermpp::down_cast<sort_expression>((*this)[1]);
    }
//--- start user section variable ---//
    /// \brief Default constructor.
    variable()
      : data_expression(core::detail::default_values::DataVarId)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit variable(const atermpp::aterm& term)
      : data_expression(term)
    {
      assert(core::detail::check_term_DataVarId(*this));
    }

    /// \brief Constructor.
/*    variable(const core::identifier_string& name, const sort_expression& sort)
      : data_expression(atermpp::aterm_appl(core::detail::function_symbol_DataVarId(),
          name,
          sort,
          atermpp::aterm_int(core::index_traits<variable, variable_key_type, 2>::insert(std::make_pair(name, sort))
        )))
    {} */
    variable(const core::identifier_string& name, const sort_expression& sort)
    {
       std::vector<aterm> arguments = { name, sort, atermpp::aterm_int(0) };

       *this = variable(atermpp::aterm_appl(core::detail::function_symbol_DataVarId(), arguments.begin(), arguments.end(),
         [&](const aterm& t)
         {
           if (t.type_is_int())
           {
             return aterm(atermpp::aterm_int(core::index_traits<variable, variable_key_type, 2>::insert(std::make_pair(name, sort))));
           }
           else
           {
             return t;
           }
         })); 
 
       /* std::size_t index=0;
       *this = variable(atermpp::aterm_appl(core::detail::function_symbol_DataVarId(), index, 3,
         [&sort, &name](const std::size_t i)->atermpp::aterm
         {
           switch (i)
           {
             case 0:
               return name;
             case 1:
               return sort;
             case 2:
               return aterm(atermpp::aterm_int(core::index_traits<variable, variable_key_type, 2>::insert(std::make_pair(name, sort))));
             default:
               assert(0);
           }
         })); */
       
    }


    /// \brief Constructor.
/*    variable(const std::string& name, const sort_expression& sort)
      : data_expression(atermpp::aterm_appl(core::detail::function_symbol_DataVarId(),
          core::identifier_string(name),
          sort,
          atermpp::aterm_int(core::index_traits<variable, variable_key_type, 2>::insert(std::make_pair(core::identifier_string(name), sort))
        )))
    {}
*/

    variable(const std::string& name, const sort_expression& sort)
    {
       std::vector<aterm> arguments = { core::identifier_string(name), sort, atermpp::aterm_int(0) };

       *this = variable(atermpp::aterm_appl(core::detail::function_symbol_DataVarId(), arguments.begin(), arguments.end(),
         [&](const aterm& t)
         {
           if (t.type_is_int())
           {
             return aterm(atermpp::aterm_int(core::index_traits<variable, variable_key_type, 2>::insert(std::make_pair(core::identifier_string(name), sort))));
           }
           else
           {
             return t;
           }
         }));
    }

//--- end user section variable ---//
};

/// \brief Make_variable constructs a new term into a given address.
/// \ \param t The reference into which the new variable is constructed. XXXXX
inline void make_variable(variable& t, const core::identifier_string& name, const sort_expression& sort)
{
  make_term_appl(t, core::detail::function_symbol_DataVarId(), name, sort);
}

/// \brief list of variables
typedef atermpp::term_list<variable> variable_list;

/// \brief vector of variables
typedef std::vector<variable>    variable_vector;

// prototype declaration
std::string pp(const variable& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const variable& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(variable& t1, variable& t2)
{
  t1.swap(t2);
}
//--- end generated class variable ---//


// template function overloads
std::string pp(const variable_list& x);
std::string pp(const variable_vector& x);
std::string pp(const std::set<variable>& x);
std::string pp(const std::set<variable>& x);
std::set<data::variable> find_all_variables(const data::variable& x);
std::set<data::variable> find_all_variables(const data::variable_list& x);
std::set<core::identifier_string> find_identifiers(const data::variable_list& x);

} // namespace data

} // namespace mcrl2


namespace std 
{

template<>
struct hash<mcrl2::data::variable>
{
  // Default constructor, required for const qualified hash functions. 
    hash()
    {}
  
    std::size_t operator()(const mcrl2::data::variable& v) const
    {
      const hash<atermpp::aterm> hasher; 
      return hasher(v);
    }
};
  
} // namespace std

#endif // MCRL2_DATA_VARIABLE_H

