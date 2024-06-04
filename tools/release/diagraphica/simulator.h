// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./simulator.h

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "diagram.h"
#include "settings.h"

class Simulator : public Visualizer
{
  Q_OBJECT

  public:
    // -- constructors and destructor -------------------------------
    Simulator(
      QWidget *parent,
      Settings* s,
      Graph* g);
    virtual ~Simulator();


    static QColor SelectColor() { return VisUtils::coolGreen; }
    std::size_t SelectedClusterIndex();

    void setDiagram(Diagram* dgrm);


    void initFrameCurr(
      Cluster* frame,
      const std::vector< Attribute* > &attrs);
    void updateFrameCurr(
      Cluster* frame,
      const Position2D& pos);

    // -- visualization functions  ----------------------------------
    void visualize() override;
    void mark() override;

    // -- event handlers --------------------------------------------
    void handleMouseEvent(QMouseEvent* e) override;
    void handleMouseLeaveEvent() override;
    void handleKeyEvent(QKeyEvent* e) override;

    QSize sizeHint() const override { return QSize(600,200); }

  public slots:

    // -- utility event handlers ------------------------------------
    void onTimer();
    void reset() { initFrameCurr(0, std::vector< Attribute* >()); }

  signals:
    void routingCluster(Cluster *cluster, QList<Cluster *> clusterSet, QList<Attribute *> attributes);
    void hoverCluster(Cluster *cluster, QList<Attribute *> attributes = QList<Attribute *>());

  private:
    // -- utility functions -----------------------------------------
    void initFramesPrevNext();
    void initBundles();
    void sortFramesPrevNext();

    void calcSettingsGeomBased();
    void calcSettingsDataBased();
    void calcIntervals();
    void calcPositions();
    void calcPosFrames();
    void calcPosBundles();

    void handleKeyUp();
    void handleKeyRgt();
    void handleKeyDwn();
    void handleKeyLft();
    void markFrameClusts();

    void clearAttributes();
    void clearDiagram();
    void clearFrames();
    void clearBundles();

    // -- hit detection ---------------------------------------------
    void handleHits(const std::vector< int > &ids);
    virtual void handleSelection(const Selection&) override;

    // -- utility drawing functions ---------------------------------
    void clear() override;

    QColor calcColor(std::size_t iter, std::size_t numr);

    template <Mode> void drawFrameCurr();
    template <Mode> void drawFramesPrev();
    template <Mode> void drawFramesNext();
    template <Mode> void drawBdlLblGridPrev();
    template <Mode> void drawBdlLblGridNext();
    template <Mode> void drawBundlesPrev();
    template <Mode> void drawBundlesNext();
    template <Mode> void drawControls();
    template <Mode> void draw();
    void animate();

    enum
    {
      ID_CANVAS,
      ID_ICON_CLEAR,
      ID_ICON_UP,
      ID_ICON_NEXT,
      ID_ICON_DOWN,
      ID_ICON_PREV,
      ID_FRAME_CURR,
      ID_FRAME_PREV,
      ID_FRAME_NEXT,
      ID_BUNDLE_PREV,
      ID_BUNDLE_NEXT,
      ID_BUNDLE_LBL,
      ID_DIAGRAM_MORE,
      ANIM_NONE,
      ANIM_POS,
      ANIM_BLEND
    };

    // -- data members ----------------------------------------------
    Diagram* m_diagram;                      // Diagram used for each frame
    std::vector< Attribute* > m_attributes;  // Attributes for the frames
    Settings* m_settings;

    Cluster* m_currentFrame;            // composition
    std::vector< Cluster* > m_previousFrames; // composition
    std::vector< Cluster* > m_nextFrames; // composition
    std::vector< Bundle* >  m_bundles;    // composition

    std::vector< Bundle* >  m_bundlesByLabel;
    std::vector< Bundle* >  m_bundlesPreviousByLabel;
    std::vector< Bundle* >  m_bundlesNextByLabel;

    double m_horizontalFrameScale;
    double m_verticalFrameScale;

    int m_currentSelection;
    int m_currentSelectionIndex;
    int m_lastSelection;
    int m_lastSelectionIndexPrevious;
    int m_lastSelectionIndexNext;

    std::size_t m_previousBundleFocusIndex;
    std::size_t m_nextBundleFocusIndex;

    Position2D m_currentFramePosition;
    std::vector< Position2D > m_previousFramePositions;
    std::vector< Position2D > m_nextFramePositions;

    std::vector< Position2D > m_previousBundleLabelPositionTL;
    std::vector< Position2D > m_previousBundleLabelPositionBR;
    std::vector< Position2D > m_nextBundleLabelPositionTL;
    std::vector< Position2D > m_nextBundleLabelPositionBR;

    std::vector< std::vector< Position2D > > m_previousBundlePositionTL;
    std::vector< std::vector< Position2D > > m_previousBundlePositionBR;
    std::vector< std::vector< Position2D > > m_nextBundlePositionTL;
    std::vector< std::vector< Position2D > > m_nextBundlePositionBR;

    // animation
    double m_totalAnimationTime;
    double m_totalBlendTime;
    int m_currentAnimationPhase;

    QTimer m_animationTimer;

    Cluster* m_animationOldFrame;
    Cluster* m_animationNewFrame;
    Position2D m_animationStartPosition;
    Position2D m_animationEndPosition;
    Position2D m_animationCurrentPosition;

    double m_animationOldFrameOpacity;
    double m_animationNewFrameOpacity;
};

#endif

// -- end -----------------------------------------------------------
