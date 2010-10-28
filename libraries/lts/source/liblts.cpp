// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts.cpp

#include <string>
#include <set>
#include <stack>
#include <bitset>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <algorithm>
// #include <boost/bind.hpp>
#include "aterm2.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/lts/lts_io.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

#ifdef USE_BCG
#include <bcg_user.h>
#endif

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisList(x) (ATgetType(x) == AT_LIST)

using namespace std;

namespace mcrl2
{
namespace lts
{
namespace detail
{

atermpp::vector < ATermAppl > state_label_lts::vector_templates;

lps::specification const& empty_specification() 
{
  static lps::specification dummy;

  return dummy;
}
}

namespace detail
{

lts_type detect_type(string const& filename)
{
  ifstream is(filename.c_str(),ifstream::in|ifstream::binary);
  if ( !is.is_open() )
  {
    throw mcrl2::runtime_error("Cannot open file '" + filename + "' for reading.");
    return lts_none;
  }

  lts_type t = detect_type(is);

  is.close();

  return t;
}

typedef struct {
  unsigned int flag;
  unsigned int size;
  unsigned int value;
} svc_int;

class svc_buffer
{
  private:
    unsigned char buffer[56];
    std::streamsize buffer_size;
    unsigned int pos;
    unsigned int count;
    istream *input;
    bool valid;

  public:
    svc_buffer(istream *is)
    {
      input = is;
      buffer_size = 0;
      pos = 0;
      count = 0;
      valid = true;
    }

    svc_buffer(unsigned char *buf, unsigned int size)
    {
      input = NULL;
      set_buffer(buf,size);
    }

    void set_input(istream *is)
    {
      input = is;
    }

    void set_buffer(unsigned char *buf, unsigned int size)
    {
      if ( size > 56 )
      {
        size = 56;
      }
      memcpy(buffer,buf,size);
      buffer_size = size;
      pos = 0;
      count = 0;
      valid = true;
    }

    void reset_buffer()
    {
      buffer_size = 0;
      pos = 0;
      count = 0;
      valid = true;
    }

    unsigned int get_count()
    {
      return count;
    }

    bool is_valid()
    {
      return valid;
    }

    unsigned int get_bit()
    {
      if ( static_cast< std::streamsize >(pos/8) == buffer_size )
      {
        input->read((char *) buffer,56);
        if ( input->eof() )
        {
          input->clear();
        }
        buffer_size = input->gcount();
        pos = 0;
      }
      if ( static_cast< std::streamsize >(pos/8) == buffer_size )
      {
        valid = false;
        return 0;
      } else {
        unsigned int r = ( buffer[pos/8] >> (7 - (pos%8)) ) & 1;
        pos++;
        count++;
        return r;
      }
    }

    unsigned char get_byte()
    {
      unsigned char r = 0;
      for (unsigned int i=0; i<8; i++)
      {
        r = r*2 + get_bit();
      }
      return r;
    }

    char get_char()
    {
      char r = 0;
      for (unsigned int i=0; i<7; i++)
      {
        r = r*2 + get_bit();
      }
      return r;
    }

    svc_int get_int()
    {
      svc_int r;
      r.flag = get_bit();
      r.size = get_bit();
      r.size = r.size*2 + get_bit();
      r.value = 0;
      for (unsigned int i=0; i<=r.size; i++)
      {
        r.value = r.value*256 + get_byte();
      }
      return r;
    }

    string get_string()
    {
      string s;
      bool is_valid = false;
      while ( true )
      {
        char c = get_char();
        if ( c == 0 )
        {
          is_valid = true;
          break;
        } else if ( (c < ' ') || (c > '~') )
        {
          break;
        } else {
          s += c;
        }
      }

      if (! (valid && is_valid) ) {
        valid = false;

        s.clear();
      }

      return s;
    }
};

lts_type detect_type(istream &is)
{
  if ( is == cin ) // XXX better test to see if is is seekable?
  {
    throw mcrl2::runtime_error("Type detection does not work on stdin.");
    return lts_none;
  }

  streampos init_pos = is.tellg();
  char buf[32]; is.read(buf,32);
  if ( is.eof() ) is.clear();
  std::streamsize r = is.gcount();
  is.seekg(init_pos);

  // detect lts_aut
  if ( r >= 3 )
  {
    // we assume that "des" is completely in buf in case this is a aut file
    int i = 0;
    // skip any spaces or tabs
    while ( (i < r) && ((buf[i] == ' ') || (buf[i] == '\t')) )
    {
      i++;
    }
    // at least need to start with des
    if ( (i+3 <= r) && !memcmp(buf+i,"des",3) )
    {
      i = i + 3;
      // skip any spaces or tabs
      while ( (i < r) && ((buf[i] == ' ') || (buf[i] == '\t')) )
      {
        i++;
      }
      // if we are not at the end of the buffer, then we expect a opening
      // parenthesis
      if ( (i >= r) || (buf[i] == '(') )
      { if (core::gsVerbose)
        { std::cerr << "detected AUT input file\n";
        }
        return lts_aut;
      }
    }
  }

  // detect lts_dot
  if ( r >= 7 )
  {
    // we assume that "digraph" is completely in buf in case this is a dot file
    int i = 0;
    // skip any spaces or tabs
    while ( (i < r) && ((buf[i] == ' ') || (buf[i] == '\t')) )
    {
      i++;
    }
    // at least need to start with digraph
    if ( (i+7 <= r) && !memcmp(buf+i,"digraph",7) )
    {
      i = i + 7;
      if (core::gsVerbose)
      { std::cerr << "Detected DOT input file.\n";
      }
      return lts_dot;
    }
  }

  // detect lts_svc, lts_mcrl and lts_lts
  if ( r >= 18 )
  {
    svc_buffer sbuf((unsigned char *) buf, static_cast< unsigned int >(r));
    sbuf.get_bit(); // indexed flag
    svc_int header_pos = sbuf.get_int(); // header pos
    if ( header_pos.flag == 0 )
    {
      if ( sbuf.get_int().flag == 0 ) // body pos
      {
        if ( sbuf.get_int().flag == 0 ) // trailer pos
        {
          svc_int version_pos = sbuf.get_int(); // version pos
          if ( (version_pos.flag == 0) &&
               (version_pos.value >= (sbuf.get_count()+7)/8) &&
               (header_pos.value >= (sbuf.get_count()+7)/8) )
          {
            is.seekg(init_pos+((streampos) version_pos.value));
            if ( is.fail() )
            {
              is.seekg(init_pos);
            } else {
              sbuf.reset_buffer();
              sbuf.set_input(&is);
              sbuf.get_string();
              if ( sbuf.is_valid() )
              {
                is.seekg(init_pos+((streampos) header_pos.value));
                if ( is.fail() )
                {
                  is.seekg(init_pos);
                } else {
                  sbuf.reset_buffer();
                  string s;
                  for (unsigned int i=0; i<4; i++)
                  {
                    s = sbuf.get_string();
                    if ( !sbuf.is_valid() )
                    {
                      break;
                    }
                  }

                  is.seekg(init_pos);
                  if ( sbuf.is_valid() )
                  {
                    if ( (s == "mCRL2") || (s == "mCRL2+info") )
                    { if (core::gsVerbose)
                      { std::cerr << "Detected mCRL2 input file.\n";
                      }
                      return lts_lts;
                    } 
                    else 
                    {
                      if (core::gsVerbose)
                      { std::cerr << "Detected SVC input file\n";
                      }
                      assert(0);
                      return lts_none; //lts_svc;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

#ifdef USE_BCG
  // detect lts_bcg
  if ( r >= 2 )
  {
    if ( (buf[0] == 0x01) && (buf[1] == 0x00) )
    {
      const unsigned int offsets[] = { 0x3, 0xb, 0x13, 0x1b, 0x23, 0x2b, 0x33, 0x3b, 0x43, 0x4b };
      const unsigned int num_offsets = 10;

      is.seekg(0,istream::end);
      streampos size = is.tellg();

      bool valid = true;
      for (unsigned int i=0; i<num_offsets; i++)
      {
        if ( offsets[i] >= size )
        {
          valid = false;
          break;
        }

        unsigned int pointer = 0; is.read((char *) &pointer,4);
        if ( pointer >= size )
        {
          valid = false;
          break;
        }
      }

      is.seekg(init_pos);

      if ( valid )
      {
        if (core::gsVerbose)
        { std::cerr << "Detected BCG input file.\n";
        }
        return lts_bcg;
      }
    }
  }
#endif

  return lts_none;
}

/* void read_from(lts_extra &l,
               istream &is, 
               const lts_type type_a) 
{
  lts_type type(type_a);
  if ( type == lts_none )
  {
    type = detect_type(is);
    if ( type == lts_none )
    { throw mcrl2::runtime_error("Could not determine type of input stream.");
    }
  }

  // l.set_type(type);

  switch ( type )
  {
    case lts_aut:
       read_from_aut(l,is);
       return; 
    case lts_lts:
    / * case lts_svc:
      throw mcrl2::runtime_error("Cannot read SVC based files from streams");
    * /
    case lts_fsm:
      assert(0);
      // read_from_fsm(l,is);
      return; 
    case lts_dot:
      assert(0);
      // read_from_dot(l,is);
      return; 
#ifdef USE_BCG
    case lts_bcg:
      throw mcrl2::runtime_error("Cannot read BCG files from streams.");
#endif
    default:
      throw mcrl2::runtime_error("Unknown source LTS type.");
  }
} */

/* void read_from(lts_extra &l,
               istream &is, 
               const lts_type type_a, 
               const data::data_specification &data_spec,
               const data::variable_list &parameters,
               const lps::action_label_list &action_decls)
{ 
  l.set_data(data_spec);
  l.set_process_parameters(parameters);
  l.set_action_labels(action_decls);
  read_from(l, is, type_a);
}

void read_from(lts_extra &l,
               const string &filename,
               const lts_type a_type)
{
  lts_type type=a_type;
  if ( type == lts_none )
  {
    type = detect_type(filename);
    if ( type == lts_none && (type = guess_format(filename)) == lts_none )
    {
      mcrl2::runtime_error("Could not determine type of input file '" + filename + "'.");
    }
  }

  switch ( type )
  {
    case lts_aut:
      break; // read using a stream.
    case lts_lts:
      read_from_svc(l,filename,lts_lts);
      return;
    / * case lts_svc:
      read_from_svc(l,filename,lts_svc);
      return; * /
    case lts_fsm:
    case lts_dot:
      break; // read using a stream.
#ifdef USE_BCG
    case lts_bcg:
      read_from_bcg(l,filename);
      return;
#endif
    default:
      throw runtime_error ("Unknown source LTS type");
  }

  // The files that were not read above, are read via streams.
  std::ifstream is(filename.c_str());

  if ( !is.is_open() )
  {
    throw mcrl2::runtime_error("cannot open file '" + filename + "' for reading.");
    return;
  }

  read_from(l,is,type);
}

void read_from(lts_extra &l,
               const string &filename, 
               const lts_type type, 
               const data::data_specification &data_spec,
               const data::variable_list &parameters,
               const lps::action_label_list &action_decls)
{ 
  l.set_data(data_spec);
  l.set_process_parameters(parameters);
  l.set_action_labels(action_decls);

  read_from(l,filename,type);
} */

} // namespace detail

/* void lts_extra::write_to(ostream &os, lts_type type) const
{
  switch ( type )
  {
    case lts_aut:
       detail::write_to_aut(*this,os);
       return;
    case lts_lts:
    / * case lts_svc:
       throw mcrl2::runtime_error("Cannot write SVC based files to streams."); * /
    case lts_fsm:
       assert(0);
       // detail::write_to_fsm(*this,os);
       return;
    case lts_dot:
       assert(0);
       // detail::write_to_dot(*this,os);
      return;
#ifdef USE_BCG
    case lts_bcg:
      throw mcrl2::runtime_error("Cannot write BCG files to streams.");
#endif
    default:
      throw mcrl2::runtime_error("Unknown target LTS type.");
  }
}

void lts_extra::write_to(string const& filename,lts_type type) const
{
  switch (type)
  {
    case lts_lts:
    / * case lts_svc:
      detail::write_to_svc(*this,filename,type);
      return; * /
    default:;
  }
  std::ofstream os(filename.c_str());

  if ( !os.is_open() )
  {
    throw mcrl2::runtime_error("cannot open file '" + filename + "' for writing.");
  }

  write_to(os,type);
} */

namespace detail
{

lts_type guess_format(string const& s) 
{
  string::size_type pos = s.find_last_of('.');

  if ( pos != string::npos )
  {
    string ext = s.substr(pos+1);

    if ( ext == "aut" )
    {
      if (core::gsVerbose)
      { std::cerr << "Detected Aldebaran extension.\n";
      }
      return lts_aut;
    } 
    else if ( ext == "lts" )
    {
      if (core::gsVerbose)
      { std::cerr << "Detected mCRL2 extension.\n";
      }
      return lts_lts;
    } 
    else if ( ext == "svc" )
    {
      if (core::gsVerbose)
      { std::cerr << "Detected SVC extension; assuming mCRL2 format.\n";
      }
      return lts_lts;
    } 
    else if ( ext == "fsm" )
    {
      if (core::gsVerbose)
      { std::cerr << "Detected Finite State Machine extension.\n";
      }
      return lts_fsm;
    } 
    else if ( ext == "dot" )
    {
      if (core::gsVerbose)
      { std::cerr << "Detected GraphViz extension.\n";
      }
      return lts_dot;
#ifdef USE_BCG
    } 
    else if ( ext == "bcg" )
    {
      if (core::gsVerbose)
      { std::cerr << "Detected Binary Coded Graph extension.\n";
      }
      return lts_bcg;
#endif
    }
  }

  return lts_none;
}

static std::string type_strings[] = { "unknown", "lts", "aut", "svc", "fsm", "dot", "bcg" };

static std::string extension_strings[] = { "", "lts", "aut", "svc", "svc", "fsm", "dot", "bcg" };

static std::string type_desc_strings[] = { "unknown LTS format",
                                           "mCRL2 LTS format",
                                           "Aldebaran format (CADP)",
                                           "SVC format",
                                           "Finite State Machine format",
                                           "GraphViz format",
                                           "Binary Coded Graph format (CADP)" 
                                         };


static std::string mime_type_strings[] = { "", "application/lts", "text/aut", "application/svc", "text/fsm", "text/dot", "application/bcg" };

lts_type parse_format(std::string const& s) 
{
  if ( s == "lts")
  {
    return lts_lts;
  } 
  else if ( s == "aut" )
  {
    return lts_aut;
  } 
  /* else if ( s == "svc" )
  {
    return lts_svc;
  }  */
  else if ( s == "fsm" )
  {
    return lts_fsm;
  } 
  else if ( s == "dot" )
  {
    return lts_dot;
#ifdef USE_BCG
  } 
  else if ( s == "bcg" )
  {
    return lts_bcg;
#endif
  }

  return lts_none;
}

std::string string_for_type(const lts_type type) 
{
  return (type_strings[type]);
}

std::string extension_for_type(const lts_type type) 
{
  return (extension_strings[type]);
}

std::string mime_type_for_type(const lts_type type) {
  return (mime_type_strings[type]);
}


void add_extra_mcrl2_lts_data(std::string const &filename, ATermAppl data_spec, ATermList params, ATermList act_labels)
{
  FILE *f = fopen(filename.c_str(),"ab");
  if ( f == NULL )
  {
    throw mcrl2::runtime_error("Could not open file '" + filename + "' to add extra LTS information.");
    return;
  }

  ATerm arg1 = (ATerm) ((data_spec == NULL)?gsMakeNil():data_spec);
  ATerm arg2 = (ATerm) ((params == NULL)?gsMakeNil():ATmakeAppl1(ATmakeAFun("ParamSpec",1,ATfalse),(ATerm) params));
  ATerm arg3 = (ATerm) ((ATisEmpty(act_labels))?gsMakeNil():core::detail::gsMakeActSpec(act_labels));
  ATerm data = (ATerm) ATmakeAppl3(ATmakeAFun("mCRL2LTS1",3,ATfalse),arg1,arg2,arg3);

  long position;
  if ( (position = ftell(f)) == -1 )
  {
    fclose(f);
    throw mcrl2::runtime_error("Could not determine file size of '" + filename + 
                          "'; not adding extra information.");
    return;
  }

  if ( ATwriteToBinaryFile(data,f) == ATfalse )
  {
    fclose(f);
    throw mcrl2::runtime_error("Error writing extra LTS information to '" + filename + 
               "', file could be corrupted.");
    return;
  }

  unsigned char buf[8+12+1] = "XXXXXXXX   1STL2LRCm";
  for (unsigned int i=0; i<8; i++)
  {
    buf[i] = position % 0x100;
    position /= 0x100;
  }
  if ( fwrite(buf,1,8+12,f) != 8+12 )
  {
    fclose(f);
    throw mcrl2::runtime_error("error writing extra LTS information to '" + filename + 
                     "', file could be corrupted.");
    return;
  }

  fclose(f);
}

static const std::set<lts_type> &initialise_supported_lts_formats()
{
  static std::set<lts_type> s;
  for (unsigned int i = lts_type_min; i<1+(unsigned int)lts_type_max; ++i)
  {
    if ( lts_none != (lts_type) i )
    {
      s.insert((lts_type) i);
    }
  }
  return s;
}
const std::set<lts_type> &supported_lts_formats()
{
  static const std::set<lts_type> &s = initialise_supported_lts_formats();
  return s;
}

std::string supported_lts_formats_text(lts_type default_format, const std::set<lts_type> &supported)
{
  vector<lts_type> types(supported.begin(),supported.end());
  std::sort(types.begin(),types.end(),boost::bind(lts_named_cmp<lts_type>,type_strings,_1,_2));

  string r;
  for (vector<lts_type>::iterator i=types.begin(); i!=types.end(); ++i)
  {
    r += "  '" + type_strings[*i] + "' for the " + type_desc_strings[*i];

    if ( *i == default_format )
    {
      r += " (default)";
    }

	// Still unsafe if types.size() < 2
	assert(types.size() >= 2);
    if ( i == types.end() - 2 )
    {
      r += ", or\n";
    } else if ( i != types.end() - 1)
    {
      r += ",\n";
    }
  }

  return r;
}

std::string supported_lts_formats_text(const std::set<lts_type> &supported)
{
  return supported_lts_formats_text(lts_none,supported);
}

std::string lts_extensions_as_string(const std::string &sep, const std::set<lts_type> &supported)
{
  vector<lts_type> types(supported.begin(),supported.end());
  std::sort(types.begin(),types.end(),boost::bind(lts_named_cmp<lts_type>,extension_strings,_1,_2));

  string r, prev;
  bool first = true;
  for (vector<lts_type>::iterator i=types.begin(); i!=types.end(); i++)
  {
    if ( extension_strings[*i] == prev ) // avoid mentioning extensions more than once
    {
      continue;
    }
    if ( first )
    {
      first = false;
    } else {
      r += sep;
    }
    r += "*." + extension_strings[*i];
    prev = extension_strings[*i];
  }

  return r;
}

std::string lts_extensions_as_string(const std::set<lts_type> &supported)
{
  return lts_extensions_as_string(",",supported);
}
} // namespace detail

ATermList sorted_insert(ATermList l,ATermAppl t)
{ 
  if (l==ATempty)
  { return ATinsert(l,(ATerm)t);
  }

  if (ATgetName(ATgetAFun(ATgetArgument(t,0)))<
         ATgetName(ATgetAFun(ATgetArgument(ATgetFirst(l),0))))
  { return ATinsert(l,(ATerm)t);
  }
  if ((ATgetName(ATgetAFun(ATgetArgument(t,0)))==
         ATgetName(ATgetAFun(ATgetArgument(ATgetFirst(l),0))))
       &&
      (ATgetArgument(t,1)< ATgetArgument(ATgetFirst(l),1)))
  { return ATinsert(l,(ATerm)t);
  }
  return ATinsert(sorted_insert(ATgetNext(l),t),ATgetFirst(l));
}

ATerm detail::sort_multi_action(ATerm ma)
{ ATermList tl=ATLgetArgument((ATermAppl)(ma),0); //get the multi_action_list
  ATermList result=ATempty;
  for( ; tl!=ATempty ; tl=ATgetNext(tl))
  { result=sorted_insert(result,ATAgetFirst(tl));
  }
  return (ATerm)gsMakeMultAct(result);
}



}
}
