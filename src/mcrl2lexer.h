#include <iostream>
#include <aterm2.h>

//Global precondition: the ATerm library has been initialised

ATermAppl mcrl2Parse(std::istream &SpecStream);
/*Pre: SpecStream is an input stream from which can be read       
  Post:the content of SpecStream is parsed as an mCRL2 specification
  Ret: the parsed formula mCRL2 specification, if everything went ok
       NULL, otherwise
*/ 
