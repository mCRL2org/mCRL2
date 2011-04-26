// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "ParityGame.h"
#include "Logger.h"
#include <assert.h>

/* N.B. The PGSolver I/O functions reverse the priorities when reading/writing
   the game description. This is done to preserve solutions, since PGSolver
   considers higher values to dominate lower values, while I assume the opposite
   (i.e. 0 is the `highest' priority) throughout the rest of the code. */

void ParityGame::read_pgsolver( std::istream &is,
                                StaticGraph::EdgeDirection edge_dir )
{
    // Read header line (if present)
    char ch = 0;
    is.get(ch);
    if (isdigit(ch))
    {
        // No header; put character back to parse later
        is.putback(ch);
    }
    else
    {
        // Skip to terminating semicolon
        while (is.get(ch) && ch != ';') ch = 0;
    }

    // Invalid vertex (used to mark uninitialized vertices)
    ParityGameVertex invalid = { (unsigned char)-1, (unsigned char)-1 };

    int max_prio = 0;
    std::vector<ParityGameVertex> vertices;
    StaticGraph::edge_list edges;

    // Read vertex specs
    while (is)
    {
        verti id;
        int prio, player;
        if (!(is >> id >> prio >> player)) break;

        assert(prio >= 0);
        assert(prio < 256);
        assert(player == 0 || player == 1);
        if (prio > max_prio) max_prio = prio;
        if (id >= vertices.size()) vertices.resize(id + 1, invalid);

        /* FIXME: the PGSolver file format description allows vertices to be
                  defined more than once (in that case, the old vertex should
                  be removed), but we currently don't support that. Instead,
                  just assert that each vertex is initialized once. */
        assert(vertices[id] == invalid);

        vertices[id].player   = player;
        vertices[id].priority = prio;

        // Read successors
        do {
            verti succ;
            if (!(is >> succ)) break;
            if (succ >= vertices.size()) vertices.resize(succ + 1, invalid);

            edges.push_back(std::make_pair(id, succ));

            // Skip to separator (comma) or end-of-list (semicolon)
            while (is.get(ch) && ch != ',' && ch != ';') ch = 0;

        } while (ch == ',');
    }

    // Ensure max_prio is even, so max_prio - p preserves parity:
    if (max_prio%2 == 1) ++max_prio;

    // Assign vertex info and recount cardinalities
    reset((verti)vertices.size(), max_prio + 1);
    for (size_t n = 0; n < vertices.size(); ++n)
    {
        assert(vertices[n] != invalid);
        vertex_[n].player   = vertices[n].player;
        vertex_[n].priority = max_prio - vertices[n].priority;
    }
    recalculate_cardinalities(vertices.size());
    vertices.clear();

    // Assign graph
    graph_.assign(edges, edge_dir);
}

void ParityGame::write_pgsolver(std::ostream &os) const
{
    // Get max priority and make it even so max_prio - p preserves parity:
    int max_prio = d();
    if (max_prio%2 == 1) ++max_prio;

    // Write out graph
    os << "parity " << graph_.V() - 1 << ";\n";
    for (verti v = 0; v < graph_.V(); ++v)
    {
        os << v << ' ' << (max_prio - priority(v)) << ' ' << player(v);
        StaticGraph::const_iterator it  = graph_.succ_begin(v),
                                    end = graph_.succ_end(v);
        assert(it != end);
        os << ' ' << *it++;
        while (it != end) os << ',' << *it++;
        os << ";\n";
    }
}

void ParityGame::read_pbes( const std::string &file_path, verti *goal_vertex,
                            StaticGraph::EdgeDirection edge_dir )
{
#ifdef WITH_MCRL2
    mcrl2::pbes_system::pbes<> pbes;
    pbes.load(file_path);  // TODO: handle exceptions raised here?
    assign_pbes(pbes, goal_vertex, edge_dir);
#else /* ifdef WITH_MCRL2 */
    (void)file_path;    // unused
    (void)edge_dir;     // unused
    (void)goal_vertex;  // unused
    Logger::fatal( "ParityGame::read_pbes() called, but "
                   "compiled without mCRL2 support!\n" );
#endif /* def WITH_MCRL2 */
}

void ParityGame::read_raw(std::istream &is)
{
    graph_.read_raw(is);
    assert(is.good());
    int d;
    is.read((char*)&d, sizeof(d));
    reset(graph_.V(), d);
    is.read((char*)vertex_, sizeof(ParityGameVertex)*graph_.V());
    is.read((char*)cardinality_, sizeof(verti)*d);
}

void ParityGame::write_raw(std::ostream &os) const
{
    graph_.write_raw(os);
    assert(os.good());
    os.write((const char*)&d_, sizeof(d_));
    os.write((const char*)vertex_, sizeof(ParityGameVertex)*graph_.V());
    os.write((const char*)cardinality_, sizeof(verti)*d_);
}

void ParityGame::write_dot(std::ostream &os) const
{
    os << "digraph {\n";
    for (verti v = 0; v < graph_.V(); ++v)
    {
        bool even = player(v) == ParityGame::PLAYER_EVEN;
        os << v << " ["
           << "shape=" << (even ? "diamond" : "box") << ", "
           << "label=\"" << priority(v) << " (" << v << ")\"]\n";

        if (graph_.edge_dir() & StaticGraph::EDGE_SUCCESSOR)
        {
            for ( StaticGraph::const_iterator it = graph_.succ_begin(v);
                  it != graph_.succ_end(v); ++it )
            {
                os << v << " -> " << *it << ";\n";
            }
        }
        else
        {
            for ( StaticGraph::const_iterator it = graph_.pred_begin(v);
                  it != graph_.pred_end(v); ++it )
            {
                os << *it << " -> " << v << ";\n";
            }
        }
    }
    os << "}\n";
}

void ParityGame::write_debug(std::ostream &os) const
{
    for (verti v = 0; v < graph_.V(); ++v)
    {
        os << v << ' ';

        // Print controlling player and vertex priority:
        char l = ' ', r = ' ';
        if (player(v) == PLAYER_EVEN) l = '<', r = '>';
        if (player(v) == PLAYER_ODD)  l = '[', r = ']';
        os << l << priority(v) << r;

        // Print outgoing edges:
        char sep = ' ';
        for (StaticGraph::const_iterator it = graph_.succ_begin(v);
             it != graph_.succ_end(v); ++it)
        {
            os << sep << *it;
            sep = ',';
        }
        os << '\n';
    }
    os << std::flush;
}
