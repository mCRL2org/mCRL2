#ifndef PARITY_GAME_SOLVER
#define PARITY_GAME_SOLVER

#include "ParityGame.h"

class ParityGameSolver
{
public:
    ParityGameSolver(const ParityGame &game) : game_(game) { };
    virtual ~ParityGameSolver() { };

    /*! Solve the game. */
    virtual bool solve() = 0;

    /*! After the game has been solved, this function returns the winner of
        the parity game when starting from vertex i. */
    virtual ParityGame::Player winner(verti v) const = 0;

    /*! Returns an estimation of the peak memory use for this solver. */
    virtual size_t memory_use() const = 0;

    /*! Returns the parity game for this solver instance. */
    const ParityGame &game() const { return game_; }

protected:
    const ParityGame &game_;
};

#endif /* ndef PARITY_GAME_SOLVER */
