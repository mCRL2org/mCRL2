#include <algorithm>
#include <iostream>
#include <cstdio>
#include <string.h>
#include "lpsparunfoldlib.h"
#include <iostream>
#include <string>
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/print.h"
#include <iterator>

using namespace std;
using namespace mcrl2::core;
using namespace mcrl2::new_data;

Sorts::Sorts(mcrl2::new_data::data_specification s, mcrl2::lps::linear_process lps)
{
  m_data_specification = s;
  m_lps = lps;
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
    //Error: s.mappings( unfoldParameter ); -> 0 functions
    boost::iterator_range<function_symbol_list::const_iterator> fsl= s.mappings();
    for (function_symbol_list::const_iterator i = fsl.begin();
                                              i != fsl.end();
                                              ++i){
      mapSet.insert(*i);
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
  gsVerboseMsg("Generated a fresh mapping: %s\n", string(nstr).c_str() ); 
  mapping_and_constructor_names.insert( nstr );
  return nstr;
}


function_symbol_list Sorts::determineAffectedConstructors()
{
  using namespace mcrl2::new_data;

  function_symbol_list k;
  for( std::set<mcrl2::new_data::function_symbol>::iterator i = consSet.begin();
                                                        i != consSet.end();
                                                        ++i){
    if( i->sort().is_function_sort() ){
      if (function_sort( i->sort()).codomain() == unfoldParameter ){
        k.push_back(*i) ;
      }
    }
    if( i->sort().is_basic_sort() ){
      if (basic_sort( i->sort() ) == unfoldParameter ){
        k.push_back(*i) ;
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

function_symbol_list Sorts::determineAffectedMappings()
{
  using namespace mcrl2::new_data;

  function_symbol_list m;
  for( std::set<mcrl2::new_data::function_symbol>::iterator i = mapSet.begin();
                                                        i != mapSet.end();
                                                         ++i){
    if(basic_sortOccursInSort_expression( i->sort(), unfoldParameter ))
    {
      m.push_back( *i );
      gsDebugMsg("%s: %s\n", i->name().c_str(), i->sort().to_string().c_str()  );
    };
    /* Leuk private functies...
    atermpp::set<sort_expression> x; 
    m_data_specification.dependent_sorts( *i, x  );
    */
  }

  gsVerboseMsg("%s has %d mapping function(s)\n", unfoldParameter.name().c_str() , m.size() );

  return m;
}


function_symbol_list Sorts::newSorts( mcrl2::new_data::function_symbol_list k )
{
  using namespace mcrl2::new_data;

  function_symbol_list set_of_new_sorts;

  for( function_symbol_list::iterator i = k.begin(); i != k.end(); ++i  )
  {
    std::string prefix = "c_";
    mcrl2::core::identifier_string fresh_name = generateFreshConMapFuncName( prefix.append( i -> name() ) );
    set_of_new_sorts.push_back( function_symbol( fresh_name , sort_new ) );
    gsDebugMsg("%s\n", function_symbol( fresh_name , sort_new ).to_string().c_str() );
    mapping_and_constructor_names.insert(fresh_name);

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
 
  gsVerboseMsg("Created C map");
  gsDebugMsg("%s", fs.to_string().c_str()); 
  gsVerboseMsg("\n");
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

mcrl2::new_data::function_symbol_list Sorts::createProjectorFunctions(function_symbol_list m)
{
  mcrl2::new_data::function_symbol_list sfs;
  std::string str = "pi_";
  str.append( sort_new.name() );

  for( function_symbol_list::iterator i = m.begin() ; i != m.end(); ++i )
  {
    if ( i->sort().is_function_sort() )
    {
      //SEGFAULT: boost::iterator_range<sort_expression_list::const_iterator> sel = function_sort( i->sort() ).domain();
      function_sort fs = function_sort( i->sort() );
      boost::iterator_range<sort_expression_list::const_iterator> sel  = fs.domain();
      for(sort_expression_list::const_iterator j = sel.begin(); j != sel.end(); j++ )
      {
        mcrl2::core::identifier_string idstr = generateFreshConMapFuncName( str );
        std::cerr << j-> to_string() << std::endl;
         
        sfs.push_back(function_symbol( idstr , mcrl2::new_data::function_sort( unfoldParameter , *j )));
      }
    }
  }
  gsVerboseMsg("Created %d projection functions\n", sfs.size() );
  return sfs;
}

std::pair< variable_list, data_equation_list > Sorts::createFunctionSection(function_symbol_list pi, function_symbol Cmap, function_symbol_list set_of_new_sorts, function_symbol_list k, function_symbol Detmap)
{
  variable_list vars;        /* Equation variables  */
  data_equation_list del;    /* Generated equations */ 
  std::set<mcrl2::core::identifier_string> var_names; /* var_names */
  mcrl2::new_data::postfix_identifier_generator generator = mcrl2::new_data::postfix_identifier_generator ("");
  variable v;
 
  std::string fstr = "y";
  std::string cstr = "c";
  std::string dstr = "d";

  /* Creating variables for projection */
  generator.add_identifiers( var_names );
  mcrl2::core::identifier_string istr = generator( fstr );

  variable lv = variable( istr, unfoldParameter );
  for(int i = 0 ; i < int(pi.size()) ; ++i){
    istr = generator( fstr );
    v = variable( istr, unfoldParameter );
    vars.push_back( v );
  }
  vars.push_back(lv);
    
  /* Creating variable for detector function */
  istr = generator( cstr );
  v = variable( istr, sort_new );
  vars.push_back( v );

  /* Create Equations */ 
  int e = 0; 
  for(function_symbol_list::iterator i = pi.begin(); i != pi.end(); ++i){
    data_expression_list args;
    args.push_back(set_of_new_sorts[e]);
    for(int j = 0 ; j < int(pi.size()) ; ++j){
      args.push_back(vars[ j ]);
    }
    //Omslachtig:
    //Zou verwachten  application( data_expression, data_expression_list )
    boost::iterator_range<data_expression_list::const_iterator> arg (args);
    data_expression lhs = application(  Cmap , arg ); 


    gsDebugMsg("Added equation %s\n", pp(data_equation( lhs, vars[e] )).c_str());
    del.push_back( data_equation( lhs, vars[e] ) );
    ++e;
  }

  {
    data_expression_list args;
    args.push_back( vars[e+1] );
    for(int j = 0 ; j < int(pi.size()) ; ++j){
        args.push_back(vars[ e ]);
    }
    boost::iterator_range<data_expression_list::const_iterator> arg (args); /* Omslachtig */
    data_expression lhs = application(  Cmap , arg ); 
    gsDebugMsg("Added equation %s\n", pp(data_equation( lhs, vars[e] )).c_str());
    del.push_back( data_equation( lhs, vars[e] ) );
  }

  /* Creating Detector equations */
  std::map< sort_expression, variable_list > type_var_list;  /* Function for looking up  Sort |-> [Variable] */
  std::map< sort_expression, int           > type_var_count; /* Function for counting the unique #Sorts of an equation */

  e = 0;
  for(function_symbol_list::iterator i = k.begin(); i != k.end(); ++i)
  {
    type_var_count.clear();

    if( i -> sort().is_basic_sort() )
    {
      data_expression lhs = application( Detmap, *i );
      gsDebugMsg("Added equation %s\n", pp(data_equation( lhs, set_of_new_sorts[e] )).c_str());
      del.push_back( data_equation( lhs, set_of_new_sorts[e] ) );
    }
    if( i -> sort().is_function_sort() )
    {
      function_sort fs = function_sort( i -> sort() );
      boost::iterator_range<sort_expression_list::const_iterator> sel = fs.domain();

      data_expression_list dal;
      for(sort_expression_list::const_iterator j = sel.begin(); j != sel.end(); ++j )
      {
         if( j -> is_basic_sort() )
         {
           /* New sort detected */
           if(type_var_list.find( *j ) != type_var_list.end())
           {
             istr = generator( dstr );
             data_expression v = variable( istr, basic_sort( *j ) );
             vars.push_back( v );
             type_var_list[ *j ].push_back( v );
             type_var_count[ *j ] = 1 ;

             dal.push_back( v ); 
           } 
           /* Sort detected that already exists */
           else {
             /* If insufficient variables for sorts are available add a new variable */
             if (type_var_count[ *j ] == int( type_var_list[ *j ].size() ) )
             {
               istr = generator( dstr );
               data_expression v = variable( istr, basic_sort( *j ) );
               vars.push_back( v );
               type_var_list[ *j ].push_back( v );
               type_var_count[ *j ] = ++type_var_count[ *j ] ;
               dal.push_back( v );
             } 
             /* Reuse if variables if there sufficient variables are available */
             else {
               dal.push_back(type_var_list[ *j ].at(type_var_count[ *j ] - 1));
             } 
           }
         } else {
           gsVerboseMsg("Expected only basic sorts in a domain");
           assert(false);
         } 
      }
      boost::iterator_range<data_expression_list::const_iterator> arg (dal); /* Omslachtig */
      data_expression narg = application( *i , arg );
      data_expression lhs = application( Detmap , narg );
      gsDebugMsg("Added equation %s\n", pp(data_equation( lhs, set_of_new_sorts[e] )).c_str());
      del.push_back( data_equation( lhs, set_of_new_sorts[e] ) );
    
      /* Equations for projection functions */
      int f = 0;
      for(function_symbol_list::iterator j = pi.begin(); j != pi.end(); ++j){
        data_expression lhs = application( *j,  narg);  
        gsDebugMsg("Added equation %s\n", pp(data_equation( lhs, dal[f] )).c_str());
        del.push_back( data_equation( lhs, dal[f] ) );
        ++f;
      }
    }
  e++;
  }

  std::pair< variable_list, data_equation_list> tuple =  std::make_pair( vars , del );

  return tuple;
}

void Sorts::algorithm()
{
   /* Var Dec */
   function_symbol_list m;
   function_symbol_list k;
   function_symbol_list set_of_new_sorts;
   function_symbol_list set_of_ProjectorFunctions;
   function_symbol Cmap;
   function_symbol Detmap;
//   variable_list data_equation_variables;
//   data_equation_list data_equations;
   std::pair< variable_list , data_equation_list > data_specification;
   /*   Alg */
   /*     1 */ sort_new = generateFreshSort( unfoldParameter.name() );
   /*     2 */ k = determineAffectedConstructors();
   /*     3 */ m = determineAffectedMappings();
   /*     4 */ set_of_new_sorts = newSorts( k );
   /*     5 */ sortSet.insert( set_of_new_sorts.begin(), set_of_new_sorts.end() );
               sortSet.insert( sort_new );
   /*     6 */ Cmap = createCMap( k.size() ); 
   /*     7 */ Detmap = createDetMap( );
   /*  8-12 */ set_of_ProjectorFunctions = createProjectorFunctions(k); 
   /* 13-xx */ data_specification = createFunctionSection(set_of_ProjectorFunctions, Cmap, set_of_new_sorts, k, Detmap);

   /*----------------*/  
   
}
