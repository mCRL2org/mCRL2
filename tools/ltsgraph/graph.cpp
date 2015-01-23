// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <QDomDocument>
#include <QTextStream>
#include <QFile>
#include <QtOpenGL>

#include "graph.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/lts_fsm.h"

namespace Graph
{

  namespace detail
  {

    inline float frand(float min, float max)
    {
      return ((float)qrand() / RAND_MAX) * (max - min) + min;
    }

    class GraphImplBase
    {
      public:

        /**
         * @brief Constructor.
         */
        GraphImplBase() : initialState(0) {}

        /**
         * @brief Destructor.
         */
        virtual ~GraphImplBase() {}

        /**
         * @brief Loads a graph with random positioning for the nodes.
         * @param filename The file which contains the graph.
         * @param min The minimum coordinates for any node.
         * @param max The maximum coordinates for any node.
         */
        virtual void load(const QString& filename, const Coord3D& min, const Coord3D& max) = 0;

        /**
         * @brief Returns true if the label is tau.
         * @param labelindex The index of the label.
         */
        virtual bool is_tau(size_t labelindex) const = 0;

        std::vector<NodeNode> nodes;                  ///< Vector containing all graph nodes.
        std::vector<Node> handles;                    ///< Vector containing all handles.
        std::vector<Edge> edges;                      ///< Vector containing all edges.
        std::vector<LabelString> transitionLabels;    ///< Vector containing all transition label strings.
        std::vector<LabelNode> transitionLabelnodes;  ///< Vector containing all transition label nodes.
        std::vector<QString> stateLabels;             ///< Vector containing all state label strings.
        std::vector<LabelNode> stateLabelnodes;       ///< Vector containing all state label nodes.
        size_t initialState;                          ///< Index of the initial state.

        template <typename label_t>
        QString transitionLabel(const label_t& label)
        {
          return QString::fromStdString(label);
        }

        template <typename label_t>
        QString stateLabel(const label_t& label)
        {
          return QString::fromStdString(label);
        }
    };

    template <typename graph_t>
    class GraphImpl : public GraphImplBase
    {
      private:
        graph_t m_graph;
      public:
        virtual bool is_tau(size_t labelindex) const
        {
          return transitionLabels[labelindex].isTau;
        }
        virtual void load(const QString& filename, const Coord3D& min, const Coord3D& max)
        {
          // Remove old graph (if it wasn't deleted yet) and load new one
          m_graph.load(filename.toUtf8().constData());

          // Resize all auxiliary data vectors
          nodes.resize(m_graph.num_states());
          edges.resize(m_graph.num_transitions());
          handles.resize(m_graph.num_transitions());

          transitionLabels.resize(m_graph.num_action_labels());
          transitionLabelnodes.resize(m_graph.num_transitions());

          stateLabels.resize(m_graph.num_state_labels());
          stateLabelnodes.resize(m_graph.num_states());

          for (size_t i = 0; i < m_graph.num_state_labels(); ++i)
          {
            stateLabels[i] = stateLabel(m_graph.state_label(i));
          }

          // Position nodes randomly
          for (size_t i = 0; i < m_graph.num_states(); ++i)
          {
            nodes[i].pos = Coord3D(frand(min.x, max.x), frand(min.y, max.y), frand(min.z, max.z));
            nodes[i].locked = false;
            nodes[i].anchored = false;
            nodes[i].selected = 0.0;
            nodes[i].color[0] = 0.0;
            nodes[i].color[1] = 0.0;
            nodes[i].color[2] = 0.0;
            stateLabelnodes[i].pos = nodes[i].pos;
            stateLabelnodes[i].locked = false;
            stateLabelnodes[i].anchored = false;
            stateLabelnodes[i].selected = 0.0;
            stateLabelnodes[i].labelindex = i;
            stateLabelnodes[i].color[0] = 0.0;
            stateLabelnodes[i].color[1] = 0.0;
            stateLabelnodes[i].color[2] = 0.0;
          }

          // Store string representations of labels
          for (size_t i = 0; i < m_graph.num_action_labels(); ++i)
          {
            if (m_graph.is_tau(i))
              transitionLabels[i].label = QChar(0x03C4);
            else
              transitionLabels[i].label = transitionLabel(m_graph.action_label(i));
            transitionLabels[i].isTau = m_graph.is_tau(i);
          }

          // Assign and position edge handles, position edge labels
          for (size_t i = 0; i < m_graph.num_transitions(); ++i)
          {
            mcrl2::lts::transition& t = m_graph.get_transitions()[i];
            edges[i].from = t.from();
            edges[i].to = t.to();
            handles[i].pos = (nodes[t.from()].pos + nodes[t.to()].pos) / 2.0;
            handles[i].locked = false;
            handles[i].anchored = false;
            handles[i].selected = 0.0;
            transitionLabelnodes[i].pos = (nodes[t.from()].pos + nodes[t.to()].pos) / 2.0;
            transitionLabelnodes[i].locked = false;
            transitionLabelnodes[i].anchored = false;
            transitionLabelnodes[i].selected = 0.0;
            transitionLabelnodes[i].labelindex = t.label();
            transitionLabelnodes[i].color[0] = 0.0;
            transitionLabelnodes[i].color[1] = 0.0;
            transitionLabelnodes[i].color[2] = 0.0;
          }

          initialState = m_graph.initial_state();
        }
    };


    template <>
    QString GraphImplBase::transitionLabel<mcrl2::lts::detail::action_label_lts>(const mcrl2::lts::detail::action_label_lts& label)
    {
      return QString::fromStdString(mcrl2::lts::detail::pp(label));
    }
    template <>
    QString GraphImplBase::stateLabel<mcrl2::lts::detail::state_label_lts>(const mcrl2::lts::detail::state_label_lts& label)
    {
      return QString::fromStdString(mcrl2::lts::detail::pp(label));
    }
    template <>
    QString GraphImplBase::stateLabel<mcrl2::lts::detail::state_label_empty>(const mcrl2::lts::detail::state_label_empty& /*label*/)
    {
      return QString("");
    }
    template <>
    QString GraphImplBase::stateLabel<mcrl2::lts::detail::state_label_dot>(const mcrl2::lts::detail::state_label_dot& label)
    {
      return QString::fromStdString(mcrl2::lts::detail::pp(label));
    }
    template <>
    QString GraphImplBase::stateLabel<mcrl2::lts::detail::state_label_fsm>(const mcrl2::lts::detail::state_label_fsm& label)
    {
      return QString::fromStdString(mcrl2::lts::detail::pp(label));
    }

  }

  Graph::Graph()
  {
    m_type = mcrl2::lts::lts_lts;
    m_impl = new detail::GraphImpl<mcrl2::lts::lts_lts_t>;
    m_empty = QString("");
  }

  Graph::~Graph()
  {
    delete m_impl;
  }

  size_t Graph::edgeCount() const
  {
    return m_impl->edges.size();
  }

  size_t Graph::nodeCount() const
  {
    return m_impl->nodes.size();
  }

  size_t Graph::transitionLabelCount() const
  {
    return m_impl->transitionLabels.size();
  }

  size_t Graph::stateLabelCount() const
  {
    return m_impl->stateLabels.size();
  }

  size_t Graph::initialState() const
  {
    return m_impl->initialState;
  }

  bool Graph::isTau(size_t labelindex) const
  {
    return m_impl->is_tau(labelindex);
  }

  void Graph::createImpl(mcrl2::lts::lts_type itype)
  {
    switch (itype)
    {
      case mcrl2::lts::lts_aut:
        m_type = mcrl2::lts::lts_aut;
        m_impl = new detail::GraphImpl<mcrl2::lts::lts_aut_t>;
        break;
      case mcrl2::lts::lts_dot:
        m_type = mcrl2::lts::lts_dot;
        m_impl = new detail::GraphImpl<mcrl2::lts::lts_dot_t>;
        break;
      case mcrl2::lts::lts_fsm:
        m_type = mcrl2::lts::lts_fsm;
        m_impl = new detail::GraphImpl<mcrl2::lts::lts_fsm_t>;
        break;
      case mcrl2::lts::lts_lts:
      default:
        m_type = mcrl2::lts::lts_lts;
        m_impl = new detail::GraphImpl<mcrl2::lts::lts_lts_t>;
        break;
    }
  }

  void Graph::load(const QString &filename, const Coord3D& min, const Coord3D& max)
  {
    mcrl2::lts::lts_type guess = mcrl2::lts::detail::guess_format(filename.toUtf8().constData());
    delete m_impl;
    createImpl(guess);
    m_impl->load(filename, min, max);
  }

  void Graph::loadXML(const QString& filename)
  {
    QDomDocument xml;
    QFile file(filename);
    if(!file.open( QFile::ReadOnly ))
    {
      mCRL2log(mcrl2::log::error) << "Could not open XML file: " << filename.toStdString() << std::endl;
      return;
    }
    QString errorMsg;
    if(!xml.setContent(&file, false, &errorMsg))
    {
      file.close();
      mCRL2log(mcrl2::log::error) << "Could not parse XML file: " << errorMsg.toStdString() << std::endl;
      return;
    }
    file.close();

    QDomElement root = xml.documentElement();
    if(root.tagName() != "Graph")
    {
      mCRL2log(mcrl2::log::error) << "XML contains no valid graph" << std::endl;
      return;
    }

    delete m_impl;
    mcrl2::lts::lts_type itype = (mcrl2::lts::lts_type) root.attribute("type").toInt();
    createImpl(itype);

    m_impl->nodes.resize(root.attribute("states").toInt());
    m_impl->edges.resize(root.attribute("transitions").toInt());
    m_impl->handles.resize(root.attribute("transitions").toInt());

    m_impl->transitionLabels.resize(root.attribute("transitionlabels").toInt());
    m_impl->transitionLabelnodes.resize(root.attribute("transitions").toInt());

    m_impl->stateLabels.resize(root.attribute("statelabels").toInt());
    m_impl->stateLabelnodes.resize(root.attribute("states").toInt());

    QDomNode node = root.firstChild();
    while (!node.isNull()) {
      QDomElement e = node.toElement();

      if (e.tagName() == "StateLabel") {
        m_impl->stateLabels[e.attribute("value").toInt()] = e.attribute("label");
      }
      if (e.tagName() == "State") {
        NodeNode *n = &m_impl->nodes[e.attribute("value").toInt()];
        n->pos.x = e.attribute("x").toFloat();
        n->pos.y = e.attribute("y").toFloat();
        n->pos.z = e.attribute("z").toFloat();
        n->locked = e.attribute("locked").toInt();
        n->anchored = n->locked;
        n->selected = 0.0f;
        if (e.attribute("isInitial").toInt())
          m_impl->initialState = e.attribute("value").toInt();
        n->color[0] = e.attribute("red").toFloat();
        n->color[1] = e.attribute("green").toFloat();
        n->color[2] = e.attribute("blue").toFloat();
      }
      if (e.tagName() == "StateLabelNode") {
        LabelNode *n = &m_impl->stateLabelnodes[e.attribute("value").toInt()];
        n->labelindex = e.attribute("labelindex").toInt();
        n->pos.x = e.attribute("x").toFloat();
        n->pos.y = e.attribute("y").toFloat();
        n->pos.z = e.attribute("z").toFloat();
        n->locked = e.attribute("locked").toInt();
        n->anchored = n->locked;
        n->selected = 0.0f;
        n->color[0] = e.attribute("red").toFloat();
        n->color[1] = e.attribute("green").toFloat();
        n->color[2] = e.attribute("blue").toFloat();
      }

      if (e.tagName() == "TransitionLabel") {
        LabelString *n = &m_impl->transitionLabels[e.attribute("value").toInt()];
        n->label = e.attribute("label");
        n->isTau = e.attribute("isTau").toInt();
      }
      if (e.tagName() == "Transition") {
        Edge *n = &m_impl->edges[e.attribute("value").toInt()];
        Node *h = &m_impl->handles[e.attribute("value").toInt()];
        n->from = e.attribute("from").toInt();
        n->to = e.attribute("to").toInt();
        h->pos.x = e.attribute("x").toFloat();
        h->pos.y = e.attribute("y").toFloat();
        h->pos.z = e.attribute("z").toFloat();
        h->locked = e.attribute("locked").toInt();
        h->anchored = h->locked;
        h->selected = 0.0f;
      }
      if (e.tagName() == "TransitionLabelNode") {
        LabelNode *n = &m_impl->transitionLabelnodes[e.attribute("value").toInt()];
        n->labelindex = e.attribute("labelindex").toInt();
        n->pos.x = e.attribute("x").toFloat();
        n->pos.y = e.attribute("y").toFloat();
        n->pos.z = e.attribute("z").toFloat();
        n->locked = e.attribute("locked").toInt();
        n->anchored = n->locked;
        n->selected = 0.0f;
        n->color[0] = e.attribute("red").toFloat();
        n->color[1] = e.attribute("green").toFloat();
        n->color[2] = e.attribute("blue").toFloat();
      }

      node = node.nextSibling();
    }

  }

  void Graph::saveXML(const QString& filename)
  {
    QDomDocument xml;
    QDomElement root = xml.createElement("Graph");
    root.setAttribute("type", (int)m_type);
    root.setAttribute("states", (int)nodeCount());
    root.setAttribute("transitions", (int)edgeCount());
    root.setAttribute("statelabels", (int)stateLabelCount());
    root.setAttribute("transitionlabels", (int)transitionLabelCount());
    xml.appendChild(root);

    for (size_t i = 0; i < stateLabelCount(); ++i)
    {
      QDomElement stateL = xml.createElement("StateLabel");
      stateL.setAttribute("value", (int)i);
      stateL.setAttribute("label", stateLabelstring(i));
      root.appendChild(stateL);
    }

    for (size_t i = 0; i < nodeCount(); ++i)
    {
      QDomElement state = xml.createElement("State");
      state.setAttribute("value", (int)i);
      state.setAttribute("x", node(i).pos.x);
      state.setAttribute("y", node(i).pos.y);
      state.setAttribute("z", node(i).pos.z);
      state.setAttribute("locked", node(i).locked);
      state.setAttribute("isInitial", (int)(i == initialState()));
      state.setAttribute("red", node(i).color[0]);
      state.setAttribute("green", node(i).color[1]);
      state.setAttribute("blue", node(i).color[2]);
      root.appendChild(state);

      QDomElement stateL = xml.createElement("StateLabelNode");
      stateL.setAttribute("value", (int)i);
      stateL.setAttribute("labelindex", (int)stateLabel(i).labelindex);
      stateL.setAttribute("x", stateLabel(i).pos.x);
      stateL.setAttribute("y", stateLabel(i).pos.y);
      stateL.setAttribute("z", stateLabel(i).pos.z);
      stateL.setAttribute("locked", stateLabel(i).locked);
      stateL.setAttribute("red", stateLabel(i).color[0]);
      stateL.setAttribute("green", stateLabel(i).color[1]);
      stateL.setAttribute("blue", stateLabel(i).color[2]);
      root.appendChild(stateL);
    }

    for (size_t i = 0; i < transitionLabelCount(); ++i)
    {
      QDomElement edgL = xml.createElement("TransitionLabel");
      edgL.setAttribute("value", (int)i);
      edgL.setAttribute("label", transitionLabelstring(i));
      root.appendChild(edgL);
    }

    for (size_t i = 0; i < edgeCount(); ++i)
    {
      QDomElement edg = xml.createElement("Transition");
      edg.setAttribute("value", (int)i);
      edg.setAttribute("from", (int)edge(i).from);
      edg.setAttribute("to", (int)edge(i).to);
      edg.setAttribute("x", handle(i).pos.x);
      edg.setAttribute("y", handle(i).pos.y);
      edg.setAttribute("z", handle(i).pos.z);
      edg.setAttribute("locked", handle(i).locked);
      root.appendChild(edg);

      QDomElement edgL = xml.createElement("TransitionLabelNode");
      edgL.setAttribute("value", (int)i);
      edgL.setAttribute("labelindex", (int)transitionLabel(i).labelindex);
      edgL.setAttribute("x", transitionLabel(i).pos.x);
      edgL.setAttribute("y", transitionLabel(i).pos.y);
      edgL.setAttribute("z", transitionLabel(i).pos.z);
      edgL.setAttribute("locked", transitionLabel(i).locked);
      edgL.setAttribute("red", transitionLabel(i).color[0]);
      edgL.setAttribute("green", transitionLabel(i).color[1]);
      edgL.setAttribute("blue", transitionLabel(i).color[2]);
      root.appendChild(edgL);
    }

    QFile data(filename);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);
        xml.save(out, 2);
    }
  }

  Edge Graph::edge(size_t index) const
  {
    return m_impl->edges[index];
  }

  NodeNode& Graph::node(size_t index) const
  {
    return m_impl->nodes[index];
  }

  Node& Graph::handle(size_t edge) const
  {
    return m_impl->handles[edge];
  }

  LabelNode& Graph::transitionLabel(size_t edge) const
  {
    return m_impl->transitionLabelnodes[edge];
  }

  LabelNode& Graph::stateLabel(size_t edge) const
  {
    return m_impl->stateLabelnodes[edge];
  }

  const QString& Graph::transitionLabelstring(size_t labelindex) const
  {
    if (labelindex >= m_impl->transitionLabels.size())
      return m_empty;
    return m_impl->transitionLabels[labelindex].label;
  }

  const QString& Graph::stateLabelstring(size_t labelindex) const
  {
    if (labelindex >= m_impl->stateLabels.size())
      return m_empty;
    return m_impl->stateLabels[labelindex];
  }

  void Graph::clip(const Coord3D& min, const Coord3D& max)
  {
    for (std::vector<NodeNode>::iterator it = m_impl->nodes.begin(); it != m_impl->nodes.end(); ++it)
      it->pos.clip(min, max);
    for (std::vector<LabelNode>::iterator it = m_impl->transitionLabelnodes.begin(); it != m_impl->transitionLabelnodes.end(); ++it)
      it->pos.clip(min, max);
    for (std::vector<Node>::iterator it = m_impl->handles.begin(); it != m_impl->handles.end(); ++it)
      it->pos.clip(min, max);
  }

}
