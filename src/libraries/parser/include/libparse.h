#include <iostream>
#include <vector>
#include <aterm2.h>

//Global precondition: the ATerm library has been initialised

ATermAppl parse_sort_expression(std::istream &se_stream);
/*Pre: se_stream is an input stream from which can be read       
  Post:the content of se_stream is parsed as an mCRL2 sort expression
  Ret: the parsed sort expression, if everything went ok
       NULL, otherwise
*/ 

ATermAppl parse_data_expression(std::istream &de_stream);
/*Pre: de_stream is an input stream from which can be read       
  Post:the content of de_stream is parsed as an mCRL2 data expression
  Ret: the parsed data expression, if everything went ok
       NULL, otherwise
*/ 

ATermAppl parse_multi_action(std::istream &pe_stream);
/*Pre: pe_stream is an input stream from which can be read       
  Post:the content of pe_stream is parsed as an mCRL2 multi-action
  Ret: the parsed multi-action, if everything went ok
       NULL, otherwise
*/ 

ATermAppl parse_proc_expression(std::istream &pe_stream);
/*Pre: pe_stream is an input stream from which can be read       
  Post:the content of pe_stream is parsed as an mCRL2 process expression
  Ret: the parsed proc expression, if everything went ok
       NULL, otherwise
*/ 

ATermAppl parse_specification(std::istream &spec_stream);
/*Pre: spec_stream is an input stream from which can be read       
  Post:the content of spec_stream is parsed as an mCRL2 specification
  Ret: the parsed mCRL2 specification, if everything went ok
       NULL, otherwise
*/ 

ATermAppl parse_state_formula(std::istream &pe_stream);
/*Pre: pe_stream is an input stream from which can be read       
  Post:the content of pe_stream is parsed as an mCRL2 state formula
  Ret: the parsed state formula, if everything went ok
       NULL, otherwise
*/ 
