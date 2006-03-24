#include <iostream>
#include <aterm2.h>

//Global precondition: the ATerm library has been initialised

ATermAppl mcrl2Parse(std::istream &FormStream);
/*Pre: FormStream is a valid formula stream from which can be read       
  Post:the mCRL2 specification in formStream is parsed
  Ret: the parsed formula, if everything went ok
       NULL, otherwise
*/ 
