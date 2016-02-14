#include "mcrl2/bes/io.h"
#include "mcrl2/bes/normal_forms.h"

namespace mcrl2
{

namespace bes
{

typedef boolean_equation_system::equation_type equation_type;
typedef equation_type::term_type term_type;
typedef core::term_traits<term_type> tr;
typedef tr::string_type string_type;
typedef std::map<string_type, size_t> variable_map;

///
/// \brief Convert a sequence of Boolean variables to PGSolver format.
///
template <typename Iter>
std::string boolean_variables2pgsolver(Iter first, Iter last, const variable_map& variables)
{
  std::set<size_t> variables_int;
  for (Iter i = first; i != last; ++i)
  {
    assert(tr::is_variable(*i));
    variable_map::const_iterator j = variables.find(tr::name(*i));
    if (j == variables.end())
    {
      throw mcrl2::runtime_error("Found undeclared variable in boolean_variables2pgsolver: " + tr::pp(*i));
    }
    variables_int.insert(j->second);
  }
  return utilities::string_join(variables_int, ", ");
}

///
/// \brief Convert a BES expression to PGSolver format.
///
static std::string bes_expression2pgsolver(const term_type& p, const variable_map& variables)
{
  std::string result;
  if (tr::is_and(p))
  {
    std::set<term_type> expressions = split_and(p);
    result = boolean_variables2pgsolver(expressions.begin(), expressions.end(), variables);
  }
  else if (tr::is_or(p))
  {
    std::set<term_type> expressions = split_or(p);
    result = boolean_variables2pgsolver(expressions.begin(), expressions.end(), variables);
  }
  else if (tr::is_prop_var(p))
  {
    variable_map::const_iterator i = variables.find(tr::name(p));
    if (i == variables.end())
    {
      throw mcrl2::runtime_error("Found undeclared variable in bes_expression2cwi: " + tr::pp(p));
    }
    std::stringstream out;
    out << i->second;
    result = out.str();
  }
  else
  {
    throw mcrl2::runtime_error("Unknown or unsupported expression encountered in bes_expression2cwi: " + tr::pp(p));
  }
  return result;
}

///
/// \brief Save a sequence of BES equations in to a stream in PGSolver format.
///
template <typename Iter>
void bes2pgsolver(Iter first, Iter last, std::ostream& out, bool maxpg)
{
  // Number the variables of the equations 0, 1, ... and put them in the map variables.
  // Also store player to which variables without operand are assigned, per
  // block, in block_to_player.
  variable_map variables;
  size_t index = 0;
  std::map<int, int> block_to_player;

  bool and_in_block = false;
  int block = 0;
  fixpoint_symbol sigma = fixpoint_symbol::nu();
  for (Iter i = first; i != last; ++i)
  {
    if(i->symbol() != sigma)
    {
      block_to_player[block++] = (and_in_block)?1:0;
      and_in_block = false;
      sigma = i->symbol();
    }
    variables[i->variable().name()] = index++;
    and_in_block = and_in_block || tr::is_and(i->formula());
  }
  block_to_player[block] = (and_in_block)?1:0;

  if(maxpg && block % 2 == 1)
  {
    ++block;
  }

  out << "parity " << index -1 << ";\n";

  int priority = 0;
  sigma = fixpoint_symbol::nu();
  for (Iter i = first; i != last; ++i)
  {
    if(i->symbol() != sigma)
    {
      ++priority;
      sigma = i->symbol();
    }

    out << variables[i->variable().name()]
        << " "
        << (maxpg?(block-priority):priority)
        << " "
        << (tr::is_and(i->formula()) ? 1 : (tr::is_or(i->formula())?0:block_to_player[priority]))
        << " "
        << bes_expression2pgsolver(i->formula(), variables)
#ifdef MCRL2_BES2PGSOLVER_PRINT_VARIABLE_NAMES
        << " "
        << "\""
        << tr::pp(i->variable())
        << "\""
#endif
      << ";\n";
  }
}

void save_bes_pgsolver(const boolean_equation_system& bes, std::ostream& stream, bool maxpg)
{
  boolean_equation_system bes_standard_form(bes);
  make_standard_form(bes_standard_form, true);
  if(bes.initial_state() != bes.equations().front().variable())
  {
    std::stringstream ss;
    ss << "The initial state " << bes_standard_form.initial_state() << " and the left hand side "
          "of the first equation (" << bes_standard_form.equations().front().variable() << ") do "
          "not correspond. Cannot save BES to PGSolver format.";
    throw mcrl2::runtime_error(ss.str());
  }
  bes2pgsolver(bes_standard_form.equations().begin(), bes_standard_form.equations().end(), stream, maxpg);
}

} // namespace bes

} // namespace mcrl2
