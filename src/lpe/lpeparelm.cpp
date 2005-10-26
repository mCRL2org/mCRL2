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
string version = "Version 0.4";
bool verbose    = false; 

data_expression_list getDataVarIDs(data_expression input)
{
  aterm_appl z = aterm_appl(input);
  //aterm_list::iterator i;
  //if (gsIsDataAppl(z)){
  //  (*z.argument_list().end())->to_string();
  //}
 



  //while (i!= aterm_appl(input).argument_list().end()){
  //  cout << i->to_string() << "===" << endl;
    

    //cout << i.argument_list().end().to_string() << endl; 
   // i++;
 //}  
 
  data_expression_list dout;
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

  for(summand_list::iterator s_current = lpe.summands().begin(); s_current != lpe.summands().end(); ++s_current){ 
    cout << s_current->condition().pp() << endl;
    cout << s_current->condition().to_string() << endl;
    cout << "++++++++++++++++++++++" << endl;
    getDataVarIDs(s_current->condition());

    cout << "======================" << endl;

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
