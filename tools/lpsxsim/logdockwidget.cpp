// Author: Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "logdockwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

LogDockWidget::LogDockWidget(QMainWindow *window):
  QDockWidget(window)
{
  setFeatures(QDockWidget::NoDockWidgetFeatures);

  m_logWidget = new mcrl2::utilities::qt::LogWidget;
  m_logWidget->layout()->setContentsMargins(0, 0, 0, 0);
  connect(m_logWidget, SIGNAL(logMessage(QString, QString, QDateTime, QString, QString)), this, SIGNAL(logMessage(QString, QString, QDateTime, QString, QString)));
  connect(m_logWidget, SIGNAL(logMessage(QString, QString, QDateTime, QString, QString)), this, SLOT(expand()));

  m_collapseButton = new QPushButton("Log output");
  m_collapseButton->setCheckable(true);
  connect(m_collapseButton, SIGNAL(toggled(bool)), this, SLOT(setExpanded(bool)));

  QHBoxLayout *horizontal = new QHBoxLayout;
  horizontal->addStretch();
  horizontal->addWidget(m_collapseButton);
  horizontal->setContentsMargins(0, 0, 0, 0);

  QVBoxLayout *vertical = new QVBoxLayout;
  vertical->addLayout(horizontal);
  vertical->addWidget(m_logWidget);
  vertical->setSpacing(0);
  QMargins margins = vertical->contentsMargins();
  margins.setTop(1);
  margins.setBottom(1);
  vertical->setContentsMargins(margins);

  QWidget *content = new QWidget;
  content->setLayout(vertical);

  setTitleBarWidget(new QWidget);
  QDockWidget::setWidget(content);

  setExpanded(false);
  m_widgetSize = m_logWidget->sizeHint();
}

void LogDockWidget::setExpanded(bool expanded)
{
  if (expanded)
  {
    m_logWidget->setSizeHint(m_widgetSize);
    m_logWidget->show();
    widget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_logWidget->setSizeHint(QSize());
    m_collapseButton->setChecked(true);
  }
  else
  {
    if (m_logWidget->isVisible())
    {
      m_widgetSize = m_logWidget->size();
    }
    m_logWidget->hide();
    m_collapseButton->setChecked(false);
    widget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  }
}

void LogDockWidget::expand()
{
  disconnect(m_logWidget, SIGNAL(logMessage(QString, QString, QDateTime, QString, QString)), this, SLOT(expand()));
  setExpanded(true);
}
