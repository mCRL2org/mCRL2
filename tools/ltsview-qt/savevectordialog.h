// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SAVEVECTORDIALOG_H
#define SAVEVECTORDIALOG_H

#include "ui_savevectordialog.h"

#include <QDialog>

#include "glcanvas.h"

class SaveVectorDialog : public QDialog
{
  Q_OBJECT

  public:
    SaveVectorDialog(QWidget *parent, GLCanvas *canvas, QString filename, GLint format);

  signals:
    void statusMessage(QString message);

  protected slots:
    void save();

  private:
    Ui::SaveVectorDialog m_ui;
    GLCanvas *m_canvas;
    QString m_filename;
    GLint m_format;
};

#endif
