// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_fsm.cpp

#include <string>
#include <fstream>
#include "mcrl2/core/print.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/parse.h"

namespace mcrl2 {

namespace lts {

struct fsm_writer
{
  std::ostream& out;
  const lts_fsm_t& fsm;

  fsm_writer(std::ostream& out_, const lts_fsm_t& fsm_)
    : out(out_), fsm(fsm_)
  {}

  void write_parameters()
  {
    // print parameters with used values
    mCRL2log(log::verbose) << "writing parameter table..." << std::endl;
    for (std::size_t i = 0; i < fsm.process_parameters().size(); i++)
    {
      const std::vector<std::string>& values = fsm.state_element_values(i);
      out << fsm.process_parameter(i).first << "(" << values.size() << ") " << fsm.process_parameter(i).second << " ";
      for (const std::string& s: values)
      {
        out << " \"" << s << "\"";
      }
      out << std::endl;
    }
  }

  void write_states()
  {
    mCRL2log(log::verbose) << "writing states..." << std::endl;
    for (std::size_t i = 0; i < fsm.num_states(); i++)
    {
      std::size_t idx = i;
      // make sure that the initial state is first
      if (i == 0)
      {
        idx = fsm.initial_state();
      }
      else if (i == fsm.initial_state())
      {
        idx = 0;
      }

      if (fsm.has_state_info())
      {
        const state_label_fsm& state_parameters = fsm.state_label(idx);
        for (std::size_t j = 0; j < state_parameters.size(); j++)
        {
          if (j > 0)
          {
            out << " ";
          }
          out << state_parameters[j];
        }
        out << std::endl;
      }
    }
  }

  void write_transitions()
  {
    mCRL2log(log::verbose) << "writing transitions..." << std::endl;
    for (const transition& t: fsm.get_transitions())
    {
      transition::size_type from = t.from();
      // correct state numbering
      if (from == 0)
      {
        from = fsm.initial_state();
      }
      else if (from == fsm.initial_state())
      {
        from = 0;
      }
      transition::size_type to = t.to();
      if (to == 0)
      {
        to = fsm.initial_state();
      }
      else if (to == fsm.initial_state())
      {
        to = 0;
      }

      // correct state numbering
      out << from + 1 << " " << to + 1 << " \"";
      out << mcrl2::lts::pp(fsm.action_label(t.label()));
      out << "\"" << std::endl;
    }
  }

  void write()
  {
    write_parameters();
    out << "---" << std::endl;
    write_states();
    out << "---" << std::endl;
    write_transitions();
  }
};

void lts_fsm_t::load(const std::string& filename)
{
  if (filename.empty())
  {
    try
    {
      parse_fsm_specification(std::cin, *this);
    }
    catch (mcrl2::runtime_error&)
    {
      throw mcrl2::runtime_error("Error parsing .fsm file from standard input.");
    }
  }
  else
  {
    std::ifstream is(filename.c_str());

    if (!is.is_open())
    {
      throw mcrl2::runtime_error("Cannot open .fsm file " + filename + ".");
    }
    try
    {
      parse_fsm_specification(is, *this);
    }
    catch (mcrl2::runtime_error&)
    {
      throw mcrl2::runtime_error("Error parsing .fsm file");
    }
    is.close();
  }
}

void lts_fsm_t::save(const std::string& filename) const
{
  if (filename=="")
  {
    fsm_writer(std::cout, *this).write();
  }
  else
  {
    std::ofstream os(filename.c_str());

    if (!os.is_open())
    {
      throw mcrl2::runtime_error("Cannot create .fsm file '" + filename + ".");
      return;
    }

    fsm_writer(os, *this).write();
    os.close();
  }
}

} // namespace lts

} // namespace mcrl2
