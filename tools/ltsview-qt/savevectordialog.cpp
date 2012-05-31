// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "savevectordialog.h"
#include <QMessageBox>
#include <gl2ps.h>
#include <stdio.h>

SaveVectorDialog::SaveVectorDialog(QWidget *parent, LtsCanvas *canvas, QString filename, GLint format):
  QDialog(parent),
  m_canvas(canvas),
  m_filename(filename),
  m_format(format)
{
  m_ui.setupUi(this);

  connect(m_ui.buttonBox, SIGNAL(accepted()), this, SLOT(save()));
  connect(m_ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void SaveVectorDialog::save()
{
  if (QMessageBox::warning(this, QString(), "Saving an vector image may create very large files.\nAre you sure you want to continue?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
  {
    return;
  }

  FILE *file = fopen(m_filename.toStdString().c_str(), "wb");
  if (!file)
  {
    QMessageBox::critical(this, "Error writing file", "Could not open file for writing.");
    return;
  }

  emit statusMessage("Saving image...");

  GLint options = GL2PS_SILENT | GL2PS_USE_CURRENT_VIEWPORT | GL2PS_BEST_ROOT;
  if (m_ui.includeBackgroundColor->isChecked())
  {
    options |= GL2PS_DRAW_BACKGROUND;
  }
  if (m_ui.landscapePaperOrientation->isChecked())
  {
    options |= GL2PS_LANDSCAPE;
  }
  if (!m_ui.postScriptLevel3->isChecked())
  {
    options |= GL2PS_NO_PS3_SHADING;
  }
  if (!m_ui.enableBlending->isChecked())
  {
    options |= GL2PS_NO_BLENDING;
  }
  if (m_ui.removeHiddenPolygons->isChecked())
  {
    options |= GL2PS_OCCLUSION_CULL;
  }
  if (m_ui.tightBoundingBox->isChecked())
  {
    options |= GL2PS_TIGHT_BOUNDING_BOX;
  }

  m_canvas->renderVectorStart();
  GLint buffsize = 0;
  bool success;
  while (true)
  {
    buffsize += 1024*1024;
    if (gl2psBeginPage(m_filename.toStdString().c_str(), "LTSView", NULL,
                     m_format, GL2PS_BSP_SORT, options,
                     GL_RGBA, 0, NULL, 0, 0, 0, buffsize, file, ""
                     ) == GL2PS_ERROR)
    {
      success = false;
      break;
    }
    m_canvas->renderVectorPage();
    GLint state = gl2psEndPage();
    if (state == GL2PS_ERROR)
    {
      success = false;
      break;
    }
    if (state != GL2PS_OVERFLOW)
    {
      success = true;
      break;
    }
  }
  m_canvas->renderVectorFinish();

  fclose(file);

  if (success)
  {
    emit statusMessage("Done");
  }
  else
  {
    emit statusMessage("Saving image failed.");
    QMessageBox::critical(this, "Error writing file", "Saving picture failed with an unknown error.");
  }

  accept();
}
