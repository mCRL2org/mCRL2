// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_aut.cpp

#include <string>
#include <sstream>
#include <fstream>
#include "mcrl2/lts/lts_aut.h"


using namespace mcrl2::lts;
using namespace std;

static string c(const size_t n)  // Awkward trick, should be a better way to do this. Please replace....
{
  stringstream out;
  out << n;
  return out.str();
}

static void read_newline(istream& is,const size_t lineno=1)
{
  char ch;
  is.get(ch);

  // Skip over spaces
  while (ch == ' ')
  {
    is.get(ch);
  }

  // Windows systems typically have a carriage return before a newline.
  if (ch == '\r')
  {
    is.get(ch);
  }

  if (ch != '\n')
  {
    if (lineno==1)
    {
      throw mcrl2::runtime_error("Expect a newline after the header des(...,...,...).");
    }
    else
    {
      throw mcrl2::runtime_error("Expect a newline after the transition at line " + c(lineno) + ".");
    }
  }
}

static void read_aut_header(
  istream& is,
  size_t& initial_state,
  size_t& num_transitions,
  size_t& num_states)
{
  string s;
  is.width(3);
  is >> skipws >> s;

  if (s!="des")
  {
    throw mcrl2::runtime_error("Expect an .aut file to start with 'des'.");
  }

  char ch;
  is >> skipws >> ch;

  if (ch != '(')
  {
    throw mcrl2::runtime_error("Expect an opening bracket '(' after 'des' in the first line of a .aut file.");
  }

  is >> skipws >> initial_state;

  is >> skipws >> ch;
  if (ch != ',')
  {
    throw mcrl2::runtime_error("Expect a comma after the first number in the first line of a .aut file.");
  }

  is >> skipws >> num_transitions;

  is >> skipws >> ch;
  if (ch != ',')
  {
    throw mcrl2::runtime_error("Expect a comma after the second number in the first line of a .aut file.");
  }

  is >> skipws >> num_states;

  is >> ch;

  if (ch != ')')
  {
    throw mcrl2::runtime_error("Expect a closing bracket ')' after the third number in the first line of a .aut file.");
  }

  read_newline(is);
}

static bool read_aut_transition(
  istream& is,
  size_t& from,
  string& label,
  size_t& to,
  const size_t lineno)
{
  char ch;
  is >> skipws >> ch;
  if (is.eof())
  {
    return false;
  }
  if (ch != '(')
  {
    throw mcrl2::runtime_error("Expect opening bracket at line " + c(lineno) + ".");
  }

  is >> skipws >> from;

  is >> skipws >> ch;
  if (ch != ',')
  {
    throw mcrl2::runtime_error("Expect that the first number is followed by a comma at line " + c(lineno) + ".");
  }

  is >> skipws >> ch;
  if (ch == '"')
  {
    label="";
    is >> ch;
    while ((ch != '"') && !is.eof())
    {
      label.push_back(ch);
      is >> ch;
    }

    if (ch != '"')
    {
      throw mcrl2::runtime_error("Expect that the second item is a quoted label (using \") at line " + c(lineno) + ".");
    }
    is >> skipws >> ch;
  }
  else
  {
    label = ch;
    is >> ch;
    while ((ch != ',') && !is.eof())
    {
      label.push_back(ch);
      is >> ch;
    }
  }

  if (ch != ',')
  {
    throw mcrl2::runtime_error("Expect a comma after the quoted label at line " + c(lineno) + ".");
  }

  is >> skipws >> to;

  is >> ch;
  if (ch != ')')
  {
    throw mcrl2::runtime_error("Expect a closing bracket at the end of the transition at line " + c(lineno) + ".");
  }

  read_newline(is,lineno);
  return true;
}

static void read_from_aut(lts_aut_t& l, istream& is)
{
  size_t line_no = 1;
  size_t initial_state=0, ntrans=0, nstate=0;

  read_aut_header(is,initial_state,ntrans,nstate);

  map <size_t,size_t> state_number_translator;

  if (nstate==0)
  {
    throw mcrl2::runtime_error("cannot parse AUT input that has no states; at least an initial state is required.");
  }

  l.set_num_states(nstate,false);

  state_number_translator[0]=initial_state;
  initial_state=0;
  l.set_initial_state(initial_state);

  map < string, size_t > labs;
  while (!is.eof())
  {
    size_t from,to;
    string s;

    line_no++;

    if (!read_aut_transition(is,from,s,to,line_no))
    {
      break; // eof encountered
    }

    map <size_t,size_t>::const_iterator j=state_number_translator.find(from);
    if (j==state_number_translator.end())
    { 
      // Not found.
      const size_t size=state_number_translator.size();
      state_number_translator[from]=size;
      from=size; 
    }
    else
    {  
      // found.
      from=j->second;
    }

    j=state_number_translator.find(to);
    if (j==state_number_translator.end())
    { 
      // Not found.
      const size_t size=state_number_translator.size();
      state_number_translator[to]=size;
      to=size; 
    }
    else
    {  
      // found.
      to=j->second;
    }

    if (state_number_translator.size() > l.num_states())
    {
      throw mcrl2::runtime_error("Number of actual states in .aut file is higher than maximum (" +
                                 c(l.num_states()) + ") given by header (found at line " + c(line_no) + ").");
    }

    size_t label;

    const map < string, size_t >::const_iterator i=labs.find(s);
    if (i==labs.end())
    {
      label=l.add_action(s,s=="tau");
      labs[s]=label;
    }
    else
    {
      label=i->second;
    }

    l.add_transition(transition(from,label,to));
  }

  if (ntrans != l.num_transitions())
  {
    throw mcrl2::runtime_error("number of transitions read (" + c(l.num_transitions()) +
                               ") does not correspond to the number of transition given in the header (" + c(ntrans) + ").");
  }
}

static void write_to_aut(const lts_aut_t& l, ostream& os)
{
  os << "des (0," << l.num_transitions() << "," << l.num_states() << ")" << endl;

  const std::vector<transition> &trans=l.get_transitions();
  for (std::vector<transition>::const_iterator t=trans.begin();  t!=trans.end(); ++t)
  {
    transition::size_type from = t->from();
    transition::size_type to = t->to();
    // AUT files need the initial state to be 0, so we will swap state 0 and
    // the initial state
    if (from == 0)
    {
      from = l.initial_state();
    }
    else if (from == l.initial_state())
    {
      from = 0;
    }
    if (to == 0)
    {
      to = l.initial_state();
    }
    else if (to == l.initial_state())
    {
      to = 0;
    }
    os << "(" << from << ",\""
       << detail::pp(l.action_label(t->label()))
       << "\"," << to << ")" << endl;
  }
}

namespace mcrl2
{
namespace lts
{

void lts_aut_t::load(const string& filename)
{
  if (filename=="")
  {
    read_from_aut(*this,cin);
  }
  else
  {
    ifstream is(filename.c_str());

    if (!is.is_open())
    {
      throw mcrl2::runtime_error("cannot open .aut file '" + filename + ".");
    }

    read_from_aut(*this,is);
    is.close();
  }
}

void lts_aut_t::load(istream& is)
{
  read_from_aut(*this,is);
}

void lts_aut_t::save(string const& filename) const
{
  if (filename=="")
  {
    write_to_aut(*this,cout);
  }
  else
  {
    ofstream os(filename.c_str());

    if (!os.is_open())
    {
      throw mcrl2::runtime_error("cannot create .aut file '" + filename + ".");
      return;
    }
    write_to_aut(*this,os);
    os.close();
  }
}


}
}
