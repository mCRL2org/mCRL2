// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./dof.h

#ifndef DOF_H
#define DOF_H

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <string>
#include <vector>
#include "attribute.h"

class DOF
{
  public:
    // -- constructors and destructor -------------------------------
    DOF(int index, QString label);
    DOF(const DOF& dof);

    // -- std::set functions ---------------------------------------------
    void setIndex(int index)      { m_index = index; }
    void setLabel(QString label)  { m_label = label; }

    void setMin(double value)     { m_values[0] = value; }
    void setMax(double value)     { m_values[m_values.size()-1] = value; }
    void clearValues()            { m_values.clear(); }
    void setValue(int index, double value);
    void addValue(double value)   { m_values.append(value); }
    void removeValue(int index);

    void setDirection(int direction)        { m_direction = direction; }
    void setAttribute(Attribute* attribute) { m_attribute = attribute; }

    // -- get functions ---------------------------------------------
    int index()             { return m_index; }
    QString label()         { return m_label; }

    double min()            { return m_values.first(); }
    double max()            { return m_values.last(); }
    int valueCount()        { return m_values.size(); }
    double value(int index);
    const QList<double> &values()  { return m_values; }
    int direction()         { return m_direction; }
    Attribute* attribute()  { return m_attribute; }


  protected:
    // -- data members ----------------------------------------------
    size_t        m_index;    // index in attribute
    QString       m_label;
    QList<double> m_values;
    int           m_direction;
    Attribute*    m_attribute; // association
};

#endif

// -- end -----------------------------------------------------------
