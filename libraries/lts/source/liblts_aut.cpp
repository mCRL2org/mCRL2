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

class state_probability_pair
{
  protected:
    size_t m_state;
    probabilistic_arbitrary_size_label m_probability;

  public:
    state_probability_pair(const size_t state, const probabilistic_arbitrary_size_label& probability)
     : m_state(state),
       m_probability(probability)
    {}

    size_t state() const
    {
      return m_state;
    }

    const probabilistic_arbitrary_size_label& probability() const
    {
      return m_probability;
    }
};

static string c(const size_t n)  // Awkward trick, should be a better way to do this. Please replace....
{
  stringstream out;
  out << n;
  return out.str();
}

static void read_newline(istream& is, const size_t lineno)
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

// reads a number, puts it in s, and reads one extra character, which must be either a space or a closing bracket.
static void read_natural_number_to_string(istream& is, string& s, const size_t lineno)
{
  assert(s.empty());
  char ch;
  is >> skipws >> ch;
  for( ; isdigit(ch) ; is.get(ch))
  {
    s.push_back(ch);
  }
  is.putback(ch);
  if (s.empty())
  {
    throw mcrl2::runtime_error("Expect a number at line " + c(lineno) + ".");
  }
}

static size_t find_probabilistic_label_index(
                  const probabilistic_arbitrary_size_label& s,
                  map < probabilistic_arbitrary_size_label, size_t >& labs,
                  lts_aut_t& l)
{
  size_t label;

  const map < probabilistic_arbitrary_size_label, size_t >::const_iterator i=labs.find(s);
  if (i==labs.end())
  {
    label=l.add_probabilistic_label(s);
    labs[s]=label;
  }
  else
  {
    label=i->second;
  }
  return label;
}

static size_t find_label_index(const string& s, map < string, size_t >& labs, lts_aut_t& l)
{
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
  return label;
}

void static add_state(size_t& state,
                      map <size_t,size_t>& state_number_translator,
                      lts_aut_t& l,
                      const bool is_probabilistic=false)
{
  map <size_t,size_t>::const_iterator j=state_number_translator.find(state);
  if (j==state_number_translator.end())
  {
    // Not found.
    const size_t new_state_number=l.add_state(lts_aut_t::state_label_t(),is_probabilistic);
    if (!is_probabilistic)
    {
      // Only non probabilistic states occur in the translation tabel.
      state_number_translator[state]=new_state_number;
    }
    state=new_state_number;
  }
  else
  {
    // found.
    state=j->second;
    assert(l.is_probabilistic(state)==is_probabilistic);
    assert(!is_probabilistic);
  }
}

// This procedure tries to read states, indicated by numbers
// with in between fractions of the shape number/number. The
// last state number is put in state. The remainder as pairs
// in the vector. Typical expected input is 3 2/3 4 1/6 78 1/6 3.
static void read_probabilistic_state(
  istream& is,
  vector<state_probability_pair>& additional_probabilistic_states,
  size_t& state,
  const size_t lineno)
{
  additional_probabilistic_states.clear();

  bool ready=false;
  while (is.good() && !ready)
  {
    is >> skipws >> state;

    if (!is.good())
    {
      throw mcrl2::runtime_error("Expect a state number at line " + c(lineno) + ".");
    }

    // Check whether the next character is a comma.
    char ch;
    is >> skipws >> ch;
    is.putback(ch);

    if (!isdigit(ch))
    {
      ready=true;
    }
    else
    {
      // Now attempt to read probabilities followed by a state.
      string enumerator;
      read_natural_number_to_string(is,enumerator,lineno);
      char ch;
      is >> skipws >> ch;
      if (ch != '/')
      {
        throw mcrl2::runtime_error("Expect a / in a probability at line " + c(lineno) + ".");
      }

      string denominator;
      read_natural_number_to_string(is,denominator,lineno);
      additional_probabilistic_states.push_back(state_probability_pair(state, probabilistic_arbitrary_size_label(enumerator,denominator)));
    }
  }
}


static void read_aut_header(
  istream& is,
  size_t& initial_state,
  vector<state_probability_pair>& additional_probabilistic_states,
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

  // is >> skipws >> initial_state;
  read_probabilistic_state(is,additional_probabilistic_states,initial_state,1);

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

  read_newline(is,1);
}

static bool read_aut_transition(
  istream& is,
  size_t& from,
  string& label,
  size_t& to,
  std::vector<state_probability_pair>& additional_probabilistic_states,
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

  // is >> skipws >> to;
  read_probabilistic_state(is,additional_probabilistic_states,to,lineno);

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
  size_t number_of_probabilistic_transitions=0,
         number_of_probabilistic_nodes=0;

  vector<state_probability_pair> additional_probabilistic_states;
  read_aut_header(is,initial_state,additional_probabilistic_states,ntrans,nstate);

  map <size_t,size_t> state_number_translator;

  if (nstate==0)
  {
    throw mcrl2::runtime_error("cannot parse AUT input that has no states; at least an initial state is required.");
  }

  // l.set_num_states(nstate,false);
  // l.clear_transitions(ntrans); // Reserve enough space for the transitions.
  l.clear();
  map < string, size_t > action_labels;
  map < probabilistic_arbitrary_size_label, size_t > probabilistic_labels;

  // state_number_translator[initial_state]=0;
  // initial_state=0;
  // l.set_initial_state(initial_state);

  if (additional_probabilistic_states.size()==0)
  {
    // This is not a probabilistic state.
    add_state(initial_state,state_number_translator,l);
    assert(initial_state==0);
    l.set_initial_state(initial_state);
  }
  else
  {
    size_t probabilistic_node=0;
    number_of_probabilistic_nodes++;
    add_state(probabilistic_node,state_number_translator,l,true);
    assert(probabilistic_node==0);
    l.set_initial_state(probabilistic_node);
    probabilistic_arbitrary_size_label remaining_probability("1","1");
    for(vector<state_probability_pair>::const_iterator i=additional_probabilistic_states.begin();
            i!=additional_probabilistic_states.end(); ++i)
    {
      size_t target=i->state();
      add_state(target,state_number_translator,l);
      number_of_probabilistic_transitions++;
      l.add_transition(transition(probabilistic_node,find_probabilistic_label_index(i->probability(),probabilistic_labels,l),target));
      remaining_probability=remaining_probability - i->probability();
    }

    add_state(initial_state,state_number_translator,l);
    number_of_probabilistic_transitions++;
    l.add_transition(transition(probabilistic_node,find_probabilistic_label_index(remaining_probability,probabilistic_labels,l),initial_state));
  }




  while (!is.eof())
  {
    size_t from,to;
    string s;

    line_no++;

    if (!read_aut_transition(is,from,s,to,additional_probabilistic_states,line_no))
    {
      break; // eof encountered
    }

    add_state(from,state_number_translator,l);

    if (additional_probabilistic_states.size()==0)
    {
      // This is not a probabilistic state.
      add_state(to,state_number_translator,l);
      l.add_transition(transition(from,find_label_index(s,action_labels,l),to));
    }
    else
    {
      size_t probabilistic_node=l.num_states();
      number_of_probabilistic_nodes++;
      add_state(probabilistic_node,state_number_translator,l,true);
      l.add_transition(transition(from,find_label_index(s,action_labels,l),probabilistic_node));
      probabilistic_arbitrary_size_label remaining_probability("1","1");
      for(vector<state_probability_pair>::const_iterator i=additional_probabilistic_states.begin();
              i!=additional_probabilistic_states.end(); ++i)
      {
        size_t target=i->state();
        add_state(target,state_number_translator,l);
        number_of_probabilistic_transitions++;
        l.add_transition(transition(probabilistic_node,find_probabilistic_label_index(i->probability(),probabilistic_labels,l),target));
        remaining_probability=remaining_probability - i->probability();
      }

      add_state(to,state_number_translator,l);
      number_of_probabilistic_transitions++;
      l.add_transition(transition(probabilistic_node,find_probabilistic_label_index(remaining_probability,probabilistic_labels,l),to));
    }

    if (state_number_translator.size() > l.num_states())
    {
      throw mcrl2::runtime_error("Number of actual states in .aut file is higher than maximum (" +
                                 c(l.num_states()) + ") given by header (found at line " + c(line_no) + ").");
    }

  }

  if (ntrans+number_of_probabilistic_transitions != l.num_transitions())
  {
    throw mcrl2::runtime_error("number of transitions read (" + c(l.num_transitions()-number_of_probabilistic_transitions) +
                               ") does not correspond to the number of transition given in the header (" + c(ntrans) + ").");
  }
}


static void write_state(const size_t state, ostream& os, const lts_aut_t& l, const std::map<size_t,size_t>& state_mapping)
{
  if (l.is_probabilistic(state))
  {
    // Inefficient.
    std::string previous_label="";
    for (const transition& t: l.get_transitions())
    {
      if (t.from()==state)
      {
        os << previous_label << state_mapping.at(t.to());
        previous_label=" " + pp(l.probabilistic_label(t.label())) + " ";
      }
    }
  }
  else
  {
    os << state_mapping.at(state);
  }
}

static void write_to_aut(const lts_aut_t& l, ostream& os)
{
  // First construct a state mapping, giving non probabilistic states
  // a consecutive numbering.
  std::map<size_t,size_t> state_mapping;
  if (!l.is_probabilistic(l.initial_state()))
  {
    state_mapping[l.initial_state()]=0;
  }
  for(size_t i=0; i<l.num_states(); ++i)
  {
    if (!l.is_probabilistic(i) && i!=l.initial_state())
    {
      state_mapping[i]=state_mapping.size();
    }
  }

  os << "des (";
  write_state(l.initial_state(),os,l,state_mapping);

  // Only transitions from non probabilistic states are stored in a .aut
  // file. So, we must first count them.

  size_t num_non_probabilistic_transitions=0;
  for (const transition& t:l.get_transitions())
  {
    if (!l.is_probabilistic(t.from()))
    {
      num_non_probabilistic_transitions++;
    }
  }

  os << "," << num_non_probabilistic_transitions << "," << state_mapping.size() << ")" << endl;

  for (const transition& t: l.get_transitions())
  {
    if (!l.is_probabilistic(t.from()))
    {
      os << "(";
      write_state(t.from(),os,l,state_mapping);
      os << ",\"" << pp(l.action_label(t.label())) << "\",";
      write_state(t.to(),os,l,state_mapping);
      os << ")" << endl;
    }
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
