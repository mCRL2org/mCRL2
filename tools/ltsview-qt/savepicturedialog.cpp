// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "savepicturedialog.h"
#include <QImage>
#include <QImageWriter>

SavePictureDialog::SavePictureDialog(QWidget *parent, GLCanvas *canvas, QString filename):
  QDialog(parent),
  m_canvas(canvas),
  m_filename(filename),
  m_inChange(false)
{
  m_ui.setupUi(this);

  int width;
  int height;
  canvas->GetClientSize(&width, &height);
  m_width = width;
  m_height = height;
  m_ui.width->setValue(m_width);
  m_ui.height->setValue(m_height);

  connect(m_ui.width, SIGNAL(valueChanged(int)), this, SLOT(widthChanged(int)));
  connect(m_ui.height, SIGNAL(valueChanged(int)), this, SLOT(heightChanged(int)));
  connect(m_ui.buttonBox, SIGNAL(accepted()), this, SLOT(save()));
  connect(m_ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void SavePictureDialog::widthChanged(int value)
{
  if (m_inChange)
  {
    return;
  }

  if (m_ui.maintainAspectRatio->isChecked())
  {
    m_inChange = true;
    m_ui.height->setValue((int)(value * m_height / m_width));
    m_inChange = false;
  }
}

void SavePictureDialog::heightChanged(int value)
{
  if (m_inChange)
  {
    return;
  }

  if (m_ui.maintainAspectRatio->isChecked())
  {
    m_inChange = true;
    m_ui.height->setValue((int)(value * m_width / m_height));
    m_inChange = false;
  }
}

void SavePictureDialog::save()
{
  m_canvas->display();

  int width = m_ui.width->value();
  int height = m_ui.height->value();

  emit statusMessage("Rendering image...");
  unsigned char* imageData = m_canvas->getPictureData(width, height);
  QImage image = QImage(imageData, width, height, QImage::Format_ARGB32).mirrored();

  emit statusMessage("Saving image...");
  QImageWriter writer(m_filename);
  if (writer.write(image))
  {
    emit statusMessage("Done");
  }
  else
  {
    emit statusMessage("Saving image failed.");
  }

  free(imageData);
  accept();
}
