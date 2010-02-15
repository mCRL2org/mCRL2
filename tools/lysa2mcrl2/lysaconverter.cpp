// Author(s): Egbert Teeselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//


#include "lysaconverter.h"
#include <typeinfo>
#include <sstream>
#include <fstream>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost;


namespace lysa
{
	vector<int> Context::get_domain(string set_name, E_ptr iset_ptr)
	{
		domain dm;
		if(set_name!="")
		{
			if(sets.count(set_name)==0)
			{
				throw "Set '" + set_name + "' not defined";
			}
			domain dm_set(sets.find(set_name)->second);
			dm.insert(dm_set.begin(), dm_set.end());
		}
		if(iset_ptr.get())
		{
			shared_ptr<Iset> iset(static_pointer_cast<Iset>(iset_ptr));
			domain dm_iset(iset->to_domain());
			dm.insert(dm_iset.begin(), dm_iset.end());
		}
		vector<int> d(dm.begin(), dm.end());
		return d;
	}

	string Converter::to_mcrl2(Let& let, Context context)
	{
		domain d = let.iset->to_domain();

		//fixme: check existence let->name
		if(context.sets.count(let.name))
		{
			throw "set identifier '" + let.name + "' redefined.";
		}
		context.sets[let.name] = d;
		return to_mcrl2(let.proc, context);
	}


	string Converter::make_proc_def(string proc_name, string process, Context &context)
	{
		string proc_def = "proc " + proc_name;
		ostringstream proc_args;
		pair<string, string> mv, v;
		bool b = false;
		BOOST_FOREACH(mv, context.mvars)
		{
			if(b++) proc_args << ", ";
			proc_args << mv.first << ": Nat";
		}
		BOOST_FOREACH(v, context.vars)
		{
			if(b++) proc_args << ", ";
			proc_args << v.second << ": Value";
		}
		if(b)
		{
			proc_def += "(" + proc_args.str() + ")";
		}
		proc_def += " = " + process;
		return proc_def;
	}
	void stream_cartesian_product_args(ostringstream& process, vector<vector<int> >& ds, string name, string add_args, string separator=" || ")
	{
		if(ds.empty())
		{
			process << name;
			if(add_args!="") process << "(" << add_args << ")";
		}
		else
		{
			//make a list of indices, one index per domain, and initialise to zero.
			vector<int> d_is;
			d_is.resize(ds.size());

			//compute cartesian product of all domains and turn into parallel proc calls.
			bool stop = false;
			bool b = false;
			while(!stop)
			{
				if(b++) process << separator;

				//draw proc call
				{
					process << name << "(";

					//draw indices
					for(unsigned int i=0; i!=d_is.size(); i++)
					{
						if(i) process << ", ";
						process << ds[i][d_is[i]];
					}
					process << add_args;
					process << ")";
				}

				//update indices; we increment each index linearly, looping back and incrementing the next
				//upon overflow. much like how digital clocks and sports set/point counters work. or numbers, if
				//all domains would have 10 elements.
				for(unsigned int i=0;;)
				{
					d_is[i]++;
					//overflow? then increase the next index instead.
					if(d_is[i] == (int) ds[i].size())
					{
						d_is[i] = 0;
						i++;
					}
					else
					{
						//nothing overflowed? then stop the loop, we incremented what we wanted.
						break;
					}
					if(i == ds.size())
					{
						//if we're still here, then the last index overflowed; in other words, 
						//we're done.
						stop = true;
						break;
					}
				}
			}
		}
	}
	string Converter::to_mcrl2(IndexedParallel& ipar, Context context)
	{
		//update Phi (context.mvars)
		BOOST_FOREACH(shared_ptr<IndexDef> idef, *ipar.index_defs)
		{
			if(context.mvars.count(idef->index))
			{
				throw "meta-variable '" + idef->index + "' redefined";
			}
			context.mvars[idef->index] = idef->set;
		}

		//convert subproc with new context
		string subproc = to_mcrl2(ipar.proc, context);
		//indent with 2 extra spaces if the process does not fit on one line.
		if(subproc.find('\n'))
		{
			replace_all(subproc, "\n", "\n  ");
			subproc = "\n  " + subproc;
		}

		//build proc declaration
		string proc_name = next_proc_name();
		string proc_def = make_proc_def(proc_name, subproc, context);
		proc_def += ";" + context.newline();
		proc_defs << proc_def;

		//build all calls to the proc definition we just made.
		//computes cartesian products, and works very c-ish because the std iterators
		//didn't agree with me. not at all optimal either, but who cares.
		ostringstream process;
		{
			//make a vector of vectors containing the domains to iterate over
			vector<vector<int> > ds;
			BOOST_FOREACH(shared_ptr<IndexDef> idef, *ipar.index_defs)
			{
				string mv_set = context.mvars[idef->index];
				vector<int> d(context.get_domain(mv_set, idef->iset));
				ds.push_back(d);
			}

			string add_args;
			//draw var names that we need to pass.
			pair<string, string> v;
			bool bb = false;
			BOOST_FOREACH(v, context.vars)
			{
				if(bb++) add_args += ", ";
				add_args += v.second;
			}

			stream_cartesian_product_args(process, ds, proc_name, add_args, " || ");
		}

		//add proc d
		return "(" + process.str() + ")";
    //return process.str();
	}
	string Converter::to_mcrl2(OrdinaryParallel& opar, Context context)
	{
		//we treat normal subprocesses and subprocesses that are also a form of parallelism different:
		//because the mCRL2 parallel operator is associative, we can embed them directly in the process
		//description rather than making a new process for them.
		//note: this differs from the formal conversion rules, but is semantically equivalent.

		list<string> subprocs;
		list<string> par_subprocs;
		BOOST_FOREACH(shared_ptr<Proc> p, opar.procs)
		{
			string subproc = to_mcrl2(p, context);

			//a hack: we also search the string for "||" because 
			// 1) we never put anything but named process calls directly in parallel with one another
			// 2) an OrdinaryParallel preceded by a new operator means that we'd still generate a proc too many
			//    if we'd dynamic_pointer_cast only
			// 3) IndexedParallel does *not* necessarily create a "||", so we still need to dynamic_pointer_cast
			//    for that.
			//what we actually want is "is the subprocess just one named process call or multiple ones in parallel?"
			//a regex could do, too, maybe.
			if(dynamic_pointer_cast<Parallel>(p) || dynamic_pointer_cast<DY>(p) || (subproc.find("||") != string::npos))
			{
				par_subprocs.push_back(subproc);
			}
			else
			{
				subprocs.push_back(subproc);
			}
		}
		
		//build proc declarations
		
		//proc_defs (Pi):
		list<string> proc_names;
		{
			//Pi U proc A = yada U proc B = yada
			BOOST_FOREACH(string subproc, subprocs)
			{
				string proc_name = next_proc_name();
				proc_names.push_back(proc_name);
				string proc_def = make_proc_def(proc_name, subproc, context);
				proc_defs << proc_def << ";" << context.newline();
			}
		}
		//process:
		ostringstream process;
		{
			bool bb = false;
			BOOST_FOREACH(string proc_name, proc_names)
			{
				if(bb++) process << " || ";
				pair<string, string> mv, v;
				ostringstream proc_args;
				process << proc_name;
				proc_args << "(";
				//pass meta-vars
				bool b = false;
				BOOST_FOREACH(mv, context.mvars)
				{
					if(b++) proc_args << ", ";
					proc_args << mv.first;
				}
				//pass vars
				BOOST_FOREACH(v, context.vars)
				{
					if(b++) proc_args << ", ";
					proc_args << v.second;
				}
				proc_args << ")";
				if(b) process << proc_args.str();
			}
			BOOST_FOREACH(string par_subproc, par_subprocs)
			{
				if(bb++) process << " || ";
				process << par_subproc;
			}
		}
		return "(" + process.str() + ")";
    //return process.str();
	}
	string Converter::to_mcrl2(Send& send, Context context)
	{
		StringTemplate st = templates.get("send");
		bool b = false;
		BOOST_FOREACH(shared_ptr<Term> t, *send.terms)
		{
			if(b++) st["terms"] << ", ";
			st["terms"] << to_mcrl2(t, context);
		}

    st["newline"] << context.newline();
    st["subproc"] << to_mcrl2(send.proc, context);

		return st.get();
	}
	string Converter::to_mcrl2(Replication& repl, Context context)
	{
		gsWarningMsg("Ignoring replication operator %s.\n", repl.position_in_input().c_str());
		return to_mcrl2(repl.proc, context);
	}	
	string Converter::to_mcrl2(Zero& zero, Context context)
	{
    StringTemplate st = templates.get("zero");
    st["zero_action"] << options.zero_action;
    if(!options.zero_action.empty()) st["zero_action"] << ".";
    return st.get();
	}
	string Converter::to_mcrl2(DY& dy, Context context)
	{
		if(!context.mvars.empty())
		{
			throw "Attacker must occur before any indexed parallel operator";
		}


		StringTemplate st = templates.get("dy");
		
    st["known_names"] << "";
		pair<string, list<string> > n;
		bool b = false;
		BOOST_FOREACH(n, context.names)
		{
			//make a vector of vectors containing the domains to iterate over
			vector<vector<int> > ds;
			BOOST_FOREACH(string n_set, n.second)
			{
				shared_ptr<Iset> empty_iset;
				vector<int> d(context.get_domain(n_set, empty_iset));
				ds.push_back(d);
			}

			if(b++) st["known_names"] << ", ";
			stream_cartesian_product_args(st["known_names"], ds, n.first, "", ", ");
		}
		string hint = dy.hint();
		if(!hint.empty())
		{
			if(b++) st["known_names"] << ", ";
			st["known_names"] << hint;
		}

		contains_attacker = true;
		return st.get();
	}
	string Converter::to_mcrl2(New& newn, Context context)
	{
		if(context.names.count(newn.name->name))
		{
			throw "name '" + newn.name->name + "' redeclared";
		}
		if(names.count(newn.name->name))
		{
			throw "not well-formed: name '" + newn.name->name + "' declared twice";
		}
		//save in list of all names
		names[newn.name->name] = newn.name->indices->size();

		//add to N component of context
		//operator[] automatically creates an empty list on the first run
		context.names[newn.name->name];
		BOOST_FOREACH(string index, *newn.name->indices)
		{
			string set_ident;
			//is this index defined in the new operator itself?
			BOOST_FOREACH(shared_ptr<IndexDef> id, *newn.index_defs)
			{
				if(index==id->index)
				{
					set_ident = id->set;
					break;
				}
			}
			//or was this defined earlier, thus inside the Upsilon component of the context?
			//map<string, string>::iterator mvar_it = context.mvars.find(index);
			//if(mvar_it!=context.mvars.end())
      if(context.mvars.count(index))
			{
				if(set_ident!="")
				{
					throw "Meta-variable '" + index + "' in new operator already defined";
				}
				set_ident = context.mvars[index];
			}
			if(set_ident=="")
			{
				throw "Meta-variable '" + index + "' in new operator used but not defined";
			}
			assert(set_ident!="");

			//add set identifier to the N component.
			context.names[newn.name->name].push_back(set_ident);
		}
		//only context is changed by new operator, so we simply return the subprocess' mcrl2 conversion.
		return to_mcrl2(newn.proc, context);
	}

	string Converter::to_mcrl2(Receive& recv, Context context)
	{
		//pair<string, string> lists_sum = to_mcrl2(*recv.terms, context);
		//string process = lists_sum.second + "read(" + lists_sum.first + ")." + context.newline();

		StringTemplate st(templates.get("receive"));
    to_mcrl2(*recv.terms, context, st);
		//st["newline"]    << context.newline();
		st["subproc"]    << to_mcrl2(recv.proc, context);
    

		return st.get();
	}
	string Converter::to_mcrl2(Decrypt& decr, Context context)
	{
		//FIXME: edit report to include "encrypt"s
		//pair<string, string> lists_sum = to_mcrl2(*decr.terms, context, decr.key);

		StringTemplate st(templates.get("decrypt"));
		st["ciphertext"] << to_mcrl2(decr.ciphertext, context);
    to_mcrl2(*decr.terms, context, st, decr.key);
		st["anno"]       << to_mcrl2(*decr.anno, context);
		st["subproc"]    << to_mcrl2(decr.proc, context);

		return st.get();
		//string process = lists_sum.second + "decrypt(" + to_mcrl2(decr.ciphertext, context) + ", " + lists_sum.first + ", " + to_mcrl2(*decr.anno) + ")." + context.newline();
	}
	//returns 2 mcrl2 list literals separated by a comma, and a sum expression
	void Converter::to_mcrl2(PMatchTerms& pmt, Context& context, StringTemplate& st, shared_ptr<Term> key)
	{
    //include "sum" part of expression only if there are > 0 variables to be sum'ed over.
    st.replace("sum", (pmt.vars->empty()) ? "" : templates.fmt_string("sum"));


    //draw terms that are used for pattern matching
		{
      st["pm_terms"]; 
			bool b = false;
			BOOST_FOREACH(shared_ptr<Term> t, *pmt.match_terms)
			{
				if(b++) st["pm_terms"] << ", ";
				st["pm_terms"] << to_mcrl2(t, context);
			}
    }

    if(key.get() && st.has("key"))
    {
      st["key"]        << to_mcrl2(key, context);
    }

    //update context, step 1.
		//update mCRL2 var names that are used in pattern matching
    //TODO: does not currently find variables that are nested deeper than top-level
    Term_list match_terms(*pmt.match_terms);
    ostringstream prime_vars;
    if(st.has("prime_vars"))
    {
      bool b = false;
      prime_vars << "";
      if(key.get())
      {
        match_terms.push_back(key);
      }
		  BOOST_FOREACH(shared_ptr<Term> t, match_terms)
		  {
			  if(shared_ptr<Name> n = dynamic_pointer_cast<Name>(t))
			  {
				  map<string, string>::iterator v = context.vars.find(n->name);
				  if(v != context.vars.end())
				  {
					  v->second += "'";
            if(b++) prime_vars << ", ";
            prime_vars << v->second;
				  }
			  }
		  }
    }
    if(key.get() && st.has("key"))
    {
      st["key_updated"]        << to_mcrl2(key, context);
    }

    //include "sum" part of expression only if there are > 0 variables to be sum'ed over.
    st.replace("sum", (pmt.vars->empty() && prime_vars.str().empty()) ? "" : templates.fmt_string("sum"));
    if(st.has("prime_vars")) st["prime_vars"] << prime_vars.str();

    //draw terms that are used for pattern matching with updated context
		{
      st["pm_terms_updated"]; 
			bool b = false;
      BOOST_FOREACH(shared_ptr<Term> t, *pmt.match_terms)
			{
				if(b++) st["pm_terms_updated"] << ", ";
				st["pm_terms_updated"] << to_mcrl2(t, context);
			}
    }

    {
			//draw vars and type_exprs + update context, step 2.
  		bool b = false;
      st["vars"];
      st["type_exprs"];
			BOOST_FOREACH(shared_ptr<Term> t, *pmt.vars)
			{
        //add varname to context
				shared_ptr<TypedVar> tv = dynamic_pointer_cast<TypedVar>(t);
			  if(!tv)
			  {
				  throw "Must have typed variable names in pattern match expression";
			  }
				if(context.vars.count(tv->name))
				{
					throw "Variable '" + (string)*tv + "' redeclared";
				}
				context.vars[tv->name] = tv->name;

        //draw separators
				if(b++) 
        {
          st["vars"] << ", ";
          st["type_exprs"] << templates.fmt_string("type_sep");
        }

        //draw type expr
        StringTemplate sf_type = (tv->type==TypedVar::V_Name) ? templates.get("type_n") : templates.get("type_c");
        sf_type["var"] << tv->name;
        st["type_exprs"] << sf_type.get();

        //draw varname
				st["vars"] << context.vars[tv->name];
			}
      st["newline"] << context.newline(2);
    }

    //draw "id" fields
		/*
    {
  		st["ids"];
			bool b = false;
			BOOST_FOREACH(shared_ptr<Term> t, *pmt.match_terms)
			{
				if(b++) st["ids"] << ", ";
				st["ids"] << "id";
			}
    }
    */
	}
	//faking virtual functions, because we want to separate the AST classes from
	//the conversion rules. 
	//doing it the hard way because the visitor pattern must be the one pattern 
	//that the "gang of four" forgot to try out themselves.
	string Converter::to_mcrl2(E_ptr expr, const Context &context)
	{
		try
		{
			if(shared_ptr<Let> let = dynamic_pointer_cast<Let>(expr))
			{
				return to_mcrl2(*let, context);
			}
			else if(shared_ptr<OrdinaryParallel> opar = dynamic_pointer_cast<OrdinaryParallel>(expr))
			{
				return to_mcrl2(*opar, context);
			}
			else if(shared_ptr<IndexedParallel> ipar = dynamic_pointer_cast<IndexedParallel>(expr))
			{
				return to_mcrl2(*ipar, context);
			}
			else if(shared_ptr<Zero> zero = dynamic_pointer_cast<Zero>(expr))
			{
				return to_mcrl2(*zero, context);
			}
			else if(shared_ptr<DY> dy = dynamic_pointer_cast<DY>(expr))
			{
				return to_mcrl2(*dy, context);
			}
			else if(shared_ptr<Send> send = dynamic_pointer_cast<Send>(expr))
			{
				return to_mcrl2(*send, context);
			}
			else if(shared_ptr<Replication> repl = dynamic_pointer_cast<Replication>(expr))
			{
				return to_mcrl2(*repl, context);
			}
			else if(shared_ptr<Receive> recv = dynamic_pointer_cast<Receive>(expr))
			{
				return to_mcrl2(*recv, context);
			}
			else if(shared_ptr<Decrypt> decr = dynamic_pointer_cast<Decrypt>(expr))
			{
				return to_mcrl2(*decr, context);
			}
			else if(shared_ptr<New> newn = dynamic_pointer_cast<New>(expr))
			{
				return to_mcrl2(*newn, context);
			}
			else if(shared_ptr<Ciphertext> ct = dynamic_pointer_cast<Ciphertext>(expr))
			{
				return to_mcrl2(*ct, context);
			}
			else if(shared_ptr<Name> name = dynamic_pointer_cast<Name>(expr))
			{
				return to_mcrl2(*name, context);
			}
		}
		catch(string e)
		{
			throw e + " " + expr->position_in_input();
		}
		throw "Error: unsupported LySa operation detected " + expr->position_in_input();
	}

	string Converter::to_mcrl2(Ciphertext& ct, const Context &context)
	{
		StringTemplate st(templates.get("ciphertext"));

		bool b = false;
		BOOST_FOREACH(shared_ptr<Term> t, *ct.terms)
		{
			if(b++) st["terms"] << ", ";
			st["terms"] << to_mcrl2(t, context);
		}

		st["key"]      << to_mcrl2(ct.key, context);
		st["anno"]     << to_mcrl2(*ct.anno, context);

		return st.get();
	}
	string Converter::to_mcrl2(Name& name, const Context &context)
	{
		
		map<string, list<string> >::const_iterator n = context.names.find(name.name);
		if(n==context.names.end())
		{
			map<string, string>::const_iterator v = context.vars.find(name.name);
			if(v==context.vars.end())
			{
				throw "Name '" + (string)name + "' used but not declared; free names are not allowed";
			}
			else
			{
				//name is a variable; ignore indices
				return v->second;
			}
		}
		else
		{
			//name is a real name.
			//FIXME: check that indices match: what should exactly match? 
			//index names? set names? set domains? set domain must be subset?
			string indices = join(*name.indices, ", ");
			if(!indices.empty()) indices = "(" + indices + ")";

			//StringTemplate st(templates.get("name"));
			StringTemplate st(templates, "name");
			st["name"] << name.name << indices;

			return st.get();
		}
	}
	string Converter::to_mcrl2(Annotation& anno, const Context &context)
	{
		if(anno.empty())
		{
			return templates.get("anno_none").get();
		}
		else
		{
      if(anno.dest_orig->empty())
      {
        StringTemplate st(templates.get("anno_s"));
        st["at"] << to_mcrl2(*anno.at);
        return st.get();
      }
      else
      {
        StringTemplate st(templates.get("anno"));
        bool b = false;
			  //Cryptopoint_list& cps = *(anno.dest_orig);
			  BOOST_FOREACH(shared_ptr<Cryptopoint> cp, *anno.dest_orig)
			  {
				  if(b++) st["destorig"] << ", ";
				  st["destorig"] << to_mcrl2(*cp);
			  }
      
			
			  st["at"] << to_mcrl2(*anno.at);

        {
          StringTemplate sf_ai(templates.get("cpdy_ai"));
          sf_ai["indices"] << "";
			    if(!anno.process_info->options.attacker_index.empty()) {
				    bool b = false;
				    pair<string, string> mv;
				    BOOST_FOREACH(mv, context.mvars)
				    {
					    if(b++) sf_ai["indices"] << ", ";
					    sf_ai["indices"] << mv.first;
				    }
            st["cpdy_ai"] << sf_ai.get();
          }
          else
          {
            st["cpdy_ai"] << "";
          }
        }

			  return st.get();
      }
		}
	}
	//aux function.
	string Converter::to_mcrl2(Cryptopoint& cp)
	{
		//save in list of all cryptopoints
		if(cryptopoints.count(cp.name))
		{
			if(cryptopoints[cp.name]!= (int) cp.indices->size())
			{
				throw "crypto-point '" + (string)cp + "' redefined with wrong amount of indices";
			}
		}
		else
		{
			cryptopoints[cp.name]=cp.indices->size();
		}

		//build string
		string is = join(*cp.indices, ", ");
		if(is=="")
			return cp.name;
		else
			return cp.name + "(" + is + ")";
	}

	void stream_sort_def(ostringstream& structs, map<string, int>& m)
	{
		pair<string, int> p;
		//find max length of identifier
		int maxlen = 0;
		BOOST_FOREACH(p, m)
		{
			maxlen = max(maxlen, (int)p.first.size());
		}

		bool b = false;
		BOOST_FOREACH(p, m)
		{
			if(b++) structs << "  | ";
			else structs << "    ";
			structs << p.first;
			if(p.second>0)
			{
				int padlen = maxlen - p.first.size();
				for(int i=0;i!=padlen;i++) structs << " ";

				structs << "(";
				for(int i=0;i!=p.second;i++)
				{
					if(i) structs << ", ";
					structs << "Nat";
				}
				structs << ")";
			}
			structs << "\n";
		}
		structs << ";\n";
	}
	string Converter::init_to_mcrl2(E_ptr expr)
	{
		Context c;
		string process = to_mcrl2(expr, c);
    if((options.make_symbolic) && (!contains_attacker))
		{
			throw "Cannot convert process to mCRL2: process must contain attacker entry point.";
		}

		ostringstream structs;

		//make name sort
		names["known_name"] = 0;
		structs << "sort Name = struct\n";
		stream_sort_def(structs, names);

		//make crypto-point sort
		structs << "sort CP = struct\n";
		cryptopoints["CPDY"] = 0;
		cryptopoints["all"] = 0;
		cryptopoints["UCP"] = 0;
		stream_sort_def(structs, cryptopoints);

    string attacker_index_mcrl2("");
    {
      string attacker_index = options.attacker_index;
      if(!attacker_index.empty())
      {
        StringTemplate st(templates.get("attacker_index"));
        st["index"] << attacker_index;
        attacker_index_mcrl2 = st.get()+"\n";
      }
    }

    StringTemplate st = templates.get("init");
    st["proc"] << process;
    st["newline"] << "\n";
		string mcrl2 = 
			structs.str() + 
      attacker_index_mcrl2 +
			proc_defs.str() + 
      st.get();

		/*string preamble;
		{
      ifstream inf(options.preamble_file_name.c_str());
			string s;
			while(getline(inf, s))
			{
				preamble += s + "\n";
			}
		}*/
		return options.strategy->getPreamble() + mcrl2;
	}
}

