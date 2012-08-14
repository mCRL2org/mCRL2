// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef DOFDIALOG_H
#define DOFDIALOG_H

#include "ui_dofdialog.h"

#include <QDialog>
#include "graph.h"
#include "shape.h"
#include "dof.h"

namespace Ui {
  class DofDialog;
}

class DofDialog : public QDialog
{
    Q_OBJECT
    
  public:
    explicit DofDialog(QWidget *parent = 0);

    void setCurrentGraph(Graph* graph) { m_currentGraph = graph; refresh(); }
    Graph* currentGraph() { return m_currentGraph; }

    void setCurrentShape(Shape* shape) { m_currentShape = shape; refresh(); }
    Shape* currentShape() { return m_currentShape; }
    
  public slots:
    void refresh();
    void attributeSelected(int index);

  protected:
    DOF* currentDof(QString name);

  private:
    Ui::DofDialog m_ui;
    Graph* m_currentGraph;
    Shape* m_currentShape;

    QStringList m_dofNames;
    QMap<QString, QComboBox*> m_comboBoxes;

};

#endif // DOFDIALOG_H
