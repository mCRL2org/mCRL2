// Interface to class BDD_Simplifier
// file: bdd_simplifier.h

#ifndef BDD_SIMPLIFIER_H
#define BDD_SIMPLIFIER_H

#include "aterm2.h"

  /// \brief A base class for classes that simplify binary decision diagrams. These classes take
  /// \brief a BDD as input and try to simplify this BDD.

class BDD_Simplifier {
  protected:
    /// \brief An integer representing the moment in time when the maximal amount of seconds has been spent on simplifying the BDD.
    int f_deadline;
  public:
    /// \brief Destructor without any additional functionality.
    virtual ~BDD_Simplifier();

    /// \brief Sets the attribute BDD_Simplifier::f_deadline.
    void set_time_limit(int a_time_limit);

    /// \brief Returns a simplified BDD, equivalent to the bdd a_bdd.
    virtual ATermAppl simplify(ATermAppl a_bdd);
};

#endif
