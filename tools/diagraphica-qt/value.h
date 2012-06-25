// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./value.h

#ifndef VALUE_H
#define VALUE_H

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <string>

class Value
{
  public:
    // -- constructors and destructor -------------------------------
    Value(
      const size_t& idx,
      const std::string& val);
    Value(const Value& val);
    virtual ~Value();

    // -- set functions ---------------------------------------------
    void setIndex(const size_t& idx);
    void setValue(const std::string& val);

    // -- get functions ---------------------------------------------
    size_t getIndex();
    std::string getValue();

  protected:
    // -- data members ----------------------------------------------
    size_t    index; // index in attribute
    std::string value; // actual value
};

#endif

// -- end -----------------------------------------------------------
