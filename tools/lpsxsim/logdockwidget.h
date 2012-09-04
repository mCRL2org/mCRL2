// Author: Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LPSXSIM_COLLAPSIBLEDOCKWIDGET_H
#define LPSXSIM_COLLAPSIBLEDOCKWIDGET_H

#include <QDockWidget>
#include <QMainWindow>
#include <QPushButton>

#include "mcrl2/utilities/logwidget.h"

class LogDockWidget : public QDockWidget
{
  Q_OBJECT

  public:
    LogDockWidget(QMainWindow *window);
    void setWindowTitle(QString title) { m_collapseButton->setText(title); }
    void setWidget(QWidget *widget) { delete widget; }

  public slots:
    void setExpanded(bool expanded);
    void expand();

  signals:
    void logMessage(QString level, QString hint, QDateTime timestamp, QString message, QString formattedMessage);

  protected:
    mcrl2::utilities::qt::LogWidget *m_logWidget;
    QPushButton *m_collapseButton;
    QSize m_widgetSize;
};

#endif
