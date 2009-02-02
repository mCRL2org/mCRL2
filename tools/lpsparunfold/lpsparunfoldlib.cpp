#include <algorithm>
#include <iostream>
#include <cstdio>
#include <string.h>
#include "lpsparunfoldlib.h"
#include <iostream>
#include <string>
#include "mcrl2/core/messaging.h"


using namespace std;
using namespace mcrl2::core;
using namespace mcrl2::new_data;

Sorts::Sorts(mcrl2::new_data::data_specification s)
{
  m_data_specification = s;
  for (sort_expression_list::const_iterator i =  s.sorts().begin(); 
                                            i != s.sorts().end();
                                            ++i){
    sortSet.insert(*i);
    if (i->is_basic_sort()) {
      sort_names.insert( (basic_sort(*i)).name() );
    }
   };

  gsVerboseMsg("Specification has %d sorts\n", sortSet.size() );

  {
    // Error: s.constructors( unfoldParameter ); -> 0 constructors 
    function_symbol_list fsl= s.constructors();
    for (function_symbol_list::const_iterator i = fsl.begin();
                                              i != fsl.end();
                                              ++i){
      consSet.insert(*i);
      mapping_and_constructor_names.insert( i -> name() );
    };
    gsVerboseMsg("Specification has %d constructors\n", consSet.size() );
  }

  {
    //Error: s.functions( unfoldParameter ); -> 0 functions
    boost::iterator_range<function_symbol_list::const_iterator> fsl= s.functions();
    for (function_symbol_list::const_iterator i = fsl.begin();
                                              i != fsl.end();
                                              ++i){
      mapSet.insert(*i);
      mapping_and_constructor_names.insert( i -> name() );
    };
    gsVerboseMsg("Specification has %d mappings \n", mapSet.size() );
  }
};


mcrl2::new_data::basic_sort Sorts::generateFreshSort( std::string str )
{
  //Generate a fresh Basic Sort
  mcrl2::new_data::postfix_identifier_generator generator = mcrl2::new_data::postfix_identifier_generator ("");
  generator.add_identifiers( sort_names );
  mcrl2::core::identifier_string nstr = generator( str );
  gsVerboseMsg("Generated a fresh sort: %s\n", string(str).c_str() );
  sort_names.insert(nstr);
  return basic_sort( std::string(nstr) );
}

mcrl2::core::identifier_string Sorts::generateFreshConMapFuncName(std::string str)
{
  //Generate a fresh name for a constructor of mapping
  mcrl2::new_data::postfix_identifier_generator generator = mcrl2::new_data::postfix_identifier_generator ("");
  generator.add_identifiers( mapping_and_constructor_names );
  mcrl2::core::identifier_string nstr = generator( str );
  gsVerboseMsg("Generated a fresh mapping: %s\n", string(nstr).c_str() );\
  mapping_and_constructor_names.insert( nstr );
  return nstr;
}


std::set<function_symbol> Sorts::determineAffectedConstructors()
{
  using namespace mcrl2::new_data;

  std::set<function_symbol> k;
  for( std::set<mcrl2::new_data::function_symbol>::iterator i = consSet.begin();
                                                        i != consSet.end();
                                                        ++i){
    if( i->sort().is_function_sort() ){
      if (function_sort( i->sort()).codomain() == unfoldParameter ){
        k.insert(*i) ;
      }
    }
    if( i->sort().is_basic_sort() ){
      if (basic_sort( i->sort() ) == unfoldParameter ){
        k.insert(*i) ;
      }
    }
  }

  gsVerboseMsg("%s has %d constructor function(s)\n", unfoldParameter.name().c_str() , k.size() );

  return k;
}

bool Sorts::basic_sortOccursInSort_expression( mcrl2::new_data::sort_expression s, mcrl2::new_data::basic_sort b )
{
  using namespace mcrl2::new_data;

  if( s.is_basic_sort() )
  {
    if (basic_sort(s) == b)
    {
      return true;
    }
  }
  if( s.is_function_sort() )
  {
    function_sort fs = function_sort(s);
    bool x = basic_sortOccursInSort_expression(fs.codomain(), b ) ;
    boost::iterator_range<sort_expression_list::const_iterator> lst(fs.domain());
    for( sort_expression_list::const_iterator i = lst.begin(); i != lst.end(); ++i ){
      x = x || basic_sortOccursInSort_expression( *i, b );
    }
    return x;
  }
  if( s.is_container_sort() )
  {
    return basic_sortOccursInSort_expression((container_sort(s)).element_sort(), b );
  }
  if( s.is_alias() )
  {
    return basic_sortOccursInSort_expression(alias(s).reference(), b );
  }
  if( s.is_structured_sort() )
  {
    gsVerboseMsg("No structs are yet supported");
    assert("false");
  }

  return false;
}

std::set<function_symbol> Sorts::determineAffectedMappings()
{
  using namespace mcrl2::new_data;

  std::set<function_symbol> m;
  for( std::set<mcrl2::new_data::function_symbol>::iterator i = mapSet.begin();
                                                        i != mapSet.end();
                                                         ++i){
    if(basic_sortOccursInSort_expression( i->sort(), unfoldParameter ))
    {
      m.insert( *i );
    };
    /* Leuk private functies...
    atermpp::set<sort_expression> x; 
    m_data_specification.dependent_sorts( *i, x  );
    */
  }

  gsVerboseMsg("%s has %d mapping function(s)\n", unfoldParameter.name().c_str() , m.size() );

  return m;
}


std::set<function_symbol> Sorts::newSorts( std::set<mcrl2::new_data::function_symbol> k )
{
  using namespace mcrl2::new_data;

  std::set<function_symbol> set_of_new_sorts;

  for( std::set<function_symbol>::iterator i = k.begin(); i != k.end(); ++i  )
  {
    std::string prefix = "c_";
    mcrl2::core::identifier_string fresh_name = generateFreshConMapFuncName( prefix.append( i -> name() ) );
    set_of_new_sorts.insert( function_symbol( fresh_name , sort_new ) );
    mapping_and_constructor_names.insert(fresh_name);
//    function_symbol_list

  }

  gsVerboseMsg("Created %d fresh mapping function(s)\n", set_of_new_sorts.size());
  return set_of_new_sorts;
}

mcrl2::new_data::function_symbol Sorts::createCMap(int k)
{
  mcrl2::new_data::function_symbol fs;
  std::string str = "C_";
  str.append( sort_new.name() );
  mcrl2::core::identifier_string idstr = generateFreshConMapFuncName( str );
  mcrl2::new_data::sort_expression_list fsl;
  fsl.push_back(sort_new);
  for(int i = 0; i < k; ++i)
  {
    fsl.push_back( unfoldParameter );
  }

  fs = function_symbol( idstr , mcrl2::new_data::function_sort( fsl, sort_new ));

  gsVerboseMsg("Created C map: %s\n", fs.to_string().c_str());

  return fs;
}

mcrl2::new_data::function_symbol Sorts::createDetMap()
{
  mcrl2::new_data::function_symbol fs;
  std::string str = "Det_";
  str.append( sort_new.name() );
  mcrl2::core::identifier_string idstr = generateFreshConMapFuncName( str );
  mcrl2::new_data::sort_expression_list fsl;
  fs = function_symbol( idstr , mcrl2::new_data::function_sort( unfoldParameter , sort_new ));
  gsVerboseMsg("Created Det map: %s\n", fs.to_string().c_str());

  return fs;
}

std::set<mcrl2::new_data::function_symbol> Sorts::createProjectorFunctions(std::set<function_symbol> m)
{
  std::set<mcrl2::new_data::function_symbol> sfs;
  std::string str = "pi_";
  str.append( sort_new.name() );

  for( std::set<function_symbol>::iterator i = m.begin() ; i != m.end(); ++i )
  {
    mcrl2::core::identifier_string idstr = generateFreshConMapFuncName( str );
    sfs.insert(function_symbol( idstr , mcrl2::new_data::function_sort( unfoldParameter , *i )));
  }
  gsVerboseMsg("Created %d projection functions map\n", sfs.size() );
  return sfs;
}

void Sorts::algorithm()
{
   /* Var Dec */
   std::set<function_symbol> m;
   std::set<function_symbol> k;
   std::set<function_symbol> set_of_new_sorts;

   /* Alg */
   /*  1 */ sort_new = generateFreshSort( unfoldParameter.name() );
   /*  2 */ k = determineAffectedConstructors();
   /*  3 */ m = determineAffectedMappings();
   /*  4 */ set_of_new_sorts = newSorts( k );
   /*  5 */ sortSet.insert( set_of_new_sorts.begin(), set_of_new_sorts.end() );
            sortSet.insert( sort_new );
   /*  6 */ createCMap( k.size() );
   /*  7 */ createDetMap( );
   /*  8 */ createProjectorFunctions(m);


}
