// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SAVEPICTUREDIALOG_H
#define SAVEPICTUREDIALOG_H

#include "ui_savepicturedialog.h"

#include <QDialog>

#include "glcanvas.h"

class SavePictureDialog : public QDialog
{
  Q_OBJECT

  public:
    SavePictureDialog(QWidget *parent, GLCanvas *canvas, QString filename);

  signals:
    void statusMessage(QString message);

  protected slots:
    void widthChanged(int value);
    void heightChanged(int value);
    void save();

  private:
    Ui::SavePictureDialog m_ui;
    GLCanvas *m_canvas;
    QString m_filename;
    float m_width;
    float m_height;
    bool m_inChange;
};

#endif
