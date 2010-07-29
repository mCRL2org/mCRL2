#~ Copyright 2010 Jonathan Nelisse.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

# This file contains tables that are used to generate classes and traversal functions
# for these classes. A prerequisite is that each class has a corresponding ATerm
# representation (the first column of each table). The second column contains the
# constructor of the classes. If the name of the class has a postfix between brackets
# like variable[_base] then the generated class will be called variable_base, but the
# traversal will use variable. This is done to enable the user of the class to add
# additional behavior to the base class.

import re
import string

NUMERIC_EXPRESSION_CLASSES = r'''
Number          | number(const number& operand)                                 | A number
Card            | card(const set_expression& set)                                         | The cardinality of a set
Length          | length(const seq_expression& seq)                                       | The length of a sequence
Plus            | plus(const numeric_expression& left, const numeric_expression& right)   | An addition
Minus           | minus(const numeric_expression& left, const numeric_expression& right)  | A subtraction
Times           | times(const numeric_expression& left, const numeric_expression& right)  | A multiplication
Div             | div(const numeric_expression& left, const numeric_expression& right)    | A division
Mod             | mod(const numeric_expression& left, const numeric_expression& right)    | A modulo operation
Min             | min(const numeric_expression& operand)                                  | A negative number
'''                                                                                                                                 

BOOLEAN_EXPRESSION_CLASSES = r'''
true            | true_()							 									  | The true value
false           | false_()																  | The false value
And				| and_(const boolean_expression& left, const boolean_expression& right)	  | An and
Or				| or_(const boolean_expression& left, const boolean_expression& right)	  | An or
Not             | not_(const boolean_expression& operand)								  | An not
Null			| null(const seq_expression& seq)										  | The empty sequence test
Elem			| elem(const expression& expr, const seq_expression& seq)				  | The membership of a sequence test
Member			| member(const expression& expr, set_expression& set)					  | The membership of a set test
Empty			| empty(const set_expression& set)										  | The empty set test
Equal			| equal(const expression& left, const expression& right)				  | An equality test
NotEqual		| notequal(const expression& left, const expression& right)				  | An inequality test
Less			| less(const expression& left, const expression& right)					  | A less test
LessOrEqual		| lessorequal(const expression& left, const expression& right)			  | A less or equal test
Greater			| greater(const expression& left, const expression& right)				  | A greater test
GreaterOrEqual	| greaterorequal(const expression& left, const expression& right)		  | A greater or equal test
'''

SET_EXPRESSION_CLASSES = r'''
ChanSet			| chanset(const targ_expression& argument)													| A simple argument (for channels)
union			| union_(const set_expression& left, const set_expression& right)			| A union
inter			| inter(const set_expression& left, const set_expression& right)			| An intersection
diff			| diff(const set_expression& left, const set_expression& right)				| A difference
Union			| Union(const set_expression& set)											| A set union
Inter			| Inter(const set_expression& set)											| A set intersection
set				| set(const seq_expression& seq)											| The set of a sequence
extensions		| extensions(const expression& expr)										| The extension set
productions		| productions(const expression& expr)										| The production set
'''

SEQ_EXPRESSION_CLASSES = r'''
Cat				| cat(const seq_expression& left, const seq_expression& right)				| The concatenation of two sequences
Concat			| concat(const seq_expression& seq)											| The concatenation of a sequence of sequences
Head			| head(const seq_expression& seq)											| The head of a non-empty sequence
Tail			| tail(const seq_expression& seq)											| The tail of a non-empty sequence
'''

TUPLE_EXPRESSION_CLASSES = r'''
Exprs			| exprs(const expression_list& elements)										| A tuple expression
'''

DOTTED_EXPRESSION_CLASSES = r'''
Dot				| dot(const expression& left, expression& right)							| A dotted expression
'''

LAMBDA_EXPRESSION_CLASSES = r'''
LambdaExpr		| lambdaexpr(const expression_list& exprs, const any_expression& function)					| A lambda expression
'''

COMMON_EXPRESSION_CLASSES = r'''
Conditional		| conditional(const boolean_expression& guard, const any_expression& thenpart, const any_expression& elsepart)	| A conditional
Name			| identifier(const name& id)													| A name
LambdaAppl		| lambdaappl(const lambda_expression& lambda, const expression_list& exprs)	| A lambda application
LocalDef		| localdef(const definition_expression_list& defs, const any_expression& within)						| A local definition
Bracketed		| bracketed(const any_expression& operand)												| A bracketed process or expression
Pattern         | pattern(const any_expression& left, const any_expression& right)                                | A combination of two patterns
'''

EXPRESSION_CLASSES = r'''
'''

ANY_EXPRESSION_CLASSES = r'''
Expr			| expr(const expression& operand)											| An expression
Proc			| proc(const process_expression& operand)												| A process
'''

DEFINITION_EXPRESSION_CLASSES = r'''
Assign			| assign(const any_expression& left, const any_expression& right)									| An assignment
Channel			| channel(const name_list& names, const type_expression& type_name)							| A channel
SimpleChannel	| simple_channel(const name_list& names)											| A simple channel
NameType		| nametype(const name& id, const type_expression& type_name)								| A nametype
DataType		| datatype(const name& id, const vartype_expression_list& vartypes)					| A datatype
SubType			| subtype(const name& id, const vartype_expression_list& vartypes)					| A subtype
External		| external(const name_list& names)											| An external
Transparent		| transparent(const trname_expression_list& trnames)									| A transparent
Assert			| assert_(const check_expression& chk)												| An assertion
Print			| print(const expression& expr)												| A print
Include			| include(const filename& file)											| An include
'''

VARTYPE_EXPRESSION_CLASSES = r'''
SimpleBranch	| simplebranch(const name& id)											| A simple branch
Branch			| branch(const name& id, const type_expression& type_name)								| A branch
'''

TYPE_EXPRESSION_CLASSES = r'''
TypeProduct		| typeproduct(const type_expression& left, const type_expression& right)							| A type product
TypeTuple		| typetuple(const type_expression_list& types)											| A type tuple
TypeSet			| typeset(const set_expression& set)										| A type set
SimpleTypeName	| simpletypename(const name& id)											| A simple type name
TypeName		| typename_(const name& id, const type_expression& type_name)								| A type name
'''

CHECK_EXPRESSION_CLASSES = r'''
BCheck			| bcheck(const boolean_expression& expr)									| A boolean check
RCheck			| rcheck(const process_expression& left, const process_expression& right, const refined& refinement)	| A refinement check
TCheck			| tcheck(const process_expression& proc, const test_expression& operand)							| A test
NotCheck		| notcheck(const check_expression& chk)												| A negated check
'''

REFINED_CLASSES = r'''
Refined	| refined(const model_expression& m)													| A model
'''

MODEL_EXPRESSION_CLASSES = r'''
Nil				| nil()																		| An empty
T				| t()																		| A traces
'''

FAILUREMODEL_EXPRESSION_CLASSES = r'''
F				| f()																		| A failures
FD				| fd()																		| A faulures/divergences
'''

TEST_EXPRESSION_CLASSES = r'''
divergence_free	| divergence_free()															| A divergence free
Test			| test(const testtype_expression& tt, const failuremodel_expression& fm)			| A complex test
'''

TESTTYPE_EXPRESSION_CLASSES = r'''
deterministic	| deterministic()															| A deterministic
deadlock_free	| deadlock_free()															| A deadlock free
livelock_free	| livelock_free()															| A livelock free
'''

TRNAME_EXPRESSION_CLASSES = r'''
normal			| normal()																	| A normal
normalise		| normalise()																| A normal
normalize		| normalize()																| A normal
sbsim			| sbsim()																	| A sbsim
tau_loop_factor	| tau_loop_factor()															| A tau_loop_factor
diamond			| diamond()																	| A diamond
model_compress	| model_compress()															| A model compress
explicate		| explicate()																| An explicate
'''

FILENAME_CLASSES = r'''
FileName		| filename(const name_list& names)											| A filename
'''

FDRSPEC_CLASSES = r'''
FDRSpec			| fdrspec(const definition_expression_list& defs)										| An FDR specification
'''

TARG_EXPRESSION_CLASSES = r'''
Nil				| nil()																		| An empty
ClosedRange		| closedrange(const numeric_expression& begin, const numeric_expression& end)	| A closed range
OpenRange		| openrange(const numeric_expression& begin)									| An open range
Compr           | compr(const expression& expr, const comprehension_expression_list& comprs)           | A comprehension
'''

COMPREHENSION_EXPRESSION_CLASSES = r'''
Nil             | nil()                                                                             | An empty
BComprehension	| bcomprehension(const boolean_expression& operand)									| A boolean
EComprehension	| ecomprehension(const expression& left, const expression& right)					| A comprehension
'''

GEN_CLASSES = r'''
SetGen          | setgen(const expression& expr, const set_expression& set)                 | A set generator
SeqGen          | seqgen(const expression& expr, const seq_expression& seq)                 | A seq generator
'''

PROCESS_EXPRESSION_CLASSES = r'''
STOP			| stop()																	| A stop
SKIP			| skip()																	| A skip
CHAOS			| chaos(const set_expression& set)											| A chaos
Prefix			| prefix(const dotted_expression& dotted, const field_expression_list& fields, const process_expression& proc)	| A prefix
ExternalChoice	| externalchoice(const process_expression& left, const process_expression& right)					| An external choice
InternalChoice	| internalchoice(const process_expression& left, const process_expression& right)					| An internal choice
SequentialComposition	| sequentialcomposition(const process_expression& left, const process_expression& right)	| A sequential composition
Interrupt		| interrupt(const process_expression& left, const process_expression& right)						| An interrupt
Hiding			| hiding(const process_expression& proc, const set_expression& set)					| An hiding
Interleave		| interleave(const process_expression& left, const process_expression& right)						| An interleave
Sharing			| sharing(const process_expression& left, const process_expression& right, const set_expression& set)	| A sharing
AlphaParallel	| alphaparallel(const process_expression& left, const process_expression& right, const set_expression& left_set, const set_expression& right_set)	| An alpha parallel
RepExternalChoice	| repexternalchoice(const setgen& gen, const process_expression& proc)	| A replicated external choice
RepInternalChoice	| repinternalchoice(const setgen& gen, const process_expression& proc)	| A replicated internal choice
RepSequentialComposition	| repsequentialcomposition(const seqgen& gen, const process_expression& proc)	| A replicated sequential composition
RepInterleave	| repinterleave(const setgen& gen, const process_expression& proc)			| A replicated interleave
RepSharing		| repsharing(const setgen& gen, const process_expression& proc, const set_expression& set)	| A replicated sharing
RepAlphaParallel	| repalphaparallel(const setgen& gen, const process_expression& proc, const set_expression& set)	| A replicated alpha parallel
UntimedTimeOut	| untimedtimeout(const process_expression& left, const process_expression& right)					| An untimed time-out
BoolGuard		| boolguard(const boolean_expression& guard, const process_expression& proc)		| A boolean guard
LinkedParallel	| linkedparallel(const process_expression& left, const process_expression& right, const linkpar_expression& linked)	| A linked parallel
RepLinkedParallel	| replinkedparallel(const seqgen& gen, const process_expression& proc, const linkpar_expression& linked)	| A replicated linked parallel
Rename	|	rename(const process_expression& proc, const renaming_expression& renaming)	|	A renaming
'''

FIELD_EXPRESSION_CLASSES = r'''
SimpleInput		| simpleinput(const expression& expr)										| A simple input
Input			| input(const expression& expr, const set_expression& restriction)			| An input
Output			| output(const expression& expr)											| An output
'''

RENAMING_EXPRESSION_CLASSES = r'''
Maps			| maps(const map_list& renamings)												| A map list
MapsGens		| mapsgens(const map_list& renamings, const comprehension_expression_list& comprs)				| A map/generator list
'''

MAP_CLASSES = r'''
Map				| map(const dotted_expression& left, const dotted_expression& right)		| A map
'''

LINKPAR_EXPRESSION_CLASSES = r'''
Links			| links(const link_list& linkpars)												| A link list
LinksGens		| linksgens(const link_list& linkpars, const comprehension_expression_list& comprs)				| A link/generator list
'''

LINK_CLASSES = r'''
Link			| link(const dotted_expression& left, const dotted_expression& right)		| A link
'''
