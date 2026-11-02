// Author(s): Jeroen Keiren, Wieger Wesselink, Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pgsolver_io.h
/// \brief Reading and writing of Boolean equation systems in the PGSolver format for parity games.
///
/// The accepted format is:
/// \code
///   <parity_game> ::= [parity <identifier> ;] [start <identifier> ;] <node_spec>+
///   <node_spec>   ::= <identifier> <priority> <owner> <successors> [<name>] ;
///   <successors>  ::= <identifier> (, <identifier>)*
///   <owner>       ::= 0 | 1
///   <name>        ::= " (any string not containing ") "
/// \endcode
/// where identifiers and priorities are natural numbers. Whitespace and line comments starting
/// with % are ignored.

#ifndef MCRL2_PBES_PGSOLVER_IO_H
#define MCRL2_PBES_PGSOLVER_IO_H

#include "mcrl2/pbes/pbes.h"

namespace mcrl2::pbes_system
{

/// \brief Parses a parity game in PGSolver format and stores it as a BES.
/// \details Node i becomes the equation for variable Xi. Nodes owned by player 0 become
///          disjunctions and nodes owned by player 1 conjunctions of their successors. Even
///          priorities become greatest and odd priorities least fixpoints. The initial state is
///          the node given by the start declaration, or otherwise the first node in the input.
/// \param result The BES in which the result is stored.
/// \param text The parity game in PGSolver format.
/// \param maxpg If true the input is interpreted as a max-parity game, otherwise as a min-parity game.
/// \throws mcrl2::runtime_error if the input is not a well-formed parity game.
void parse_bes_pgsolver(pbes& result, std::string_view text, bool maxpg = true);

/// \brief Reads a parity game in PGSolver format from a stream, and stores it as a BES.
/// \see parse_bes_pgsolver
void load_bes_pgsolver(pbes& result, std::istream& stream, bool maxpg = true);

/// \brief Saves a BES to a stream in PGSolver format.
/// \param maxpg If true a max-parity game is written, otherwise a min-parity game.
void save_bes_pgsolver(const pbes& bes, std::ostream& stream, bool maxpg = true);

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_PGSOLVER_IO_H
