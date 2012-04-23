#include "outputpanel.h"
#include <QDateTime>

void logRelay::output(const log_level_t level, const std::string& hint, const time_t timestamp, const std::string& msg)
{
  emit logOutput(level, QString::fromStdString(hint), timestamp, QString::fromStdString(msg));
}

OutputPanel::OutputPanel(QWidget *parent) :
  QWidget(parent)
{
  ui.setupUi(this);
  qRegisterMetaType<log_level_t>("log_level_t");
  qRegisterMetaType<time_t>("time_t");
  connect(&m_relay, SIGNAL(logOutput(log_level_t, QString, time_t, QString)), this, SLOT(writeMessage(log_level_t, QString, time_t, QString)));
  mcrl2_logger::register_output_policy(m_relay);
}


void OutputPanel::writeMessage(const log_level_t level, QString hint, const time_t timestamp, QString message)
{
  QDateTime dateTime = QDateTime::fromTime_t(timestamp);
  QString dateString = dateTime.toString("hh:mm:ss");
  QString logLevel = QString::fromStdString(log_level_to_string(level));
  QString logMessage = QString("[%1 %2] %3 %4").arg(dateString).arg(logLevel).arg(hint).arg(message).trimmed();
  ui.editOutput->appendPlainText(logMessage);
}

