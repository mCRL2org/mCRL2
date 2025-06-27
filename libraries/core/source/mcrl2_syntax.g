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

${declare subparser ProcExpr}

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
  | SortExpr ('->' $binary_op_right 0) SortExpr                  // Function sort (book encodes priorities of '#' and '->' in the rules)
  | SortExpr ('#' $binary_op_left 1) SortExpr                    // Product sort
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
  | DataExpr 'whr' AssignmentList 'end'                 $left 0  // Where clause
  | 'forall' VarsDeclList '.' DataExpr                 $right 1  // Universal quantifier
  | 'exists' VarsDeclList '.' DataExpr                 $right 1  // Existential quantifier
  | 'lambda' VarsDeclList '.' DataExpr                 $right 1  // Lambda abstraction
  | DataExpr ('=>' $binary_op_right 2) DataExpr                  // Implication
  | DataExpr ('||' $binary_op_right 3) DataExpr                  // Disjunction
  | DataExpr ('&&' $binary_op_right 4) DataExpr                  // Conjunction
  | DataExpr ('==' $binary_op_left 5) DataExpr                   // Equality
  | DataExpr ('!=' $binary_op_left 5) DataExpr                   // Inequality
  | DataExpr ('<' $binary_op_left 6) DataExpr                    // Smaller
  | DataExpr ('<=' $binary_op_left 6) DataExpr                   // Smaller equal
  | DataExpr ('>=' $binary_op_left 6) DataExpr                   // Larger equal
  | DataExpr ('>' $binary_op_left 6) DataExpr                    // Larger
  | DataExpr ('in' $binary_op_left 6) DataExpr                   // Set, bag, list membership
  | DataExpr ('|>' $binary_op_right 7) DataExpr                  // List cons
  | DataExpr ('<|' $binary_op_left 8) DataExpr                   // List snoc
  | DataExpr ('++' $binary_op_left 9) DataExpr                   // List concatenation
  | DataExpr ('+' $binary_op_left 10) DataExpr                   // Addition, set/bag union
  | DataExpr ('-' $binary_op_left 10) DataExpr                   // Subtraction, set/bag difference
  | DataExpr ('/' $binary_op_left 11) DataExpr                   // Division
  | DataExpr ('div' $binary_op_left 11) DataExpr                 // Integer div
  | DataExpr ('mod' $binary_op_left 11) DataExpr                 // Integer mod
  | DataExpr ('*' $binary_op_left 12) DataExpr                   // Multiplication, set/bag intersection
  | DataExpr ('.' $binary_op_left 12) DataExpr                   // List element at position
  | '!' DataExpr                                      $right 12  // Negation, set complement
  | '-' DataExpr                                      $right 12  // Unary minus
  | '#' DataExpr                                      $right 12  // Size of a list
  | DataExpr '[' DataExpr '->' DataExpr ']'            $left 13  // Function update
  | DataExpr '(' DataExprList ')'                      $left 13  // Function application
;

DataExprUnit
  : Id                                                           // Identifier
  | Number                                                       // Number
  | 'true'                                                       // True
  | 'false'                                                      // False
  | '(' DataExpr ')'                                             // Bracket
  | '!' DataExprUnit                                  $right 13  // Negation, set complement
  | '-' DataExprUnit                                  $right 13  // Unary minus
  | '#' DataExprUnit                                  $right 13  // Size of a list
  | DataExprUnit '(' DataExprList ')'                 $left  14  // Function application
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
  | ProcExpr ('+' $binary_op_left 1) ProcExpr                    // Choice operator
  | 'sum' VarsDeclList '.' ProcExpr                    $right 2  // Sum operator
  | 'dist' VarsDeclList '[' DataExpr ']' '.' ProcExpr  $right 2  // Distribution operator
  | ProcExpr ('||' $binary_op_right 3) ProcExpr                  // Parallel operator
  | ProcExpr ('||_' $binary_op_right 4) ProcExpr                 // Leftmerge operator
  | DataExprUnit '->' ProcExpr                         $right 5  // If-then operator
  | DataExprUnit IfThen ProcExpr                       $right 5  // If-then-else operator
  | ProcExpr ('<<' $binary_op_left 6) ProcExpr                   // Until operator
  | ProcExpr ('.' $binary_op_right 7) ProcExpr                   // Sequential composition operator
  | ProcExpr '@' DataExprUnit                           $left 8  // At operator
  | ProcExpr ('|' $binary_op_left 9) ProcExpr                    // Communication merge
  ;

// Process expressions that do not contain if expressions.
ProcExprNoIf 
  : Action                                                           // Action or process instantiation
  | Id '(' AssignmentList? ')'                                       // Process assignment
  | 'delta'                                                          // Delta, deadlock, inaction
  | 'tau'                                                            // Tau, hidden action, empty multi-action
  | 'block' '(' ActIdSet ',' ProcExpr ')'                            // Block or encapsulation operator
  | 'allow' '(' MultActIdSet ',' ProcExpr ')'                        // Allow operator
  | 'hide' '(' ActIdSet ',' ProcExpr ')'                             // Hiding operator
  | 'rename' '(' RenExprSet ',' ProcExpr ')'                         // Action renaming operator
  | 'comm' '(' CommExprSet ',' ProcExpr ')'                          // Communication operator
  | '(' ProcExpr ')'                                                 // Brackets
  | ProcExprNoIf ('+' $binary_op_left 1) ProcExprNoIf                // Choice operator
  | 'sum' VarsDeclList '.' ProcExprNoIf                    $right 2  // Sum operator
  | 'dist' VarsDeclList '[' DataExpr ']' '.' ProcExprNoIf  $right 2  // Distribution operator
  | ProcExprNoIf ('||' $binary_op_right 3) ProcExprNoIf              // Parallel operator
  | ProcExprNoIf ('||_' $binary_op_right 4) ProcExprNoIf             // Leftmerge operator (has priority 3 in book, but that is inconsistent with ProcExpr)
  | DataExprUnit IfThen ProcExprNoIf                       $right 5  // If-then-else operator
  | ProcExprNoIf ('<<' $binary_op_left 6) ProcExprNoIf               // Until operator
  | ProcExprNoIf ('.' $binary_op_right 7) ProcExprNoIf               // Sequential composition operator
  | ProcExprNoIf '@' DataExprUnit                           $left 8  // At operator
  | ProcExprNoIf ('|' $binary_op_left 9) ProcExprNoIf                // Communication merge
  ;

IfThen
  : '->' ProcExprNoIf '<>' ;                                          // Auxiliary if-then-else

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

//BesInit: 'init' BesVar ';' ;                                   // Initial BES variable
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

// BesSpec has been removed

PbesExpr
  :
  DataValExpr                                                    // Boolean data expression
  | '(' PbesExpr ')'                                             // Brackets
  | 'true'                                                       // True
  | 'false'                                                      // False
  | Id ( '(' DataExprList ')' )?                                 // Instantiated PBES variable or data application
  | 'forall' VarsDeclList '.' PbesExpr                $right  0  // Universal quantifier
  | 'exists' VarsDeclList '.' PbesExpr                $right  0  // Existential quantifier
  | PbesExpr ('=>' $binary_op_right 2) PbesExpr                  // Implication
  | PbesExpr ('||' $binary_op_right 3) PbesExpr                  // Disjunction
  | PbesExpr ('&&' $binary_op_right 4) PbesExpr                  // Conjunction
  | '!' PbesExpr                                      $right  5  // Negation
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
  | 'eqinf' '(' PresExpr ')'                                     // Equal infinity
  | 'eqninf' '(' PresExpr ')'                                    // Equal to infinity
  | 'condsm' '(' PresExpr ',' PresExpr ',' PresExpr ')'          // Conditional smaller than 0 with or. 
  | 'condeq' '(' PresExpr ',' PresExpr ',' PresExpr ')'          // Conditional smaller equal 0 with and. 
  | 'inf' VarsDeclList '.' PresExpr                   $right  0  // Infimum operator
  | 'sup' VarsDeclList '.' PresExpr                   $right  0  // Supremum operator
  | 'sum' VarsDeclList '.' PresExpr                   $right  0  // Sum operator
  | PresExpr ('+' $binary_op_right 2) PresExpr                   // Addition
  | PresExpr ('=>' $binary_op_right 3) PresExpr                  // Implication
  | PresExpr ('||' $binary_op_right 4) PresExpr                  // Disjunction
  | PresExpr ('&&' $binary_op_right 5) PresExpr                  // Conjunction
  | DataValExpr '*' PresExpr                          $right  6  // Left multiplication with a positive constant
  | PresExpr ('*' $binary_op_left 6) DataValExpr                 // Right multiplication with a positive constant
  | '-' PresExpr                                       $right 7  // Unary minus
  ;


//--- Action formulas

ActFrm
  : DataValExpr                                                  // Boolean data expression
  | MultAct                                                      // Multi-action
  | '(' ActFrm ')'                                               // Brackets
  | 'true'                                                       // True
  | 'false'                                                      // False
  | 'forall' VarsDeclList '.' ActFrm                  $right  0  // Universal quantifier
  | 'exists' VarsDeclList '.' ActFrm                  $right  0  // Existential quantifier
  | ActFrm ('=>' $binary_op_right 2) ActFrm                      // Implication
  | ActFrm ('||' $binary_op_right 3) ActFrm                      // Union of actions
  | ActFrm ('&&' $binary_op_right 4) ActFrm                      // Intersection of actions
  | ActFrm '@' DataExpr                                $left  5  // At operator
  | '!' ActFrm                                        $right  6  // Negation
  ;

//--- Regular formulas

RegFrm
  : ActFrm                                                       // Action formula
  | '(' RegFrm ')'                                               // Brackets
  | RegFrm ('+' $binary_op_left 1) RegFrm                        // Alternative composition
  | RegFrm ('.' $binary_op_right 2) RegFrm                       // Sequential composition
  | RegFrm '*'                                         $left  3  // Iteration (must be $left because that is the non-terminal)
  | RegFrm '+'                                         $left  3  // Nonempty iteration (must be $left because that is the non-terminal)
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
  : DataValExpr                                                  // Boolean data expression (no priority, is not ambiguous)
  | '(' StateFrm ')'                                             // Brackets
  | 'true'                                                       // True
  | 'false'                                                      // False
  | Id ( '(' DataExprList ')' )?                                 // Instantiated PBES variable
  | 'delay' ( '@' DataExpr )?                                    // Delay
  | 'yaled' ( '@' DataExpr )?                                    // Yaled
  | 'mu' StateVarDecl '.' StateFrm                     $right 1  // Minimal fixed point
  | 'nu' StateVarDecl '.' StateFrm                     $right 1  // Maximal fixed point
  | 'forall' VarsDeclList '.' StateFrm                 $right 2  // Universal quantification
  | 'exists' VarsDeclList '.' StateFrm                 $right 2  // Existential quantification
  | 'inf' VarsDeclList '.' StateFrm                    $right 2  // The infimum operator (for quantitative formulas)
  | 'sup' VarsDeclList '.' StateFrm                    $right 2  // The supremum operator (for quantitative formulas)
  | 'sum' VarsDeclList '.' StateFrm                    $right 2  // The sum operator (for quantitative formulas)
  | StateFrm ('+' $binary_op_right 3) StateFrm                   // Addition (for quantitative formulas)
  | StateFrm ('=>' $binary_op_right 4) StateFrm                  // Implication
  | StateFrm ('||' $binary_op_right 5) StateFrm                  // Disjunction
  | StateFrm ('&&' $binary_op_right 6) StateFrm                  // Conjunction
  | DataValExpr '*' StateFrm                           $right 7  // Left constant multiply (for quantitative formulas)
  | StateFrm ('*' $binary_op_left 7) DataValExpr                 // Right constant multiply (for quantitative formulas)
  | '[' RegFrm ']' StateFrm                            $right 8  // Box modality
  | '<' RegFrm '>' StateFrm                            $right 8  // Diamond modality
  | '-' StateFrm                                       $right 9  // Unary minus (for quantitative formulas)
  | '!' StateFrm                                       $right 9  // Negation
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

Number: "0|([1-9][0-9]*)" $term -1 ;                             // Positive number

//--- Whitespace

whitespace: "([ \t\n\r]|(%[^\n\r]*))*" ;                         // Whitespace
