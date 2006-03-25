#include <iostream>
#include <vector>
#include <aterm2.h>

//Global precondition: the ATerm library has been initialised

ATermAppl parse_streams(std::vector<std::istream*> &streams);
/*Pre: streams contains at least one element
       each element of streams is opened for reading 
  Post:the contents of streams is parsed
  Ret: the parsed content, if everything went ok
       NULL, otherwise
*/ 
