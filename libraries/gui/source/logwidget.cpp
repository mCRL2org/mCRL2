// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/gui/logwidget.h"
#include "ui_logwidget.h"

using namespace mcrl2::gui;
using namespace mcrl2::gui::qt;

void LogRelay::output(const log_level_t level,
                      const time_t timestamp, 
                      const std::string& msg, 
                      const bool /* print_time_information */)
{
  emit logMessage(QString::fromStdString(log_level_to_string(level).data()),
                  QDateTime::fromSecsSinceEpoch(timestamp), 
                  QString::fromStdString(msg));
}

LogWidget::LogWidget(QWidget *parent)
  : QWidget(parent),
    m_ui(new Ui::LogWidget)
{
  m_ui->setupUi(this);
  connect(&m_relay, SIGNAL(logMessage(QString, QDateTime, QString)), this, SLOT(writeMessage(QString, QDateTime, QString)));
  logger::register_output_policy(m_relay);
}

LogWidget::~LogWidget()
{
  delete m_ui;
}

void LogWidget::writeMessage(QString level, QDateTime timestamp, QString message)
{
  message = message.trimmed();
  if (!message.isEmpty())
  {
    QString formattedMessage = QString("[%1 %2] %3").arg(timestamp.toString("hh:mm:ss")).arg(level).arg(message).trimmed();
    switch (log_level_from_string(level.toStdString()))
    {
      case error:
        m_ui->editOutput->setTextColor(Qt::red);
        break;
      case warning:
        m_ui->editOutput->setTextColor(QColor("orange"));
        break;
      default:
        setTextEditTextColor(m_ui->editOutput, Qt::black, Qt::white);
        break;
    }

    m_ui->editOutput->append(formattedMessage);
    emit logMessage(level, timestamp, message, formattedMessage);
  }
}
