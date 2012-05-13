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

    struct EdgeExtra {
        float selected;
    };

    class GraphImplBase
    {
      public:
        virtual std::vector<mcrl2::lts::transition>& transitions() = 0;
        virtual void load(const QString& filename, const Coord3D& min, const Coord3D& max) = 0;
        virtual bool is_tau(size_t labelindex) const = 0;
        virtual ~GraphImplBase() {}
        std::vector<NodeNode> nodes;
        std::vector<LabelNode> labelnodes;
        std::vector<Node> handles;
        std::vector<EdgeExtra> edges;
        std::vector<QString> labels;
        template <typename label_t>
        QString label(const label_t& label)
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
        virtual std::vector<mcrl2::lts::transition>& transitions()
        {
          return m_graph.get_transitions();
        }
        virtual bool is_tau(size_t labelindex) const
        {
          return m_graph.is_tau(labelindex);
        }
        virtual void load(const QString& filename, const Coord3D& min, const Coord3D& max)
        {
          qDebug() << "Loading with m_impl 1";
          // Remove old graph (if it wasn't deleted yet) and load new one
          m_graph.load(filename.toUtf8().constData());

          qDebug() << "Loading with m_impl 2";
          // Resize all auxiliary data vectors
          labels.resize(m_graph.num_action_labels());
          nodes.resize(m_graph.num_states());
          edges.resize(m_graph.num_transitions());
          handles.resize(m_graph.num_transitions());
          labelnodes.resize(m_graph.num_transitions());

          qDebug() << "Loading with m_impl 3";
          // Store string representations of labels
          for (size_t i = 0; i < m_graph.num_action_labels(); ++i)
          {
            if (m_graph.is_tau(i))
              labels[i] = QChar(0x03C4);
            else
              labels[i] = label(m_graph.action_label(i));
          }

          qDebug() << "Loading with m_impl 4";
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
          }

          qDebug() << "Loading with m_impl 5";
          // Assign and position edge handles, position edge labels
          for (size_t i = 0; i < m_graph.num_transitions(); ++i)
          {
            mcrl2::lts::transition& t = m_graph.get_transitions()[i];
            handles[i].pos = (nodes[t.from()].pos + nodes[t.to()].pos) / 2.0;
            handles[i].locked = false;
            handles[i].anchored = false;
            handles[i].selected = 0.0;
            labelnodes[i].pos = (nodes[t.from()].pos + nodes[t.to()].pos) / 2.0;
            labelnodes[i].locked = false;
            labelnodes[i].anchored = false;
            labelnodes[i].selected = 0.0;
            labelnodes[i].labelindex = t.label();
          }
        }
    };


    template <>
    QString GraphImplBase::label<mcrl2::lts::detail::action_label_lts>(const mcrl2::lts::detail::action_label_lts& label)
    {
      return QString::fromStdString(mcrl2::lts::detail::pp(label));
    }


  }

  Graph::Graph()
  {
    m_impl = new detail::GraphImpl<mcrl2::lts::lts_lts_t>;
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

  size_t Graph::labelCount() const
  {
    return m_impl->labels.size();
  }

  bool Graph::isTau(size_t labelindex) const
  {
    return m_impl->is_tau(labelindex);
  }

  void Graph::load(const QString &filename, const Coord3D& min, const Coord3D& max)
  {
    qDebug() << "Guessing Format";
    mcrl2::lts::lts_type guess = mcrl2::lts::detail::guess_format(filename.toUtf8().constData());
    delete m_impl;
    switch (guess)
    {
      case mcrl2::lts::lts_aut:
        m_impl = new detail::GraphImpl<mcrl2::lts::lts_aut_t>;
        break;
      case mcrl2::lts::lts_dot:
        m_impl = new detail::GraphImpl<mcrl2::lts::lts_dot_t>;
        break;
      case mcrl2::lts::lts_fsm:
        m_impl = new detail::GraphImpl<mcrl2::lts::lts_fsm_t>;
        break;
      case mcrl2::lts::lts_lts:
      default:
        m_impl = new detail::GraphImpl<mcrl2::lts::lts_lts_t>;
        break;
    }
    qDebug() << "Loading with m_impl";
    m_impl->load(filename, min, max);
    qDebug() << "Loaded with m_impl";
  }

  Edge Graph::edge(size_t index) const
  {
    mcrl2::lts::transition& t = m_impl->transitions()[index];
    detail::EdgeExtra& e = m_impl->edges[index];
    return Edge(t.from(), t.to(), e.selected);
  }

  NodeNode& Graph::node(size_t index) const
  {
    return m_impl->nodes[index];
  }

  Node& Graph::handle(size_t edge) const
  {
    return m_impl->handles[edge];
  }

  LabelNode& Graph::label(size_t edge) const
  {
    return m_impl->labelnodes[edge];
  }

  const QString& Graph::labelstring(size_t labelindex) const
  {
    return m_impl->labels[labelindex];
  }

  void Graph::selectEdge(size_t index, float amount)
  {
    m_impl->edges[index].selected = amount;
  }

  void Graph::clip(const Coord3D& min, const Coord3D& max)
  {
    for (std::vector<NodeNode>::iterator it = m_impl->nodes.begin(); it != m_impl->nodes.end(); ++it)
      it->pos.clip(min, max);
    for (std::vector<LabelNode>::iterator it = m_impl->labelnodes.begin(); it != m_impl->labelnodes.end(); ++it)
      it->pos.clip(min, max);
    for (std::vector<Node>::iterator it = m_impl->handles.begin(); it != m_impl->handles.end(); ++it)
      it->pos.clip(min, max);
  }

}
