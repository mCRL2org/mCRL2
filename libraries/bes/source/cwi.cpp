#include "mcrl2/bes/io.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/find.h"

namespace mcrl2
{

namespace bes
{

///
/// \brief Convert a BES expression to cwi format.
///
template <class term_type, class variable_map>
void bes_expression2cwi(const term_type& p, const variable_map& variables, std::ostream& out)
{
  typedef core::term_traits<term_type> tr;
  if (tr::is_true(p))
  {
    out << "T";
  }
  else if (tr::is_false(p))
  {
    out << "F";
  }
  else if (tr::is_and(p))
  {
    out << "(";
    bes_expression2cwi(tr::left(p), variables, out);
    out << "&";
    bes_expression2cwi(tr::right(p), variables, out);
    out << ")";
  }
  else if (tr::is_or(p))
  {
    out << "(";
    bes_expression2cwi(tr::left(p), variables, out);
    out << "|";
    bes_expression2cwi(tr::right(p), variables, out);
    out << ")";
  }
  else if (tr::is_prop_var(p))
  {
    typename variable_map::const_iterator i = variables.find(tr::name(p));
    if (i == variables.end())
    {
      throw mcrl2::runtime_error("Found undeclared variable in bes_expression2cwi: " + tr::pp(p));
    }
    out << "X" << i->second;
  }
  else
  {
    throw mcrl2::runtime_error("Unknown expression encountered in bes_expression2cwi: " + tr::pp(p));
  }
}

///
/// \brief Save a sequence of BES equations in CWI format to a stream.
///
template <typename Iter>
void bes2cwi(Iter first, Iter last, std::ostream& out)
{
  std::map<typename core::term_traits<typename Iter::value_type::term_type>::string_type, size_t> variables;
  int index = 1;
  for (Iter i = first; i != last; ++i)
  {
    variables[i->variable().name()] = index++;
  }

  for (Iter i = first; i != last; ++i)
  {
    out << (i->symbol().is_mu() ? "min " : "max ") << "X" << variables[i->variable().name()] << "=";
    bes_expression2cwi(i->formula(), variables, out);
    out << std::endl;
  }
}

///
/// \brief swap_equations Swaps the first equation in equations with the equation for
///        first, if that equation occurs in the first block. If the swap succeeds, the
///        function returns true, otherwise it returns false.
///
template <class equation_type, class term_type>
bool swap_equations(std::vector<equation_type>& equations, const term_type& first)
{
  const fixpoint_symbol& block_symbol = equations.front().symbol();
  size_t index = 0;
  while(index < equations.size() && block_symbol == equations[index].symbol())
  {
    if (equations[index].variable() == first)
    {
      std::swap(equations[0], equations[index]);
      return true;
    }
    ++index;
  }
  return false;
}

template <class equation_type>
struct variable_traits;

template <>
struct variable_traits<boolean_equation>
{
  template <class container>
  static std::set<core::identifier_string> find_variable_names(const container& equations)
  {
    std::set<boolean_variable> occ = find_boolean_variables(equations);
    std::set<core::identifier_string> occ_ids;
    for(auto it = occ.begin(); it != occ.end(); ++it)
    {
      occ_ids.insert(it->name());
    }
    return occ_ids;
  }
};

template <>
struct variable_traits<pbes_system::pbes_equation>
{
  template <class container>
  static std::set<core::identifier_string> find_variable_names(const container& equations)
  {
    std::set<pbes_system::propositional_variable_instantiation> occ;
    auto oit = std::inserter(occ, occ.end());
    std::set<core::identifier_string> occ_ids;
    for(auto it = equations.begin(); it != equations.end(); ++it)
    {
      pbes_system::detail::make_find_propositional_variables_traverser<pbes_system::pbes_expression_traverser>(oit).apply(it->formula());
      occ_ids.insert(it->variable().name());
    }
    for(auto it = occ.begin(); it != occ.end(); ++it)
    {
      occ_ids.insert(it->name());
    }
    return occ_ids;
  }
};

///
/// \brief add_fresh_equation Prepends a new equation to equations with a fresh variable as
///        left hand side, and rhs as right hand side.
///
template <class equation_type, class term_type>
void add_fresh_equation(std::vector<equation_type>& equations, const term_type& rhs)
{
  std::set<core::identifier_string> occ_ids = variable_traits<equation_type>::find_variable_names(equations);
  utilities::number_postfix_generator generator(occ_ids.begin(), occ_ids.end(), "X");
  equations.insert(equations.begin(),
                   equation_type(equations.front().symbol(),
                                 typename equation_type::variable_type(generator()),
                                 rhs));
}

template <class equation_system>
void save_bes_cwi_impl(const equation_system& system, std::ostream& stream)
{
  if (system.initial_state() == system.equations().front().variable())
  {
    bes::bes2cwi(system.equations().begin(), system.equations().end(), stream);
  }
  else
  {
    mCRL2log(log::warning) << "The initial state " << system.initial_state() << " and the left hand "
                              "side of the first equation " << system.equations().begin()->variable()
                           << " do not correspond." << std::endl;

    std::vector<typename equation_system::equation_type> equations(system.equations().begin(), system.equations().end());
    if (is_boolean_variable(system.initial_state()) && swap_equations(equations, system.initial_state()))
    {
      mCRL2log(log::warning) << "Fixed by swapping equations for " << equations[0].variable()
                             << " and " << system.initial_state() << std::endl;
    }
    else
    {
      add_fresh_equation(equations, system.initial_state());
      mCRL2log(log::warning) << "Fixed by prepending a new equation " << equations.front() << "."
                             << std::endl;
    }
    bes2cwi(equations.begin(), equations.end(), stream);
  }
}

void save_bes_cwi(const boolean_equation_system& bes, std::ostream& stream)
{
  save_bes_cwi_impl(bes, stream);
}

void save_bes_cwi(const pbes_system::pbes& bes, std::ostream& stream)
{
  save_bes_cwi_impl(bes, stream);
}

} // namespace bes

} // namespace mcrl2
