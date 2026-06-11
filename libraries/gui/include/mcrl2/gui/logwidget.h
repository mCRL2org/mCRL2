// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_UTILITIES_LOGWIDGET_H
#define MCRL2_UTILITIES_LOGWIDGET_H

#include <memory>

#include <QDateTime>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/gui/utilities.h"

namespace Ui
{
  class LogWidget;
}

namespace mcrl2::gui::qt
{

class LogRelay : public QObject, public mcrl2::log::output_policy
{
  Q_OBJECT

  public:
    void output(mcrl2::log::log_level_t level,
      time_t timestamp,
      const std::string& msg,
      bool print_time_information) override;

  signals:
    void logMessage(QString level, QDateTime timestamp, QString message);
};

class LogWidget: public QWidget
{
  Q_OBJECT

  public:
    LogWidget(QWidget *parent = nullptr);
    ~LogWidget() override;

  signals:
    void logMessage(QString level, QDateTime timestamp, QString message, QString formattedMessage);

  private slots:
    void writeMessage(QString level, QDateTime timestamp, QString message);

  private:
    std::unique_ptr<Ui::LogWidget> m_ui;
    LogRelay m_relay;
    QSize m_sizeHint;
};

} // namespace mcrl2::gui::qt

#endif // MCRL2_UTILITIES_LOGWIDGET_H
