// Author(s): Michael Weber, Maks Verver, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pgsolver.cpp

#include "boost.hpp" // precompiled headers

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <memory>

#include "mcrl2/utilities/input_tool.h"
//#include "mcrl2/utilities/rewriter_tool.h"
//#include "mcrl2/utilities/pbes_rewriter_tool.h"
#include "ParityGame.h"
#include "logging.h"
#include "timing.h"
#include "SCC.h"
#include "ParityGame.h"
#include "LinearLiftingStrategy.h"
#include "PredecessorLiftingStrategy.h"
#include "mcrl2/pbes/parity_game_generator.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using utilities::tools::input_tool;
//using utilities::tools::rewriter_tool;
//using utilities::tools::pbes_rewriter_tool;

//-------------------------------------------------------------------------------------------------------------------//
// main.cc
//-------------------------------------------------------------------------------------------------------------------//
enum InputFormat {
    INPUT_NONE = 0, INPUT_RAW, INPUT_RANDOM, INPUT_PGSOLVER, INPUT_PBES
};

enum Reordering { REORDER_NONE = 0, REORDER_BFS, REORDER_DFS };

static InputFormat  arg_input_format          = INPUT_NONE;
static std::string  arg_dot_file              = "";
static std::string  arg_pgsolver_file         = "";
static std::string  arg_raw_file              = "";
static std::string  arg_winners_file          = "";
static std::string  arg_spm_lifting_strategy  = "";
static bool         arg_scc_decomposition     = false;
static bool         arg_solve_dual            = false;
static Reordering   arg_reordering            = REORDER_NONE;
static int          arg_random_size           = 1000000;
static int          arg_random_seed           =       1;
static int          arg_random_out_degree     =      10;
static int          arg_random_priorities     =      20;

static const double MB = 1048576.0;  // one megabyte

struct MStat {
    int size, resident, share, text, lib, data, dt;
};

static bool read_mstat(MStat &stat)
{
    std::ifstream ifs("/proc/self/statm");
    return ifs >> stat.size >> stat.resident >> stat.share >> stat.text
               >> stat.lib >> stat.data >> stat.dt;
}

//static double get_vmsize()
//{
//    MStat mstat;
//    if (!read_mstat(mstat)) return -1;
//    return mstat.size*(getpagesize()/MB);
//}

static void print_usage()
{
    printf(
"Options:\n"
"  --help/-h              show help\n"
"  --input/-i <format>    input format: random, raw, PGSolver or PBES\n"
"  --size <int>           size of randomly generated graph\n"
"  --outdegree <int>      average out-degree in randomly generated graph\n"
"  --priorities <int>     number of priorities in randomly generated game\n"
"  --seed <int>           random seed\n"
"  --strategy/-l <desc>   Small Progress Measures lifting strategy\n"
"  --dot/-d <file>        write parity game in GraphViz dot format to <file>\n"
"  --pgsolver/-p <file>   write parity game in PGSolver format to <file>\n"
"  --raw/-r <file>        write parity game in raw format to <file>\n"
"  --winners/-w <file>    write compact winners specification to <file>\n"
"  --scc                  solve strongly connected components individually\n"
"  --dual                 solve the dual game\n"
"  --reorder/-e (bfs|dfs) reorder vertices\n");
}

//static void parse_args(int argc, char *argv[])
//{
//    static struct option long_options[] = {
//        { "help",       false, NULL, 'h' },
//        { "input",      true,  NULL, 'i' },
//        { "size",       true,  NULL,  1  },
//        { "outdegree",  true,  NULL,  2  },
//        { "priorities", true,  NULL,  3  },
//        { "seed",       true,  NULL,  4  },
//        { "strategy",   true,  NULL, 'l' },
//        { "dot",        true,  NULL, 'd' },
//        { "pgsolver",   true,  NULL, 'p' },
//        { "raw",        true,  NULL, 'r' },
//        { "winners",    true,  NULL, 'w' },
//        { "scc",        false, NULL,  5  },
//        { "dual",       false, NULL,  6  },
//        { "reorder",    true,  NULL, 'e' },
//        { NULL,         false, NULL,  0  } };
//
//    static const char *short_options = "hi:l:d:p:r:w:e:";
//
//    for (;;)
//    {
//        int ch = getopt_long(argc, argv, short_options, long_options, NULL);
//        if (ch == -1) break;
//
//        switch (ch)
//        {
//        case 'h':   /* help */
//            print_usage();
//            exit(0);
//            break;
//
//        case 'i':   /* input format */
//            if (strcasecmp(optarg, "random") == 0)
//            {
//                arg_input_format = INPUT_RANDOM;
//            }
//            else
//            if (strcasecmp(optarg, "raw") == 0)
//            {
//                arg_input_format = INPUT_RAW;
//            }
//            else
//            if (strcasecmp(optarg, "pgsolver") == 0)
//            {
//                arg_input_format = INPUT_PGSOLVER;
//            }
//            else
//            if (strcasecmp(optarg, "pbes") == 0)
//            {
//#ifdef WITH_MCRL2
//                arg_input_format = INPUT_PBES;
//#else
//                printf("PBES input requires linking to mCRL2\n");
//                exit(1);
//#endif
//            }
//            else
//            {
//                printf("Invalid input format: %s\n", optarg);
//                exit(1);
//            }
//            break;
//
//        case 1:     /* random graph size */
//            arg_random_size = atoi(optarg);
//            break;
//
//        case 2:     /* random graph out-degree */
//            arg_random_out_degree = atoi(optarg);
//            break;
//
//        case 3:     /* random game number of priorities */
//            arg_random_priorities = atoi(optarg);
//            break;
//
//        case 4:     /* random seed */
//            arg_random_seed = atoi(optarg);
//            break;
//
//        case 'l':   /* Small Progress Measures lifting strategy */
//            arg_spm_lifting_strategy = optarg;
//            break;
//
//        case 'd':   /* dot output file */
//            arg_dot_file = optarg;
//            break;
//
//        case 'p':   /* PGSolver output file */
//            arg_pgsolver_file = optarg;
//            break;
//
//        case 'r':   /* raw output file */
//            arg_raw_file = optarg;
//            break;
//
//        case 'w':   /* winners output file */
//            arg_winners_file = optarg;
//            break;
//
//        case 5:     /* decompose into strongly connected components */
//            arg_scc_decomposition = true;
//            break;
//
//        case 6:     /* solve dual game */
//            arg_solve_dual = true;
//            break;
//
//        case 'e':
//            if (strcasecmp(optarg, "bfs") == 0)
//            {
//                arg_reordering = REORDER_BFS;
//            }
//            else
//            if (strcasecmp(optarg, "dfs") == 0)
//            {
//                arg_reordering = REORDER_DFS;
//            }
//            else
//            {
//                printf("Invalid reordering: %s\n", optarg);
//                exit(1);
//            }
//            break;
//
//        case '?':
//            {
//                printf("Unrecognized option!\n");
//                exit(1);
//            }
//            break;
//        }
//    }
//
//    if (arg_input_format == INPUT_NONE)
//    {
//        printf("No input format specified!\n");
//        print_usage();
//        exit(0);
//    }
//}

/*! Write summary of winners. For each node, a single character is printed:
    'E' or 'O', depending on whether player Even or Odd wins the parity game
    starting from this node. */
static void write_winners(std::ostream &os, const ParityGameSolver &solver)
{
    verti next_newline = 80;
    for (verti v = 0; v < solver.game().graph().V(); ++v)
    {
        if (v == next_newline)
        {
            os << '\n';
            next_newline += 80;
        }
        ParityGame::Player winner = solver.winner(v);
        os << ( (winner == ParityGame::PLAYER_EVEN) ^ arg_solve_dual ? 'E' :
                (winner == ParityGame::PLAYER_ODD)  ^ arg_solve_dual ? 'O' :
                                                                       '?' );
    }
    os << '\n';
}

bool read_input(ParityGame &game)
{
    switch (arg_input_format)
    {
    case INPUT_RANDOM:
        info( "Generating random parity game with %d vertices, "
                "out-degree %d, and %d priorities...", arg_random_size,
                arg_random_out_degree, arg_random_priorities );
        srand(arg_random_seed);

        game.make_random(
            arg_random_size, arg_random_out_degree,
            StaticGraph::EDGE_BIDIRECTIONAL, arg_random_priorities );

        return true;

    case INPUT_RAW:
        info("Reading raw input...");
        game.read_raw(std::cin);
        return true;

    case INPUT_PGSOLVER:
        info("Reading PGSolver input...");
        game.read_pgsolver(std::cin, StaticGraph::EDGE_BIDIRECTIONAL);
        return true;

    case INPUT_PBES:
        info("Generating parity game from PBES input....");
        game.read_pbes("", StaticGraph::EDGE_BIDIRECTIONAL);
        return true;

    case INPUT_NONE:
        return false;
    }

    return false;
}

void write_output(const ParityGame &game, const ParityGameSolver *solver)
{
    /* Write dot file */
    if (!arg_dot_file.empty())
    {
        if (arg_dot_file == "-")
        {
            game.write_dot(std::cout);
            if (!std::cout) error("Writing failed!");
        }
        else
        {
            info("Writing GraphViz dot game description to file %s...",
                arg_dot_file.c_str());
            std::ofstream ofs(arg_dot_file.c_str());
            game.write_dot(ofs);
            if (!ofs) error("Writing failed!");
        }
    }

    /* Write PGSolver file */
    if (!arg_pgsolver_file.empty())
    {
        if (arg_pgsolver_file == "-")
        {
            game.write_pgsolver(std::cout);
        }
        else
        {
            info( "Writing PGSolver game description to file %s...",
                  arg_pgsolver_file.c_str() );
            std::ofstream ofs(arg_pgsolver_file.c_str());
            game.write_pgsolver(ofs);
            if (!ofs) error("Writing failed!");
        }
    }

    /* Write raw parity game file */
    if (!arg_raw_file.empty())
    {
        if (arg_raw_file == "-")
        {
            game.write_raw(std::cout);
        }
        else
        {
            info( "Writing raw game description to file %s...",
                  arg_raw_file.c_str() );
            std::ofstream ofs(arg_raw_file.c_str());
            game.write_raw(ofs);
            if (!ofs) error("Writing failed!");
        }
    }

    /* Write winners file */
    if (!arg_winners_file.empty() && solver != NULL)
    {
        if (arg_winners_file == "-")
        {
            write_winners(std::cout, *solver);
            if (!std::cout) error("Writing failed!");
        }
        else
        {
            info("Writing winners to file %s...", arg_winners_file.c_str());
            std::ofstream ofs(arg_winners_file.c_str());
            write_winners(ofs, *solver);
            if (!ofs) error("Writing failed!");
        }
    }
}

/*! A solver that breaks down the game graph into strongly connected components,
    and uses the SPM algorithm to solve independent subgames. */
class ComponentSolver : public ParityGameSolver
{
public:
    ComponentSolver(const ParityGame &game, LiftingStatistics *stats);
    ~ComponentSolver();

    bool solve();
    ParityGame::Player winner(verti v) const { return winners_[v]; }
    const ParityGame &game() const { return game_; }
    size_t memory_use() const { return memory_used_; }

private:
    // SCC callback
    int operator()(const verti *vertices, size_t num_vertices);
    friend class SCC<ComponentSolver>;

protected:
    std::vector<ParityGame::Player> winners_;
    LiftingStatistics *stats_;
    size_t memory_used_;
};

ComponentSolver::ComponentSolver( const ParityGame &game,
                                  LiftingStatistics *stats )
    : ParityGameSolver(game),
      winners_(game.graph().V(), ParityGame::PLAYER_NONE),
      stats_(stats), memory_used_(0)
{
}

ComponentSolver::~ComponentSolver()
{
}

bool ComponentSolver::solve()
{
    return decompose_graph(game_.graph(), *this) == 0;
}

int ComponentSolver::operator()(const verti *vertices, size_t num_vertices)
{
    info("Constructing subgame with %d vertices...", (int)num_vertices);

    // Construct a subgame
    ParityGame subgame;
    subgame.make_subgame(game_, vertices, num_vertices, &winners_[0]);

    // Compress vertex priorities
    int old_d = subgame.d();
    subgame.compress_priorities();
    info( "Priority compression removed %d of %d priorities.",
          old_d - subgame.d(), old_d );

    // Solve the subgame
    info("Solving subgame...", (int)num_vertices);
    std::auto_ptr<LiftingStrategy> spm_strategy(
        LiftingStrategy::create(subgame, arg_spm_lifting_strategy) );
    assert(spm_strategy.get() != NULL);

    /* FIXME:  we mess up the vertex statistics here (since vertex indices are
               reordered); instead, we should use a new statistics object and
               then map the results back into the main statistics. */
    SmallProgressMeasures spm(subgame, *spm_strategy, stats_);
    if (!spm.solve())
    {
        error("Solving failed!\n");
        return 1;
    }

    // Copy winners from subgame
    for (size_t n = 0; n < num_vertices; ++n)
    {
        winners_[vertices[n]] = spm.winner(n);
    }

    // Update (peak) memory use
    size_t mem = subgame.memory_use() + spm.memory_use();
    if (mem > memory_used_) memory_used_ = mem;

    return 0;
}

edgei count_forward_edges(const StaticGraph &g)
{
    edgei res = 0;
    for (verti v = 0; v < g.V(); ++v)
    {
        for ( StaticGraph::const_iterator it = g.succ_begin(v);
              it != g.succ_end(v); ++it )
        {
            if (*it > v) ++res;
        }
    }
    return res;
}

/*! Traverses the graph in breadth-first search order, and returns the result
    in `perm', such that perm[v] = i if v is the i-th visited vertex. */
void get_bfs_order(const StaticGraph &graph, std::vector<verti> &perm)
{
    assert(perm.empty());
    perm.resize(graph.V(), (verti)-1);

    std::queue<verti> queue;
    verti new_v = 0;
    for (verti root = 0; root < graph.V(); ++root)
    {
        if (perm[root] != (verti)-1) continue;
        perm[root] = new_v++;
        queue.push(root);
        while (!queue.empty())
        {
            verti v = queue.front();
            queue.pop();
            StaticGraph::const_iterator it = graph.succ_begin(v);
            while (it != graph.succ_end(v))
            {
                verti w = *it++;
                if (perm[w] == (verti)-1)
                {
                    perm[w] = new_v++;
                    queue.push(w);
                }
            }
        }
    }
    assert(new_v == graph.V());
}

/*! Traverses the graph in depth-first search order, and returns the result
    in `perm', such that perm[v] = i if v is the i-th visited vertex. */
void get_dfs_order(const StaticGraph &graph, std::vector<verti> &perm)
{
    assert(perm.empty());
    perm.resize(graph.V(), (verti)-1);

    std::stack<std::pair<verti, StaticGraph::const_iterator> > stack;
    verti new_v = 0;
    for (verti root = 0; root < graph.V(); ++root)
    {
        if (perm[root] != (verti)-1) continue;
        perm[root] = new_v++;
        stack.push(std::make_pair(root, graph.succ_begin(root)));
        while (!stack.empty())
        {
            verti v = stack.top().first;
            StaticGraph::const_iterator &it = stack.top().second;
            if (it == graph.succ_end(v))
            {
                stack.pop();
            }
            else
            {
                verti w = *it++;
                if (perm[w] == (verti)-1)
                {
                    perm[w] = new_v++;
                    stack.push(std::make_pair(w, graph.succ_begin(w)));
                }
            }
        }
    }
    assert(new_v == graph.V());
}
//-------------------------------------------------------------------------------------------------------------------//

// class pg_solver_tool: public pbes_rewriter_tool<rewriter_tool<input_tool> >
// TODO: extend the tool with rewriter options
class pg_solver_tool: public input_tool
{
  protected:
    typedef input_tool super;

    StaticGraph::EdgeDirection m_edge_direction;

    /// \return A string representation of the edge direction.
    std::string edge_direction_string() const
    {
      if (m_edge_direction == StaticGraph::EDGE_SUCCESSOR)
      {
        return "successor";
      }
      else if (m_edge_direction == StaticGraph::EDGE_PREDECESSOR)
      {
        return "predecessor";
      }
      else if (m_edge_direction == StaticGraph::EDGE_BIDIRECTIONAL)
      {
        return "bidirectional";
      }
      return "unknown";
    }

    /// \brief Sets the edge direction.
    /// \param s An edge direction.
    void set_edge_direction(const std::string& s)
    {
      if (s == "successor")
      {
        m_edge_direction = StaticGraph::EDGE_SUCCESSOR;
      }
      else if (s == "predecessor")
      {
        m_edge_direction = StaticGraph::EDGE_PREDECESSOR;
      }
      else if (s == "bidirectional")
      {
        m_edge_direction = StaticGraph::EDGE_BIDIRECTIONAL;
      }
      else
      {
        throw std::runtime_error("unknown edge direction specified (got `" + s + "')");
      }
    }

    void parse_options(const command_line_parser& parser)
    {
    	super::parse_options(parser);
      try
      {
        set_edge_direction(parser.option_argument("edge-direction"));
      }
      catch (std::logic_error)
      {
        set_edge_direction("successor");
      }
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
//      desc.
//        add_option("edge-direction",
//          make_optional_argument("NAME", "successor"),
//          "Use the edge direction NAME:\n"
//          "  'successor' (default), or\n"
//          "  'predecessor'\n"
//          "  'bidirectional'",
//          'e');
    }

  public:
    pg_solver_tool()
      : super(
          "pgsolver",
          "Michael Weber, Maks Verver, Wieger Wesselink",
          "Solve a PBES using a parity game solver",
          "Reads a file containing a PBES, instantiates it into a BES, and applies a\n"
          "parity game solver to it. If INFILE is not present, standard input is used."
        )
    {}

    bool run()
    {
      // TODO: make command line parameter for strategy
      arg_spm_lifting_strategy = "maxmeasure";
      arg_winners_file = "-";

      if (mcrl2::core::gsVerbose)
      {
        std::clog << "pgsolver parameters:" << std::endl;
        std::clog << "  input file:       " << m_input_filename << std::endl;
        std::clog << "  edge direction:   " << edge_direction_string() << std::endl;
      }

      ParityGame game;
      time_initialize();
      game.read_pbes(input_filename(), StaticGraph::EDGE_BIDIRECTIONAL);

      /* Do priority compression at the start too. */
      int old_d = game.d();
      game.compress_priorities();
      
      if (arg_solve_dual)
      {
          info("Switching to dual game...");
          game.make_dual();
      }
      
      if (arg_reordering == REORDER_BFS)
      {
          info("Reordering vertices in bread-first search preordering.");
          std::vector<verti> perm;
          get_bfs_order(game.graph(), perm);
          game.shuffle(perm);
      }
      
      if (arg_reordering == REORDER_DFS)
      {
          info("Reordering vertices in depth-first search preordering.");
          std::vector<verti> perm;
          get_dfs_order(game.graph(), perm);
          game.shuffle(perm);
      }

      if (mcrl2::core::gsVerbose)
      {
        info("Number of vertices:        %12lld", (long long)game.graph().V());
        info("Number of edges:           %12lld", (long long)game.graph().E());
        info("Forward edge ratio:        %.10f",
              (double)count_forward_edges(game.graph())/game.graph().E() );
        info("Number of priorities:      %12d (was %d)", game.d(), old_d);
        for (int p = 0; p < game.d(); ++p)
            info("  %2d occurs %d times", p, game.cardinality(p));
      }

      if (!arg_spm_lifting_strategy.empty())
      {
        LiftingStatistics stats(game);
        if (mcrl2::core::gsVerbose)
        {
          info( "SPM lifting strategy:      %12s",
                arg_spm_lifting_strategy.c_str() );
        } 
        double solve_time = time_used();
        if (mcrl2::core::gsVerbose)
        {
          info("Starting solve...");
        }
      
        // Allocate data structures
        ParityGameSolver *solver = NULL;
        std::auto_ptr<ComponentSolver> comp_solver;
        std::auto_ptr<LiftingStrategy> spm_strategy;
        std::auto_ptr<SmallProgressMeasures> spm;
        if (arg_scc_decomposition)
        {
            comp_solver.reset(new ComponentSolver(game, &stats));
            solver = comp_solver.get();
        }
        else
        {
            spm_strategy.reset(
                LiftingStrategy::create(game, arg_spm_lifting_strategy) );
            assert(spm_strategy.get() != NULL);
            spm.reset(new SmallProgressMeasures(game, *spm_strategy, &stats));
            solver = spm.get();
        }
      
        // Solve game
        solver->solve();
      
        solve_time = time_used() - solve_time;
      
        long long lifts_total       = stats.lifts_attempted();
        long long lifts_successful  = stats.lifts_succeeded();
        long long lifts_failed      = lifts_total - lifts_successful;

        if (mcrl2::core::gsVerbose)
        {
          info("Time used to solve:          %10.3f s", solve_time);
          size_t total_memory_use = game.memory_use() + solver->memory_use();
          info("Memory required to solve:    %10.3f MB", total_memory_use /MB);
          info(" .. used by parity game:     %10.3f MB", game.memory_use()/MB);
          info("     .. used by graph:       %10.3f MB", game.graph().memory_use()/MB);
          info(" .. used by solver:          %10.3f MB", solver->memory_use()/MB);
          info("Lifting attempts failed:      %12lld", lifts_failed);
          info("Lifting attempts succeeded:   %12lld", lifts_successful);
          info("Total lifting attempts:       %12lld", lifts_total);
          // info("Minimum lifts required:    %12lld", 0LL);  // TODO
        }
        write_output(game, solver);
      }
      else
      {
        write_output(game, NULL);
      }

      return true;
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  pg_solver_tool tool;
  return tool.execute(argc, argv);
}
