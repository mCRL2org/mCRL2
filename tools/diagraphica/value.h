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
    Value(size_t idx, const std::string& val): index(idx), value(val) {}

    void setIndex(size_t idx) { index = idx; }
    void setValue(const std::string& val) { value = val; }

    size_t getIndex() const { return index; }
    std::string getValue() const { return value; }

  protected:
    size_t    index; // index in attribute
    std::string value; // actual value
};

#endif
