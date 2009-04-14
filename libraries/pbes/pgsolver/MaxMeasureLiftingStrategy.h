#ifndef MAX_MEASURE_LIFTING_STRATEGY_H_INCLUDED
#define MAX_MEASURE_LIFTING_STRATEGY_H_INCLUDED

#include "SmallProgressMeasures.h"
#include <vector>
#include <set>
#include <utility>

class MaxMeasureLiftingStrategy : public LiftingStrategy
{
public:
    MaxMeasureLiftingStrategy(const ParityGame &game);
    ~MaxMeasureLiftingStrategy();

    verti next(verti prev_vertex, bool prev_lifted);
    size_t memory_use() const;

protected:
    std::vector<verti> vec(verti v) {
        return std::vector<verti>(spm_->vec(v), spm_->vec(v) + spm_->len(v));
    }

private:
    typedef std::set<std::pair<std::vector<verti>, verti> > queue_t;
    queue_t queue_;
    std::vector<queue_t::iterator> queue_pos_;
};

#endif /* ndef PREDECESSOR_LIFTING_STRATEGY_H_INCLUDED */
