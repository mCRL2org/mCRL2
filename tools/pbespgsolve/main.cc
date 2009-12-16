// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#if defined(__unix__) || defined(__linux__)
#define POSIX
#endif

#include "ComponentSolver.h"
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
#include <memory>

#ifdef POSIX
#include <unistd.h>
#include <signal.h>
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
static std::string  arg_spm_lifting_strategy  = "";
static bool         arg_scc_decomposition     = false;
static bool         arg_solve_dual            = false;
static Reordering   arg_reordering            = REORDER_NONE;
static int          arg_random_size           = 1000000;
static int          arg_random_seed           =       1;
static int          arg_random_out_degree     =      10;
static int          arg_random_priorities     =      20;
static int          arg_timeout               =       0;
static bool         arg_verify                = false;
static bool         arg_zielonka              = false;

static const double MB = 1048576.0;  // one megabyte

static volatile bool g_timed_out = false;


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
"  --lifting/-l <desc>    Small Progress Measures lifting strategy\n"
"  --dot/-d <file>        write parity game in GraphViz dot format to <file>\n"
"  --pgsolver/-p <file>   write parity game in PGSolver format to <file>\n"
"  --raw/-r <file>        write parity game in raw format to <file>\n"
"  --winners/-w <file>    write compact winners specification to <file>\n"
"  --scc                  solve strongly connected components individually\n"
"  --dual                 solve the dual game\n"
"  --reorder/-e (bfs|dfs) reorder vertices\n"
"  --timeout/-t <t>       abort solving after <t> seconds\n"
"  --verify/-V            verify solution after solving\n"
"  --zielonka/-z          use Zielonka's recursive algorithm\n"
"  --verbosity/-v         message verbosity (0-5; default: 4)\n"
"  --quiet/-q             no messages (equivalent to -v0)\n" );
}

static void parse_args(int argc, char *argv[])
{
    static struct option long_options[] = {
        { "help",       0, NULL, 'h' },
        { "input",      1, NULL, 'i' },
        { "size",       1, NULL,  1  },
        { "outdegree",  1, NULL,  2  },
        { "priorities", 1, NULL,  3  },
        { "seed",       1, NULL,  4  },
        { "lifting",    1, NULL, 'l' },
        { "dot",        1, NULL, 'd' },
        { "pgsolver",   1, NULL, 'p' },
        { "raw",        1, NULL, 'r' },
        { "winners",    1, NULL, 'w' },
        { "strategy",   1, NULL, 's' },
        { "scc",        0, NULL,  5  },
        { "dual",       0, NULL,  6  },
        { "reorder",    1, NULL, 'e' },
        { "timeout",    1, NULL, 't' },
        { "verify",     0, NULL, 'V' },
        { "zielonka",   0, NULL, 'z' },
        { "verbosity",  1, NULL, 'v' },
        { "quiet",      0, NULL, 'q' },
        { NULL,         0, NULL,  0  } };

    static const char *short_options = "hi:l:d:p:r:w:s:e:t:Vzv:q";

    for (;;)
    {
        int ch = getopt_long(argc, argv, short_options, long_options, NULL);
        if (ch == -1) break;

        switch (ch)
        {
        case 'h':   /* help */
            print_usage();
            exit(EXIT_SUCCESS);
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
            }
            else
            if (strcasecmp(optarg, "pgsolver") == 0)
            {
                arg_input_format = INPUT_PGSOLVER;
            }
            else
            if (strcasecmp(optarg, "pbes") == 0)
            {
#ifdef WITH_MCRL2
                arg_input_format = INPUT_PBES;
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

        case 'l':   /* Small Progress Measures lifting strategy */
            arg_spm_lifting_strategy = optarg;
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

        case 5:     /* decompose into strongly connected components */
            arg_scc_decomposition = true;
            break;

        case 6:     /* solve dual game */
            arg_solve_dual = true;
            break;

        case 'e':   /* reorder vertices */
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

        case 't':   /* time limit (in seconds) */
            arg_timeout = atoi(optarg);
            break;

        case 'V':   /* verify solution */
            arg_verify = true;
            break;

        case 'z':   /* use Zielonka's algorithm instead of SPM */
            arg_zielonka = true;
            break;

        case 'v':   /* set logger severity to NONE - verbosity */
            {
                int severity = Logger::NONE - atoi(optarg);
                if (severity > Logger::NONE)  severity = Logger::NONE;
                if (severity < Logger::DEBUG) severity = Logger::DEBUG;
                Logger::severity((Logger::Severity)severity);
            } break;

        case 'q':  /* set logger severity to NONE */
            Logger::severity(Logger::NONE);
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
        print_usage();
        exit(EXIT_FAILURE);
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
        return true;

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
    const ParityGame::Strategy &strategy = ParityGame::Strategy() )
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
            Logger::info("Writing winners to file %s...", arg_winners_file.c_str());
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
            Logger::info("Writing strategy to file %s...", arg_strategy_file.c_str());
            std::ofstream ofs(arg_strategy_file.c_str());
            write_strategy(ofs, strategy);
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
    warn("Time-out not available.");
}
#endif

int main(int argc, char *argv[])
{
    Logger::severity(Logger::INFO);

#ifdef WITH_MCRL2
    MCRL2_ATERMPP_INIT(argc, argv);
#endif

    parse_args(argc, argv);


    ParityGame game;
    if (!read_input(game))
    {
        Logger::fatal("Couldn't parse parity game from input!");
    }

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
        /* Don't solve; just convert data. */
        write_output(game);
        failed = false;
    }
    else
    {
        std::auto_ptr<LiftingStatistics> stats;

        // Allocate lifting strategy:
        std::auto_ptr<LiftingStrategyFactory> spm_strategy;
        if (!arg_spm_lifting_strategy.empty())
        {
            Logger::info( "SPM lifting strategy:      %12s",
                          arg_spm_lifting_strategy.c_str() );

            spm_strategy.reset(
                LiftingStrategyFactory::create(arg_spm_lifting_strategy) );
        }

        // Create appropriate solver factory:
        std::auto_ptr<ParityGameSolverFactory> solver_factory;

        // Create SPM solver facory if requested:
        if (spm_strategy.get() != NULL)
        {
            stats.reset(
                new LiftingStatistics(game) );

            solver_factory.reset(
                new SmallProgressMeasuresFactory(*spm_strategy, stats.get()) );
        }

        // Create recursive solver factory if requested:
        if (arg_zielonka)
        {
            solver_factory.reset(new RecursiveSolverFactory());
        }

        if (arg_timeout > 0) set_timeout(arg_timeout);

        Timer timer;
        Logger::info("Starting solve...");

        // Create solver instance:
        assert(solver_factory.get() != NULL);
        std::auto_ptr<ParityGameSolver> solver;
        if (arg_scc_decomposition)
        {
            solver.reset(new ComponentSolver(game, *solver_factory));
        }
        else
        {
            solver.reset(solver_factory->create(game));
        }

        // Now solve the game:
        ParityGame::Strategy strategy = solver->solve();

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
        Logger::info("Time used to solve:          %10.3f s", timer.elapsed());
        Logger::info("Current memory use:           %10.3f MB", get_vmsize());
        size_t total_memory_use = game.memory_use() + solver->memory_use();
        Logger::info( "Memory required to solve:    %10.3f MB",
                      total_memory_use /MB );
        Logger::info( " .. used by parity game:     %10.3f MB",
                      game.memory_use()/MB );
        Logger::info( "     .. used by graph:       %10.3f MB",
                      game.graph().memory_use()/MB );
        Logger::info( " .. used by solver:          %10.3f MB",
                      solver->memory_use()/MB );

        if (stats.get() != NULL)
        {
            long long lifts_total       = stats->lifts_attempted();
            long long lifts_successful  = stats->lifts_succeeded();
            long long lifts_failed      = lifts_total - lifts_successful;

            Logger::info( "Lifting attempts failed:      %12lld",
                           lifts_failed );
            Logger::info( "Lifting attempts succeeded:   %12lld",
                           lifts_successful );
            Logger::info( "Total lifting attempts:       %12lld",
                           lifts_total );
            Logger::info( "Minimum lifts required:    %12lld",
                           0LL);  // TODO
        }

        if (!failed && arg_verify)
        {
            Timer timer;

            Logger::info("Starting verification...");
            if (game.verify(strategy))
            {
                Logger::info("Verification succeeded.");
            }
            else
            {
                failed = true;
                Logger::info("Verification failed!");
            }
            Logger::info( "Time used to verify:         %10.3f s",
                           timer.elapsed() );
        }

        write_output(game, strategy);
    }

    Logger::info("Exiting.");

    exit(failed ? EXIT_FAILURE : EXIT_SUCCESS);
}
