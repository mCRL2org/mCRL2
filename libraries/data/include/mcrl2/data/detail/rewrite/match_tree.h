// Author(s): Muck van Weerdenburg/Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite/jittyc.h

#ifndef MCRL2_DATA_MATCH_TREE_H
#define MCRL2_DATA_MATCH_TREE_H

#include "mcrl2/data/function_symbol.h"
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/data/machine_number.h"

namespace mcrl2::data::detail
{

/// This is a list where variables and aterm ints can be stored.
class variable_or_number: public atermpp::aterm
{
  public:
    /// Default constructor
    variable_or_number() = default;

    /// Constructor
    variable_or_number(const atermpp::aterm& v):
       atermpp::aterm(v)
    {
      assert(is_variable(v) || v.type_is_int());
    }
};

using variable_or_number_list = atermpp::term_list<variable_or_number>;

class match_tree:public atermpp::aterm
{
  public:
    /// Default constructor
    match_tree()
     : atermpp::aterm(afunUndefined())
    {}

    /// Constructor based on an aterm.
    match_tree(const atermpp::aterm& t):
      atermpp::aterm(t)
    {
      assert(!is_defined() || isS() || isA() || isM() || isF() || isMachineNumber() ||
             isN() || isD() || isR () || isC() || isX() || isRe() || 
             isCRe() || isMe());
    }
  
  protected:
    atermpp::function_symbol afunUndefined() const
    {
      static atermpp::function_symbol afunUndefined("@@Match_tree_dummy",0); // Undefined match term. Used as default match term
      return afunUndefined;
    }

    atermpp::function_symbol afunS() const
    {
      static atermpp::function_symbol afunS("@@S",2); // Store term ( target_variable, result_tree )
      return afunS;
    }

    atermpp::function_symbol afunA() const
    {
      static atermpp::function_symbol afunA("@@A",1); // Rewrite argument ( number of an argument as an aterm_int  )
      return afunA;
    }


    atermpp::function_symbol afunM() const
    {
      static atermpp::function_symbol afunM("@@M",3); // Match term ( match_variable, true_tree , false_tree )
      return afunM;
    }

    atermpp::function_symbol afunF() const
    {
      static atermpp::function_symbol afunF("@@F",3); // Match function ( match_function, true_tree, false_tree )
      return afunF;
    }

    atermpp::function_symbol afunMachineNumber() const
    {
      static atermpp::function_symbol afunNumber("@@MachineNumber",3); // Match function ( match_function, true_tree, false_tree )
      return afunNumber;
    } 
    
    atermpp::function_symbol afunN() const
    {
      static atermpp::function_symbol afunN("@@N",1); // Go to next parameter ( result_tree )
      return afunN;
    }

    atermpp::function_symbol afunD() const
    {
      static atermpp::function_symbol afunD("@@D",1); // Go down a level ( result_tree )
      return afunD;
    }

    atermpp::function_symbol afunR() const
    {
      static atermpp::function_symbol afunR("@@R",1); // End of tree ( matching_rule )
      return afunR;
    }

    atermpp::function_symbol afunC() const
    {
      static atermpp::function_symbol afunC("@@C",3); // Check condition ( condition, true_tree, false_tree )
      return afunC;
    }

    atermpp::function_symbol afunX() const
    {
      static atermpp::function_symbol afunX("@@X",0); // End of tree
      return afunX;
    }

    atermpp::function_symbol afunRe() const
    {
      static atermpp::function_symbol afunRe("@@Re",2); // End of tree ( matching_rule , vars_of_rule)
      return afunRe;
    }

    atermpp::function_symbol afunCRe() const
    {
      static atermpp::function_symbol afunCRe("@@CRe",4); // End of tree ( condition, matching_rule, vars_of_condition, vars_of_rule )
      return afunCRe;
    }

    atermpp::function_symbol afunMe() const
    {
      static atermpp::function_symbol afunMe("@@Me",2); // Match term ( match_variable, variable_index )
      return afunMe;
    }

  public:
    bool is_defined() const
    {
      return this->function()!=afunUndefined();
    }

    bool isS() const
    {
      return this->function()==afunS();
    }

    bool isA() const
    {
      return this->function()==afunA();
    }
      
    bool isM() const
    {
      return this->function()==afunM();
    }
      
    bool isF() const
    {
      return this->function()==afunF();
    }

    bool isMachineNumber() const
    {
      return this->function()==afunMachineNumber();
    }
      
    bool isN() const
    {
      return this->function()==afunN();
    }
      
    bool isD() const
    {
      return this->function()==afunD();
    }
      
    bool isR() const
    {
      return this->function()==afunR();
    }
      
    bool isC() const
    {
      return this->function()==afunC();
    }
      
    bool isX() const
    {
      return this->function()==afunX();
    }
      
    bool isRe() const
    {
      return this->function()==afunRe();
    }
      
    bool isCRe() const
    {
      return this->function()==afunCRe();
    }
      
    bool isMe() const
    {
      return this->function()==afunMe();
    }
};

// Store term ( target_variable, result_tree )
class match_tree_S:public match_tree
{
  public:
    match_tree_S() = default;

    match_tree_S(const atermpp::aterm& t)
     : match_tree(t)
    {
      assert(isS());
    }
    
    match_tree_S(const variable& target_variable, const match_tree& result_tree)
     : match_tree(atermpp::aterm(afunS(),target_variable,result_tree))
    {}

    const variable& target_variable() const
    {
      return atermpp::down_cast<const variable>((*this)[0]);
    }

    const match_tree& subtree() const
    {
      return atermpp::down_cast<const match_tree>((*this)[1]);
    }
};

// Rewrite argument ( a number of an arguments as a aterm_int )

class match_tree_A:public match_tree
{
  public:
    match_tree_A() = default;

    match_tree_A(const atermpp::aterm& t)
     : match_tree(t)
    {
      assert(isA());
    }

    match_tree_A(const std::size_t n)
     : match_tree(atermpp::aterm(afunA(),atermpp::aterm_int(n)))
    {}

    std::size_t variable_index() const
    {
      return atermpp::down_cast<const atermpp::aterm_int>((*this)[0]).value();
    }
};


// Match term ( match_variable, true_tree , false_tree )
class match_tree_M:public match_tree
{
  public:
    match_tree_M() = default;

    match_tree_M(const atermpp::aterm& t)
     : match_tree(t)
    {
      assert(isM());
    }
    
    match_tree_M(const variable& match_variable, const match_tree& true_tree, const match_tree& false_tree)
     : match_tree(atermpp::aterm(afunM(),match_variable,true_tree,false_tree))
    {}

    const variable& match_variable() const
    {
      return atermpp::down_cast<const variable>((*this)[0]);
    }

    const match_tree& true_tree() const
    {
      return atermpp::down_cast<const match_tree>((*this)[1]);
    }

    const match_tree& false_tree() const
    {
      return atermpp::down_cast<const match_tree>((*this)[2]);
    }
};

// Match function ( match_function, true_tree, false_tree )
class match_tree_F: public match_tree
{
  public:
    match_tree_F() = default;

    match_tree_F(const atermpp::aterm& t)
     : match_tree(t)
    {
      assert(isF());
    }
    
    match_tree_F(const data::function_symbol& function, const match_tree& true_tree, const match_tree& false_tree)
     : match_tree(atermpp::aterm(afunF(),function,true_tree,false_tree))
    {}

    const data::function_symbol& function() const
    {
      return atermpp::down_cast<const data::function_symbol>((*this)[0]);
    }

    const match_tree& true_tree() const
    {
      return atermpp::down_cast<const match_tree>((*this)[1]);
    }

    const match_tree& false_tree() const
    {
      return atermpp::down_cast<const match_tree>((*this)[2]);
    }
};

// Match function ( match_function, true_tree, false_tree )
class match_tree_MachineNumber: public match_tree
{
  public:
    match_tree_MachineNumber() = default;

    match_tree_MachineNumber(const atermpp::aterm& t):
          match_tree(t)
    {
      assert(isMachineNumber());
    }
    
    match_tree_MachineNumber(const data::machine_number& mn, const match_tree& true_tree, const match_tree& false_tree):
          match_tree(atermpp::aterm(afunMachineNumber(),mn,true_tree,false_tree))
    {}
    
    const data::machine_number& number() const
    {
      return atermpp::down_cast<const data::machine_number>((*this)[0]);
    }

    const match_tree& true_tree() const
    {
      return atermpp::down_cast<const match_tree>((*this)[1]);
    } 

    const match_tree& false_tree() const
    {
      return atermpp::down_cast<const match_tree>((*this)[2]);
    }
};


// Go to next parameter ( result_tree )
class match_tree_N:public match_tree
{
  public:
    match_tree_N() = default;

    match_tree_N(const atermpp::aterm& t)
     : match_tree(t)
    {
      assert(isN());
    }
    
    /// Constructor. Builds a new term around a match_tree.
    /// The extra non-used std::size_t is provided, to distinghuish this
    /// constructor from the default copy constructor.
    match_tree_N(const match_tree& result_tree, std::size_t)
     : match_tree(atermpp::aterm(afunN(),result_tree))
    {}

    const match_tree& subtree() const
    {
      return atermpp::down_cast<const match_tree>((*this)[0]);
    }
};

// Go down a level ( result_tree )
class match_tree_D:public match_tree
{
  public:
    match_tree_D() = default;

    match_tree_D(const atermpp::aterm& t)
     : match_tree(t)
    {
      assert(isD());
    }
    
    /// Constructor. Builds a new term around a match_tree.
    /// The extra non-used std::size_t is provided, to distinghuish this
    /// constructor from the default copy constructor.
    match_tree_D(const match_tree& result_tree, std::size_t)
     : match_tree(atermpp::aterm(afunD(),result_tree))
    {}

    const match_tree& subtree() const
    {
      return atermpp::down_cast<const match_tree>((*this)[0]);
    }
};

// End of tree ( matching_rule )
class match_tree_R:public match_tree
{

  public:
    match_tree_R() = default;

    match_tree_R(const atermpp::aterm& t)
     : match_tree(t)
    {
      assert(isR());
    }
    
    match_tree_R(const data_expression& e)
     : match_tree(atermpp::aterm(afunR(),e))
    {}

    const data_expression& result() const
    {
      return atermpp::down_cast<const data_expression>((*this)[0]);
    }
};

// Check condition ( condition, true_tree, false_tree )
class match_tree_C:public match_tree
{
  public:
    match_tree_C() = default;

    match_tree_C(const atermpp::aterm& t)
     : match_tree(t)
    {
      assert(isC());
    }
    
    match_tree_C(const data_expression& condition, const match_tree& true_tree, const match_tree& false_tree)
     : match_tree(atermpp::aterm(afunC(),condition,true_tree,false_tree))
    {}

    const data_expression& condition() const
    {
      return atermpp::down_cast<const data_expression>((*this)[0]);
    }

    const match_tree& true_tree() const
    {
      return atermpp::down_cast<const match_tree>((*this)[1]);
    }

    const match_tree& false_tree() const
    {
      return atermpp::down_cast<const match_tree>((*this)[2]);
    }
};

// End of tree
class match_tree_X:public match_tree
{
  public:
    match_tree_X(const atermpp::aterm& t)
     : match_tree(t)
    {
      assert(isX());
    }
    
    match_tree_X()
     : match_tree(atermpp::aterm(afunX()))
    {}
};

// End of tree ( matching_rule , vars_of_rule)
// The var_of_rule is a list with variables and aterm_ints.
class match_tree_Re:public match_tree
{
  public:
    match_tree_Re() = default;

    match_tree_Re(const atermpp::aterm& t)
     : match_tree(t)
    {
      assert(isRe());
    }
    
    match_tree_Re(const data_expression& result, const variable_or_number_list& vars)
     : match_tree(atermpp::aterm(afunRe(),result,vars))
    {}

    const data_expression& result() const
    {
      return atermpp::down_cast<const data_expression>((*this)[0]);
    }

    const variable_or_number_list& variables() const
    {
      return atermpp::down_cast<const variable_or_number_list>((*this)[1]);
    }
};

// End of tree ( condition, matching_rule, vars_of_condition, vars_of_rule )
// The last two parameters consist of a list with variables and numbers.
class match_tree_CRe:public match_tree
{
  public:
    match_tree_CRe() = default;

    match_tree_CRe(const atermpp::aterm& t)
     : match_tree(t)
    {
      assert(isCRe());
    }
    
    match_tree_CRe(const data_expression& condition, const data_expression& result, const variable_or_number_list& vars_condition, const variable_or_number_list& vars_rule)
     : match_tree(atermpp::aterm(afunCRe(),condition,result,vars_condition,vars_rule))
    {}

    const data_expression& condition() const
    {
      return atermpp::down_cast<const data_expression>((*this)[0]);
    }

    const data_expression& result() const
    {
      return atermpp::down_cast<const data_expression>((*this)[1]);
    }

    const variable_or_number_list& variables_condition() const
    {
      return atermpp::down_cast<const variable_or_number_list>((*this)[2]);
    }

    const variable_or_number_list& variables_result() const
    {
      return  atermpp::down_cast<const variable_or_number_list>((*this)[3]);
    }
};

// Match term ( match_variable, variable_index )
class match_tree_Me:public match_tree
{
  public:
    match_tree_Me() = default;

    match_tree_Me(const atermpp::aterm& t)
     : match_tree(t)
    {
      assert(isMe());
    }
    
    match_tree_Me(const variable& match_variable, const std::size_t variable_index)
     : match_tree(atermpp::aterm(afunMe(),match_variable,atermpp::aterm_int(variable_index)))
    {}

    const variable& match_variable() const
    {
      return atermpp::down_cast<const variable>((*this)[0]);
    }

    std::size_t variable_index() const
    {
      return (atermpp::down_cast<const atermpp::aterm_int>((*this)[1])).value();
    }
};

using match_tree_list = atermpp::term_list<match_tree>;
using match_tree_vector = std::vector<match_tree>;
using match_tree_list_list = atermpp::term_list<match_tree_list>;
using match_tree_list_list_list = atermpp::term_list<match_tree_list_list>;

// Structure for build_tree parameters
class build_pars
{
public:
  match_tree_list_list Flist;       // List of sequences of which the first action is an F
  match_tree_list_list Slist;       // List of sequences of which the first action is an S
  match_tree_list_list Mlist;       // List of sequences of which the first action is an M
  match_tree_list_list_list stack;  // Stack to maintain the sequences that do not have to
                                    // do anything in the current term
  match_tree_list_list upstack;     // List of sequences that have done an F at the current
                                    // level

  // Initialise. 
  build_pars()
   : Flist(),
     Slist(),
     Mlist(),
     stack({ match_tree_list_list() }),
     upstack()

  {
  }
};


inline
std::ostream& operator<<(std::ostream& s, const match_tree& t)
{
  using atermpp::down_cast;
  if (t.isS())
  {
    const match_tree_S& tS = down_cast<match_tree_S>(t);
    s << "@@S(" << tS.target_variable() << ", " << tS.subtree() << ")";
  }
  else
  if (t.isA())
  {
    const match_tree_A& tA = down_cast<match_tree_A>(t);
    s << "@@A(" << tA.variable_index() << ")";
  }
  else
  if (t.isM())
  {
    const match_tree_M& tM = down_cast<match_tree_M>(t);
    s << "@@M(" << tM.match_variable() << ", " << tM.true_tree() << ", " << tM.false_tree() << ")";
  }
  else
  if (t.isF())
  {
    const match_tree_F& tF = down_cast<match_tree_F>(t);
    s << "@@F(" << tF.function() << ", " << tF.true_tree() << ", " << tF.false_tree() << ")";
  }
  else
  if (t.isMachineNumber())
  { 
    const match_tree_MachineNumber& tM = down_cast<match_tree_MachineNumber>(t);
    s << "@@MachineNumber(" << tM.function() << ", " << tM.true_tree() << ", " << tM.false_tree() << ")";
  }       
  else
  if (t.isN())
  {
    const match_tree_N& tN = down_cast<match_tree_N>(t);
    s << "@@N(" << tN.subtree() << ")";
  }
  else
  if (t.isD())
  {
    const match_tree_D& tD = down_cast<match_tree_D>(t);
    s << "@@D(" << tD.subtree() << ")";
  }
  else
  if (t.isR())
  {
    const match_tree_R& tR = down_cast<match_tree_R>(t);
    s << "@@R(" << tR.result() << ")";
  }
  else
  if (t.isC())
  {
    const match_tree_C& tC = down_cast<match_tree_C>(t);
    s << "@@C(" << tC.condition() << ", " << tC.true_tree() << ", " << tC.false_tree() << ")";
  }
  else
  if (t.isX())
  {
    s << "@@X";
  }
  else
  if (t.isRe())
  {
    const match_tree_Re& tRe = down_cast<match_tree_Re>(t);
    s << "@@Re(" << tRe.result() << ", " << tRe.variables() << ")";
  }
  else
  if (t.isCRe())
  {
    const match_tree_CRe& tCRe = down_cast<match_tree_CRe>(t);
    s << "@@CRe(" << tCRe.condition() << ", " << tCRe.result() << ", "
      << tCRe.variables_condition() << ", " << tCRe.variables_result() << ")";
  }
  else
  if (t.isMe())
  {
    const match_tree_Me& tMe = down_cast<match_tree_Me>(t);
    s << "@@Me(" << tMe.match_variable() << ", " << tMe.variable_index() << ")";
  }
  return s;
}

} // namespace mcrl2::data::detail

#endif // MCRL2_DATA_MATCH_TREE_H
