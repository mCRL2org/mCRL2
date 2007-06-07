#ifndef PARSER_TYPECHECK_H
#define PARSER_TYPECHECK_H

#include <aterm2.h>
#include "mcrl2/lps/specification.h"

//Global preconditions:
//- the ATerm library has been initialised

ATermAppl type_check_spec(ATermAppl spec);
/*Pre: spec represents an mCRL2 specification that adheres to the initial
 *     internal ATerm structure.
 *Post:spec is type checked.
 *Ret: if type checking went ok, an equivalent version of spec is returned
 *     that adheres to the internal ATerm structure after type checking.
 *     if something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl type_check_spec_part(ATermAppl spec);
/*Pre: spec represents a (partial) mCRL2 specification that adheres to the initial
 *     internal ATerm structure (without init).
 *Post:spec is type checked.
 *Ret: if type checking went ok, an equivalent version of spec is returned
 *     that adheres to the internal ATerm structure after type checking.
 *     if something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl type_check_sort_expr(ATermAppl sort_expr, lps::specification &lps_spec);
/*Pre: sort_expr represents an mCRL2 sort expression that adheres to the
 *     initial internal ATerm structure.
 *     lps_spec represents an LPS specification
 *Post:sort_expr is type checked using the declaration from lps_spec
 *Ret: if type checking went ok, an equivalent version of sort_expr is returned
 *     that adheres to the internal ATerm structure after type checking.
 *     if something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl type_check_sort_expr_part(ATermAppl sort_expr, ATermAppl spec=NULL);
/*Pre: sort_expr represents an mCRL2 sort expression that adheres to the
 *     initial internal ATerm structure.
 *     spec represents a (partial) mCRL2 specification that adheres to the initial
 *     internal ATerm structure (without init).
 *Post:sort_expr is type checked using the declaration from spec
 *Ret: if type checking went ok, an equivalent version of sort_expr is returned
 *     that adheres to the internal ATerm structure after type checking.
 *     if something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl type_check_data_expr(ATermAppl data_expr, ATermAppl sort_expr, lps::specification &lps_spec, ATermTable Vars=NULL);
/*Pre: data_expr represents an mCRL2 data expression that adheres to the
 *     initial internal ATerm structure.
 *     sort_expr represents an mCRL2 sort expression that adheres to the
 *     internal ATerm structure after type checking or is NULL.
 *     lps_spec represents an LPS specification
 *Post:data_expr is type checked using the declaration from lps_spec and, if
 *     sort_expr is not NULL, it is type checked as being of type sort_expr
 *Ret: if type checking went ok, an equivalent version of data_expr is returned
 *     that adheres to the internal ATerm structure after type checking.
 *     if something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl type_check_data_expr_part(ATermAppl data_expr, ATermAppl sort_expr, ATermAppl spec=NULL, ATermTable Vars=NULL);
/*Pre: data_expr represents an mCRL2 data expression that adheres to the
 *     initial internal ATerm structure.
 *     sort_expr represents an mCRL2 sort expression that adheres to the
 *     internal ATerm structure after type checking.
 *     spec represents a (partial) mCRL2 specification that adheres to the initial
 *     internal ATerm structure (without init).
 *Post:data_expr is type checked as being of type sort_expr using the
       declaration from spec
 *Ret: if type checking went ok, an equivalent version of data_expr is returned
 *     that adheres to the internal ATerm structure after type checking.
 *     if something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl type_check_mult_act(ATermAppl mult_act, lps::specification &lps_spec);
/*Pre: mult_act represents an mCRL2 multi-action that adheres to the initial
 *     internal ATerm structure.
 *     lps_spec represents an LPS specification
 *Post:mult_act is type checked using the declarations from lps_spec
 *Ret: if type checking went ok, an equivalent version of mult_act is returned
 *     that adheres to the internal ATerm structure after type checking.
 *     if something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl type_check_proc_expr(ATermAppl proc_expr, lps::specification &lps_spec);
/*Pre: proc_expr represents an mCRL2 proc expression that adheres to the
 *     initial internal ATerm structure.
 *     lps_spec represents an LPS specification
 *Post:proc_expr is type checked using the declaration from lps_spec
 *Ret: if type checking went ok, an equivalent version of proc_expr is returned
 *     that adheres to the internal ATerm structure after type checking.
 *     if something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

ATermAppl type_check_state_frm(ATermAppl state_formula, lps::specification &lps_spec);
/*Pre: state_formula represents an mCRL2 state formula that adheres to the
 *     initial internal ATerm structure.
 *     lps_spec represents an LPS specification
 *Post:state_formula is type checked using the declarations from lps_spec
 *Ret: if type checking went ok, an equivalent version of state_formula is
 *     returned that adheres to the internal ATerm structure after type
 *     checking.
 *     if something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

#endif // PARSER_TYPECHECK_H
