// Author(s): Egbert Teeselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LYSA_H
#define MCRL2_LYSA_H

#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <cstdio>

#include <boost/smart_ptr.hpp>

/*
* we use the standard mCRL2 toolset functions for console messages. these functions
* (and the patched FlexLexer.h workaround) are the only direct dependencies on the mCRL2
* toolset code for the parser itself.
*
* As such, if you want to reuse the LySa parser outside the mCRL2 toolset, reimplement 
* the following functions (and copy build/workarounds/all/FlexLexer.h to somewhere 
* sensible).
*/

#include "mcrl2/core/messaging.h"
using mcrl2::core::gsErrorMsg;
using mcrl2::core::gsVerboseMsg;
using mcrl2::core::gsWarningMsg;
using mcrl2::core::gsDebugMsg;


namespace lysa
{
#define S(v) ((string)(*(v)))
  using boost::shared_ptr;
  using boost::dynamic_pointer_cast;
  using boost::static_pointer_cast;
  using boost::weak_ptr;
  using std::string;	
  using std::vector;

  //identical to bison's YYLTYPE
  struct parse_location
  {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
  };

  struct lysa_options
  {
    string prefix;	           //prefix for LySa identifiers
    string fmt_file_name;      //filename of format strings
    string preamble_file_name; //filename of preamble (or empty)
    bool make_symbolic;        //if true, make a symbolic attacker
    string attacker_index;     //if nonempty, add CPDYonAttackerIndex to every
                               //destorig and set attackerIndex map
    string zero_action;        //if nonempty, first this action before delta'ing if Zero is encountered
  };

  enum Calculus { LySa, TypedLySa, Unknown };

  class Var;
  class Name;
  class Identifier;
  class Expression;
  class Indices;

  typedef shared_ptr<Expression> E_ptr;
  typedef std::set<int> domain;

  class ProcessInfo
  {
  protected:
    Calculus _calculus;
  public:
    lysa_options options;
    int current_line;
    int current_col;

    ProcessInfo(lysa_options options) : _calculus(Unknown), options(options) {};
    Calculus calculus() { return _calculus; };
    void set_calculus(Calculus c);
    void override_calculus(Calculus c);
  };

  extern shared_ptr<ProcessInfo> current_process_info;
  void start_parsing(lysa_options &options);
  void set_current_position(parse_location &pos);
  template<typename T> string join(T& input, string sep);
  template<typename T> string join_ptr(T& input, string sep);


  class Expression
  {
  protected:
    int line_in_input;
    int col_in_input;
    string _hint;
  public:
    string position_in_input();
    shared_ptr<ProcessInfo> process_info;
    virtual operator string() = 0;
    void hint(string h)    { _hint = h; };
    string hint()		  		 { return _hint; };
    virtual vector<E_ptr> subexpressions();
    Expression() : process_info(current_process_info)
    {
      line_in_input = process_info->current_line;
      col_in_input = process_info->current_col;
    }

    string typed_lysa_to_lysa();
    virtual E_ptr find_opar_before_dy(E_ptr last_opar);
    std::list<E_ptr> find_let_until(E_ptr stop_at);
  };

  class String : public Expression
  {
  public:
    string s;
    String(string s) : s(s) {};
    virtual operator string() { return s; };
  };

  class Term : public Expression {};
  class Proc : public Expression {};
  class IndexDef;
  class Annotation;

  typedef std::list<string> string_list;

  class Indices : public Expression, public string_list
  {
  public:
    Indices() {};
    Indices(E_ptr i);
    //version for splitting every character into a separate index
    Indices(E_ptr s, bool isShort);
    virtual operator string();
    virtual void push_back(string s);
  };

  class Iset;
  class IndexDef : public Expression
  {
  public:
    string index;
    string set;
    shared_ptr<Iset> iset;

    
    IndexDef(E_ptr i, E_ptr s) : 
      index(process_info->options.prefix + static_pointer_cast<String>(i)->s),
      set(static_pointer_cast<String>(s)->s) {} ;
    IndexDef(E_ptr i, E_ptr s, E_ptr iset) : 
      index(process_info->options.prefix + static_pointer_cast<String>(i)->s),
      set(static_pointer_cast<String>(s)->s), 
      iset(static_pointer_cast<Iset>(iset)) {};
    virtual operator string();
  };
  typedef std::list<shared_ptr<IndexDef> > IndexDef_list;
  class IndexDefs : public Expression, public IndexDef_list
  {
  public:
    operator string();
    IndexDefs() {};
    IndexDefs(E_ptr id);
    virtual void push_back(E_ptr id);
  };

  typedef std::list<shared_ptr<Term> > Term_list;
  class Terms : public Expression, public Term_list
  {
  public:
    Terms() {};
    Terms(E_ptr t);
    virtual void push_back(E_ptr t);
    virtual operator string();
    virtual vector<E_ptr> subexpressions();
  };

  class Identifier : public Term
  {
  public:
    string name;
    shared_ptr<Indices> indices;

    Identifier(E_ptr s)          : name(process_info->options.prefix + static_pointer_cast<String>(s)->s), indices(new Indices()) {};
    Identifier(E_ptr s, E_ptr i) : name(process_info->options.prefix + static_pointer_cast<String>(s)->s), indices(static_pointer_cast<Indices>(i)) {};
    virtual operator string();
  };

  //NOTE: a Name is any identifier that is not a typed variable definition, crypto-point or asymmetic name!
  //in other words, it can be either a literal name or a variable. only scope can tell th difference between
  //those two.
  class Name : public Identifier 
  {	
  public:
    Name(E_ptr s) : Identifier(s) {};
    Name(E_ptr s, E_ptr i) : Identifier(s, i) {};
  };

  class TypedVar : public Name 
  {	
  public:
    typedef enum {V_None, V_Name, V_Ciphertext} Type;
    Type type;

    TypedVar(E_ptr s, Type type) : Name(s), type(type) {};
    TypedVar(E_ptr s, E_ptr i, Type type) : Name(s, i), type(type) {};
    virtual operator string();
  };

  class ASymName : public Name
  {
  public:
    bool hasPlus;

    ASymName(E_ptr s, bool hasPlus) : Name(s), hasPlus(hasPlus) {};
    ASymName(E_ptr s, E_ptr i, bool hasPlus) : Name(s, i), hasPlus(hasPlus) {};
    virtual operator string();
  };

  class Ciphertext : public Term
  {
  public:
    shared_ptr<Terms> terms;
    shared_ptr<Term> key;
    shared_ptr<Annotation> anno;
    bool isASym;

    Ciphertext(bool isASym, E_ptr t, E_ptr k, E_ptr a) : 
    terms(static_pointer_cast<Terms>(t)), key(static_pointer_cast<Term>(k)), anno(static_pointer_cast<Annotation>(a)), isASym(isASym) {};
    virtual operator string();
    virtual vector<E_ptr> subexpressions();
  };

  class Iset : public Expression
  {
  public:
    virtual domain to_domain() = 0;
  };

  class IsetIndices : public Iset
  {
  public:
    shared_ptr<Indices> indices;

    IsetIndices(E_ptr i) : indices(static_pointer_cast<Indices>(i)) {};
    virtual domain to_domain();
    virtual operator string();
    virtual vector<E_ptr> subexpressions();
  };

  enum IsetDefSet { ZERO, NATURAL1, NATURAL2, NATURAL3, NATURAL01, NATURAL02, NATURAL03 };
  class IsetDefSetSemval : public Expression
  {
  public:
    IsetDefSet v;
    IsetDefSetSemval(IsetDefSet v) : v(v) {};
    virtual operator string() { return ""; };
  };
  class IsetDef : public Iset
  {
  public:
    IsetDefSet def_set;
    IsetDef(E_ptr d) : def_set((static_pointer_cast<IsetDefSetSemval>(d))->v) {};
    virtual domain to_domain();
    virtual operator string();
  };

  class IsetUnion : public Iset
  {
  public:
    shared_ptr<Iset> iset_left;
    shared_ptr<Iset> iset_right;

    IsetUnion(E_ptr l, E_ptr r) : 
    iset_left(static_pointer_cast<Iset>(l)), iset_right(static_pointer_cast<Iset>(r)) {};
    virtual domain to_domain();
    virtual operator string();
    virtual vector<E_ptr> subexpressions();
  };

  class Zero : public Proc 
  {
  public:
    virtual operator string() { return "0"; }
  };

  class DY : public Proc 
  {
  public:
    virtual operator string();
    DY() { process_info->set_calculus(lysa::TypedLySa); };
    virtual E_ptr find_opar_before_dy(E_ptr last_opar);
  };

  class Let : public Proc
  {
  public:
    string name;
    shared_ptr<Iset> iset;
    shared_ptr<Proc> proc;

    Let(Calculus calc, E_ptr n, E_ptr is, E_ptr p) :
    name(static_pointer_cast<String>(n)->s), iset(static_pointer_cast<Iset>(is)), proc(static_pointer_cast<Proc>(p)) 
    {
      process_info->set_calculus(calc);
    };
    virtual operator string();
    string to_string_without_deepening();
    virtual vector<E_ptr> subexpressions();
  };

  class Send : public Proc
  {
  public:
    shared_ptr<Terms> terms;
    shared_ptr<Proc>  proc;

    Send(E_ptr t, E_ptr p) : terms(static_pointer_cast<Terms>(t)), proc(static_pointer_cast<Proc>(p)) {};
    virtual operator string();
    virtual vector<E_ptr> subexpressions();
  };

  class PMatchTerms : public Expression
  {
  public:
    shared_ptr<Terms> match_terms;
    shared_ptr<Terms> vars;

    PMatchTerms(E_ptr t, E_ptr v);
    virtual operator string();
    virtual vector<E_ptr> subexpressions();
  };

  class Receive : public Proc
  {
  public:
    shared_ptr<PMatchTerms> terms;
    shared_ptr<Proc>  proc;

    Receive(E_ptr t, E_ptr p) : terms(static_pointer_cast<PMatchTerms>(t)), proc(static_pointer_cast<Proc>(p)) {};
    virtual operator string();
    virtual vector<E_ptr> subexpressions();
  };

  class Decrypt : public Proc
  {
  public:
    shared_ptr<Term> ciphertext;
    shared_ptr<PMatchTerms> terms;
    shared_ptr<Term> key;
    shared_ptr<Proc> proc;
    shared_ptr<Annotation> anno;

    Decrypt(E_ptr c, E_ptr t, E_ptr k, E_ptr a, E_ptr p) : 
    ciphertext(static_pointer_cast<Name>(c)), terms(static_pointer_cast<PMatchTerms>(t)), key(static_pointer_cast<Term>(k)), proc(static_pointer_cast<Proc>(p)), anno(static_pointer_cast<Annotation>(a)) {};
    virtual operator string();
    virtual vector<E_ptr> subexpressions();
  };

  class New : public Proc
  {
  public:
    bool isASym;
    shared_ptr<Name> name;
    shared_ptr<Proc> proc;
    shared_ptr<IndexDefs> index_defs;

    New(E_ptr n, bool isASym, E_ptr p) : 
    isASym(isASym), name(static_pointer_cast<Name>(n)), proc(static_pointer_cast<Proc>(p)), index_defs(new IndexDefs()) {};
    New(E_ptr n, bool isASym, E_ptr id, E_ptr p) :
    isASym(isASym), name(static_pointer_cast<Name>(n)), proc(static_pointer_cast<Proc>(p)), index_defs(static_pointer_cast<IndexDefs>(id)) {};
    virtual operator string();
    virtual vector<E_ptr> subexpressions();
  };

  class Parallel : public Proc {};

  class OrdinaryParallel : public Parallel 
  {
  public:
    std::list<shared_ptr<Proc> > procs;

    OrdinaryParallel(std::list<E_ptr> ps);
    virtual operator string();
    virtual vector<E_ptr> subexpressions();
  };

  class IndexedParallel : public Parallel 
  {
  public:
    shared_ptr<Proc> proc;
    shared_ptr<IndexDefs> index_defs;

    IndexedParallel(E_ptr p, E_ptr id) : proc(static_pointer_cast<Proc>(p)), index_defs(static_pointer_cast<IndexDefs>(id)) {}
    virtual operator string();
    virtual vector<E_ptr> subexpressions();
  };

  class Replication : public Parallel 
  {
  public:
    shared_ptr<Proc> proc;

    Replication(E_ptr p) : proc(static_pointer_cast<Proc>(p)) {}
    virtual operator string();
    virtual vector<E_ptr> subexpressions();
  };


  class Cryptopoint : public Identifier
  {
  public:
    Cryptopoint(E_ptr s) : Identifier(s) {};
    Cryptopoint(E_ptr s, E_ptr i) : Identifier(s, i) {};
  };

  typedef std::list<shared_ptr<Cryptopoint> > Cryptopoint_list;
  class Cryptopoints : public Expression, public Cryptopoint_list
  {
  public:
    Cryptopoints() {};
    Cryptopoints(E_ptr c);
    virtual void push_back(E_ptr c);
    virtual operator string();
    virtual vector<E_ptr> subexpressions();
  };

  class Annotation : public Expression
  {
  public:
    shared_ptr<Cryptopoint> at;
    bool isDest;
    bool empty() { return at.get()==0; }
    shared_ptr<Cryptopoints> dest_orig;

    Annotation(bool isDest) : isDest(isDest), dest_orig(new Cryptopoints()) {};
    Annotation(bool isDest, E_ptr a) : at(static_pointer_cast<Cryptopoint>(a)), isDest(isDest), dest_orig(new Cryptopoints()) {};
    Annotation(bool isDest, E_ptr a, E_ptr d_o);
    virtual operator string();
    virtual vector<E_ptr> subexpressions();
  };


#undef S


}

//used by bison-generated parser.
typedef lysa::parse_location YYLTYPE;
#define YYLTYPE_IS_DECLARED

#endif // MCRL2_LYSA_H
