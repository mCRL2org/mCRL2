#ifndef GRAPHINFORMATION_H
#define GRAPHINFORMATION_H

#include <QDockWidget>
#include "graph.h"

namespace Ui
{
  class DockWidgetInfo;
}

namespace Graph
{
  class InformationUi;

  class Information
  {
    private:
      Graph& m_graph;
      InformationUi* m_ui;
    public:
      Information(Graph& graph);
      virtual ~Information();
      InformationUi* ui(QWidget* parent = 0);
      QString m_initial;
      QString m_initialstring;
      int m_nodes;
      int m_edges;
      int m_labels;
      void update();
  };

  class InformationUi : public QDockWidget
  {
      Q_OBJECT
    private:
      Information& m_info;
      Ui::DockWidgetInfo* m_ui;
    public:
      InformationUi(Information& info, QWidget* parent=0);
      virtual ~InformationUi();
      void updateLabels();
  };
}

#endif // GRAPHINFORMATION_H
