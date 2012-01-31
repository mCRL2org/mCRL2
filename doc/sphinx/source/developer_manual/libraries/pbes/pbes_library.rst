Concepts
========

In this section we define concepts for boolean equation systems.

A class or built-in type T models the `FixpointEquation` concept if it satisfies
the following requirements:

.. table:: FixpointEquation Requirements

   ===================  =================================================  ==========
   Expression           Requirement                                        Semantics 
   ===================  =================================================  ==========
   T::expression_type   is a valid C++ type
   T::variable_type     is a valid C++ type
   T::symbol_type       is a valid C++ type
   eq.variable()        is a valid expression of type T::variable_type
   eq.variable() = x    is a valid expression
   eq.formula()         is a valid expression of type T::expression_type
   eq.formula() = phi   is a valid expression
   eq.symbol()          is a valid expression of type T::symbol_type
   eq.symbol() = sigma  is a valid expression
   ===================  =================================================  ==========

where `eq`, `x`, `phi` and `sigma` are expressions of type `T`, `T::expression_type`, `T::variable_type`
and `T::symbol_type` respectively.

A class or built-in type T models the `BooleanExpressionTraits` concept if it satisfies
the following requirements:

.. table:: BooleanExpressionTraits Requirements

   ==================  ============================================================   =========
   Expression          Requirement                                                    Semantics
   ==================  ============================================================   =========
   T::equation_type    is a valid C++ type that models the FixpointEquation concept
   T::expression_type  is a valid C++ type
   T::variable_type    is a valid C++ type
   T::symbol_type      is a valid C++ type
   s = print(eq)`      is a valid expression
   ==================  ============================================================   =========

where
`eq` is an expression of type `T::equation_type`,
`x` is an expression of type `T::variable_type`,
`s` is an expression of type `std::string`,
`phi` and `psi` are expressions of type `T::expression_type`,
and `sigma` is an expression of type `T::symbol_type`.

Examples of types modeling `BooleanExpressionTraits` are `boolean_expression_traits`
and `pbes_expression_traits`.

A class or built-in type T models the `FixpointEquationSolver` concept if it satisfies
the following requirements:

.. table:: FixpointEquationSolver Requirements

   ================  ============================================================
   Expression        Requirement
   ================  ============================================================
   T::equation_type  is a valid C++ type that models the FixpointEquation concept
   solve(eq)         is a valid expression
   ================  ============================================================


where
`eq` is an expression of type `T::equation_type`
and
`solve` is an expression of type `T`.

Algorithms
==========
In this section we define algorithms for boolean equation systems.

Gauss elimination
-----------------
The class `gauss_elimination_algorithm` in the header file `mcrl2/pbes/gauss_elimination_algorithm.h`
implements Gauss elimination for fixpoint equation systems.

.. literalinclude:: ../test/gauss_elimination_test.cpp

Parameterised Boolean Equation Systems
======================================
This section gives an overview of the classes and algorithms on parameterised
boolean equation systems (PBESs). For an introduction about PBESs see
__pbes_introduction__.
All classes and algorithms of the PBES Library reside in the namespace `pbes_system`.
They are explained in more detail in the upcoming sections.

PBESses are defined using the following grammar:

.. math::

   \left\{
   \begin{array}{c}
   \sigma _{1}X_{1}(d_{1}:D_{1},\cdots ,d_{n}:D_{n})=\varphi _{1} \\
   \vdots  \\
   \sigma _{m}X_{m}(d_{1}:D_{1},\cdots ,d_{n}:D_{n})=\varphi _{m}
   \end{array}
   \right.

.. table:: Overview of PBES Library classes

   ====================================  =======================================
   type                                  class
   ====================================  =======================================
   propositional variable declaration    propositional_variable
   propositional variable instantiation  propositional_variable_instantiation
   predicate formula                     pbes_expression
   initial state of a PBES               pbes_initializer
   fixpoint symbol                       fixed_point_symbol
   PBES equation                         pbes_equation
   PBES                                  template<typename Container> class pbes
   ====================================  =======================================

PBES expressions
----------------
PBES expressions (or predicate formulae) are defined using the following
grammar

.. math::

   \begin{array}{lrl}
   \varphi & ::= &  c
                \: \mid \: \neg \varphi
                \: \mid \: \varphi \wedge \varphi
                \: \mid \: \varphi \vee \varphi
                \: \mid \: \varphi \Rightarrow \varphi
                \: \mid \: \forall d{:}D .\:\varphi
                \: \mid \: \exists d{:}D .\:\varphi
                \: \mid \: X(e)
   \end{array}

Here /c/ is a data term of sort /Bool/, /X/ is a parameterised propositional variable,
/d/ is a variable of sort /D/ and /e/ is a vector of data expressions.

.. tip::

   The negation and implication operators are usually not defined
   in the theory about PBESs. For practical reasons they are supported in the
   implementation.

.. note::

   PBES expressions must be *monotonous*: every occurrence of a propositional
   variable should be in a scope such that the number of "!" operators plus the
   number of left-hand sides of the "=>" operator is even.

Examples
^^^^^^^^
PBES expressions can be parsed from a string:

.. code-block:: c++

    const std::string var_decl =
      "datavar         \n"
      "  n: Nat;       \n"
      "                \n"
      "predvar         \n"
      "  X: Bool, Pos; \n"
      "  Y: Nat;       \n"
      ;
    pbes_expression x = parse_pbes_expression("X(true, 2) && Y(n+1)", var_decl);

Operations on PBES expressions
------------------------------

The following operations are defined on boolean expressions.
Let `x` and `y` be two PBES expressions, and let `v:V` be a sequence of data
variables.

.. table:: Operations on PBES expressions

    ============ ======================
    Expression   Meaning
    ============ ======================
    true_()      :math:`true`
    false_()     :math:`false` 
    not_(x)      :math:`\neg(x)`
    and_(x, y)   :math:`x \land y`   
    or_(x, y)    :math:`x \lor y`
    imp(x, y)    :math:`x \implies y`
    exists(v, x) :math:`\exists v:V. x`
    forall(v, x) :math:`\forall v:V. x`
    ============ ======================

.. note::

   Some of the names of the operations have a trailing underscore character.
   This is only the case when the name itself (like `and` or `not`) is a reserved
   C++ keyword.

There are also recognize functions available to determine the type of a
boolean expression. For example, if `t` is a boolean expression, then `is_and(t)`
tells if `t` is a conjunction. Finally, there are accessor functions for decomposing
a boolean expression. For example, if `t` is a conjunction, then the operands of
`t` are given by `left(t)` and `right(t)`.

All of these operations on boolean expressions are defined in the traits class
`core::term_traits<boolean_expression>`. This is done to support generic algorithms
that work both for both BES and PBES expressions.

.. todo::

   Change example and add comments

Example
^^^^^^^

.. code-block:: c++

  typedef core::term_traits<boolean_expression> tr;
  boolean_variable x("X");
  boolean_variable y("Y");
  boolean_expression z = tr::and_(x, y);
  if (tr::is_and(z))
  {
  	boolean_expression x1 = tr::left(z);
  	boolean_expression y1 = tr::right(z);
  	assert(x == x1);
  	assert(y == y1);
  }

Algorithms on PBES expressions
------------------------------
The following table gives an overview of the available algorithms on PBES expressions.

.. table:: Algorithms on PBES expressions
   
   +------------------------------------------------+------------------------------------------------------------------------------------------------------------------+------------+
   |algorithm                                       |description                                                                                                        header      |
   +================================================+==================================================================================================================+============+
   |complement                                      |Pushes negations as far as possible inwards towards data expressions                                              |complement.h|
   +------------------------------------------------+------------------------------------------------------------------------------------------------------------------+------------+
   |normalize                                       |Brings a PBES expression into positive normal form, i.e. without occurrences of :math:`\neg` and :math:`\implies` |normalize.h |
   +------------------------------------------------+------------------------------------------------------------------------------------------------------------------+------------+
   |find_propositional_variable                     |Checks if a PBES expression has a given propositional variable instantiation as subterm                           |find.h      |
   +------------------------------------------------+------------------------------------------------------------------------------------------------------------------+------------+
   |find_all_propositional_variable_instantiations  |Finds all propositional variable instantiations occurring in a PBES expression                                    |find.h      |
   +------------------------------------------------+------------------------------------------------------------------------------------------------------------------+------------+
   |find_free_variables                             |Finds all free variables occurring in a PBES expression                                                           |find.h      |
   +------------------------------------------------+------------------------------------------------------------------------------------------------------------------+------------+
   |replace_propositional_variables                 |Generic replace function for propositional variable instantiations                                                |replace.h   |
   +------------------------------------------------+------------------------------------------------------------------------------------------------------------------+------------+
   |propositional_variable_sequence_replace         |Replaces propositional variable instantiations, where the replacements are stored in two sequences,               |            |
   |                                                |one with the original values and one with the new values                                                          |replace.h   |
   +------------------------------------------------+------------------------------------------------------------------------------------------------------------------+------------+
   |propositional_variable_map_replace              |Replaces propositional variable instantiations, where the replacements are stored in a map                        |replace.h   |
   +------------------------------------------------+------------------------------------------------------------------------------------------------------------------+------------+
   |substitute_propositional_variable               |Applies the substitution :math:`X(e) := \varphi[d := e]` to all instantiations :math:`X(e)` of the propositional  |            |
   |                                                |variable declaration :math:`X(d)` in a PBES expression, with :math:`\varphi` a given PBES expression              |replace.h   |
   +------------------------------------------------+------------------------------------------------------------------------------------------------------------------+------------+

.. note::

   Specifications of the algorithms can be found in __pbes_implementation__

Rewriters for PBES expressions
------------------------------
The following rewriters are available

.. table:: PBES expression rewriters

   ==============================  ======================================================================
   name                            description
   ==============================  ======================================================================
   simplifying_rewriter            Simplifies a PBES expression
   enumerate_quantifiers_rewriter  Eliminates quantifiers by enumerating quantifier variables
   pfnf_rewriter                   Brings a PBES expression into PFNF normal form (not yet implemented!)
   bdd_prover_rewriter             Rewrites a PBES expression using a BDD prover (see ...)
   ==============================  ======================================================================

.. todo::

   Add reference to rewriter section

.. note::

   Specifications of the PBES expression rewriters can be found in __pbes_implementation__

Visitor classes for PBES expressions
------------------------------------
PBES expressions are stored as terms, defined by the following grammar::

<PBExpr>       ::= <DataExpr>
                 | PBESTrue
                 | PBESFalse
                 | PBESNot(<PBExpr>)
                 | PBESAnd(<PBExpr>, <PBExpr>)
                 | PBESOr(<PBExpr>, <PBExpr>)
                 | PBESImp(<PBExpr>, <PBExpr>)
                 | PBESForall(<DataVarId>+, <PBExpr>)
                 | PBESExists(<DataVarId>+, <PBExpr>)
                 | <PropVarInst>,

where =<PBExpr>= represents a PBES expression, =<DataExpr>= a data expression,
=<DataVarId>= a data variable and =<PropVarInst>= a propositional variable
instantiation.

Many algorithms need to traverse the tree structured PBES expressions. To this
end a couple of visitor classes are available. The class `pbes_expression_visitor`
can be used to visit the nodes of a PBES expression without modifying them,
and the class `pbes_expression_builder` can be used to visit the nodes while
modifying them.

For each type of node a virtual function called `visit_`/<node>/ is defined that
will be called whenever a node of the corresponding type is encountered. In an
algorithm, only the visit functions that are of interest need to be overloaded.
For example, the following program fragment demonstrates how to collect all
conjunctions appearing in a PBES expression.

.. code-block:: c++

    // visitor class that collects conjunctions
    struct and_visitor: public pbes_expression_visitor<pbes_expression>
    {
      std::set<pbes_expression> result;

      bool visit_and(const pbes_expression& x, const pbes_expression& /* left */, const pbes_expression& /* right */)
      {
        result.insert(x);
        return continue_recursion;
      }
    };

    // returns all conjunctions that appear as a subterm of x
    std::set<pbes_expression> find_conjunctions(const pbes_expression& x)
    {
      and_visitor visitor;
      visitor.visit(x);
      return visitor.result;
    }

In this example we see that only the `visit_and` function is overloaded, since
this is the type of nodes we are interested in. Note that this visit function
returns a boolean value `continue_recursion`. This indicates that also the children
of this node should be visited. By returning `stop_recursion` one can indicate
that child nodes must be skipped.

.. todo:: Additional arguments
.. todo:: Leave function
.. todo:: Template parameters
.. todo:: Builder example

.. warning::

   The design of the visitor classes is not stable yet. The current design
   has two problems. First, the visitor algorithms for PBES expressions and BES
   expressions can not be reused, even if they operate on the same subset of nodes.
   Second, the visitor classes use virtual functions. A new design that doesn't
   have these problems is under development.

PBES equations
--------------

.. todo:: introduce PBES equations
.. todo:: introduce fixpoint symbols

PBESs
-----
Parameterised Boolean Equation System (PBESs) are represented by the class
`template <typename Container> class pbes`.
A PBES is a sequence of equations with an initial state, which is a propositional
variable instantiation. The user can choose what container is used for storing
the equations using the template argument `Container`. The default container is
`atermpp::vector`. PBESs may have unbound data variables, that we will refer to
as ['free variables]. Each PBES stores a set of free variables, which is a
superset of the unbound data variables that actually occur in the PBES. Finally
a PBES contains a data specification in which all the data types of the parameters
occurring in the PBES must be defined.

A PBES must satisfy the following well typedness constraints:

  * the sorts occurring in the free variables of the equations are declared in the data specification
  * the sorts occurring in the binding variable parameters are declared in the data specification
  * the sorts occurring in the quantifier variables of the equations are declared in the data specification
  * the binding variables of the equations have unique names (well formedness)
  * the free variables occurring in the equations are declared in `free_variables()`
  * the free variables occurring in the equations with the same name are identical
  * the declared free variables and the quantifier variables occurring in the equations have different names

Algorithms on PBESs
^^^^^^^^^^^^^^^^^^^
.. table:: Algorithms on PBES expressions

   ========================  ========================================================================  =================
   algorithm                 description                                                               header
   ========================  ========================================================================  =================
   txt2pbes                  Parses a textual description of a PBES                                    txt2pbes.h
   lps2pbes                  Generates a PBES from a linear process specification and a state formula  lps2pbes.h
   constelm                  Removes constant parameters from a PBES                                   constelm.h
   parelm                    Removes unused parameters from a PBES                                     parelm.h
   pbesrewr                  Rewrites the predicate formulae of a PBES                                 pbesrewr.h
   pbes2bes                  Transforms a PBES to a BES by instantiating predicate variables           pbes2bes.h
   pbes_gauss_elimination    Solves a BES using Gauss elimination                                      bes_algorithms.h
   ========================  ========================================================================  =================

Examples
^^^^^^^^
PBESs can be loaded and saved using the `load` and `save` member functions:

.. code-block:: c++

  // load a PBES from file
  pbes<> p;
  p.load("pbes1.pbes");

  // print the equations
  for (atermpp::vector<pbes_equation>::iterator i = p.equations().begin(); i != p.equations().end(); ++i)
  {
    std::cout << pp(*i) << std::endl;
  }

  // print the initial state
  propositional_variable_instantiation init = p.initial_state();
  std::cout << "initial state: " << pp(init) << std::endl;

  // save a PBES to file
  p.save("pbes2.pbes");


PBESs can be parsed, using the function `txt2pbes`:

.. code-block:: c++

    std::string text =
      "pbes                                             \n"
      "nu X(b:Bool) = val(b) && Y(!b);                  \n"
      "mu Y(c:Bool) = forall d:Bool. X(d && c) || Y(d); \n"
      "init X(true);                                    \n"
      ;
    pbes<> p = txt2pbes(text);


.. todo:: Add more examples


Boolean Equation Systems
========================

Definition
----------
A Boolean Equation System (BES) is a PBES in which every predicate variable
is of type boolean and every formula :math:`\varphi` adheres to the following grammar:

.. todo::

   Write documentation

Boolean variables
-----------------
Boolean variables are represented by the class :cpp:class:`boolean_variable`.
The only attribute they have is a name.

Boolean expressions
-------------------
Boolean expressions adhere to the following grammar:

.. math::

   \begin{array}{lrl}
   \varphi & ::= & true
             \: \mid \: false
             \: \mid \: X
             \: \mid \: \neg \varphi
             \: \mid \: \varphi \wedge \varphi
             \: \mid \: \varphi \vee \varphi
             \: \mid \: \varphi \rightarrow \varphi
   \end{array}

Boolean expressions are represented by the class :cpp:class:`boolean_expression`.
The following operators are defined on boolean expressions.
Let x and y be two boolean expressions.

.. table:: Operations on boolean types

   =====================  =================
   Expression             Meaning
   =====================  =================
   true_()                the value true
   false_()               the value false
   not_(x)     or !x      negation
   and_(x, y)  or x && y  conjunction
   or_(x, y)   or x || y  disjunction
   imp(x, y)              implication
   =====================  =================

There are also recognize functions available to determine the type of a
boolean expression. For example, if `t` is a boolean expression, then `is_and(t)`
tells if `t` is a conjunction. Finally, there are accessor functions for decomposing
a boolean expression. For example, if `t` is a conjunction, then the operands of
`t` are given by `left(t)` and `right(t)`.

All of these operations on boolean expressions are defined in the traits class
`core::term_traits<boolean_expression>`. This is done to support generic algorithms
that work both for both BES and PBES expressions.

Example
^^^^^^^

.. code-block:: c++

  typedef core::term_traits<boolean_expression> tr;
  boolean_variable x("X");
  boolean_variable y("Y");
  boolean_expression z = tr::and_(x, y);
  if (tr::is_and(z))
  {
  	boolean_expression x1 = tr::left(z);
  	boolean_expression y1 = tr::right(z);
  	assert(x == x1);
  	assert(y == y1);
  }

Boolean equations
-----------------
Boolean equations are represented by the class =boolean_equation=. A boolean
equation has a symbol (:math:`\mu` or :math:`\nu`), a variable on the left hand side and
a formula on the right hand side.

Example
^^^^^^^

.. code-block:: c++

  boolean_equation e = ...;
  boolean_variable x = e.variable();
  boolean_expression phi = e.formula();
  fixpoint_symbol sigma = e.symbol();

.. note::

   Boolean equations are internally stored using ATerms. However, it is highly
   unlikely that boolean equations are shared in memory, so this has to be
   changed.

Boolean equation systems
------------------------
Boolean equation systems are represented by the class [^template<typename Container> class boolean_equation_system].
The template argument =Container= determines how the equations are stored internally.
This is done to give the programmer the choice of the most optimal representation
for algorithms on BESs. The default container type is =atermpp::vector<boolean_equation>=.

Example
^^^^^^^

.. code-block:: c++

  // load a BES from file
  boolean_equation_system<> bes1;
  bes1.load("bes1.bes");

  // print the equations to standard output
  for (boolean_equation_system<>::iterator i = bes1.equations().begin(); i != bes1.equations().end(); ++i)
  {
  	std::cout << pp(*i) << std::endl;
  }

  // print the initial state
  boolean_expression init = bes1.initial_state();
  std::cout << "initial state: " << pp(init) << std::endl;

  // solve the BES
  bool b = gauss_elimination(bes1);

