#ifndef PARSING_H
#define PARSING_H

#include "mcrl2/process/parse.h"
#include "mcrl2/data/parse.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cctype>

#include <QPoint>
#include <QString>
#include <QStringList>
#include <QRegExp>

using namespace mcrl2::log;

namespace mcrl2xi_qt
{

  inline
  void parseMcrl2Specification(const std::string input, mcrl2::data::data_specification& data_spec, std::set<mcrl2::data::variable>& vars)
  {
    mCRL2log(info) << "Parsing and type checking specification" << std::endl;

    log_level_t old_level = mcrl2_logger::get_reporting_level();
    std::streambuf *old = std::cerr.rdbuf();

    try
    {
      // Dirty hack: redirect cerr such that is becomes silent and parse errors are ignored.
      mcrl2_logger::set_reporting_level(quiet);
      std::stringstream ss;
      std::cerr.rdbuf (ss.rdbuf());

      data_spec = mcrl2::data::parse_data_specification(input);

      //Restore cerr such that parse errors become visible.
      std::cerr.rdbuf (old);
      mcrl2_logger::set_reporting_level(old_level);

      mCRL2log(info) << "Specification is a valid data specification" << std::endl;
      return;
    }
    catch (mcrl2::runtime_error e)
    {
      std::cerr.rdbuf (old);
      mcrl2_logger::set_reporting_level(old_level);
    }

    try
    {
      mcrl2::process::process_specification spec = mcrl2::process::parse_process_specification(input);
      data_spec = spec.data();
      vars = spec.global_variables();

      mCRL2log(info) << "Specification is a valid mCRL2 specification" << std::endl;
      return;
    }
    catch (mcrl2::runtime_error e)
    {
      throw(mcrl2::runtime_error(QString("Specification contains no valid data or mCRL2 specification: ").append(e.what()).toStdString()));
    }
  }

  inline
  void parseMcrl2Specification(const std::string input)
  {
    mcrl2::data::data_specification data_spec;
    std::set<mcrl2::data::variable> vars;
    parseMcrl2Specification(input, data_spec, vars);
  }

}
#endif // PARSING_H
