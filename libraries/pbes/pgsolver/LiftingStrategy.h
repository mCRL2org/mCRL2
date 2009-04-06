#ifndef LIFTING_STRATEGY_H_INCLUDED
#define LIFTING_STRATEGY_H_INCLUDED

#include "ParityGame.h"
#include <string>

class SmallProgressMeasures;

/*! Instances of this class encapsulate vertex lifting strategies to be used
    with the small progress measures parity game solver. */
class LiftingStrategy
{
public:
    /*! Create a lifting strategy for the given game from a string description.
        Returns NULL if the  description could not be interpreted.

        String descriptions are as follows:

            linear:backward
                Use a linear lifting strategy (swiping).
                If backward is non-zero, scan vertices backward.
                Default: linear:0

            predecessor:backward:stack
                Use a predecessor lifting strategy (worklist).
                If backward is non-zero, scan vertices backward.
                If stack is non-zero, use a stack instead of a queue.
                Default: predecessor:0:0

            focuslist:backward:max_size
                Use a lifting strategy with a focus list.
                If backward is non-zero, scan vertices backward.
                max_size specificies the maximum size of the focus list, either
                as an absolute size greater than 1, or as a ratio between zero
                and 1, relative to the total number of vertices in the graph.
                Default: focuslist:0:0.1
    */
    static LiftingStrategy *create( const ParityGame &game,
                                    const std::string description );

    /*! Construct a strategy for the given parity game. */
    LiftingStrategy(const ParityGame &game)
        : graph_(game.graph()), game_(game), spm_(NULL) { };

    /*! Destroy the strategy */
    virtual ~LiftingStrategy() { };

    /*! Select the next vertex to lift.

        This method is called repeatedly by the SPM solver; the return value
        indicates which vertex to attempt to lift next. If lifting succeeds,
        the vertex will have a greater progress measure vector assigned to it.
        When no more vertices can be lifted, NO_VERTEX should be returned.

        \param prev_vertex Index of the vertex returned by the previous call
                           (or NO_VERTEX for the first call).
        \param prev_lifted Indicates wheter the vertex could be lifted.
    */
    virtual verti next(verti prev_vertex, bool prev_lifted) = 0;

    /*! Returns an estimation of the peak memory use for this strategy. */
    virtual size_t memory_use() const { return 0; }

    /*! Sets the SPM instance that uses this strategy */
    void spm(SmallProgressMeasures *spm) { spm_ = spm; }

    /*! Returns the SPM instance that uses this strategy */
    SmallProgressMeasures *spm() { return spm_; }

protected:
    const StaticGraph &graph_;      //!< the game graph to work on
    const ParityGame &game_;        //!< the parity game to work on
    SmallProgressMeasures *spm_;    //!< the SPM instance using this strategy
};

#endif /* ndef LIFTING_STRATEGY_H_INCLUDED */
