// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/**

  @file information.h
  @author R. Boudewijns

  This file contains an implementation and user interface which is able to calculate and display statistics for a graph.

*/

#ifndef GRAPHINFORMATION_H
#define GRAPHINFORMATION_H

#include <QDockWidget>
#include <QtOpenGL>
#include "ui_information.h"

#include "graph.h"

namespace Graph
{
  class InformationUi;

  class Information
  {
    private:
      Graph& m_graph;         ///< The graph for the statistics
      InformationUi* m_ui;    ///< The user interface which displays the information
    public:

      /**
       * @brief Constructor.
       * @param graph The graph for the statistics
       */
      Information(Graph& graph);

      /**
       * @brief Destructor.
       */
      virtual ~Information();

      /**
       * @brief Returns the user interface object. If no user interface is available,
       *        one is created using the provided @e parent.
       * @param The parent of the user inferface in the case none exists yet.
       */
      InformationUi* ui(QWidget* parent = 0);

      /**
       * @brief Updates the information.
       */
      void update();

      QString m_initial;        ///< The initial node index.
      QString m_initialstring;  ///< The initial node label.
      int m_nodes;              ///< The total number of nodes.
      int m_edges;              ///< The total number of edges.
      int m_slabels;            ///< The total number of state labels.
      int m_tlabels;            ///< The total number of transition labels.
  };

  class InformationUi : public QDockWidget
  {
      Q_OBJECT
    private:
      Information& m_info;      ///< The Information containing all needed information.
      Ui::DockWidgetInfo m_ui;  ///< The user interface of this objest.
    public:

      /**
       * @brief Constructor.
       * @param info The Information object this user interface corresponds to.
       * @param parent The parent widget for this user interface.
       */
      InformationUi(Information& info, QWidget* parent=0);

      /**
       * @brief Updates all labels with the information available in the Information object.
       */
      void updateLabels();
  };
}

#endif // GRAPHINFORMATION_H
