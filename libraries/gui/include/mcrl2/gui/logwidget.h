// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_LOGWIDGET_H
#define MCRL2_UTILITIES_LOGWIDGET_H

#include <QDateTime>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/gui/utilities.h"

using namespace mcrl2::log;

namespace Ui
{
  class LogWidget;
}

namespace mcrl2
{

namespace gui
{

namespace qt
{

class LogRelay : public QObject, public output_policy
{
  Q_OBJECT

  public:
    void output(const log_level_t level, const time_t timestamp, const std::string& msg, const bool print_time_information);

  signals:
    void logMessage(QString level, QDateTime timestamp, QString message);
};

class LogWidget: public QWidget
{
  Q_OBJECT

  public:
    LogWidget(QWidget *parent = nullptr);
    ~LogWidget();
     virtual QSize sizeHint () const { return m_sizeHint.isValid() ? m_sizeHint : QWidget::sizeHint(); }
     void setSizeHint(QSize sizeHint) { m_sizeHint = sizeHint; }

  signals:
    void logMessage(QString level, QString hint, QDateTime timestamp, QString message, QString formattedMessage);

  private slots:
    void writeMessage(QString level, QString hint, QDateTime timestamp, QString message);

  private:
    Ui::LogWidget *m_ui;
    LogRelay m_relay;
    QSize m_sizeHint;
};

} // namespace qt

} // namespace gui

} // namespace mcrl2

#endif // MCRL2_UTILITIES_LOGWIDGET_H
