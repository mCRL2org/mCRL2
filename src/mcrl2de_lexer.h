#include <iostream>
#include <aterm2.h>

//Global precondition: the ATerm library has been initialised

ATermAppl mcrl2deParse(std::istream &DEStream);
/*Pre: DEStream is an input stream from which can be read
  Post:the content of DEStream is parsed as an mCRL2 data expression
  Ret: the parsed mCRL2 data expression, if everything went ok
       NULL, otherwise
*/ 
