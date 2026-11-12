// Author(s): Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pgsolver_io.cpp
/// \brief Reading and writing of Boolean equation systems in the PGSolver format.

#include "mcrl2/pbes/pgsolver_io.h"

#include "mcrl2/pbes/join.h"
#include "mcrl2/pbes/normal_forms.h"

#include <charconv>
#include <iterator>
#include <optional>
#include <unordered_map>

namespace mcrl2::pbes_system
{

namespace
{

using identifier_t = std::size_t;
using priority_t = std::size_t;

/// \brief The player in a parity game.
enum class player : bool
{
  even = false, // player 0
  odd = true   // player 1
};

/// \brief A node of a parity game as it is given in the input.
struct pg_node
{
  identifier_t id = 0;
  priority_t priority = 0;
  player owner = player::even;
  std::vector<identifier_t> successors;
};

/// \brief A hand written recursive descent parser for the PGSolver format, see mcrl2/pbes/pgsolver_io.h.
class pgsolver_parser
{
public:
  explicit pgsolver_parser(std::string_view text)
    : m_text(text)
  {}

  void parse(pbes& result, bool maxpg)
  {
    std::optional<identifier_t> start;
    if (accept_keyword("parity"))
    {
      parse_number("the maximal node identifier");
      expect(';');
    }
    if (accept_keyword("start"))
    {
      start = parse_number("the initial node");
      expect(';');
    }

    std::vector<pg_node> nodes;
    std::unordered_map<identifier_t, std::size_t> index;
    while (!at_end())
    {
      const std::size_t node_position = m_position;
      pg_node node = parse_node();
      if (!index.emplace(node.id, nodes.size()).second)
      {
        error("node " + std::to_string(node.id) + " is defined more than once", node_position);
      }
      nodes.push_back(std::move(node));
    }

    if (nodes.empty())
    {
      error("the parity game contains no nodes", m_position);
    }
    if (start && !index.contains(*start))
    {
      error("the initial node " + std::to_string(*start) + " is not defined", 0);
    }

    // One instantiation per node, shared by its equation and all edges to it.
    std::vector<propositional_variable_instantiation> instantiations;
    instantiations.reserve(nodes.size());
    for (const pg_node& node: nodes)
    {
      instantiations.emplace_back(variable_name(node.id));
    }

    // Group the equations into blocks of equal priority; within a block the input order is kept.
    // Use vector for O(1) lookup since priorities are contiguous and start at 0
    std::vector<std::vector<pbes_equation>> blocks;
    std::size_t max_priority = 0;
    for (std::size_t i = 0; i < nodes.size(); ++i)
    {
      pg_node& node = nodes[i];
      std::ranges::sort(node.successors);
      const auto [first, last] = std::ranges::unique(node.successors);
      node.successors.erase(first, last);

      std::vector<pbes_expression> operands;
      operands.reserve(node.successors.size());
      for (identifier_t successor: node.successors)
      {
        const auto it = index.find(successor);
        if (it == index.end())
        {
          throw mcrl2::runtime_error("pgsolver: successor " + std::to_string(successor) + " of node "
                                     + std::to_string(node.id) + " is not defined.");
        }
        operands.emplace_back(instantiations[it->second]);
      }

      const pbes_expression rhs
        = node.owner == player::odd ? join_and(operands.begin(), operands.end()) : join_or(operands.begin(), operands.end());
      const fixpoint_symbol sigma = node.priority % 2 == 0 ? fixpoint_symbol::nu() : fixpoint_symbol::mu();
      
      if (node.priority > max_priority) max_priority = node.priority;
      if (blocks.size() <= node.priority) blocks.resize(node.priority + 1);
      blocks[node.priority].emplace_back(sigma, propositional_variable(instantiations[i].name()), rhs);
    }

    // In a max-parity game the highest priority is the most significant, which corresponds to the
    // first equation in the BES, and in a min-parity game the lowest priority is.
    std::vector<pbes_equation> equations;
    equations.reserve(nodes.size());
    if (maxpg)
    {
      // Iterate in reverse priority order (highest priority first)
      for (std::size_t p = max_priority + 1; p-- > 0; )
      {
        if (!blocks[p].empty())
        {
          equations.insert(equations.end(), blocks[p].begin(), blocks[p].end());
        }
      }
    }
    else
    {
      // Iterate in normal priority order (lowest priority first)
      for (std::size_t p = 0; p <= max_priority; ++p)
      {
        if (!blocks[p].empty())
        {
          equations.insert(equations.end(), blocks[p].begin(), blocks[p].end());
        }
      }
    }

    result = pbes(data::data_specification(),
      equations,
      start ? instantiations[index.at(*start)] : instantiations.front());
  }

private:
  std::string_view m_text;
  std::size_t m_position = 0;

  static core::identifier_string variable_name(identifier_t id)
  {
    return core::identifier_string("X" + std::to_string(id));
  }

  [[noreturn]] void error(const std::string& message, std::size_t position) const
  {
    const std::string_view prefix = m_text.substr(0, position);
    const std::size_t line = std::ranges::count(prefix, '\n') + 1;
    const std::size_t newline = prefix.rfind('\n');
    const std::size_t column = position - (newline == std::string_view::npos ? 0 : newline + 1) + 1;
    throw mcrl2::runtime_error(
      "pgsolver: line " + std::to_string(line) + ", column " + std::to_string(column) + ": " + message + ".");
  }

  /// \brief Skips whitespace and line comments that start with %.
  void skip_whitespace()
  {
    while (m_position < m_text.size())
    {
      const char c = m_text[m_position];
      if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
      {
        ++m_position;
      }
      else if (c == '%')
      {
        const std::size_t newline = m_text.find('\n', m_position);
        m_position = newline == std::string_view::npos ? m_text.size() : newline + 1;
      }
      else
      {
        break;
      }
    }
  }

  bool at_end()
  {
    skip_whitespace();
    return m_position == m_text.size();
  }

  bool accept(char c)
  {
    if (!at_end() && m_text[m_position] == c)
    {
      ++m_position;
      return true;
    }
    return false;
  }

  void expect(char c)
  {
    if (!accept(c))
    {
      error(std::string("expected '") + c + "'", m_position);
    }
  }

  bool accept_keyword(std::string_view keyword)
  {
    skip_whitespace();
    const std::size_t end = m_position + keyword.size();
    if (m_text.substr(m_position).starts_with(keyword)
        && (end == m_text.size() || std::isalnum(static_cast<unsigned char>(m_text[end])) == 0))
    {
      m_position = end;
      return true;
    }
    return false;
  }

  std::size_t parse_number(const std::string& description)
  {
    skip_whitespace();
    const std::size_t first = m_position;
    while (m_position < m_text.size() && std::isdigit(static_cast<unsigned char>(m_text[m_position])) != 0)
    {
      ++m_position;
    }
    if (first == m_position)
    {
      error("expected " + description, first);
    }

    std::size_t result = 0;
    const auto [ptr, ec] = std::from_chars(m_text.data() + first, m_text.data() + m_position, result);
    if (ec != std::errc())
    {
      error("the number " + std::string(m_text.substr(first, m_position - first)) + " is too large", first);
    }
    return result;
  }

  /// \brief Parses <identifier> <priority> <owner> <successors> [<name>] ;
  pg_node parse_node()
  {
    pg_node result;
    result.id = parse_number("a node identifier");
    result.priority = parse_number("a priority");

    skip_whitespace();
    const std::size_t owner_position = m_position;
    const std::size_t owner = parse_number("an owner");
    if (owner > 1)
    {
      error("the owner must be 0 or 1", owner_position);
    }

    if (owner == 1) 
    {
      result.owner = player::odd;
    } 
    else 
    {
      result.owner = player::even;
    }

    do
    {
      result.successors.push_back(parse_number("a successor"));
    }
    while (accept(','));

    if (accept('"'))
    {
      const std::size_t quote = m_text.find('"', m_position);
      if (quote == std::string_view::npos)
      {
        error("unterminated node name", m_position - 1);
      }
      m_position = quote + 1;
    }
    expect(';');
    return result;
  }
};

} // namespace

using variable_map = std::map<core::identifier_string, std::size_t>;

///
/// \brief Convert a sequence of Boolean variables to PGSolver format.
///
template <typename Iter>
std::string boolean_variables2pgsolver(Iter first, Iter last, const variable_map& variables)
{
  std::set<std::size_t> variables_int;
  for (Iter i = first; i != last; ++i)
  {
    const propositional_variable_instantiation& b = atermpp::down_cast<propositional_variable_instantiation>(*i);
    variable_map::const_iterator j = variables.find(b.name());
    if (j == variables.end())
    {
      throw mcrl2::runtime_error("Found undeclared variable in boolean_variables2pgsolver: " + pbes_system::pp(b));
    }
    variables_int.insert(j->second);
  }
  return utilities::string_join(variables_int, ", ");
}

///
/// \brief Convert a BES expression to PGSolver format.
///
static std::string bes_expression2pgsolver(const pbes_expression& p, const variable_map& variables)
{
  std::string result;
  if (is_and(p))
  {
    std::set<pbes_expression> expressions = split_and(p);
    result = boolean_variables2pgsolver(expressions.begin(), expressions.end(), variables);
  }
  else if (is_or(p))
  {
    std::set<pbes_expression> expressions = split_or(p);
    result = boolean_variables2pgsolver(expressions.begin(), expressions.end(), variables);
  }
  else if (is_propositional_variable_instantiation(p))
  {
    const propositional_variable_instantiation& b = atermpp::down_cast<propositional_variable_instantiation>(p);
    variable_map::const_iterator i = variables.find(b.name());
    if (i == variables.end())
    {
      throw mcrl2::runtime_error("Found undeclared variable in bes_expression2cwi: " + pbes_system::pp(p));
    }
    std::stringstream out;
    out << i->second;
    result = out.str();
  }
  else
  {
    throw mcrl2::runtime_error("Unknown or unsupported expression encountered in bes_expression2cwi: " + pbes_system::pp(p));
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
  std::size_t index = 0;
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
    and_in_block = and_in_block || is_and(i->formula());
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
        << (is_and(i->formula()) ? 1 : (is_or(i->formula())?0:block_to_player[priority]))
        << " "
        << bes_expression2pgsolver(i->formula(), variables)
        << ";\n";
  }
}

void save_bes_pgsolver(const pbes& bes, std::ostream& stream, bool maxpg)
{
  pbes bes_standard_form(bes);
  make_standard_form(bes_standard_form, true);
  if(bes.initial_state().name() != bes.equations().front().variable().name())
  {
    std::stringstream ss;
    ss << "The initial state " << bes_standard_form.initial_state() << " and the left hand side "
          "of the first equation (" << bes_standard_form.equations().front().variable() << ") do "
          "not correspond. Cannot save BES to PGSolver format.";
    throw mcrl2::runtime_error(ss.str());
  }
  bes2pgsolver(bes_standard_form.equations().begin(), bes_standard_form.equations().end(), stream, maxpg);
}

void parse_bes_pgsolver(pbes& result, std::string_view text, bool maxpg)
{
  pgsolver_parser(text).parse(result, maxpg);
}

void load_bes_pgsolver(pbes& result, std::istream& stream, bool maxpg)
{
  const std::string text{std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
  parse_bes_pgsolver(result, text, maxpg);
}

} // namespace mcrl2::pbes_system


