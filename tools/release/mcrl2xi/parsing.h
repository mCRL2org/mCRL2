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
namespace detail
{

inline bool contains_keyword_init(const std::string& input)
{
  QString no_alphanumeric = "[^a-ZA-Z0-9]";
  QRegExp rx(no_alphanumeric + "init" + no_alphanumeric, Qt::CaseInsensitive);
  return rx.indexIn(QString::fromStdString(input)) != -1;
}

} // end namespace detail

inline
void parseMcrl2Specification(const std::string& input, mcrl2::data::data_specification& data_spec, std::set<mcrl2::data::variable>& vars)
{
  mCRL2log(info) << "Parsing and type checking specification" << std::endl;

  try
  {
    // First check whether the keyword "init" occurs in the string. If yes, it is an mCRL2 specification.
    // If no, it is a data specification. 
  
    if (detail::contains_keyword_init(input))
    {
      mcrl2::process::process_specification spec = mcrl2::process::parse_process_specification(input);
      data_spec = spec.data();
      vars = spec.global_variables();

      mCRL2log(info) << "Specification is a valid mCRL2 specification" << std::endl;
      return;
    }
    else
    {
      data_spec = mcrl2::data::parse_data_specification(input);

      mCRL2log(info) << "Specification is a valid data specification" << std::endl;
      return;
    }
  }
  catch (mcrl2::runtime_error e)
  {
    throw(mcrl2::runtime_error(QString("Specification does not contain a valid data or mCRL2 specification.\n").append(e.what()).toStdString()));
  }
}

inline
void parseMcrl2Specification(const std::string& input)
{
  mcrl2::data::data_specification data_spec;
  std::set<mcrl2::data::variable> vars;
  parseMcrl2Specification(input, data_spec, vars);
}

}
#endif // PARSING_H
