#ifndef PARSING_H
#define PARSING_H

#include "mcrl2/process/parse.h"
#include "mcrl2/data/parse.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cctype>

using namespace mcrl2::log;

namespace mcrl2xi_qt
{

inline
void parse_mcrl2_specification(const std::string input)
{
    log_level_t old_level = mcrl2_logger::get_reporting_level();
    std::streambuf *old = std::cerr.rdbuf();

    try
    {

      mCRL2log(info) << "Parsing and type checking specification" << std::endl;

      // Dirty hack: redirect cerr such that is becomes silent and parse errors are ignored.
      mcrl2_logger::set_reporting_level(quiet);
      std::stringstream ss;
      std::cerr.rdbuf (ss.rdbuf());
      mcrl2::data::data_specification spec = mcrl2::data::parse_data_specification(input);

      //Restore cerr such that parse errors become visible.
      std::cerr.rdbuf (old);
      mcrl2_logger::set_reporting_level(old_level);
      mCRL2log(info) << "Specification is a valid data specification" << std::endl;
    }
    catch (mcrl2::runtime_error e)
    {
      std::cerr.rdbuf (old);
      mcrl2_logger::set_reporting_level(old_level);

      try
      {
        mcrl2::process::process_specification spec = mcrl2::process::parse_process_specification(input);
        mCRL2log(info) << "Specification is a valid mCRL2 specification" << std::endl;

      }
      catch (mcrl2::runtime_error e)
      {
        mCRL2log(error) << "Specification contains no valid data or mCRL2 specification." << std::endl;
        mCRL2log(error) << e.what() << std::endl;
      }
    }
}

inline
bool parse_mcrl2_specification_with_variables(const std::string s, mcrl2::data::data_specification& data_spec, std::set<mcrl2::data::variable>& vars)
{
  log_level_t old_level = mcrl2_logger::get_reporting_level();
  std::streambuf *old = std::cerr.rdbuf();
  try
  {
    mcrl2_logger::set_reporting_level(quiet);

    // Dirty hack: redirect cerr such that is becomes silent and parse errors are ignored.

    std::stringstream ss;
    std::cerr.rdbuf (ss.rdbuf());
    data_spec = mcrl2::data::parse_data_specification(s);

    //Restore cerr such that parse errors become visible.
    std::cerr.rdbuf (old);
    mcrl2_logger::set_reporting_level(old_level);
  }
  catch(mcrl2::runtime_error& )
  {
    std::cerr.rdbuf (old);
    mcrl2_logger::set_reporting_level(old_level);
    mcrl2::process::process_specification spec;
    try
    {
       spec = mcrl2::process::parse_process_specification(s);
       data_spec = spec.data();
       vars = spec.global_variables();
    }
    catch(mcrl2::runtime_error& e)
    {
      mCRL2log(error) << e.what() << std::endl;
      return false;
    }
  }
  return true;
}
}
#endif // PARSING_H
