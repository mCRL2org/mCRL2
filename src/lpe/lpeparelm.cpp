/* 

Version 0.1

*/

#include <iostream>
#include "atermpp/aterm.h"
#include "mcrl2/specification.h"
#include <boost/program_options.hpp>

using namespace std;
using namespace mcrl2;
using namespace atermpp;

//Constanten
string version = "Version 0.333";
bool verbose    = false; 

//
// Returns a vector in which each element is a AtermsAppl (DataVarID)  
//

vector< aterm_appl > getDataVarIDs(aterm_appl input)
{
  vector< aterm_appl > dout;
  vector< aterm_appl > tmp;

  if (gsIsDataVarId(input)){
    dout.push_back(input);
  };
  aterm_list::iterator i = input.argument_list().begin();
  while (i!= aterm_appl(input).argument_list().end()){ 
    if(!gsIsDataVarId(aterm_appl(*i))){
      tmp  = getDataVarIDs(*i);
      if (tmp.size() == 1) {
        dout.push_back(*tmp.begin());
      }
    } else {
      dout.push_back(aterm_appl(*i));
      return dout;
    }
    i++;
  }
  return dout; 
}

void parelm(string filename, string outputfile , int option) 
{
  specification spec;
  if (!spec.load(filename))
  {
    cerr << "could not read " << filename << endl;
    return;
  }
 
  cout << endl <<" Read from input file : " << filename << endl;
  LPE lpe = spec.lpe();

  set< data_variable > S;
  vector< aterm_appl > z;
  vector< aterm_appl > ta;

  
  for(summand_list::iterator s_current = lpe.summands().begin(); s_current != lpe.summands().end(); ++s_current){ 
    z = getDataVarIDs(aterm_appl(s_current->condition()));
    for(vector< aterm_appl>::iterator i = z.begin(); i != z.end(); i++){
      S.insert(data_variable(*i));
    }
    
   // if (s_current->has_time()){
      z = getDataVarIDs(aterm_appl(s_current->time()));
      for(vector< aterm_appl>::iterator i = z.begin(); i != z.end(); i++){
        S.insert(data_variable(*i));
      };
    //}
    
    for(action_list::iterator i = s_current->actions().begin(); i != s_current->actions().end(); i++){
      for(data_expression_list::iterator j = i->arguments().begin(); j != i->arguments().end(); j++){
        z = getDataVarIDs(aterm_appl(*j));
//        cout << j->to_string() << endl;
//	cout << "\033[0;38m"<< z.size() << "\033[0m"<< endl;
        for(vector< aterm_appl>::iterator k = z.begin(); k != z.end(); k++){
//          cout << "\033[0;37m"<< data_variable(*k).to_string() << "\033[0m"<< endl;
	  S.insert(*k);
	};
      };  
    };
  }

  //cout << "found data vars" << endl;
  //cout << S.size() << endl;
  //for(set< data_variable >::iterator i = S.begin(); i != S.end(); i++){
  //    cout << i->name() << endl;
  //}

  
  for(summand_list::iterator s_current = lpe.summands().begin(); s_current != lpe.summands().end(); ++s_current){   
    for(data_assignment_list::iterator di = s_current->assignments().begin(); di != s_current->assignments().end(); di++){
      ta = getDataVarIDs(aterm_appl(di->rhs()));
      for(vector< aterm_appl >::iterator ja = ta.begin(); ja!= ta.end(); ja++){
   //   cout << "ssss" << endl;
   //	cout << ja->to_string() << endl;
   //	cout << "ssss" << endl;
	if (S.count(data_variable(*ja)) > 0 ){
          S.insert(di->lhs());
	}
      };	
    }
  };

  set< data_variable > V;
  set< data_variable > R;
 // for(data_variable_list::iterator di = lpe.free_variables().begin(); di != lpe.free_variables().end() ; di++){
 //   V.insert(*di);	  
 // };
  for(data_variable_list::iterator di = lpe.process_parameters().begin(); di != lpe.process_parameters().end() ; di++){
    V.insert(*di);	  
  };
   
  set_difference(V.begin(), V.end(), S.begin(), S.end(), inserter(R, R.begin()));
  
  cout << "Number of found proces parameters to elminate: " << R.size() << endl;
  for(set< data_variable >::iterator i = R.begin(); i != R.end(); i++){
      cout << i->name() << endl;
  }
  return;
}

namespace po = boost::program_options;
po::variables_map vm;

string addparelmstring(string filename)
{
  string token = "";
  string::size_type begIdx;
  string::size_type endIdx;

  // Find first position after last appearance
  //  of forward- or backslash

  endIdx = filename.find_last_of( "." );
  if ( endIdx == string::npos )
    endIdx = 0;
  else
    ++endIdx;

  begIdx = 0;
  token = filename.substr( begIdx, endIdx ).append("lpeparelm.lpe");
  
  return token;

}

int main(int ac, char* av[])
{
   ATerm bot;
   ATinit(0,0,&bot);
   gsEnableConstructorFunctions();

  
   vector< string > filename;
   int opt = 0;

   try {
     po::options_description desc;
     desc.add_options()
       ("help,h",      "display this help")
       ("version,v",   "display version information")
       ("monitor,m",   "display progress information")
     ;
	
	   po::options_description hidden("Hidden options");
	   hidden.add_options()
       ("INFILE", po::value< vector<string> >(), "input file")
	   ;
	
	   po::options_description cmdline_options;
	   cmdline_options.add(desc).add(hidden);
	
	   po::options_description visible("Allowed options");
	   visible.add(desc);
	
	   po::positional_options_description p;
	   p.add("INFILE", -1);

     po::variables_map vm;
     store(po::command_line_parser(ac, av).
       options(cmdline_options).positional(p).run(), vm);
     
     if (vm.count("help") || ac == 1) {
       cerr << "Usage: "<< av[0] << " [OPTION]... INFILE [OUTFILE] \n";
       cerr << "Remove inert parameters from the LPE in INFILE, and write the result" << endl;
       cerr << "to stdout." << endl;
       cerr << endl;
       cerr << desc;
       return 0;
     }
        
     if (vm.count("version")) {
	     cerr << version << endl;
	     return 0;
	   }

     if (vm.count("monitor")) {
       //cerr << "Displaying progress" << endl;
       verbose = true;
	   }

     if (vm.count("INFILE")){
       filename = vm["INFILE"].as< vector<string> >();
	   }

     if (filename.size() > 2){
        cerr << "Specify only INPUT and/or OUTPUT file (Too many arguments)."<< endl;
     };
             

     if (filename.size() == 2){
       parelm(filename[0], filename[1], opt); 
     };
	
     if(filename.size() == 1){
      parelm(filename[0], addparelmstring(filename[0]) , opt);
        
    };

    }
    catch(exception& e){
      cerr << e.what() << "\n";
      return 1;
    }    
 
    return 0;
}
