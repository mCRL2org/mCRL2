#include <iostream>
#include <vector>
#include <aterm2.h>

//Global precondition: the ATerm library has been initialised

ATermAppl parse_specification(std::istream &spec_stream);
/*Pre: spec_stream is an input stream from which can be read       
  Post:the content of spec_stream is parsed as an mCRL2 specification
  Ret: the parsed mCRL2 specification, if everything went ok
       NULL, otherwise
*/ 

ATermAppl parse_data_expression(std::istream &de_stream);
/*Pre: de_stream is an input stream from which can be read       
  Post:the content of de_stream is parsed as an mCRL2 data expression
  Ret: the parsed data expression, if everything went ok
       NULL, otherwise
*/ 
