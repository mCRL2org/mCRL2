// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2-syntax.g
/// \brief dparser grammar of the mCRL2 language

${declare tokenize}
${declare longest_match}

//--- Sort expressions and sort declarations

SortExpr
  : 'Bool'                                                       // Booleans
  | 'Pos'                                                        // Positive numbers
  | 'Nat'                                                        // Natural numbers
  | 'Int'                                                        // Integers
  | 'Real'                                                       // Reals
  | 'List' '(' SortExpr ')'                                      // List sort
  | 'Set' '(' SortExpr ')'                                       // Set sort
  | 'Bag' '(' SortExpr ')'                                       // Bag sort
  | 'FSet' '(' SortExpr ')'                                      // Finite set sort
  | 'FBag' '(' SortExpr ')'                                      // Finite bag sort
  | Id                                                           // Sort reference
  | '(' SortExpr ')'                                             // Sort expression with parentheses
  | 'struct' ConstrDeclList                                      // Structured sort
  | SortExpr '->' SortExpr                             $right 0  // Function sort
  | SortExpr '#' SortExpr                               $left 1  // Product sort
  ;

SortProduct : SortExpr ;                                         // SortExpr in which # is allowed as top-level operator

SortSpec: 'sort' SortDecl+ ;                                     // Sort specification

SortDecl
  : IdList ';'                                                   // List of sort identifiers
  | Id '=' SortExpr ';'                                          // Sort alias
  ;

ConstrDecl: Id ( '(' ProjDeclList ')' )? ( '?' Id )? ;           // Constructor declaration

ConstrDeclList: ConstrDecl ( '|' ConstrDecl )* ;                 // Constructor declaration list

ProjDecl: ( Id ':' )? SortExpr ;                                 // Domain with optional projection

ProjDeclList: ProjDecl ( ',' ProjDecl )* ;                       // Declaration of projection functions

//--- Constructors and mappings

IdsDecl: IdList ':' SortExpr ;                                   // Typed parameters

ConsSpec: 'cons' ( IdsDecl ';' )+ ;                              // Declaration of constructors

MapSpec: 'map' ( IdsDecl ';' )+ ;                                // Declaration of mappings

//--- Equations

GlobVarSpec: 'glob' ( VarsDeclList ';' )+ ;                      // Declaration of global variables

VarSpec: 'var' ( VarsDeclList ';' )+ ;                           // Declaration of variables

EqnSpec: VarSpec? 'eqn' EqnDecl+ ;                               // Definition of equations

EqnDecl: (DataExpr '->')? DataExpr '=' DataExpr ';' ;            // Conditional equation

//--- Data expressions

VarDecl: Id ':' SortExpr ;                                       // Typed variable

VarsDecl: IdList ':' SortExpr ;                                  // Typed variables

VarsDeclList: VarsDecl ( ',' VarsDecl )* ;                       // Individually typed variables

DataExpr
  : Id                                                           // Identifier
  | Number                                                       // Number
  | 'true'                                                       // True
  | 'false'                                                      // False
  | '[' ']'                                                      // Empty list
  | '{' '}'                                                      // Empty set
  | '{'':''}'                                                    // Empty bag
  | '[' DataExprList ']'                                         // List enumeration
  | '{' BagEnumEltList '}'                                       // Bag enumeration
  | '{' VarDecl '|' DataExpr '}'                                 // Set/bag comprehension
  | '{' DataExprList '}'                                         // Set enumeration
  | '(' DataExpr ')'                                             // Brackets
  | DataExpr '[' DataExpr '->' DataExpr ']'            $left 13  // Function update
  | DataExpr '(' DataExprList ')'                      $left 13  // Function application
  | '!' DataExpr                                      $right 12  // Negation, set complement
  | '-' DataExpr                                      $right 12  // Unary minus
  | '#' DataExpr                                      $right 12  // Size of a list
  | 'forall' VarsDeclList '.' DataExpr                 $right 1  // Universal quantifier
  | 'exists' VarsDeclList '.' DataExpr                 $right 1  // Existential quantifier
  | 'lambda' VarsDeclList '.' DataExpr                 $right 1  // Lambda abstraction
  | DataExpr '=>'  DataExpr                            $right 2  // Implication
  | DataExpr '||'  DataExpr                            $right 3  // Disjunction
  | DataExpr '&&'  DataExpr                            $right 4  // Conjunction
  | DataExpr '==' DataExpr                              $left 5  // Equality
  | DataExpr '!=' DataExpr                              $left 5  // Inequality
  | DataExpr '<' DataExpr                               $left 6  // Smaller
  | DataExpr '<='  DataExpr                             $left 6  // Smaller equal
  | DataExpr '>='  DataExpr                             $left 6  // Larger equal
  | DataExpr '>'  DataExpr                              $left 6  // Larger
  | DataExpr 'in'  DataExpr                             $left 6  // Set, bag, list membership
  | DataExpr '|>'  DataExpr                            $right 7  // List cons
  | DataExpr '<|'  DataExpr                             $left 8  // List snoc
  | DataExpr '++' DataExpr                              $left 9  // List concatenation
  | DataExpr '+' DataExpr                              $left 10  // Addition, set/bag union
  | DataExpr '-' DataExpr                              $left 10  // Subtraction, set/bag difference
  | DataExpr '/' DataExpr                              $left 11  // Division
  | DataExpr 'div' DataExpr                            $left 11  // Integer div
  | DataExpr 'mod' DataExpr                            $left 11  // Integer mod
  | DataExpr '*' DataExpr                              $left 12  // Multiplication, set/bag intersection
  | DataExpr '.' DataExpr                              $left 12  // List element at position
  | DataExpr 'whr' AssignmentList 'end'                 $left 0  // Where clause
;

DataExprUnit
  : Id                                                           // Identifier
  | Number                                                       // Number
  | 'true'                                                       // True
  | 'false'                                                      // False
  | '(' DataExpr ')'                                             // Bracket
  | DataExprUnit '(' DataExprList ')'                 $left  14  // Function application
  | '!' DataExprUnit                                  $right 13  // Negation, set complement
  | '-' DataExprUnit                                  $right 13  // Unary minus
  | '#' DataExprUnit                                  $right 13  // Size of a list
  ;

Assignment: Id '=' DataExpr ;                                    // Assignment

AssignmentList: Assignment ( ',' Assignment )* ;                 // Assignment list

DataExprList: DataExpr ( ',' DataExpr )* ;                       // Data expression list

BagEnumElt: DataExpr ':' DataExpr ;                              // Bag element with multiplicity

BagEnumEltList: BagEnumElt ( ',' BagEnumElt )* ;                 // Elements in a finite bag

//--- Communication and renaming sets

ActIdSet: '{' IdList '}' ;                                       // Action set

MultActId: Id ( '|' Id )* ;                                      // Multi-action label

MultActIdList: MultActId ( ',' MultActId )* ;                    // Multi-action labels

MultActIdSet: '{' MultActIdList? '}' ;                           // Multi-action label set

CommExpr: Id '|' MultActId '->' Id ;                             // Action synchronization

CommExprList: CommExpr ( ',' CommExpr )* ;                       // Action synchronizations

CommExprSet: '{' CommExprList? '}' ;                             // Action synchronization set

RenExpr: Id '->' Id ;                                            // Action renaming

RenExprList: RenExpr ( ',' RenExpr )* ;                          // Action renamings

RenExprSet: '{' RenExprList? '}' ;                               // Action renaming set

//--- Process expressions

ProcExpr 
  : Action                                                       // Action or process instantiation
  | Id '(' AssignmentList? ')'                                   // Process assignment
  | 'delta'                                                      // Delta, deadlock, inaction
  | 'tau'                                                        // Tau, hidden action, empty multi-action
  | 'block' '(' ActIdSet ',' ProcExpr ')'                        // Block or encapsulation operator
  | 'allow' '(' MultActIdSet ',' ProcExpr ')'                    // Allow operator
  | 'hide' '(' ActIdSet ',' ProcExpr ')'                         // Hiding operator
  | 'rename' '(' RenExprSet ',' ProcExpr ')'                     // Action renaming operator
  | 'comm' '(' CommExprSet ',' ProcExpr ')'                      // Communication operator
  | '(' ProcExpr ')'                                             // Brackets
  | ProcExpr '+' ProcExpr                               $left 1  // Choice operator
  | 'sum' VarsDeclList '.' ProcExpr                    $right 2  // Sum operator
  | ProcExpr '||'  ProcExpr                            $right 3  // Parallel operator
  | ProcExpr '||_' ProcExpr                            $right 4  // Leftmerge operator
  | DataExprUnit '->' ProcExpr                         $right 5  // If-then operator
  | DataExprUnit '->'ProcExpr '<>' ProcExpr            $right 6  // If-then-else operator
  | ProcExpr '<<' ProcExpr                              $left 8  // Until operator
  | ProcExpr '.' ProcExpr                               $left 9  // Sequential composition operator
  | ProcExpr '@' DataExprUnit                          $left 10  // At operator
  | ProcExpr '|' ProcExpr                              $left 11  // Communication merge
  | 'dist' VarsDeclList '[' DataExpr ']' '.' ProcExpr  $right 2  // Distribution operator
  ;

//--- Actions

Action: Id ( '(' DataExprList ')' )? ;                           // Action, process instantiation

ActDecl: IdList ( ':' SortProduct )? ';' ;                       // Declarations of actions

ActSpec: 'act' ActDecl+ ;                                        // Action specification

MultAct
  : 'tau'                                                        // Tau, hidden action, empty multi-action
  | ActionList                                                   // Multi-action
  ;

ActionList: Action ( '|' Action )* ;                             // List of actions

//--- Process and initial state declaration

ProcDecl: Id ( '(' VarsDeclList ')' )? '=' ProcExpr ';' ;        // Process declaration

ProcSpec: 'proc' ProcDecl+ ;                                     // Process specification

Init: 'init' ProcExpr ';' ;                                      // Initial process

//--- Data specification

DataSpec: ( SortSpec | ConsSpec | MapSpec | EqnSpec )+ ;         // Data specification

//--- mCRL2 specification

mCRL2Spec: mCRL2SpecElt* Init mCRL2SpecElt* ;                    // MCRL2 specification

mCRL2SpecElt
  : SortSpec                                                     // Sort specification
  | ConsSpec                                                     // Constructor specification
  | MapSpec                                                      // Map specification
  | EqnSpec                                                      // Equation specification
  | GlobVarSpec                                                  // Global variable specification
  | ActSpec                                                      // Action specification
  | ProcSpec                                                     // Process specification
  ;

//BesInit: 'init' BesVar ';' ;                                     // Initial BES variable
//
//--- Parameterized Boolean equation systems

PbesSpec: DataSpec? GlobVarSpec? PbesEqnSpec PbesInit ;          // PBES specification

PbesEqnSpec: 'pbes' PbesEqnDecl+ ;                               // Declaration of PBES equations

PbesEqnDecl: FixedPointOperator PropVarDecl '=' PbesExpr ';' ;   // PBES equation

FixedPointOperator
  : 'mu'                                                         // Minimal fixed point operator
  | 'nu'                                                         // Maximal fixed point operator
  ;

PropVarDecl: Id ( '(' VarsDeclList ')' )? ;                      // PBES variable declaration

PropVarInst: Id ( '(' DataExprList ')' )? ;                      // Instantiated PBES variable

PbesInit: 'init' PropVarInst ';' ;                               // Initial PBES variable

DataValExpr: 'val' '(' DataExpr ')' $left 20 ;                   // Marked data expression

PbesExpr
  :
  DataValExpr                                                    // Boolean data expression
  | '(' PbesExpr ')'                                             // Brackets
  | 'true'                                                       // True
  | 'false'                                                      // False
  | Id ( '(' DataExprList ')' )?                                 // Instantiated PBES variable or data application
  | 'forall' VarsDeclList '.' PbesExpr                $right 21  // Universal quantifier
  | 'exists' VarsDeclList '.' PbesExpr                $right 21  // Existential quantifier
  | PbesExpr '=>'  PbesExpr                           $right 22  // Implication
  | PbesExpr '||'  PbesExpr                           $right 23  // Disjunction
  | PbesExpr '&&'  PbesExpr                           $right 24  // Conjunction
  | '!' PbesExpr                                      $right 25  // Negation
  ;

//--- Parameterized real equation systems

PresSpec: DataSpec? GlobVarSpec? PresEqnSpec PresInit ;          // PRES specification

PresEqnSpec: 'pres' PresEqnDecl+ ;                               // Declaration of PRES equations

PresEqnDecl: FixedPointOperator PropVarDecl '=' PresExpr ';' ;   // PRES equation

PresInit: 'init' PropVarInst ';' ;                               // Initial PRES variable

PresExpr
  : DataValExpr                                                  // Real data expression
  | '(' PresExpr ')'                                             // Brackets
  | 'true'                                                       // True, representing infinity
  | 'false'                                                      // False, representing minus infinity
  | Id ( '(' DataExprList ')' )?                                 // Instantiated PRES variable or data application
  | 'inf' VarsDeclList '.' PresExpr                   $right 21  // Infimum operator
  | 'sup' VarsDeclList '.' PresExpr                   $right 21  // Supremum operator
  | 'sum' VarsDeclList '.' PresExpr                   $right 21  // Sum operator
  | PresExpr '+' PresExpr                             $right 22  // Addition
  | PresExpr '=>' PresExpr                            $right 23  // Implication
  | PresExpr '||' PresExpr                            $right 24  // Disjunction
  | PresExpr '&&' PresExpr                            $right 25  // Conjunction
  | DataValExpr '*' PresExpr                          $right 26  // Left multiplication with a positive constant
  | PresExpr '*' DataValExpr                           $left 26  // Right multiplication with a positive constant
  | 'eqinf' '(' PresExpr ')'                                     // Equal infinity
  | 'eqninf' '(' PresExpr ')'                                    // Equal to infinity
  | 'condsm' '(' PresExpr ',' PresExpr ',' PresExpr ')'          // Conditional smaller than 0 with or. 
  | 'condeq' '(' PresExpr ',' PresExpr ',' PresExpr ')'          // Conditional smaller equal 0 with and. 
  | '-' PresExpr                                      $right 25  // Unary minus
  ;


//--- Action formulas

ActFrm
  : DataValExpr                                                  // Boolean data expression
  | MultAct                                                      // Multi-action
  | '(' ActFrm ')'                                               // Brackets
  | 'true'                                                       // True
  | 'false'                                                      // False
  | 'forall' VarsDeclList '.' ActFrm                  $right 21  // Universal quantifier
  | 'exists' VarsDeclList '.' ActFrm                  $right 21  // Existential quantifier
  | ActFrm '=>' ActFrm                                $right 22  // Implication
  | ActFrm '||' ActFrm                                $right 23  // Union of actions
  | ActFrm '&&' ActFrm                                $right 24  // Intersection of actions
  | ActFrm '@' DataExpr                                $left 25  // At operator
  | '!' ActFrm                                        $right 26  // Negation
  ;

//--- Regular formulas

RegFrm
  : ActFrm                                                       // Action formula
  | '(' RegFrm ')'                                               // Brackets
  | RegFrm '+'  RegFrm                                 $left 31  // Alternative composition
  | RegFrm '.'  RegFrm                                $right 32  // Sequential composition
  | RegFrm '*'                                         $left 33  // Iteration
  | RegFrm '+'                                         $left 33  // Nonempty iteration
  ;

//--- State formula specification

StateFrmSpec
  : StateFrm                                                     // Single state formula
  | (StateFrmSpecElt*) FormSpec (StateFrmSpecElt*)               // State formula specification
  ;

FormSpec : 'form' StateFrm ';' ;

StateFrmSpecElt
  : SortSpec                                                     // Sort specification
  | ConsSpec                                                     // Constructor specification
  | MapSpec                                                      // Map specification
  | EqnSpec                                                      // Equation specification
  | ActSpec                                                      // Action specification
  ;


StateFrm
  : DataValExpr                                                  // Boolean data expression
  | '(' StateFrm ')'                                             // Brackets
  | 'true'                                                       // True
  | 'false'                                                      // False
  | Id ( '(' DataExprList ')' )?                                 // Instantiated PBES variable
  | 'delay' ( '@' DataExpr )?                                    // Delay
  | 'yaled' ( '@' DataExpr )?                                    // Yaled
  | 'mu' StateVarDecl '.' StateFrm                    $right 41  // Minimal fixed point
  | 'nu' StateVarDecl '.' StateFrm                    $right 41  // Maximal fixed point
  | 'forall' VarsDeclList '.' StateFrm                $right 42  // Universal quantification
  | 'exists' VarsDeclList '.' StateFrm                $right 42  // Existential quantification
  | 'inf' VarsDeclList '.' StateFrm                   $right 42  // The infimum operator (for quantitative formulas)
  | 'sup' VarsDeclList '.' StateFrm                   $right 42  // The supremum operator (for quantitative formulas)
  | 'sum' VarsDeclList '.' StateFrm                   $right 42  // The sum operator (for quantitative formulas)
  | StateFrm '+' StateFrm                             $right 43  // Addition (for quantitative formulas)
  | DataValExpr '*' StateFrm                          $right 44  // Left constant multiply (for quantitative formulas)
  | StateFrm '*' DataValExpr                           $left 44  // Right constant multiply (for quantitative formulas)
  | StateFrm '=>' StateFrm                            $right 45  // Implication
  | StateFrm '||' StateFrm                            $right 46  // Disjunction
  | StateFrm '&&' StateFrm                            $right 47  // Conjunction
  | '[' RegFrm ']' StateFrm                           $right 48  // Box modality
  | '<' RegFrm '>' StateFrm                           $right 48  // Diamond modality
  | '-' StateFrm                                      $right 48  // Unary minus (for quantitative formulas)
  | '!' StateFrm                                      $right 48  // Negation
  ;

StateVarDecl: Id ( '(' StateVarAssignmentList ')' )? ;           // PBES variable declaration

StateVarAssignment: Id ':' SortExpr '=' DataExpr ;               // Typed variable with initial value

StateVarAssignmentList: StateVarAssignment ( ',' StateVarAssignment )* ;  // Typed variable list

//--- Action Rename Specifications

ActionRenameSpec: (SortSpec | ConsSpec | MapSpec | EqnSpec | ActSpec | ActionRenameRuleSpec)+ ; // Action rename specification

ActionRenameRuleSpec: VarSpec? 'rename' ActionRenameRule+ ;      // Action rename rule section

ActionRenameRule: (DataExpr '->')? Action '=>' ActionRenameRuleRHS ';' ; // Conditional action renaming

ActionRenameRuleRHS
  : Action                                                       // Action
  | 'tau'                                                        // Tau, hidden action, empty multi-action
  | 'delta'                                                      // Delta, deadlock, inaction
  ;

//--- Identifiers

IdList: Id ( ',' Id )* ;                                         // List of identifiers

Id: "[A-Za-z_][A-Za-z_0-9']*" $term -1 ;                         // Identifier

Number: "0|([1-9][0-9]*)" $term -1 ;                             // Number

//--- Whitespace

whitespace: "([ \t\n\r]|(%[^\n\r]*))*" ;                         // Whitespace
