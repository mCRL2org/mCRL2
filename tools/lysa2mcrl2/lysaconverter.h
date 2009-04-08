// Author(s): Egbert Teeselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LYSACONVERTER_H
#define MCRL2_LYSACONVERTER_H

#include <string>
#include <sstream>
#include <vector>

#include "lysa.h"
#include "string_template.h"

using namespace std;

namespace lysa
{
  /**
   * very slow "ordered map": a list of a pair but with map-ish interfaces.
   * key lookup and insertion take linear time.
   *
   * entries are ordered by insert sequence order.
   *
   * better would be to use boost multi-index containers, but those are currently not
   * provided in the mCRL2 toolset.
   */
  template<class _key_type, class _value_type>
	class sloppy_ordered_map : public list<pair<const _key_type, _value_type > >
	{
  public:
    typedef pair<const _key_type, _value_type > _el_type;
    typedef list<_el_type> _Mybase;
    int count(const _key_type& key)
    {
      for(typename _Mybase::iterator v=_Mybase::begin();v!=_Mybase::end();v++) if(v->first==key) return 1;
      return 0;
    }
	  _value_type& operator[](const _key_type& key)
		{
      for(typename _Mybase::iterator v=_Mybase::begin();v!=_Mybase::end();v++) if(v->first==key) return v->second;
      this->push_back(_el_type(key, _value_type()));
      return this->back().second;
		}
  };

	struct Context
	{
		map<string, domain> sets;         //Gamma
		map<string, string> vars;         //Phi
		map<string, list<string> > names; //N
		sloppy_ordered_map<string, string> mvars;        //Upsilon
		int indent;
		string newline(int incr=0)
		{
			indent += incr;
			string s(indent,' ');
			return "\n" + s;
		}
		vector<int> get_domain(string set, E_ptr iset);
		Context() : indent(0) {};
	};

	class Converter
	{
	protected:
		StringTemplateFile templates;
		ostringstream proc_defs;
		map<string, int> names;
		map<string, int> cryptopoints;
		bool contains_attacker;
		int proc_count;
    lysa_options& options;

		string next_proc_name()
		{
			int c = proc_count++; 
			string r;
			do
			{
				r.insert((unsigned int)0,(unsigned int)1,(char)(c%26)+('A'));
			} while((c/=26) > 1);
			return "p_"+r;
		}
	public:
    Converter(lysa_options& options) : templates(options.fmt_file_name), contains_attacker(false), proc_count(0), options(options) {};
		static string to_mcrl2(E_ptr expr) 
		{ 
			Converter conv(expr->process_info->options); 
			return conv.init_to_mcrl2(expr); 
		};
	protected:
		string init_to_mcrl2(E_ptr expr);

		//faking virtual functions
		string to_mcrl2(E_ptr expr, const Context &context);

		//procs
		string to_mcrl2(Let& let, Context context);
		string to_mcrl2(IndexedParallel& ipar, Context context);
		string to_mcrl2(OrdinaryParallel& opar, Context context);
		string to_mcrl2(Send& send, Context context);
		string to_mcrl2(Replication& repl, Context context);
		string to_mcrl2(Zero& zero, Context context);
		string to_mcrl2(DY& dy, Context context);
		string to_mcrl2(New& newn, Context context);
		string to_mcrl2(Receive& recv, Context context);
		string to_mcrl2(Decrypt& decr, Context context);

		//terms - context is a reference because it is never changed in the recursion
		string to_mcrl2(Name& name, const Context &context);
		string to_mcrl2(Ciphertext& ct, const Context &context);
		string to_mcrl2(Annotation& anno, const Context &context);
		void to_mcrl2(PMatchTerms& pmt, Context& context, StringTemplate& sf, shared_ptr<Term> = shared_ptr<Term>());
		string to_mcrl2(Cryptopoint& cp);
		
		//aux
		string make_proc_def(string proc_name, string process, Context &context);


	};
}

#endif //MCRL2_LYSACONVERTER_H
