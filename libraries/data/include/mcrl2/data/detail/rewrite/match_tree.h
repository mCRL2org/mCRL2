// Author(s): Muck van Weerdenburg/Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite/jittyc.h

#ifndef __MATCH_TREE_H
#define __MATCH_TREE_H

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/variable.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

/// This is a list where variables and aterm ints can be stored.
class variable_or_number: public atermpp::aterm
{
  public:
    /// Default constructor
    variable_or_number()
    {}

    /// Constructor
    variable_or_number(const atermpp::aterm& v):
       atermpp::aterm(v)
    {
      assert(is_variable(atermpp::down_cast<atermpp::aterm_appl>(v)) || v.type_is_int());
    }
};

typedef atermpp::term_list<variable_or_number> variable_or_number_list;

class match_tree:public atermpp::aterm_appl
{
  public:
    /// Default constructor
    match_tree():
      atermpp::aterm_appl(afunUndefined())
    {}

  protected:
    /// Constructor based on an aterm.
    match_tree(const atermpp::aterm_appl& t):
      atermpp::aterm_appl(t)
    {}
  
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
    match_tree_S()
    {}

    match_tree_S(const atermpp::aterm_appl& t):
          match_tree(t)
    {
      assert(isS());
    }
    
    match_tree_S(const variable& target_variable, const match_tree& result_tree):
          match_tree(atermpp::aterm_appl(afunS(),target_variable,result_tree))
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
    match_tree_A()
    {}

    match_tree_A(const atermpp::aterm_appl& t):
          match_tree(t)
    {
      assert(isA());
    }

    match_tree_A(const std::size_t n):
          match_tree(atermpp::aterm_appl(afunA(),atermpp::aterm_int(n)))
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
    match_tree_M()
    {}

    match_tree_M(const atermpp::aterm_appl& t):
          match_tree(t)
    {
      assert(isM());
    }
    
    match_tree_M(const variable& match_variable, const match_tree& true_tree, const match_tree& false_tree):
          match_tree(atermpp::aterm_appl(afunM(),match_variable,true_tree,false_tree))
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
class match_tree_F:public match_tree
{
  public:
    match_tree_F()
    {}

    match_tree_F(const atermpp::aterm_appl& t):
          match_tree(t)
    {
      assert(isF());
    }
    
    match_tree_F(const data::function_symbol& function, const match_tree& true_tree, const match_tree& false_tree):
          match_tree(atermpp::aterm_appl(afunF(),function,true_tree,false_tree))
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

// Go to next parameter ( result_tree )
class match_tree_N:public match_tree
{
  public:
    match_tree_N()
    {}

    match_tree_N(const atermpp::aterm_appl& t):
          match_tree(t)
    {
      assert(isN());
    }
    
    /// Constructor. Builds a new term around a match_tree.
    /// The extra non-used std::size_t is provided, to distinghuish this
    /// constructor from the default copy constructor.
    match_tree_N(const match_tree& result_tree, std::size_t):
          match_tree(atermpp::aterm_appl(afunN(),result_tree))
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
    match_tree_D()
    {}

    match_tree_D(const atermpp::aterm_appl& t):
          match_tree(t)
    {
      assert(isD());
    }
    
    /// Constructor. Builds a new term around a match_tree.
    /// The extra non-used std::size_t is provided, to distinghuish this
    /// constructor from the default copy constructor.
    match_tree_D(const match_tree& result_tree, std::size_t):
          match_tree(atermpp::aterm_appl(afunD(),result_tree))
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
    match_tree_R()
    {}

    match_tree_R(const atermpp::aterm_appl& t):
          match_tree(t)
    {
      assert(isR());
    }
    
    match_tree_R(const data_expression& e):
          match_tree(atermpp::aterm_appl(afunR(),e))
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
    match_tree_C()
    {}

    match_tree_C(const atermpp::aterm_appl& t):
          match_tree(t)
    {
      assert(isC());
    }
    
    match_tree_C(const data_expression& condition, const match_tree& true_tree, const match_tree& false_tree):
        match_tree(atermpp::aterm_appl(afunC(),condition,true_tree,false_tree))
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
    match_tree_X(const atermpp::aterm_appl& t):
          match_tree(t)
    {
      assert(isX());
    }
    
    match_tree_X():
        match_tree(atermpp::aterm_appl(afunX()))
    {}
};

// End of tree ( matching_rule , vars_of_rule)
// The var_of_rule is a list with variables and aterm_ints.
class match_tree_Re:public match_tree
{
  public:
    match_tree_Re()
    {}

    match_tree_Re(const atermpp::aterm_appl& t):
          match_tree(t)
    {
      assert(isRe());
    }
    
    match_tree_Re(const data_expression& result, const variable_or_number_list& vars):
           match_tree(atermpp::aterm_appl(afunRe(),result,vars))
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
    match_tree_CRe()
    {}

    match_tree_CRe(const atermpp::aterm_appl& t):
          match_tree(t)
    {
      assert(isCRe());
    }
    
    match_tree_CRe(const data_expression& condition, const data_expression& result, const variable_or_number_list& vars_condition, const variable_or_number_list& vars_rule):
           match_tree(atermpp::aterm_appl(afunCRe(),condition,result,vars_condition,vars_rule))
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
    match_tree_Me()
    {}

    match_tree_Me(const atermpp::aterm_appl& t):
          match_tree(t)
    {
      assert(isMe());
    }
    
    match_tree_Me(const variable& match_variable, const std::size_t variable_index):
           match_tree(atermpp::aterm_appl(afunMe(),match_variable,atermpp::aterm_int(variable_index)))
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

typedef atermpp::term_list < match_tree > match_tree_list;
typedef std::vector < match_tree > match_tree_vector;
typedef atermpp::term_list < match_tree_list > match_tree_list_list;
typedef atermpp::term_list < match_tree_list_list > match_tree_list_list_list;

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

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // __MATCH_TREE_H
