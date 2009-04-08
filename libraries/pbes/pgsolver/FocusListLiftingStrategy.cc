#include "FocusListLiftingStrategy.h"
#include <assert.h>

/*! Credit for a vertex when it is put on the focus list. */
static const unsigned initial_credit  = 2;

/*! Credit increase when a vertex on the focus list is successfully lifted. */
static const unsigned credit_increase = 2;


FocusListLiftingStrategy::FocusListLiftingStrategy(
    const ParityGame &game, bool backward, size_t max_size )
    : LiftingStrategy(game), max_size_(max_size), pass_(1),
      lls_(game, backward), last_vertex_(NO_VERTEX), last_lifted_(false),
      num_lift_attempts_(0), focus_list_(), focus_pos_(focus_list_.end())
{
}

verti FocusListLiftingStrategy::next(verti prev_vertex, bool prev_lifted)
{
    verti res;
    switch (pass_)
    {
    case 1:
        res = pass1(prev_vertex, prev_lifted);
        break;

    case 2:
        res = pass2(prev_vertex, prev_lifted);
        break;

    default:
        res = NO_VERTEX;
        assert(0);
        break;
    }
    return res;
}

verti FocusListLiftingStrategy::pass1(verti prev_vertex, bool prev_lifted)
{
    last_vertex_ = prev_vertex;
    last_lifted_ = prev_lifted;

    /* Check if last vertex was successfully lifted */
    if (prev_lifted)
    {
        // Put successfully lifted vertex on the focus list
        assert(prev_vertex != NO_VERTEX);
        focus_list_.push_back(std::make_pair(prev_vertex, initial_credit));
    }

    if ( focus_list_.size() == max_size_ ||
         num_lift_attempts_ == game_.graph().V() )
    {
        // Switch to pass 2
        pass_ = 2;
        return pass2(NO_VERTEX, false);
    }

    num_lift_attempts_ += 1;
    return lls_.next(last_vertex_, last_lifted_);
}

verti FocusListLiftingStrategy::pass2(verti prev_vertex, bool prev_lifted)
{
    if (prev_vertex == NO_VERTEX)
    {
        // Position at start of the focus list
        assert(!focus_list_.empty());
        focus_pos_ = focus_list_.begin();
    }
    else
    {
        // Adjust previous vertex credit and move to next position
        focus_list::iterator old_pos = focus_pos_++;
        assert(old_pos->first == prev_vertex);
        if (prev_lifted)
        {
            old_pos->second += credit_increase;
        }
        else
        {
            old_pos->second /= 2;
            if (old_pos->second == 0)
            {
                focus_list_.erase(old_pos);
            }
        }
    }

    // Check if we've reached the end of the focus list
    if (focus_pos_ == focus_list_.end())
    {
        if (focus_list_.empty())
        {
            // Focus list exhausted; move back to pass 1
            pass_ = 1;
            num_lift_attempts_ = 0;
            return pass1(last_vertex_, last_lifted_);
        }
        else
        {
            // Restart at beginning of the list
            focus_pos_ = focus_list_.begin();
        }
    }

    // Return current item on the focus list
    return focus_pos_->first;
}

size_t FocusListLiftingStrategy::memory_use() const
{
    return max_size_*sizeof(focus_list::value_type);
}
