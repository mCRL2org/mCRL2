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
#include <sstream>
#include "svc/svc.h"
#include "mcrl2/aterm/aterm_ext.h"
#include "mcrl2/lts/lts_lts.h"


using namespace std;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::lts;
using namespace mcrl2::core::detail;
using namespace mcrl2::data::detail;
using namespace mcrl2::log;

static void read_from_lts(lts_lts_t& l, string const& filename)
{
  SVCfile f;
  SVCbool b;
  bool svc_file_has_state_info = false;

  if (SVCopen(&f,const_cast< char* >(filename.c_str()),SVCread,&b))
  {
    throw mcrl2::runtime_error("cannot open lts file '" + filename + "' for reading.");
  }

  string svc_type = SVCgetType(&f);
  if (svc_type == "mCRL2")
  {
    svc_file_has_state_info = false; // redundant.
  }
  else if (svc_type == "mCRL2+info")
  {
    svc_file_has_state_info = true;
  }
  else
  {
    throw mcrl2::runtime_error("lts file '" + filename + "' is not in the mCRL2 format.");
  }

  assert(SVCgetInitialState(&f)==0);
  if (svc_file_has_state_info)
  {
    using namespace mcrl2::data;
    using namespace mcrl2::lts::detail;
    ATermAppl state_label=(ATermAppl)SVCstate2ATerm(&f,(SVCstateIndex) SVCgetInitialState(&f));
    l.add_state(state_label_lts(state_label));
  }
  else
  {
    l.add_state();
  }
  l.set_initial_state((size_t) SVCgetInitialState(&f));


  SVCstateIndex from, to;
  SVClabelIndex label;
  SVCparameterIndex param;
  while (SVCgetNextTransition(&f,&from,&label,&to,&param))
  {
    size_t max = (from > to)?from:to;
    for (size_t i=l.num_states(); i<=max; i++)
    {
      if (svc_file_has_state_info)
      {
        using namespace mcrl2::data;
        using namespace mcrl2::lts::detail;
        ATermAppl state_label=(ATermAppl)SVCstate2ATerm(&f,(SVCstateIndex) i);
        l.add_state(state_label_lts(state_label));
      }
      else
      {
        l.add_state();
      }
    }

    for (size_t i=l.num_action_labels(); i<=(size_t)label; i++)
    {
      ATermAppl lab = (ATermAppl) SVClabel2ATerm(&f,(SVClabelIndex) i);
      l.add_action((ATerm) lab,(ATisEmpty(ATLgetArgument(lab,0))==true)?true:false);
    }

    l.add_transition(transition((size_t) from,
                                (size_t) label,
                                (size_t) to));
  }

  SVCclose(&f);

  // Check to see if there is extra data at the end

  const std::string error_message="The .lts file " + filename +
                                  " does not appear to contain datatypes, action declarations and process parameters";
  FILE* g = fopen(filename.c_str(),"rb");
  if ((g == NULL) ||
      (fseek(g,-(12+8),SEEK_END) != 0))
  {
    throw mcrl2::runtime_error(error_message + " (cannot reopen file)");
  }
  else
  {
    unsigned char buf[8+12];
    if (fread(&buf,1,8+12,g) != 8+12)
    {
      throw mcrl2::runtime_error(error_message + " (file does not contain control information)");
    }
    else
    {
      if (!strncmp(((char*) buf)+8,"   1STL2LRCm",12))
      {
        ATerm data;
        long position = 0;
        for (unsigned char i=0; i<8; i++)
        {
          position = (position << 8) + buf[7-i];
        }
        if ((fseek(g,position,SEEK_SET) != 0) ||
            ((data = ATreadFromFile(g)) == NULL))
        {
          throw mcrl2::runtime_error(error_message + " (control information is incorrect)");
        }
        else
        {
          data::data_specification data_spec(atermpp::aterm_appl(ATgetArgument((ATermAppl)data,0)));
          data_spec.declare_data_specification_to_be_type_checked(); // We can assume that this data spec is well typed.
          l.set_data(data::data_specification(data_spec));
          if (!gsIsNil((ATermAppl)ATgetArgument((ATermAppl)data,1)))
          {
            // The parameters below have the structure "ParamSpec(variable list);
            l.set_process_parameters(data::variable_list(ATgetArgument(ATAgetArgument((ATermAppl)data,1),0)));
          }
          if (!gsIsNil((ATermAppl)ATgetArgument((ATermAppl)data,2)))
          {
            // The parameters below have the structure "ActSpec(variable list);
            l.set_action_labels(lps::action_label_list(ATgetArgument(ATAgetArgument((ATermAppl)data,2),0)));
          }
        }
      }
    }
    if (g != NULL)
    {
      fclose(g);
    }
  }
}

/* \brief Add an mCRL2 data specification, parameter list and action
 *         specification to a mCRL2 LTS in SVC format.
 * \param[in] filename   The file name of the mCRL2 LTS.
 * \param[in] data_spec  The data specification to add in mCRL2 internal
 *                       format (or NULL for none).
 * \param[in] params     The list of state(/process) parameters in mCRL2
 *                       internal format (or NULL for none).
 * \param[in] act_spec   The action specification to add in mCRL2 internal
 *                       format (or NULL for none).
 * \pre                  The LTS in filename is a mCRL2 SVC without extra
 *                       information. */
static void add_extra_mcrl2_lts_data(
  const std::string& filename,
  const bool has_data_spec,
  const ATermAppl data_spec,
  const bool has_params,
  const ATermList params,
  const bool has_act_labels,
  const ATermList act_labels)
{
  FILE* f = fopen(filename.c_str(),"ab");
  if (f == NULL)
  {
    throw mcrl2::runtime_error("Could not open file '" + filename + "' to add extra LTS information.");
    return;
  }

  ATerm arg1 = (ATerm)(has_data_spec?data_spec:gsMakeNil());
  ATerm arg2 = (ATerm)(has_params?ATmakeAppl1(AFun("ParamSpec",1,false),(ATerm) params):gsMakeNil());
  ATerm arg3 = (ATerm)(has_act_labels?core::detail::gsMakeActSpec(act_labels):gsMakeNil());
  ATerm data = (ATerm) ATmakeAppl3(AFun("mCRL2LTS1",3,false),arg1,arg2,arg3);

  /* From the remarks on MSDN:
   *
   *   "Note that when a file is opened for appending data, the current file position is determined
   *    by the last I/O operation, not by where the next write would occur. For example, if a file
   *    is opened for an append and the last operation was a read, the file position is the point
   *    where the next read operation would start, not where the next write would start. (When a
   *    file is opened for appending, the file position is moved to end of file before any write
   *    operation.) If no I/O operation has yet occurred on a file opened for appending, the file
   *    position is the beginning of the file."
   *
   * In practice, this means that the Microsoft implementation of ftell() returns 0 at this point
   * in our code. Simply trying to read one byte past the end of the file (the read will fail) seems
   * to resolve this issue.
   */
  char c;
  fseek(f,0,SEEK_END);
  if(fread(&c,1,1,f) != 0)
    throw mcrl2::runtime_error("Unexpectedly able to read past end of file.");

  long position = ftell(f);
  if (position == -1)
  {
    fclose(f);
    throw mcrl2::runtime_error("Could not determine file size of '" + filename +
                               "'; not adding extra information.");
    return;
  }

  if (ATwriteToBinaryFile(data,f) == false)
  {
    fclose(f);
    throw mcrl2::runtime_error("Error writing extra LTS information to '" + filename +
                               "', file could be corrupted.");
    return;
  }

  unsigned char buf[8+12+1] = "XXXXXXXX   1STL2LRCm";
  for (size_t i=0; i<8; i++)
  {
    buf[i] = position % 0x100;
    position >>= 8;
  }
  if (fwrite(buf,1,8+12,f) != 8+12)
  {
    fclose(f);
    throw mcrl2::runtime_error("error writing extra LTS information to '" + filename +
                               "', file could be corrupted.");
    return;
  }

  fclose(f);
}


static void write_to_lts(const lts_lts_t& l, string const& filename)
{
  SVCfile f;
  SVCbool b = l.has_state_info() ? SVCfalse : SVCtrue;
  if (SVCopen(&f,const_cast< char* >(filename.c_str()),SVCwrite,&b))
  {
    throw mcrl2::runtime_error("cannot open .lts file '" + filename + "' for writing.");
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
  SVCsetInitialState(&f,SVCnewState(&f, l.has_state_info() ? (ATerm)(ATermAppl)l.state_label(l.initial_state()) : (ATerm) ATmakeInt((int)l.initial_state()) ,&b));

  SVCparameterIndex param = SVCnewParameter(&f,(ATerm) ATmakeList0(),&b);

  const std::vector < transition> &trans=l.get_transitions();
  for (std::vector < transition>::const_iterator t=trans.begin(); t!=trans.end(); ++t)
  {
    assert(t->from()< ((size_t)1 << (sizeof(int)*8-1)));
    SVCstateIndex from = SVCnewState(&f, l.has_state_info() ? (ATerm)(ATermAppl)l.state_label(t->from()) : (ATerm) ATmakeInt((int)t->from()) ,&b);
    SVClabelIndex label = SVCnewLabel(&f, (ATerm)l.action_label(t->label()).aterm_without_time(), &b);
    assert(t->to()< ((size_t)1 << (sizeof(int)*8-1)));
    SVCstateIndex to = SVCnewState(&f, l.has_state_info() ? (ATerm)(ATermAppl)l.state_label(t->to()) : (ATerm) ATmakeInt((int)t->to()) ,&b);
    SVCputTransition(&f,from,label,to,param);
  }

  SVCclose(&f);


  ATermAppl  data_spec = mcrl2::data::detail::data_specification_to_aterm_data_spec(l.data());
  ATermList params = l.process_parameters();
  ATermList act_spec = l.action_labels();
  add_extra_mcrl2_lts_data(filename,l.has_data(),data_spec,l.has_process_parameters(),params,l.has_action_labels(),act_spec);
}

namespace mcrl2
{
namespace lts
{

void lts_lts_t::save(const std::string& filename) const
{
  if (filename=="")
  {
    throw mcrl2::runtime_error("Cannot write svc/lts file " + filename + " to stdout");
  }
  else
  {
    mCRL2log(verbose) << "Starting to save file " << filename << "\n";
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
    read_from_lts(*this,filename);
  }


}

}
}
