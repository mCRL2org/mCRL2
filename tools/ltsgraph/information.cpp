// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "information.h"
#include "ui_information.h"

namespace Graph
{

  //
  // Information
  //

  Information::Information(Graph &graph)
    : m_graph(graph), m_ui(NULL), m_initial(""), m_initialstring(""), m_nodes(0), m_edges(0), m_slabels(0), m_tlabels(0)
  {
    update();
  }

  Information::~Information()
  {
    delete m_ui;
  }

  void Information::update()
  {
    if (m_graph.initialState() < m_graph.nodeCount())
    {
      m_initial = QString::number(m_graph.initialState());
      m_initialstring = m_graph.stateLabelstring(m_graph.initialState());
    } else {
      m_initial = QString("None");
      m_initialstring = QString("");
    }
    m_nodes = m_graph.nodeCount();
    m_edges = m_graph.edgeCount();
    m_slabels = m_graph.stateLabelCount();
    m_tlabels = m_graph.transitionLabelCount();
    if (m_ui != NULL)
      m_ui->updateLabels();
  }

  InformationUi* Information::ui(QWidget* parent)
  {
    if (m_ui == NULL)
      m_ui = new InformationUi(*this, parent);
    return m_ui;
  }

  InformationUi::InformationUi(Information &info, QWidget *parent)
    : QDockWidget(parent), m_info(info)
  {
    m_ui.setupUi(this);
    updateLabels();
  }

  void InformationUi::updateLabels()
  {
    m_ui.lblInitialValue->setText(m_info.m_initial);
    m_ui.lblInitialValue->setToolTip(m_info.m_initialstring);
    m_ui.lblStatesValue->setText(QString::number(m_info.m_nodes));
    m_ui.lblTransitionsValue->setText(QString::number(m_info.m_edges));
    m_ui.lblStateLabelsValue->setText(QString::number(m_info.m_slabels));
    m_ui.lblTransitionLabelsValue->setText(QString::number(m_info.m_tlabels));
  }

}
