// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include "mcrl2/utilities/logger.h"
#include <mcrl2/data/rewrite_strategy.h>
#include <mcrl2/pbes/io.h>
#include <mcrl2/pbes/parity_game_generator.h>
#include "ParityGame.h"


/* N.B. The PGSolver I/O functions reverse the priorities when reading/writing
   the game description. This is done to preserve solutions, since PGSolver
   considers higher values to dominate lower values, while I assume the opposite
   (i.e. 0 is the `highest` priority) throughout the rest of the code. */

void ParityGame::read_pgsolver( std::istream &is,
                                StaticGraph::EdgeDirection edge_dir )
{
    priority_t max_prio = 0;
    std::vector<ParityGameVertex> vertices;
    StaticGraph::edge_list edges;

    // Read "parity" header line (if present)
    char ch = 0;
    while (!isalnum(ch)) is.get(ch);
    is.putback(ch);
    if (!isdigit(ch))
    {
        std::string parity;
        verti max_vertex;

        if (!(is >> parity >> max_vertex)) return;
        if (parity != "parity") return;
        vertices.reserve(max_vertex + 1);

        // Skip to terminating semicolon
        while (is.get(ch) && ch != ';') ch = 0;
    }

    // Read and discard "start" line (if present)
    while (!isalnum(ch)) is.get(ch);
    is.putback(ch);
    if (!isdigit(ch))
    {
        std::string start;
        verti vertex;

        if (!(is >> start >> vertex)) return;
        if (start != "start") return;

        // Skip to terminating semicolon
        while (is.get(ch) && ch != ';') ch = 0;
    }

    // Invalid vertex (used to mark uninitialized vertices)
    ParityGameVertex invalid = { PLAYER_EVEN, (priority_t)-1 };

    // Read vertex specs
    while (is)
    {
        verti id;
        int prio_raw, player_raw;

        if (!(is >> id >> prio_raw >> player_raw)) break;

        assert(prio_raw >= 0);
        assert(prio_raw < 65536);
        priority_t prio = prio_raw;

        assert(player_raw == 0 || player_raw == 1);
        player_t player = static_cast<player_t>(player_raw);

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

            // Skip to separator (comma) or end-of-list (semicolon), while
            // ignoring the contents of quoted strings.
            bool quoted = false, escaped = false;
            while (is.get(ch)) {
                if (ch == '"' && !escaped) quoted = !quoted;
                escaped = ch == '\\' && !escaped;
                if ((ch == ',' || ch == ';') && !quoted) break;
            }

        } while (is && ch == ',');
    }

    // Ensure max_prio is even, so max_prio - p preserves parity:
    if (max_prio%2 == 1) ++max_prio;

    // Look for unused vertex indices:
    std::vector<verti> vertex_map(vertices.size(), NO_VERTEX);
    verti used = 0;
    for (verti v = 0; v < (verti)vertices.size(); ++v)
    {
        if (vertices[v] != invalid) {
            vertices[used] = vertices[v];
            vertex_map[v] = used++;
        }
    }
    if (used < (verti)vertices.size())
    {
        // Remove unused vertices:
        vertices.erase(vertices.begin() + used, vertices.end());

        // Remap edges to new vertex indices:
        for ( StaticGraph::edge_list::iterator it = edges.begin();
              it != edges.end(); ++it )
        {
            it->first  = vertex_map[it->first];
            it->second = vertex_map[it->second];
            assert(it->first != NO_VERTEX && it->second != NO_VERTEX);
        }
    }

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
    if (max_prio%2 == 1) --max_prio;

    // Write out graph
    os << "parity " << (long long)graph_.V() - 1 << ";\n";
    for (verti v = 0; v < graph_.V(); ++v)
    {
        os << v << ' ' << (max_prio - priority(v)) << ' ' << player(v);
        StaticGraph::const_iterator it  = graph_.succ_begin(v),
                                    end = graph_.succ_end(v);
        assert(it < end);
        os << ' ' << *it++;
        while (it != end) os << ',' << *it++;
        os << ";\n";
    }
}

void ParityGame::read_pbes( const std::string &file_path, verti *goal_vertex,
                            StaticGraph::EdgeDirection edge_dir,
                            const std::string &rewrite_strategy )
{
    mcrl2::pbes_system::pbes pbes;
    load_pbes(pbes, file_path);
    assign_pbes(pbes, goal_vertex, edge_dir, rewrite_strategy);
}

void ParityGame::assign_pbes(mcrl2::pbes_system::pbes &pbes, verti *goal_vertex,
                             StaticGraph::EdgeDirection edge_dir,
                             const std::string &rewrite_strategy)
{
    /* NOTE: this code assumes the vertices generated by parity_game_generator
             are numbered from 2 to num_vertices-1 with no gaps, with 0 and 1
             representing true and false (respectively) and 2 representing the
             initial condition. */

    if (goal_vertex) *goal_vertex = 2;

    // Generate min-priority parity game
    mcrl2::pbes_system::parity_game_generator pgg( pbes, true, true,
        mcrl2::data::parse_rewrite_strategy(rewrite_strategy) );

    // Build the edge list
    StaticGraph::edge_list edges;
    verti begin = 0, end = 3;
    for (verti v = begin; v < end; ++v)
    {
        std::set<size_t> deps = pgg.get_dependencies(v);
        for ( std::set<size_t>::const_iterator it = deps.begin();
                it != deps.end(); ++it )
        {
            verti w = (verti)*it;
            assert(w >= begin);
            if (w >= end) end = w + 1;
            edges.push_back(std::make_pair(v - begin, w - begin));
        }
    }

    // Determine maximum prioirity
    int max_prio = 0;
    for (verti v = begin; v < end; ++v)
    {
        max_prio = std::max(max_prio, (int)pgg.get_priority(v));
    }

    // Assign vertex info and recount cardinalities
    reset(end - begin, max_prio + 1);
    for (verti v = begin; v < end; ++v)
    {
        bool and_op = pgg.get_operation(v) ==
                        mcrl2::pbes_system::parity_game_generator::PGAME_AND;
        vertex_[v - begin].player = and_op ? PLAYER_ODD : PLAYER_EVEN;
        vertex_[v - begin].priority = pgg.get_priority(v);
    }
    recalculate_cardinalities(end - begin);

    // Assign graph
    graph_.assign(edges, edge_dir);
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
        bool even = player(v) == PLAYER_EVEN;
        os << v << " ["
           << "shape=" << (even ? "diamond" : "box") << ", "
           << "label=\"" << priority(v) << " (" << v << ")\"]\n";

        if (graph_.edge_dir() & StaticGraph::EDGE_SUCCESSOR)
        {
            for ( StaticGraph::const_iterator it = graph_.succ_begin(v);
                  it != graph_.succ_end(v); ++it )
            {
                os << v << " -> " << *it;
                // if (*it < v) os << " [color=red]";
                os << ";\n";
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

void ParityGame::write_debug(const Strategy &s, std::ostream &os) const
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

        // Print strategy (if applicable)
        if (!s.empty() && s.at(v) != NO_VERTEX) os << " -> " << s.at(v);

        os << '\n';
    }
    os << std::flush;
}
