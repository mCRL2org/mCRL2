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

#include "logging.h"
#include "timing.h"
#include "ParityGame.h"
#include "LinearLiftingStrategy.h"
#include "PredecessorLiftingStrategy.h"
#include "ComponentSolver.h"
#include "GraphOrdering.h"

#include <aterm_init.h>

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
static std::string  arg_spm_lifting_strategy  = "";
static bool         arg_scc_decomposition     = false;
static bool         arg_solve_dual            = false;
static Reordering   arg_reordering            = REORDER_NONE;
static int          arg_random_size           = 1000000;
static int          arg_random_seed           =       1;
static int          arg_random_out_degree     =      10;
static int          arg_random_priorities     =      20;
static int          arg_timeout               =       0;

static const double MB = 1048576.0;  // one megabyte

static volatile ParityGameSolver *g_solver = NULL;
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
"  --strategy/-l <desc>   Small Progress Measures lifting strategy\n"
"  --dot/-d <file>        write parity game in GraphViz dot format to <file>\n"
"  --pgsolver/-p <file>   write parity game in PGSolver format to <file>\n"
"  --raw/-r <file>        write parity game in raw format to <file>\n"
"  --winners/-w <file>    write compact winners specification to <file>\n"
"  --scc                  solve strongly connected components individually\n"
"  --dual                 solve the dual game\n"
"  --reorder/-e (bfs|dfs) reorder vertices\n"
"  --timeout/-t <t>       abort solving after <t> seconds\n");
}

static void parse_args(int argc, char *argv[])
{
    static struct option long_options[] = {
        { "help",       false, NULL, 'h' },
        { "input",      true,  NULL, 'i' },
        { "size",       true,  NULL,  1  },
        { "outdegree",  true,  NULL,  2  },
        { "priorities", true,  NULL,  3  },
        { "seed",       true,  NULL,  4  },
        { "strategy",   true,  NULL, 'l' },
        { "dot",        true,  NULL, 'd' },
        { "pgsolver",   true,  NULL, 'p' },
        { "raw",        true,  NULL, 'r' },
        { "winners",    true,  NULL, 'w' },
        { "scc",        false, NULL,  5  },
        { "dual",       false, NULL,  6  },
        { "reorder",    true,  NULL, 'e' },
        { "timeout",    true,  NULL, 't' },
        { NULL,         false, NULL,  0  } };

    static const char *short_options = "hi:l:d:p:r:w:e:t:";

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

#ifdef POSIX
static void alarm_handler(int sig)
{
    if (sig == SIGALRM && !g_timed_out)
    {
        g_timed_out = true;
        if (g_solver != NULL) ((ParityGameSolver*)g_solver)->abort();
    }
}

static void set_timeout(int t)
{
    g_timed_out = false;

    /* Set handler for alarm signal */
    struct sigaction act;
    act.sa_handler = &alarm_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    /* Schedule alarm signal */
    int res = sigaction(SIGALRM, &act, NULL);
    if (res != 0)
    {
        warn("Couldn't install signal handler.");
    }
    else
    {
        alarm(arg_timeout);
    }
}
#else
static void set_timeout(int t)
{
    g_timed_out = false;
    warn("Time-out not available.");
}
#endif

int main(int argc, char *argv[])
{
    time_initialize();

#ifdef WITH_MCRL2
    MCRL2_ATERMPP_INIT(argc, argv);
#endif

    parse_args(argc, argv);


    ParityGame game;
    if (!read_input(game))
    {
        fatal("Couldn't parse parity game from input!");
    }

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

    /* Print some game info: */
    info("Number of vertices:        %12lld", (long long)game.graph().V());
    info("Number of edges:           %12lld", (long long)game.graph().E());
    info("Forward edge ratio:        %.10f",
          (double)count_forward_edges(game.graph())/game.graph().E() );
    info("Number of priorities:      %12d (was %d)", game.d(), old_d);
    for (int p = 0; p < game.d(); ++p)
        info("  %2d occurs %d times", p, game.cardinality(p));

    bool failed = true;

    if (arg_spm_lifting_strategy.empty())
    {
        /* Don't solve; just convert data. */
        write_output(game, NULL);
        failed = false;
    }
    else
    {
        LiftingStatistics stats(game);
        info( "SPM lifting strategy:      %12s",
              arg_spm_lifting_strategy.c_str() );

        if (arg_timeout > 0) set_timeout(0);

        double solve_time = time_used();
        info("Starting solve...");

        // Allocate data structures
        ParityGameSolver *solver;
        std::auto_ptr<ComponentSolver> comp_solver;
        std::auto_ptr<LiftingStrategy> spm_strategy;
        std::auto_ptr<SmallProgressMeasures> spm;
        if (arg_scc_decomposition)
        {
            comp_solver.reset(
                new ComponentSolver(game, arg_spm_lifting_strategy, &stats) );
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
        g_solver = solver;
        if (!g_timed_out)
        {
            failed = !solver->solve();
        }
        g_solver = NULL;

        if (failed)
        {
            if (solver->aborted())
            {
                error("time limit exceeded!");
            }
            else
            {
                error("solving failed!");
            }
        }


        solve_time = time_used() - solve_time;

        long long lifts_total       = stats.lifts_attempted();
        long long lifts_successful  = stats.lifts_succeeded();
        long long lifts_failed      = lifts_total - lifts_successful;

        // Print some statistics
        info("Time used to solve:          %10.3f s", solve_time);
        // info("Peak memory usage:           %10.3f MB", get_vmsize()); // TODO
        size_t total_memory_use = game.memory_use() + solver->memory_use();
        info("Memory required to solve:    %10.3f MB", total_memory_use /MB);
        info(" .. used by parity game:     %10.3f MB", game.memory_use()/MB);
        info("     .. used by graph:       %10.3f MB", game.graph().memory_use()/MB);
        info(" .. used by solver:          %10.3f MB", solver->memory_use()/MB);
        info("Lifting attempts failed:      %12lld", lifts_failed);
        info("Lifting attempts succeeded:   %12lld", lifts_successful);
        info("Total lifting attempts:       %12lld", lifts_total);
        // info("Minimum lifts required:    %12lld", 0LL);  // TODO

        write_output(game, solver);
        solver = NULL;
    }

    info("Exiting.");

    exit(failed ? EXIT_FAILURE : EXIT_SUCCESS);
}
