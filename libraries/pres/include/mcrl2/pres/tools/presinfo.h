// Author(s): Jan Friso Groote. Based on pbes/pbesinfo.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/tools/presinfo.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_TOOLS_PRESINFO_H
#define MCRL2_PRES_TOOLS_PRESINFO_H

#include "mcrl2/pres/detail/pres_property_map.h"
#include "mcrl2/pres/io.h"

namespace mcrl2 {

namespace pres_system {

void presinfo(const std::string& input_filename,
              const std::string& input_file_message,
              const utilities::file_format& file_format,
              bool opt_full
             )
{
  pres p;
  load_pres(p, input_filename, file_format);

  detail::pres_property_map info(p);

  // Show file from which PRES was read
  std::cout << input_file_message << "\n\n";

  // Show number of equations
  std::cout << "Number of equations:     " << p.equations().size() << std::endl;

  // Show number of mu's with the predicate variables from the mu's
  std::cout << "Number of mu's:          " << info["mu_equation_count"] << std::endl;

  // Show number of nu's with the predicate variables from the nu's
  std::cout << "Number of nu's:          " << info["nu_equation_count"] << std::endl;

  // Show number of nu's with the predicate variables from the nu's
  std::cout << "Block nesting depth:     " << info["block_nesting_depth"] << std::endl;

  // Show if PRES is closed and well formed
  std::cout << "The PRES is closed:      " << std::flush;
  std::cout << (p.is_closed() ? "yes" : "no ") << std::endl;
  std::cout << "The PRES is well formed: " << std::flush;
  std::cout << (p.is_well_typed() ? "yes" : "no ") << std::endl;

  // Show binding variables with their signature
  if (opt_full)
  {
    std::cout << "Predicate variables:\n";
    for (std::vector<pres_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
    {
      std::cout << core::pp(i->symbol()) << "." << pres_system::pp(i->variable()) << std::endl;
    }
  }
}

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_TOOLS_PRESINFO_H
