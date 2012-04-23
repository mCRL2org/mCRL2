#ifndef OUTPUTPANEL_H
#define OUTPUTPANEL_H

#include <QWidget>
#include <QPlainTextEdit>
#include "mcrl2/utilities/logger.h"
#include "ui_outputpanel.h"

using namespace mcrl2::log;

class logRelay : public QObject, public output_policy
{
  Q_OBJECT
  public:
    void output(const log_level_t level, const std::string& hint, const time_t timestamp, const std::string& msg);
  signals:
    void logOutput(const log_level_t level, QString hint, const time_t timestamp, QString message);
};

namespace Ui {
    class OutputPanel;
}

class OutputPanel : public QWidget
{
    Q_OBJECT
    
  public:
    explicit OutputPanel(QWidget *parent = 0);

  private slots:
    void writeMessage(const log_level_t level, QString hint, const time_t timestamp, QString message);
    
  private:
    Ui::OutputPanel ui;
    logRelay m_relay;
};

#endif // OUTPUTPANEL_H
