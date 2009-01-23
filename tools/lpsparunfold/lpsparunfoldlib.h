#ifndef LPSPARUNFOLDLIB_H
//Fileinfo
#define LPSPARUNFOLDLIB_H

#include <string>
#include <set>
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/postfix_identifier_generator.h"
#include "mcrl2/data/basic_sort.h"

#include <boost/range/iterator_range.hpp>
class Sorts 
{
  public:
    //Following line needs the include <boost/range/iterator_range.hpp>
    Sorts( boost::iterator_range< mcrl2::data::sort_expression_list::const_iterator > s);
    Sorts( ){ };
    ~Sorts(){};
    void generateFreshSort();
    mcrl2::data::sort_expression unfoldParameter;
    boost::iterator_range< mcrl2::data::sort_expression_list::const_iterator > sortSet;
  private:
    mcrl2::data::sort_expression generatedSort ;
};

/*class Constructors
{
  public:
    Constructors();
    ~Constructors();
  private:
}*/

/*class Mappings
{
  public:
  private:
}

class Variables
{
  public:
  private:
}*/

#endif
