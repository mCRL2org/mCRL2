// Author: Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_SETTING_H
#define MCRL2_UTILITIES_SETTING_H

#include <QColor>
#include <QList>
#include <QObject>
#include <QPair>

namespace mcrl2
{
namespace utilities
{
namespace qt
{

/*
 * Unfortunately, MOC doesn't understand templates or macros, or this would be a whole lot cleaner.
 */
class SettingInt : public QObject
{
  Q_OBJECT
  public:
    SettingInt(int value): m_value(value) {}
    int value() { return m_value; }
  public slots:
    void setValue(int value) { if (value != m_value) { m_value = value; emit changed(value); } }
  signals:
    void changed(int value);
  private:
    int m_value;
};

class SettingBool : public QObject
{
  Q_OBJECT
  public:
    SettingBool(bool value): m_value(value) {}
    bool value() { return m_value; }
  public slots:
    void setValue(bool value) { if (value != m_value) { m_value = value; emit changed(value); } }
  signals:
    void changed(bool value);
  private:
    bool m_value;
};

class SettingFloat : public QObject
{
  Q_OBJECT
  public:
    SettingFloat(float value): m_value(value) {}
    float value() { return m_value; }
  public slots:
    void setValue(float value) { if (value != m_value) { m_value = value; emit changed(value); } }
  signals:
    void changed(float value);
  private:
    float m_value;
};

class SettingColor : public QObject
{
  Q_OBJECT
  public:
    SettingColor(QColor value): m_value(value) {}
    QColor value() { return m_value; }
  public slots:
    void setValue(QColor value) { if (value != m_value) { m_value = value; emit changed(value); } }
  signals:
    void changed(QColor value);
  private:
    QColor m_value;
};

class SettingEnum : public SettingInt
{
  Q_OBJECT
  public:
    typedef QPair<int, QString> Item;
    SettingEnum(QList<Item> items, int value = -1): SettingInt(value == -1 ? items.first().first : value), m_items(items) {}
    QList<Item> items() const { return m_items; }
  private:
    QList<Item> m_items;
};

} // namespace qt
} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_SETTING_H
