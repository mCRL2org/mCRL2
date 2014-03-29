// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_lts.cpp

#include <string>
#include <cstring>
#include <sstream>
#include "svc/svc.h"
#include "svc/svcerrno.h"
#include "mcrl2/core/nil.h"
#include "mcrl2/data/detail/io.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/atermpp/aterm_int.h"

namespace mcrl2
{
namespace lts
{

/**
 * @brief An aterm_appl that contains a data specification, process parameter
 *        list and action label list. This is the information that is appended
 *        at the end of an LTS file.
 */
class trailer_data : public atermpp::aterm_appl
{
private:
  static const atermpp::function_symbol m_function_symbol;
  trailer_data(const atermpp::aterm& data_spec, 
               const atermpp::aterm& process_parameters,
               const atermpp::aterm& action_labels)
    : atermpp::aterm_appl(m_function_symbol, data_spec, process_parameters, action_labels)
  { }

  const atermpp::aterm_appl& get_arg(size_t i) const
  {
    return atermpp::aterm_cast<atermpp::aterm_appl>((*this)[i]);
  }

public:

  trailer_data(const atermpp::aterm& term)
    : atermpp::aterm_appl(term)
  { }

  trailer_data()
    : atermpp::aterm_appl(m_function_symbol, core::nil(), core::nil(), core::nil())
  { }

  static trailer_data create(const lts_lts_t& ts)
  {
    atermpp::aterm d = data::detail::remove_index(data::detail::data_specification_to_aterm_data_spec(ts.data()));
    atermpp::aterm p = ts.has_process_parameters() ? data::detail::remove_index(ts.process_parameters()) : core::nil();
    atermpp::aterm a = ts.has_action_labels() ? data::detail::remove_index(ts.action_labels()) : core::nil();
    return trailer_data(d, p, a);
  }

  bool is_valid() const
  {
    return !core::is_nil(get_arg(0));
  }

  data::data_specification data() const
  {
    data::data_specification data_spec(atermpp::aterm_cast<atermpp::aterm_appl>(data::detail::add_index((*this)[0])));
    data_spec.declare_data_specification_to_be_type_checked();  // Assume that the data specification in an lts is well typed.
    return data_spec;
  }

  bool has_process_parameters() const
  { 
    return !core::is_nil(get_arg(1)); 
  }

  data::variable_list process_parameters() const
  {
    assert(has_process_parameters());
    return data::variable_list(data::detail::add_index(get_arg(1)));
  }  

  bool has_action_labels() const
  { 
    return !core::is_nil(get_arg(2)); 
  }

  lps::action_label_list action_labels() const
  {
    assert(has_action_labels());
    return lps::action_label_list(data::detail::add_index(get_arg(2)));
  }  
};

const atermpp::function_symbol trailer_data::m_function_symbol = atermpp::function_symbol("mCRL2LTS1", 3);

#define LTS_TRAILER_TAG_LENGTH 12

/**
 * @brief A file stream that can append and retrieve trailer_data objects
 *        to and from a file.
 */
class trailer : public std::fstream
{
private:
  static const char* LTS_TRAILER_TAG;
  union
  {
    char buf[LTS_TRAILER_TAG_LENGTH + 8];
    struct
    {
      unsigned char info_pos[8];
      char trailer_tag[LTS_TRAILER_TAG_LENGTH];
    };
  } m_data;

  std::string m_filename;

  long stream_length()
  {
    /* Determine the position at which the additional information starts.
     * Due to the way in which file operations are implemented on Windows, 
     * we need to use the get pointer for determining the length of the 
     * stream. (seekp gives invalid results, leading to a wrong encoded 
     * position in the output of the LTS file).
	   * According to the example at 
     *   http://www.cplusplus.com/reference/istream/istream/seekg/
	   * this is the more-or-less standard way to determine the lenght of 
     * the file. 
     */
    long oldpos = tellg();
    seekg(0, std::ios::end);
    long position = tellg();
    seekg(oldpos, std::ios::beg);
    return position;
  }

  void write_tag(long extra_info_pos)
  {
    for (int i = 0; i < 8; ++i)
    {
      m_data.info_pos[i] = extra_info_pos % 0x100;
      extra_info_pos >>= 8;
    }
    std::fstream::write(&m_data.buf[0], sizeof(m_data));
  }

  bool read_tag(long& extra_info_pos)
  {
    seekg(-sizeof(m_data), std::ios_base::end);
    if (good())
    {
      std::fstream::read(&m_data.buf[0], sizeof(m_data));
      if (good())
      {
        if (!strncmp(LTS_TRAILER_TAG, m_data.trailer_tag, 12))
        {
          extra_info_pos = 0;
          for (int i = 7; i >= 0; --i)
          {
            extra_info_pos <<= 8;
            extra_info_pos |= m_data.info_pos[i];
          }
        }
        return true;
      }
    }
    return false;
  }

public:
  trailer(const std::string& filename, std::ios_base::openmode mode)
    : std::fstream(filename, mode | std::ios_base::binary), m_filename(filename)
  {
    if (mode & std::ios_base::out)
    {
      strncpy(m_data.trailer_tag, LTS_TRAILER_TAG, LTS_TRAILER_TAG_LENGTH);
    }
  }

  trailer_data read()
  {
    long position = 0;
    if (read_tag(position))
    {
      if (position == 0)
      {
        return trailer_data();
      }
      seekg(position, std::ios::beg);
      if (good())
      {
        return trailer_data(data::detail::add_index(atermpp::read_term_from_binary_stream(*this)));  
      }
    }
    throw mcrl2::runtime_error("Could not read trailer tag in '" + m_filename + "'.");
  }

  void write(const trailer_data& data)
  {
    long position = stream_length();
    if (position == -1)
    {
      throw mcrl2::runtime_error("Could not determine file size of '" + m_filename + "'; not adding extra information.");
    }
    seekp(0, std::ios::end);
    atermpp::write_term_to_binary_stream(data, *this);
    write_tag(position);
  }
  
};

const char* trailer::LTS_TRAILER_TAG = "   1STL2LRCm";
#undef LTS_TRAILER_TAG_LENGTH

static void read_from_lts(lts_lts_t& l, const std::string& filename)
{
  SVCfile f;
  SVCbool b;

  // Open file
  if (SVCopen(&f, const_cast<char*>(filename.c_str()), SVCread, &b))
  {
    throw mcrl2::runtime_error("cannot open lts file '" + filename + "' for reading (" + SVCerror(SVCerrno) + ").");
  }

  // Determine file type
  bool svc_file_has_state_info = false;
  std::string svc_type = SVCgetType(&f);
  if (svc_type == "mCRL2+info")
  {
    svc_file_has_state_info = true;
  }
  else if (svc_type != "mCRL2")
  {
    throw mcrl2::runtime_error("lts file '" + filename + "' is not in the mCRL2 format.");
  }

  // Read transitions
  SVCstateIndex from, to;
  SVClabelIndex label;
  SVCparameterIndex param;
  while (SVCgetNextTransition(&f, &from, &label, &to, &param))
  {
    l.add_transition(transition(from, label, to));
  }

  // Read state labels
  SVCint num_states = SVCnumStates(&f);
  detail::state_label_lts state_label;
  for (SVCstateIndex i = 0; i < num_states; ++i)
  {
    if (svc_file_has_state_info)
    {
      state_label = detail::state_label_lts(atermpp::aterm_cast<atermpp::aterm_appl>(data::detail::add_index(SVCstate2ATerm(&f, i))));
    }
    l.add_state(state_label);
  }

  // Set initial state
  assert(SVCgetInitialState(&f) == 0);
  l.set_initial_state((size_t)SVCgetInitialState(&f));

  // Read action labels
  SVCint num_labels = SVCnumLabels(&f);
  detail::action_label_lts action_label;
  const lps::action_list tau = lps::action_list();
  for (SVCstateIndex i = 0; i < num_labels; ++i)
  {
    action_label = detail::action_label_lts(atermpp::aterm_cast<atermpp::aterm_appl>(data::detail::add_index(SVClabel2ATerm(&f, i))));
    l.add_action(action_label, action_label.actions() == tau);
  }

  SVCclose(&f);

  // Check to see if there is extra data at the end
  try
  {
    trailer t(filename, std::ios::in);
    trailer_data data = t.read();
    if (data.is_valid())
    {
      l.set_data(data.data());
      if (data.has_process_parameters())
      {
        l.set_process_parameters(data.process_parameters());
      }
      if (data.has_action_labels())
      {
        l.set_action_labels(data.action_labels());
      }
    }
    t.close();
  }
  catch (std::runtime_error& e)
  {
    throw mcrl2::runtime_error("Error while reading datatypes, action declarations and "
                               "process parameters from '" + filename + "' (" + e.what() + ")");
  }
}

static void write_to_lts(const lts_lts_t& l, const std::string& filename)
{
  SVCfile f;
  SVCbool b = l.has_state_info() ? SVCfalse : SVCtrue;
  if (SVCopen(&f, const_cast<char*>(filename.c_str()), SVCwrite, &b))
  {
    throw mcrl2::runtime_error("cannot open .lts file '" + filename + "' for writing (" + SVCerror(SVCerrno) + ").");
  }

  if (l.has_state_info())
  {
    SVCsetType(&f,const_cast < char* >("mCRL2+info"));
  }
  else
  {
    SVCsetType(&f,const_cast < char* >("mCRL2"));
  }

  SVCsetCreator(&f,const_cast < char* >("liblts (mCRL2)"));

  assert(l.initial_state()< ((size_t)1 << (sizeof(int)*8-1)));
  if (l.has_state_info())
  {
    SVCsetInitialState(&f, SVCnewState(&f, data::detail::remove_index(l.state_label(l.initial_state())), &b));
  }
  else 
  {
    SVCsetInitialState(&f, SVCnewState(&f, atermpp::aterm_int(l.initial_state()), &b));
  }

  SVCparameterIndex param = SVCnewParameter(&f, atermpp::aterm_list(), &b);

  const std::vector<transition> &trans = l.get_transitions();
  SVCstateIndex from, label, to;
  for (std::vector<transition>::const_iterator t = trans.begin(); t != trans.end(); ++t)
  {
    assert(t->from() < ((size_t)1 << (sizeof(int)*8-1)));
    assert(t->to() < ((size_t)1 << (sizeof(int)*8-1)));
    if (l.has_state_info())
    {
      from = SVCnewState(&f, data::detail::remove_index(l.state_label(t->from())), &b); 
      to = SVCnewState(&f, data::detail::remove_index(l.state_label(t->to())), &b);
    }
    else
    {
      from = SVCnewState(&f, atermpp::aterm_int(t->from()), &b); 
      to = SVCnewState(&f, atermpp::aterm_int(t->to()), &b);
    }
    label = SVCnewLabel(&f, data::detail::remove_index(l.action_label(t->label()).aterm_without_time()), &b);
    SVCputTransition(&f, from, label, to, param);
  }

  SVCclose(&f);

  trailer t(filename, std::ios::out | std::ios::app);
  t.write(trailer_data::create(l));
  t.close();
}

void lts_lts_t::save(const std::string& filename) const
{
  if (filename=="")
  {
    throw mcrl2::runtime_error("Cannot write svc/lts file " + filename + " to stdout");
  }
  else
  {
    mCRL2log(log::verbose) << "Starting to save file " << filename << "\n";
    write_to_lts(*this,filename);
  }
}

void lts_lts_t::load(const std::string& filename)
{
  if (filename=="")
  {
    throw mcrl2::runtime_error("Cannot read svc/lts file " + filename + " from stdin");
  }
  else
  {
    mCRL2log(log::verbose) << "Starting to load file " << filename << "\n";
    read_from_lts(*this,filename);
  }
}

}
}
