// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#if defined(__unix__) || defined(__linux__)
#define POSIX
#endif

#include "ComponentSolver.h"
#include "DecycleSolver.h"
#include "DeloopSolver.h"
#include "GraphOrdering.h"
#include "Logger.h"
#include "ParityGame.h"
#include "RecursiveSolver.h"
#include "SmallProgressMeasures.h"
#include "Timer.h"

#ifdef WITH_MCRL2
#include <aterm_init.h>
#endif

#include <assert.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <memory>

#ifdef POSIX
#include <unistd.h>
#include <signal.h>
#endif

#ifdef WITH_THREADS
#include <omp.h>
#include "ConcurrentRecursiveSolver.h"
#endif

#ifdef WITH_MPI
#include "MpiUtils.h"
#include "MpiSpmSolver.h"
#include "MpiRecursiveSolver.h"
#endif

#define strcasecmp compat_strcasecmp


enum InputFormat {
    INPUT_NONE = 0, INPUT_RAW, INPUT_RANDOM, INPUT_PGSOLVER, INPUT_PBES
};

enum Reordering { REORDER_NONE = 0, REORDER_BFS, REORDER_DFS };

static InputFormat  arg_input_format          = INPUT_NONE;
static std::string  arg_dot_file              = "";
static std::string  arg_pgsolver_file         = "";
static std::string  arg_raw_file              = "";
static std::string  arg_winners_file          = "";
static std::string  arg_strategy_file         = "";
static std::string  arg_paritysol_file        = "";
static std::string  arg_hot_vertices_file     = "";
static std::string  arg_debug_file            = "";
static std::string  arg_spm_lifting_strategy  = "";
static bool         arg_collect_stats         = false;
static bool         arg_alternate             = false;
static bool         arg_decycle               = false;
static bool         arg_deloop                = false;
static bool         arg_scc_decomposition     = false;
static bool         arg_solve_dual            = false;
static Reordering   arg_reordering            = REORDER_NONE;
static bool         arg_priority_propagation  = false;
static int          arg_random_size           = 1000000;
static int          arg_random_seed           =       1;
static int          arg_random_out_degree     =       3;
static int          arg_random_priorities     =      20;
static int          arg_timeout               =       0;
static bool         arg_verify                = false;
static bool         arg_zielonka              = false;
static bool         arg_zielonka_sync         = false;
static int          arg_threads               = 0;
static bool         arg_mpi                   = false;
static int          arg_chunk_size            = -1;

static const double MB = 1048576.0;  // one megabyte

static volatile bool g_timed_out = false;

#ifdef POSIX
struct MStat {
    int size, resident, share, text, lib, data, dt;
};

static bool read_mstat(MStat &stat)
{
    std::ifstream ifs("/proc/self/statm");
    return ifs >> stat.size >> stat.resident >> stat.share >> stat.text
               >> stat.lib >> stat.data >> stat.dt;
}

static double get_vmsize()
{
    MStat mstat;
    if (!read_mstat(mstat)) return -1;
    return mstat.size*(getpagesize()/MB);
}
#else
static double get_vmsize()
{
    return 0;
}
#endif

static void print_usage(const char *argv0)
{
    printf( "Usage: %s [<options>] [<input>]\n\n", argv0);
    printf(
"General options:\n"
"  --help/-h              show this help message\n"
"  --verbosity/-v <level> message verbosity (0-6; default: 4)\n"
"  --quiet/-q             no messages (equivalent to -v0)\n"
"\n"
"Input:\n"
"  --input/-i <format>    input format: random, raw, PGSolver or PBES\n"
"  --priorities <int>     (random only) number of priorities\n"
"  --size <int>           (random only) number of vertices\n"
"  --outdegree <int>      (random only) average out-degree\n"
"  --seed <int>           (random only) random nubmer generator seed\n"
"\n"
"Preprocessing:\n"
"  --deloop               detect loops won by the controlling player\n"
"  --decycle              detect cycles won and controlled by a single player\n"
"  --scc                  solve strongly connected components individually\n"
"  --dual                 solve the dual game\n"
"  --reorder (bfs|dfs)    order vertices by breadth-/depth-first-search order\n"
"  --propagate            propagate minimum priorities to predecessors\n"
"\n"
"Solving with Small Progress Measures:\n"
"  --lifting/-l <desc>    Small Progress Measures lifting strategy to use\n"
"  --alternate/-a         use Friedmann's alternating solving approach\n"
"\n"
"\nZielonka's recursive algorithm:\n"
"  --zielonka/-z          use Zielonka's recursive algorithm\n"
"  --threads <count>      solve concurrently using threads\n"
"  --mpi                  solve in parallel using MPI\n"
"  --chunk/-c <size>      (MPI only) chunk size for partitioning\n"
"  --sync                 (MPI only) use synchronized MPI algorithm\n"
"\n"
"Output:\n"
"  --dot/-d <file>        write parity game in GraphViz dot format to <file>\n"
"  --pgsolver/-p <file>   write parity game in PGSolver format to <file>\n"
"  --raw/-r <file>        write parity game in raw format to <file>\n"
"  --winners/-w <file>    write compact winners specification to <file>\n"
"  --strategy/-s <file>   write optimal strategy for both players to <file>\n"
"  --paritysol/-P <file>  write solution in PGSolver format to <file>\n"
"\n"
"Benchmarking/testing:\n"
"  --stats/-S             collect lifting statistics during SPM solving\n"
"  --timeout/-t <t>       abort solving after <t> seconds\n"
"  --verify/-V            verify solution after solving\n"
"  --hot/-H <file>        write 'hot' vertices in GraphViz format to <file>\n"
"  --debug/-D <file>      write solution in debug format to <file>\n");
}

static void parse_args(int argc, char *argv[])
{
    enum FileMode { text, binary, none } input_mode = none;

    static struct option long_options[] = {
        { "help",       no_argument,       NULL, 'h' },
        { "verbosity",  required_argument, NULL, 'v' },
        { "quiet",      no_argument,       NULL, 'q' },

        { "input",      required_argument, NULL, 'i' },
        { "size",       required_argument, NULL,  1  },
        { "outdegree",  required_argument, NULL,  2  },
        { "priorities", required_argument, NULL,  3  },
        { "seed",       required_argument, NULL,  4  },

        { "decycle",    no_argument,       NULL,  5  },
        { "deloop",     no_argument,       NULL,  6  },
        { "scc",        no_argument,       NULL,  7  },
        { "dual",       no_argument,       NULL,  8  },
        { "reorder",    required_argument, NULL,  9  },
        { "propagate",  no_argument,       NULL, 10  },

        { "lifting",    required_argument, NULL, 'l' },
        { "alternate",  no_argument,       NULL, 'a' },

        { "zielonka",   no_argument,       NULL, 'z' },
        { "threads",    required_argument, NULL, 11  },
        { "mpi",        no_argument,       NULL, 12  },
        { "chunk",      required_argument, NULL, 'c' },
        { "sync",       no_argument,       NULL, 13  },

        { "dot",        required_argument, NULL, 'd' },
        { "pgsolver",   required_argument, NULL, 'p' },
        { "raw",        required_argument, NULL, 'r' },
        { "winners",    required_argument, NULL, 'w' },
        { "strategy",   required_argument, NULL, 's' },
        { "paritysol",  required_argument, NULL, 'P' },

        { "stats",      no_argument,       NULL, 'S' },
        { "timeout",    required_argument, NULL, 't' },
        { "verify",     no_argument,       NULL, 'V' },
        { "hot",        required_argument, NULL, 'H' },
        { "debug",      required_argument, NULL, 'D' },
        { NULL,         no_argument,       NULL,  0  } };

    std::string options;
    for (struct option *opt = long_options; opt->name; ++opt)
    {
        if (opt->val > ' ')
        {
            options += (char)opt->val;
            if (opt->has_arg == required_argument) options += ':';
        }
    }

    for (;;)
    {
        int ch = getopt_long(argc, argv, options.c_str(), long_options, NULL);
        if (ch == -1) break;

        switch (ch)
        {
        case 'h':   /* help */
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
            break;

        case 'v':   /* set logger severity to (NONE - verbosity) */
            {
                int severity = Logger::LOG_NONE - atoi(optarg);
                if (severity > Logger::LOG_NONE)  severity = Logger::LOG_NONE;
                if (severity < Logger::LOG_DEBUG) severity = Logger::LOG_DEBUG;
                Logger::severity((Logger::Severity)severity);
            } break;

        case 'q':  /* set logger severity to NONE */
            Logger::severity(Logger::LOG_NONE);
            break;

        case 'i':   /* input format */
            if (strcasecmp(optarg, "random") == 0)
            {
                arg_input_format = INPUT_RANDOM;
            }
            else
            if (strcasecmp(optarg, "raw") == 0)
            {
                arg_input_format = INPUT_RAW;
                input_mode = binary;
            }
            else
            if (strcasecmp(optarg, "pgsolver") == 0)
            {
                arg_input_format = INPUT_PGSOLVER;
                input_mode = text;
            }
            else
            if (strcasecmp(optarg, "pbes") == 0)
            {
#ifdef WITH_MCRL2
                arg_input_format = INPUT_PBES;
                input_mode = binary;
#else
                printf("PBES input requires linking to mCRL2\n");
                exit(EXIT_FAILURE);
#endif
            }
            else
            {
                printf("Invalid input format: %s\n", optarg);
                exit(EXIT_FAILURE);
            }
            break;

        case 1:     /* random graph size */
            arg_random_size = atoi(optarg);
            break;

        case 2:     /* random graph out-degree */
            arg_random_out_degree = atoi(optarg);
            break;

        case 3:     /* random game number of priorities */
            arg_random_priorities = atoi(optarg);
            break;

        case 4:     /* random seed */
            arg_random_seed = atoi(optarg);
            break;

        case 5:     /* remove p-controlled i-cycles when p == i%2 */
            arg_decycle = true;
            break;

        case 6:     /* preprocess vertices with loops */
            arg_deloop = true;
            break;

        case 7:     /* decompose into strongly connected components */
            arg_scc_decomposition = true;
            break;

        case 8:     /* solve dual game */
            arg_solve_dual = true;
            break;

        case 9:    /* reorder vertices */
            if (strcasecmp(optarg, "bfs") == 0)
            {
                arg_reordering = REORDER_BFS;
            }
            else
            if (strcasecmp(optarg, "dfs") == 0)
            {
                arg_reordering = REORDER_DFS;
            }
            else
            {
                printf("Invalid reordering: %s\n", optarg);
                exit(EXIT_FAILURE);
            }
            break;

        case 10:    /* enable priority propagation */
            arg_priority_propagation = true;
            break;

        case 'l':   /* Small Progress Measures lifting strategy */
            arg_spm_lifting_strategy = optarg;
            break;

        case 'a':  /* Alternate SPM solver */
            arg_alternate = true;
            break;

        case 'z':   /* use Zielonka's algorithm instead of SPM */
            arg_zielonka = true;
            break;

        case 11:    /* concurrent solving */
            arg_threads = atoi(optarg);
            if (arg_threads < 1)
            {
                fprintf(stderr, "Invalid number of threads: %s\n", optarg);
                exit(EXIT_FAILURE);
            }
            break;

        case 12:    /* parallize solving with MPI */
            arg_mpi = true;
            break;

        case 'c':   /* use given chunk size */
            arg_chunk_size = atoi(optarg);
            if (arg_chunk_size < 1)
            {
                fprintf(stderr, "Invalid chunk size: %s\n", optarg);
                exit(EXIT_FAILURE);
            }
            break;

        case 13:    /* use synchronized algorithm */
            arg_zielonka_sync = true;
            break;

        case 'd':   /* dot output file */
            arg_dot_file = optarg;
            break;

        case 'p':   /* PGSolver output file */
            arg_pgsolver_file = optarg;
            break;

        case 'r':   /* raw output file */
            arg_raw_file = optarg;
            break;

        case 'w':   /* winners output file */
            arg_winners_file = optarg;
            break;

        case 's':   /* strategy output file */
            arg_strategy_file = optarg;
            break;

        case 'P':   /* "paritysol" (PGSolver --solonly format) output file */
            arg_paritysol_file = optarg;
            break;

        case 'S':   /* collect lifting statistics*/
            arg_collect_stats = true;
            break;

        case 't':   /* time limit (in seconds) */
            arg_timeout = atoi(optarg);
            break;

        case 'V':   /* verify solution */
            arg_verify = true;
            break;

        case 'H':   /* debug hot vertices file */
            arg_hot_vertices_file = optarg;
            break;

        case 'D':   /* debug output file */
            arg_debug_file = optarg;
            break;

        case '?':
            {
                printf("Unrecognized option!\n");
                exit(EXIT_FAILURE);
            }
            break;
        }
    }

    if (arg_input_format == INPUT_NONE)
    {
        printf("No input format specified!\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (optind < argc)
    {
        /* Rmaining argument specifies an input file */
        if (input_mode == none || argc - optind > 1)
        {
            printf("Too many (non-option) arguments specified!\n");
            exit(EXIT_FAILURE);
        }
        const char *path = argv[optind];
        const char *mode = (input_mode == text) ? "rt" : "r";
        if (freopen(path, mode, stdin) == NULL)
        {
            printf("Could not open file \"%s\" for reading!\n", path);
            exit(EXIT_FAILURE);
        }
    }
}

/*! Write summary of winners. For each node, a single character is printed:
    'E' or 'O', depending on whether player Even or Odd wins the parity game
    starting from this node. */
static void write_winners( std::ostream &os, const ParityGame &game,
                           const ParityGame::Strategy &strategy )
{
    verti next_newline = 80;
    for (verti v = 0; v < game.graph().V(); ++v)
    {
        if (v == next_newline)
        {
            os << '\n';
            next_newline += 80;
        }
        ParityGame::Player winner = game.winner(strategy, v);
        os << ( (winner == ParityGame::PLAYER_EVEN) ^ arg_solve_dual ? 'E' :
                (winner == ParityGame::PLAYER_ODD)  ^ arg_solve_dual ? 'O' :
                                                                       '?' );
    }
    os << '\n';
}

/*! Write strategy description. For each vertex won by its player, a single line
    is printed, of the form: v->w (where v and w are 0-based vertex indices). */
static void write_strategy( std::ostream &os,
                            const ParityGame::Strategy &strategy )
{
    for (verti v = 0; v < (verti)strategy.size(); ++v)
    {
        if (strategy[v] != NO_VERTEX) os << v << "->" << strategy[v] << '\n';
    }
}

/*! Write solution in PGSolver --solonly format, which can be parsed by tools
    like MLSsolver. */
static void write_paritysol( std::ostream &os,
                             const ParityGame &game,
                             const ParityGame::Strategy &strategy )
{
    const StaticGraph &graph = game.graph();
    const verti V = graph.V();
    assert(strategy.size() == V);
    os << "paritysol " << (long long)V - 1 << ";\n";
    for (verti v = 0; v < V; ++v)
    {
        os << v << ' ' << (int)game.winner(strategy, v);
        if (strategy[v] != NO_VERTEX) os << ' ' << strategy[v];
        os << ";\n";
    }
}

/*! Write a subgraph containing hot vertices (vertices that were lifted at
   least `threshold' times) in GraphViz format to given output stream. */
static void write_hot_vertices( std::ostream &os, const ParityGame &game,
    const LiftingStatistics &stats, long long threshold )
{
    const StaticGraph &graph = game.graph();
    std::set<std::pair<verti, verti> > edges;
    std::set<verti> vertices, hot;
    for (verti v = 0; v < graph.V(); ++v)
    {
        if (stats.lifts_succeeded(v) >= threshold)
        {
            hot.insert(v);
            vertices.insert(v);
            for ( StaticGraph::const_iterator it = graph.succ_begin(v);
                    it != graph.succ_end(v); ++it )
            {
                vertices.insert(*it);
                edges.insert(std::make_pair(v, *it));
            }
            for ( StaticGraph::const_iterator it = graph.pred_begin(v);
                    it != graph.pred_end(v); ++it )
            {
                vertices.insert(*it);
                edges.insert(std::make_pair(*it, v));
            }
        }
    }
    os << "digraph {\n";
    for ( std::set<verti>::const_iterator it = vertices.begin();
            it != vertices.end(); ++it )
    {
        os << *it << " [shape=" << (game.player(*it) ? "box": "diamond")
            << ", label=\"" << game.priority(*it) << "\\n(" << *it << ")\"";
        if (hot.count(*it)) os << ", style=\"filled\"";
        os << "]\n";
    }
    for ( std::set<verti>::const_iterator it = hot.begin();
            it != hot.end(); ++it )
    {
        os << *it << "->l" << *it << " [arrowhead=none];\n"
           << "l" << *it << " [shape=plaintext, label=\""
           << stats.lifts_succeeded(*it) << " /\\n"
           << stats.lifts_attempted(*it) << "\"]\n";
    }
    for ( std::set<std::pair<verti, verti> >::const_iterator
            it = edges.begin(); it != edges.end(); ++it )
    {
        os << it->first << "->" << it->second << ";\n";
    }
    os << "}\n";
}

bool read_input(ParityGame &game)
{
    switch (arg_input_format)
    {
    case INPUT_RANDOM:
        Logger::info( "Generating random parity game with %d vertices, "
                      "out-degree %d, and %d priorities...", arg_random_size,
                      arg_random_out_degree, arg_random_priorities );
        srand(arg_random_seed);

        game.make_random(
            arg_random_size, arg_random_out_degree,
            StaticGraph::EDGE_BIDIRECTIONAL, arg_random_priorities );
        return true;

    case INPUT_RAW:
        Logger::info("Reading raw input...");
        game.read_raw(std::cin);
        return true;

    case INPUT_PGSOLVER:
        Logger::info("Reading PGSolver input...");
        game.read_pgsolver(std::cin);
        return !game.empty();

    case INPUT_PBES:
        Logger::info("Generating parity game from PBES input....");
        game.read_pbes("");
        return true;

    case INPUT_NONE:
        return false;
    }

    return false;
}

void write_output( const ParityGame &game,
    const ParityGame::Strategy &strategy = ParityGame::Strategy(),
    LiftingStatistics *stats = NULL )
{
    /* Write dot file */
    if (!arg_dot_file.empty())
    {
        if (arg_dot_file == "-")
        {
            game.write_dot(std::cout);
            if (!std::cout) Logger::error("Writing failed!");
        }
        else
        {
            Logger::info( "Writing GraphViz dot game description to file %s...",
                          arg_dot_file.c_str() );
            std::ofstream ofs(arg_dot_file.c_str());
            game.write_dot(ofs);
            if (!ofs) Logger::error("Writing failed!");
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
            Logger::info( "Writing PGSolver game description to file %s...",
                          arg_pgsolver_file.c_str() );
            std::ofstream ofs(arg_pgsolver_file.c_str());
            game.write_pgsolver(ofs);
            if (!ofs) Logger::error("Writing failed!");
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
            Logger::info( "Writing raw game description to file %s...",
                          arg_raw_file.c_str() );
            std::ofstream ofs(arg_raw_file.c_str());
            game.write_raw(ofs);
            if (!ofs) Logger::error("Writing failed!");
        }
    }

    /* Write winners file */
    if (!arg_winners_file.empty() && !strategy.empty())
    {
        if (arg_winners_file == "-")
        {
            write_winners(std::cout, game, strategy);
            if (!std::cout) Logger::error("Writing failed!");
        }
        else
        {
            Logger::info( "Writing winners to file %s...",
                          arg_winners_file.c_str() );
            std::ofstream ofs(arg_winners_file.c_str());
            write_winners(ofs, game, strategy);
            if (!ofs) Logger::error("Writing failed!");
        }
    }

    /* Write strategy file */
    if (!arg_strategy_file.empty() && !strategy.empty())
    {
        if (arg_strategy_file == "-")
        {
            write_strategy(std::cout, strategy);
            if (!std::cout) Logger::error("Writing failed!");
        }
        else
        {
            Logger::info( "Writing strategy to file %s...",
                          arg_strategy_file.c_str() );
            std::ofstream ofs(arg_strategy_file.c_str());
            write_strategy(ofs, strategy);
            if (!ofs) Logger::error("Writing failed!");
        }
    }

    /* Write paritysol file */
    if (!arg_paritysol_file.empty() && !strategy.empty())
    {
        if (arg_paritysol_file == "-")
        {
            write_paritysol(std::cout, game, strategy);
            if (!std::cout) Logger::error("Writing failed!");
        }
        else
        {
            Logger::info( "Writing PGSolver solution description to file %s...",
                          arg_paritysol_file.c_str() );
            std::ofstream ofs(arg_paritysol_file.c_str());
            write_paritysol(ofs, game, strategy);
            if (!ofs) Logger::error("Writing failed!");
        }
    }

    /* Write hot vertices file */
    if (stats != NULL && !arg_hot_vertices_file.empty())
    {
        // FIXME: make this a parameter?
        long long threshold = stats->lifts_succeeded()/1000;
        if (arg_hot_vertices_file == "-")
        {
            write_hot_vertices(std::cout, game, *stats, threshold);
        }
        else
        {
            Logger::info( "Writing hot vertices to file %s...",
                          arg_hot_vertices_file.c_str() );
            std::ofstream ofs(arg_hot_vertices_file.c_str());
            write_hot_vertices(ofs, game, *stats, threshold);
            if (!ofs) Logger::error("Writing failed!");
        }
    }

    /* Write debug file */
    if (!arg_debug_file.empty())
    {
        if (arg_debug_file == "-")
        {
            game.write_debug(strategy, std::cout);
        }
        else
        {
            Logger::info( "Writing debug info to file %s...",
                          arg_debug_file.c_str() );
            std::ofstream ofs(arg_debug_file.c_str());
            game.write_debug(strategy, ofs);
            if (!ofs) Logger::error("Writing failed!");
        }
    }
}

#ifdef POSIX
static void alarm_handler(int sig)
{
    if (sig == SIGALRM && !g_timed_out)
    {
        g_timed_out = true;
        Abortable::abort_all();
    }
}

static void set_timeout(int t)
{
    /* Set handler for alarm signal */
    struct sigaction act;
    act.sa_handler = &alarm_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    /* Schedule alarm signal */
    int res = sigaction(SIGALRM, &act, NULL);
    if (res != 0)
    {
        Logger::warn("Couldn't install signal handler.");
    }
    else
    {
        alarm(t);
    }
}
#else
static void set_timeout(int t)
{
    (void)t;  // unused
    Logger::warn("Time-out not available.");
}
#endif

int main(int argc, char *argv[])
{
    Logger::severity(Logger::LOG_WARN);

#ifdef WITH_MCRL2
    MCRL2_ATERMPP_INIT(argc, argv);
#endif

#ifdef WITH_MPI
    MPI::Init(argc, argv);
    mpi_rank = MPI::COMM_WORLD.Get_rank();
    mpi_size = MPI::COMM_WORLD.Get_size();
#endif

    parse_args(argc, argv);

    if (arg_mpi)
    {
        // Make stderr line-buffered to avoid interleaving of log output
        // from different MPI processes.
        static char stderr_buf[1024];
        setvbuf(stderr, stderr_buf, _IOLBF, sizeof(stderr_buf));
    }

    ParityGame game;
    if (!read_input(game))
    {
        Logger::fatal("Couldn't parse parity game from input!");
    }
    assert(game.proper());

    /* Do priority compression at the start too. */
    int old_d = game.d();
    game.compress_priorities();

    if (arg_solve_dual)
    {
        Logger::info("Switching to dual game...");
        game.make_dual();
    }

    if (arg_reordering == REORDER_BFS)
    {
        Logger::info("Reordering vertices in bread-first search preordering.");
        std::vector<verti> perm;
        get_bfs_order(game.graph(), perm);
        game.shuffle(perm);
    }

    if (arg_reordering == REORDER_DFS)
    {
        Logger::info("Reordering vertices in depth-first search preordering.");
        std::vector<verti> perm;
        get_dfs_order(game.graph(), perm);
        game.shuffle(perm);
    }

    /* Print some game info: */
    Logger::info( "Number of vertices:        %12lld",
                  (long long)game.graph().V() );
    Logger::info( "Number of edges:           %12lld",
                  (long long)game.graph().E() );
    Logger::info( "Forward edge ratio:        %.10f",
                  (double)count_forward_edges(game.graph())/game.graph().E() );
    Logger::info( "Number of priorities:      %12d (was %d)",
                  game.d(), old_d);
    for (int p = 0; p < game.d(); ++p)
    {
        Logger::info("  %2d occurs %d times", p, game.cardinality(p));
    }

    bool failed = true;

    if (arg_spm_lifting_strategy.empty() && !arg_zielonka)
    {
        // Don't solve; just convert data.
        write_output(game);
        failed = false;
    }
    else
    {
        std::auto_ptr<LiftingStatistics> stats;

        if (arg_threads)
        {
#ifndef WITH_THREADS
            Logger::fatal("Thread support was not compiled in!");
#else
            omp_set_num_threads(arg_threads);
#endif
        }

#ifndef WITH_MPI
        if (arg_mpi) Logger::fatal("MPI support was not compiled in!");
#endif

#ifdef WITH_MPI
        VertexPartition *vpart = NULL;
        if (arg_mpi)
        {
            vpart = new VertexPartition( mpi_size, arg_chunk_size > 0
                ? arg_chunk_size : (game.graph().V() + mpi_size - 1)/mpi_size );
        }
#endif

        // Create appropriate solver factory:
        std::auto_ptr<ParityGameSolverFactory> solver_factory;

        // Allocate lifting strategy:
        if (!arg_spm_lifting_strategy.empty())
        {
            Logger::info( "SPM lifting strategy:      %12s",
                          arg_spm_lifting_strategy.c_str() );

            LiftingStrategyFactory *spm_strategy = 
                LiftingStrategyFactory::create(arg_spm_lifting_strategy);

            if (!spm_strategy)
            {
                Logger::fatal( "Invalid lifting strategy description: %s",
                               arg_spm_lifting_strategy.c_str() );
            }

            if (arg_collect_stats) stats.reset(new LiftingStatistics(game));

            if (!arg_mpi)
            {
                solver_factory.reset(new SmallProgressMeasuresSolverFactory(
                        spm_strategy, arg_alternate, stats.get() ));
            }
#ifdef WITH_MPI
            else
            {
                solver_factory.reset(new MpiSpmSolverFactory(
                    spm_strategy, vpart, stats.get() ));
            }
#endif
            spm_strategy->deref();
        }

        // Create recursive solver factory if requested:
        if (arg_zielonka)
        {
            if (!arg_mpi)
            {
                if (!arg_threads)
                {
                    solver_factory.reset(new RecursiveSolverFactory());
                }
#ifdef WITH_THREADS
                else
                {
                    solver_factory.reset(new ConcurrentRecursiveSolverFactory());
                }
#endif
            }
#ifdef WITH_MPI
            else
            {
                solver_factory.reset(
                    new MpiRecursiveSolverFactory(!arg_zielonka_sync, vpart) );
            }
#endif
        }

#ifdef WITH_MPI
        if (vpart)
        {
            vpart->deref();
            vpart = NULL;
        }
#endif

        if (arg_timeout > 0) set_timeout(arg_timeout);

        {
            // FIXME: this should probably count towards solving time
            // FIXME: it might be useful when using SPM to solve only
            Logger::info("Preprocessing graph...");
            edgei old_edges = game.graph().E();
            SmallProgressMeasuresSolver::preprocess_game(game);
            edgei rem_edges = old_edges - game.graph().E();
            Logger::info( "Removed %d edge%s...",
                            rem_edges, rem_edges == 1 ? "" : "s" );
        }

        /* Note: priority propagation is done after preprocessing, because
                 it benefits from removed loops (since priorities can only be
                 propagated to vertices with loops). */
        if (arg_priority_propagation)
        {
            Logger::info("Propagating priorities...");
            long long updates = game.propagate_priorities();
            Logger::info("Reduced summed priorities by %lld.", updates);
            game.compress_priorities();
            for (int p = 0; p < game.d(); ++p)
            {
                Logger::info("  %2d occurs %d times", p, game.cardinality(p));
            }
        }

        if (arg_decycle)
        {
            solver_factory.reset(
                new DecycleSolverFactory(*solver_factory.release()) );
        }
        else
        if (arg_deloop)
        {
            // N.B. current implementation of the DeloopSolver assumes
            //      the game has been preprocessed as done above!
            solver_factory.reset(
                new DeloopSolverFactory(*solver_factory.release()) );
        }

        // Wrap component solver, if solving by components requested:
        if (arg_scc_decomposition)
        {
            solver_factory.reset(
                new ComponentSolverFactory(*solver_factory.release()) );
        }

        Timer timer;
        Logger::info("Starting solve...");

        // Create solver instance:
        assert(solver_factory.get() != NULL);
        std::auto_ptr<ParityGameSolver> solver(solver_factory->create(game));

        // Now solve the game:
        ParityGame::Strategy strategy = solver->solve();

#ifdef WITH_MPI
        if (mpi_rank > 0)
        {
            // Join processes here, to avoid writing output multiple times.
            MPI::Finalize();
            return EXIT_SUCCESS;
        }
#endif

        failed = strategy.empty();

        if (failed)
        {
            if (solver->aborted())
            {
                Logger::error("time limit exceeded!");
            }
            else
            {
                Logger::error("solving failed!");
            }
        }

        // Print some statistics
        Logger::message("Time used to solve:          %10.3f s", timer.elapsed());
        Logger::message("Current memory use:          %10.3f MB", get_vmsize());
        size_t total_memory_use = game.memory_use() + solver->memory_use();
        Logger::message( "Memory required to solve:    %10.3f MB",
                         total_memory_use /MB );
        Logger::message( " .. used by parity game:     %10.3f MB",
                         game.memory_use()/MB );
        Logger::message( "     .. used by graph:       %10.3f MB",
                         game.graph().memory_use()/MB );
        Logger::message( " .. used by solver:          %10.3f MB",
                         solver->memory_use()/MB );

        if (stats.get() != NULL)
        {
            long long lifts_total       = stats->lifts_attempted();
            long long lifts_successful  = stats->lifts_succeeded();
            long long lifts_failed      = lifts_total - lifts_successful;

            Logger::message( "Lifting attempts failed:      %12lld",
                             lifts_failed );
            Logger::message( "Lifting attempts succeeded:   %12lld",
                             lifts_successful );
            Logger::message( "Total lifting attempts:       %12lld",
                             lifts_total );
            /*
            Logger::message( "Minimum lifts required:       %12lld",
                             0LL);  // TODO
            */
        }

        if (!failed && arg_verify)
        {
            Timer timer;
            verti error;

            Logger::info("Starting verification...");
            if (game.verify(strategy, &error))
            {
                Logger::message("Verification succeeded.");
            }
            else
            {
                failed = true;
                // Complain loudly so this message gets noticed:
                Logger::error("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                Logger::error("!!                                    !!");
                Logger::error("!!        Verification failed!        !!");
                Logger::error("!!                                    !!");
                Logger::error("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                Logger::error("Error at vertex: %d", (int)error);
            }
            Logger::message( "Time used to verify:         %10.3f s",
                             timer.elapsed() );
        }

        write_output(game, strategy, stats.get());
    }

    Logger::info("Exiting.");

#ifdef WITH_MPI
    MPI::Finalize();
#endif

    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
