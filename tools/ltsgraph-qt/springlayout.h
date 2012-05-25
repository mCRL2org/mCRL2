#ifndef SPRINGLAYOUT_H
#define SPRINGLAYOUT_H

#include <QDockWidget>
#include "graph.h"

namespace Ui
{
    class DockWidget;
}

namespace Graph
{
    class SpringLayoutUi;

    class SpringLayout
    {
    public:
        enum ForceCalculation
        {
            ltsgraph,
            linearsprings
        };
    private:
        float m_speed;
        float m_attraction;
        float m_repulsion;
        float m_natLength;
        float m_controlPointWeight;
        std::vector<Coord3D> m_nforces, m_hforces, m_lforces;
        Coord3D m_clipMin;
        Coord3D m_clipMax;
        Coord3D (SpringLayout::*m_forceCalculation)(const Coord3D&, const Coord3D&, float);
        Coord3D forceLinearSprings(const Coord3D& a, const Coord3D& b, float ideal);
        Coord3D forceLTSGraph(const Coord3D& a, const Coord3D& b, float ideal);
        Graph& m_graph;
        SpringLayoutUi* m_ui;
    public:
        SpringLayout(Graph& graph);
        virtual ~SpringLayout();
        int speed() const { return m_speed * 10000.0; }
        int attraction() const { return m_attraction * 500.0; }
        int repulsion() const { return m_repulsion; }
        int controlPointWeight() const { return m_controlPointWeight * 1000.0; }
        int naturalTransitionLength() const { return m_natLength; }
        ForceCalculation forceCalculation();
        void setSpeed(int v) { m_speed = (float)v / 10000.0; }
        void setAttraction(int v) { m_attraction = (float)v / 500.0; }
        void setRepulsion(int v) { m_repulsion = v * m_natLength * m_natLength * m_natLength; }
        void setControlPointWeight(int v) { m_controlPointWeight = (float)v / 1000.0; }
        void setNaturalTransitionLength(int v) {
            m_repulsion /= m_natLength * m_natLength * m_natLength;
            m_natLength = v;
            m_repulsion *= m_natLength * m_natLength * m_natLength;
        }
        void apply();
        void setForceCalculation(ForceCalculation c);
        void setClipRegion(const Coord3D& min, const Coord3D& max);
        SpringLayoutUi* ui(QWidget* parent = 0);
    };

    class SpringLayoutUi : public QDockWidget
    {
        Q_OBJECT
    private:
        SpringLayout& m_layout;
        Ui::DockWidget* m_ui;
        QThread* m_thread;
    public:
        SpringLayoutUi(SpringLayout& layout, QWidget* parent=0);
        virtual ~SpringLayoutUi();
    signals:
        void runningChanged(bool);
    public slots:
        void onAttractionChanged(int value);
        void onRepulsionChanged(int value);
        void onSpeedChanged(int value);
        void onHandleWeightChanged(int value);
        void onNatLengthChanged(int value);
        void onForceCalculationChanged(int value);
        void onStartStop();
        void onStarted();
        void onStopped();
        void setActive(bool active);
    };
}

#endif // SPRINGLAYOUT_H
