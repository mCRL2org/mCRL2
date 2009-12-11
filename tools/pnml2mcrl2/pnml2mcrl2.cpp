// Author(s): Yaroslav Usenko and Johfra Kamphuis
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pnml2mcrl2.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

#define TOOLNAME "pnml2mcrl2"
#define AUTHOR "Johfra Kamphuis and Yaroslav Usenko"

#include <sstream>
#include <ostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <ticpp.h>
#include <aterm2.h>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/numeric_string.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/exception.h"

//Tool framework
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/squadt_tool.h"


using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2;
using utilities::tools::input_output_tool;
using namespace mcrl2::utilities::tools;

typedef struct {
  ATbool Abort;                 // if an element has no ID, this boolean is used to grant abortion of the conversion

  // read-in
  ATermTable place_name;	// place_id -> name
  ATermTable place_mark;	// place_id -> Nat
  ATermTable place_mark_mcrl2;  // place_id -> Bag(SortExpr)
  ATermTable place_type_mcrl2;	// place_id -> SortExpr
  ATermTable trans_name;	// trans_id -> name
  ATermTable trans_predicate;	// trans_id -> DataExpr
  ATermTable arc_in;	        // arc_id -> trans_id x place_id
  ATermTable arc_out;	        // arc_id -> place_id x trans_id
  ATermTable arc_inhibit;	// arc_id -> place_id x trans_id (always this way)
  ATermTable arc_reset;	        // arc_id -> trans_id x place_id (always this way)
  ATermTable arc_name;          // arc_id -> name

  // generate
  ATermTable place_in;	        // place_id -> List(arc_id) (arc_in)
  ATermTable trans_in;	        // trans_id -> List(arc_id) (arc_out+arc_inhibit)
  ATermTable place_out;	        // place_id -> List(arc_id) (arc_out)
  ATermTable trans_out;	        // trans_id -> List(arc_id) (arc_in+arc_reset)
  ATermTable place_inhibit;     // place_inhibit -> List(arc_id) (arc_inhibit)
  ATermTable place_reset;       // place_reset -> List(arc_id) (arc_reset)
  ATermTable arc_type_mcrl2;    // arc_id -> SortExpr
  // not needed as thay are the same as arc_out
  //ATermTable transition_inhibit;	// transition_inhibit -> List(arc_id) (arc_inhibit)
  // not needed as thay are the same as arc_in
  //ATermTable transition_reset;	// transition_reset -> List(arc_id) (arc_reset)

  // needed for the creation of general mCRL2 processes
  ATermList transitions;      // store all the mCRL2 processes (involving transitions) needed for the Trans-process
  ATermTable place_process_name; // place_id -> name of the corresponding process
} Context;
static Context context;

static ATbool rec_par=ATtrue;
static ATbool colored=ATfalse;
static ATbool with_colors=ATtrue;
static ATbool reset_arcs=ATfalse;
static ATbool inhibitor_arcs=ATfalse;
static unsigned long error=0;
static ATbool hide=ATfalse;

static ATermAppl Appl0;                //=gsString2ATermAppl("_");
static ATermAppl IdX;                  //=gsMakeId(gsString2ATermAppl("x"));
static ATermAppl Number0;              //=gsMakeId(gsString2ATermAppl("0"));
static ATermAppl Number1;              //=gsMakeId(gsString2ATermAppl("1"));
static ATermAppl OpAnd;                //=gsMakeId(gsMakeOpIdNameAnd());
static ATermAppl OpAdd;                //=gsMakeId(gsMakeOpIdNameAdd());
static ATermAppl OpSubt;               //=gsMakeId(gsMakeOpIdNameSubt());
static ATermAppl OpMax;                //=gsMakeId(gsMakeOpIdNameMax());
static ATermAppl OpMin;                //=gsMakeId(gsMakeOpIdNameMin());
static ATermAppl OpGT;                 //=gsMakeId(gsMakeOpIdNameGT());
static ATermAppl OpLTE;                //=gsMakeId(gsMakeOpIdNameLTE());
static ATermAppl OpEq;                 //=gsMakeId(gsMakeOpIdNameEq());
static ATermAppl OpInt2Nat;            //=gsMakeId(gsMakeOpIdNameInt2Nat());
static ATermAppl EmptyBag;             //=gsMakeId(gsMakeOpIdNameEmptyBag());
static ATermAppl OpBagEnum;            //=gsMakeId(gsMakeOpIdNameBagEnum());
static ATermAppl nMaxTokens;           //=gsMakeId(gsString2ATermAppl("nMaxTokens"));
static ATermAppl ErrorAction;          //=gsMakeParamId(ATmakeAppl0(ATmakeAFunId("_error")), ATmakeList0());

static ATermList pn2gsGeneratePlaceAlternative(ATerm PlaceID);
static ATermList pn2gsGenerateTransitionAlternative(ATerm TransID);
static inline ATermAppl pn2gsMakeDataApplProd2(ATermAppl Op, ATermAppl Left, ATermAppl Right){
  return gsMakeDataAppl(Op,ATmakeList2((ATerm)Left,(ATerm)Right));
}

static inline ATermAppl pn2gsMakeIfThenUntimed(ATermAppl Cond, ATermAppl Then){
  return gsMakeIfThenElse(Cond,Then,gsMakeDelta());
}

static ATermList pn2gsMakeIds(ATermList l);
static ATermList pn2gsMakeDataVarIds(ATermList l, ATermAppl Type);
static ATermAppl pn2gsMakeBagVars(ATermList l);
static ATermList pn2gsMakeListOfLists(ATermList l);

static std::string pn2gsGetText(ticpp::Element* cur);

bool do_pnml2mcrl2(char const* InFileName, std::ostream& output_stream);

class pnml2mcrl2_tool: public squadt_tool< input_output_tool>
{
  protected:
    typedef squadt_tool< input_output_tool> super;

  public:
    pnml2mcrl2_tool()
      : super(
          TOOLNAME,
          AUTHOR,
          "convert a Petri net to an mCRL2 specification",
          "Convert a Petri net in INFILE to an mCRL2 specification, and write it to "
          "OUTFILE. If INFILE is not present, stdin is used. If OUTFILE is not present, "
          "stdout is used. INFILE is supposed to conform to the EPNML 1.1 standard."
          "\n\n"
          "Only classical Petri nets are translated, i.e. places, transitions and arcs. "
          "Other constructs such as timing, coloring, inhibitor arcs and hierarchy are "
          "not taken into account. "
          "With the -p option turned on, more functionality is supported.")
    {}

  private:

    void parse_options(const command_line_parser& parser)
    { super::parse_options(parser);

      if (parser.options.count("error")) {
        error = parser.option_argument_as< unsigned long >("error");
      }
      if (parser.options.count("hide")) {
        hide = ATtrue;
      }
      if (parser.options.count("no-rec-par")) {
        rec_par = ATfalse;
      }
    }


    void add_options(interface_description& desc)
    { super::add_options(desc);
      desc.add_option("error", make_optional_argument("NUM", "2"),
                    "an __error action will happen if a place gets NUM or more "
                    "tokens (default is 2)", 'e').
           add_option("hide",
                    "hide (rename to tau) all transition monitoring actions to "
                    "hide all but one action edit the generated file and remove "
                    "that action from the hide list", 'i').
           add_option("no-rec-par",
                    "generate non-recursive mCRL2 process for the places, "
                    "and enable the translation of inhibitor and reset arcs", 'p');
    }

  public:
    bool run()
    {
      if(output_filename().empty())
      {
        return do_pnml2mcrl2(input_filename().c_str(), std::cout);
      }
      else
      {
        std::ofstream output_stream(output_filename().c_str());
        if(!output_stream.is_open())
        {
          throw mcrl2::runtime_error("cannot open file '" + output_filename() + "' for writing\n");
        }
        bool result = do_pnml2mcrl2(input_filename().c_str(), output_stream);
        output_stream.close();
        return result;
      }
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY
    void set_capabilities(tipi::tool::capabilities& c) const
    {
      c.add_input_configuration("main-input", tipi::mime_type("pnml", tipi::mime_type::text), tipi::tool::category::transformation);
    }

    void user_interactive_configuration(tipi::configuration& c) {
      /* set the squadt configuration to be sent back, such
       * that mcrl22lps can be restarted later with exactly
       * the same parameters
       */
      if (c.fresh()) {
        if (!c.output_exists("main-output")) {
          c.add_output("main-output", tipi::mime_type("mcrl2", tipi::mime_type::text), c.get_output_name(".mcrl2"));
        }
      }
    }

    bool check_configuration(tipi::configuration const& c) const
    {
      return c.input_exists("main-input") ||
             c.output_exists("main-output");
    }

    bool perform_task(tipi::configuration& c)
    { using namespace tipi;

      rec_par=ATfalse;

      m_input_filename  = c.get_input("main-input").location();
      m_output_filename = c.get_output("main-output").location();

      return run();
    }
#endif
};

  //====================================
  //ADDED BY YARICK: AFun extensions.
  //====================================

  //==================================================
  // ATmakeAFunInt functions as ATmakeAFun, except that the name is an int and not a (char *)
  //==================================================
  static AFun ATmakeAFunInt(int name, int arity, ATbool quoted) {
    // input: an integer value (name), it's arity and whether it is quoted or not
    // output: an AFun, as in ATmakeAFun, but now with a name from an integer value
    std::ostringstream s;

    s << name;
    AFun f = ATmakeAFun(s.str().c_str(), arity, quoted);
    return f;
  }

  //==================================================
  // ATmakeAFunInt0 functions as ATmakeAFunInt(name,0,ATtrue)
  //==================================================
  static inline AFun ATmakeAFunInt0(int name){
    return ATmakeAFunInt(name, 0, ATtrue);
  }

  //==================================================
  // ATmakeAFunId functions as ATmakeAFun(name,0,ATtrue)
  //==================================================
  static inline AFun ATmakeAFunId(char const*name){
    return ATmakeAFun(name, 0, ATtrue);
  }

  //==================================================
  // ATprependAFun functions as ATmakeAFun
  //==================================================
  static AFun ATprependAFun(const char *str, AFun id) {
    // input: an AFun
    // output: an AFun prepended with str

    char *name=ATgetName(id);
    char *buf = (char *) malloc(strlen(str)+strlen(name)+1);
    assert(buf);

    strcpy(buf,str);
    strcat(buf,name);

    AFun Res=ATmakeAFun(buf, ATgetArity(id), ATisQuoted(id));
    free(buf);

    return Res;
  }

  //==================================================
  // ATappendAFun functions as ATmakeAFun
  //==================================================
  static AFun ATappendAFun(AFun id, const char *str) {
    // input: an AFun
    // output: an AFun appended with str

    char *name=ATgetName(id);
    char *buf = (char *) malloc(strlen(str)+strlen(name)+1);
    assert(buf);

    strcpy(buf,name);
    strcat(buf,str);

    AFun Res=ATmakeAFun(buf, ATgetArity(id), ATisQuoted(id));
    free(buf);

    return Res;
  }

  //====================================
  //End ADDED BY YARICK: AFun extensions.
  //====================================


  /*                                 */
  /*                                 */
  /*                                 */
  /* Functions for the read-in phase */
  /*                                 */
  /*                                 */
  /*                                 */

  //==================================================
  // pn2gsCheckString checks if a string is of the format [a-zA-Z_][a-zA-Z0-9_]*
  //==================================================
  static AFun pn2gsCheckAFun(AFun id) {
    // input: an AFun
    // output: an AFun with the string of the format [a-zA-Z_][a-zA-Z0-9_]*
    //         If the input-string is already of this format, it is returned unchanged.
    //         If a string is of a different format, all characters that do not follow the format are replaced with an _
    std::string name(ATgetName(id));

    // check if the first character is of format [a-zA-Z_]
    if(!(isalpha(name[0])||name[0]=='_')) {
      // first character does not follow the format
      // put 'c_' in front of the String
      id=ATprependAFun("c_",id);
    }

    name=ATgetName(id);

    for(size_t i=0; i< name.size(); ++i) {
      if(!(isalnum(name[i]))) {
  // if a character in the string is not [a-zA-Z0-9_], replace it by an '_'
  name[i]='_';
      }
    }

    id=ATmakeAFun(name.c_str(),ATgetArity(id),ATisQuoted(id));

    return id;
  }

  //==================================================
  // pn2gsRetrieveTextWithCheck gets the contents of a child <text> element of cur, and checks it to format [a-zA-Z_][a-zA-Z0-9_]*
  //==================================================
  static ATermAppl pn2gsRetrieveTextWithCheck(ticpp::Element* cur) {
    // input: a pointer to the current element
    // output: the contents of the first child <text> attribute
    //         of the current element, following format [a-zA-Z_][a-zA-Z0-9_]*

    // this function is used for the retrieval of names and ids

    for (cur = cur->FirstChildElement(false); cur != 0; cur = cur->NextSiblingElement(false)) {
      if (cur->Value() == "text") {
        return ATmakeAppl0(pn2gsCheckAFun(ATmakeAFunId(cur->GetText().c_str())));
      }
    }

    return 0;
  }

  //==================================================
  // pn2gsRetrieveText gets the contents of a child <text> element of cur
  //==================================================
  static ATermAppl pn2gsRetrieveText(ticpp::Element* cur) {
    // input: a pointer to the current element
    // output: the contents of the first child <text> attribute
    //         of the current element

    // this function is used for the retrieval of types, initial markings, etc.
    for (cur = cur->FirstChildElement(false); cur != 0; cur = cur->NextSiblingElement(false)) {
      if (cur->Value() == "text") {
        return (gsString2ATermAppl(cur->GetText().c_str()));
      }
    }

    return 0;
  }
  //==================================================
  // pn2gsGetText gets the contents of a child <text> element of cur
  //==================================================
  static std::string pn2gsGetText(ticpp::Element* cur) {
    // input: a pointer to the current element
    // output: the contents of the first <text> attribute
    //         of the current element

    // this function is used for the retrieval of types, initial markings, etc.
    while (cur != 0) {
      if (cur->Value() == "text") {
        return cur->GetText(); //const_cast < char* > (cur->GetText().c_str());
      }

      cur = cur->NextSiblingElement(false);
    }

    return std::string();
  }

  //==================================================
  // pn2gsGetElement gets the contents of a child name element of cur
  //==================================================
  static std::string pn2gsGetElement(ticpp::Element* cur, std::string const& name) {
    // input: a pointer to the current element
    // output: the contents of the first child <name> attribute
    //         of the current element

    // this function is used for the retrieval of types, initial markings, etc.

    for (cur = cur->FirstChildElement(false); cur != 0; cur = cur->NextSiblingElement(false)) {
      if (cur->Value() == name) {
        return (pn2gsGetText(cur->FirstChildElement(false)));
      }
    }

    return std::string();
  }

  //==================================================
  // pn2gsAterm_place converts a pnml-place to a usable ATerm
  //==================================================
  static ATermAppl pn2gsAterm_place(ticpp::Element* cur) {
    // input: a pointer to the current place
    // output: a usable translation of the place,
    //         if the place needs to be translated

    gsDebugMsg("> Start examining a place...  \n");
    // first, we want to retrieve the id of the place
    ATermAppl Aid;
    if (char const* id = cur->GetAttribute("id", false).c_str()) {
      // the place has an id, put it in Aid
      Aid = ATmakeAppl0((pn2gsCheckAFun(ATmakeAFunId(id))));
      gsDebugMsg("    id: '%T'\n", Aid);
    } else {
      // the place has NO id, so translation should be aborted!
      context.Abort = ATtrue;
      gsErrorMsg("A place has no ID. \n");
      return 0;
    }

    // second, we want to retrieve the necessary attributes of the place

    // temporary variables that contain data of the current place so this data can be returned
    // default values are assigned here
    ATermAppl Aname = gsString2ATermAppl("default_name");
    ATermAppl AinitialMarking = gsMakeId(gsString2ATermAppl("0"));
    ATermAppl Atype = gsString2ATermAppl("channel");

    ATermAppl Place_type=Appl0; //gsMakeSortId(gsString2ATermAppl("Unit"));
    ATermAppl Place_mcrl2initialMarking=Appl0;

    // this loop goes through all the children of the <place>element
    // these children will be translated or ignored, this depends on the element name
    for (cur = cur->FirstChildElement(false); cur != 0; cur = cur->NextSiblingElement(false)) {
      // current elements that are conceivable for translation are:
      // <name>  -  <initialMarking>  -  <type>
      // all other elements will be ignored in the translation

      if (cur->Value() == "name") {
  // the place contains a <name> element
  // a <name> element contains a childelement <text> which contains the name of the place
  // the name is retrieved below and assigned to Aname
  if (!(Aname=pn2gsRetrieveTextWithCheck(cur))) {
    Aname = gsString2ATermAppl("default_name");
  }
  gsDebugMsg("    name: '%T'\n", Aname);
      }
      else if (cur->Value() == "initialMarking") {
  // the place contains an <initialMarking> element
  // this element contains a childelement <text> which contains the initial marking of the place
  // this marking is retrieved below and assigned to AinitialMarking
        //for coloured petri nets initialMarking can also contain a toolspecific element
        for (ticpp::Element* curl = cur->FirstChildElement(false); curl != 0; curl = curl->NextSiblingElement(false)) {
          if (curl->Value() == "text") {
      std::string im=pn2gsGetText(curl);
            //if(im){
            std::istringstream iss(im);
            ATermAppl Marking=parse_data_expr(iss);
            if(!Marking) {gsErrorMsg("Parsing of the initial marking for place %T failed\n", Aid); return NULL;}
      AinitialMarking=Marking;
            //}

            //if (!im) im="0";
      //if (atoi(im) < 0) {
      //  // if the initial marking is less than zero, it is resetted to zero
      //  im="0";
      //  gsWarningMsg("Place with id '%T' has initial marking is less than 0, resetting initial marking to 0! \n", Aid);
            //}
            //AinitialMarking=im;
          }
          else if(with_colors && curl->Value() == "toolspecific") {
            std::string mcrl2marking=pn2gsGetElement(curl,"mcrl2marking");
            if(mcrl2marking!=""){
              colored=ATtrue;
              std::istringstream iss(mcrl2marking);
              ATermAppl A=parse_data_expr(iss);
              if(!A) {gsErrorMsg("Parsing of the mCRL2 initial marking for place %T failed\n", Aid); return NULL;}
              Place_mcrl2initialMarking=A;
            } else {
              gsWarningMsg("Ignore an element named 'toolspecific'\n");
            }
          }
    else {
            gsWarningMsg("Ignore an element named '%s'.\n", curl->Value().c_str());
          }
  }
  gsDebugMsg("    initialMarking: '%T'\n", AinitialMarking);
      }
      else if (cur->Value() == "type") {
  // the place contains an <type> element
  // this element contains a childelement <text> which contains the type of the place

  if (!(Atype=pn2gsRetrieveText(cur))) {
    Atype = gsString2ATermAppl("channel");
  }
  if (!ATisEqual(Atype, gsString2ATermAppl("channel"))) {
    // the type should either be omitted or have the value "channel"
    // otherwise the place does not need to be translated!
    gsWarningMsg("Place with id '%T' has type '%T' and will not be translated.\n", Aid, Atype);
    return NULL;
  }
  gsDebugMsg("    type: '%T'\n", Atype);
      }
      else if (cur->Value() == "toolspecific") {
  // the place contains an <toolspecific> element
  // this element contains a childelement <mcrl2sort> which contains
  // a childelement <text> which contains the type of the place

  std::string sort=pn2gsGetElement(cur,"mcrl2sort");

  if(!sort.empty()){
    colored=ATtrue;
    std::istringstream iss(sort);

    ATermAppl Sort=parse_sort_expr(iss);
    if(!Sort) {gsErrorMsg("Parsing of mCRL2 sort for place %T failed\n", Aid); return NULL;}
          Place_type=Sort;
  }
  else {
    gsWarningMsg("Ignore an element named 'toolspecific'\n");
  }
      }
      else {
  gsWarningMsg("Ignore an element named '%s'.\n", cur->Value().c_str());
      }
    }

    // argument order of returnvalue is id - name - initialMarking - Sort
    return ATmakeAppl5(ATmakeAFun("place", 5, ATfalse), (ATerm)Aid, (ATerm)Aname, (ATerm)AinitialMarking, (ATerm)Place_type, (ATerm)Place_mcrl2initialMarking);
  }

  //==================================================
  // pn2gsAterm_trans converts a pnml-transition to a usable ATerm
  //==================================================
  static ATermAppl pn2gsAterm_trans(ticpp::Element* cur) {
    // input: a pointer to the current transition
    // output: a usable translation of the transition,
    //         if the transition needs to be translated

    gsDebugMsg("> Start examining a transition...  \n");
    // first, we want to retrieve the id of the transition
    ATermAppl Aid;
    if (char const* id = cur->GetAttribute("id", false).c_str()) {
      // the transition has an id, put it in Aid
      Aid = ATmakeAppl0((pn2gsCheckAFun(ATmakeAFunId(id))));
      gsDebugMsg("    id: '%T'\n", Aid);
    } else {
      // the transition has NO id, so translation should be aborted!
      context.Abort = ATtrue;
      gsErrorMsg("A transition has no ID. \n");
      return 0;
    }

    // second, we want to retrieve the necessary attributes of the transition

    // temporary variables that contain data of the current transition so this data can be returned
    // default values are assigned here
    ATermAppl Aname = gsString2ATermAppl("default_name");
    ATermAppl Atype = gsString2ATermAppl("AND");

    ATermAppl Trans_predicate=Appl0;

    // this loop goes through all the children of the <transition>element
    // these children will be translated or ignored, this depends on the element name
    for (cur = cur->FirstChildElement(false); cur != 0; cur = cur->NextSiblingElement(false)) {
      // current elements that are conceivable for translation are:
      // <name>  -  <type>
      // all other elements will be ignored in the translation

      if (cur->Value() == "name") {
  // the transition contains a <name> element
  // a <name> element contains a childelement <text> which contains the name of the transition
  // the name is retrieved below and assigned to Aname
  if (!(Aname=pn2gsRetrieveTextWithCheck(cur))) {
    Aname = gsString2ATermAppl("default_name");
  }
  gsDebugMsg("    name: '%T'\n", Aname);
      }
      else if (with_colors && cur->Value() == "toolspecific") {
  // the transition contains a <toolspecific> element
  // this element contains a childelement <mcrl2sort> which contains
  // a childelement <text> which contains the type of the place

  std::string predicate=pn2gsGetElement(cur,"mcrl2predicate");
        //gsVerboseMsg("predicate %s\n", predicate);

  if(!predicate.empty()){
    colored=ATtrue;
    std::istringstream iss(predicate);
    ATermAppl Predicate=parse_data_expr(iss);
          if(!Predicate) {gsErrorMsg("Parsing of the mCRL2 predicate for transition %T failed\n", Aid); return NULL;}
    Trans_predicate=Predicate;
  }
  else {
    gsWarningMsg("Ignore an element named 'toolspecific'\n");
  }
      }
      else if (cur->Value() == "type") {
  // the transition contains an <type> element
  // this element contains a childelement <text> which contains the type of the transition
  if (!(Atype=pn2gsRetrieveText(cur))) {
    Atype = gsString2ATermAppl("AND");
  }
  if (!ATisEqual(Atype, gsString2ATermAppl("AND"))) {
    // the type should either be omitted or have the value "AND"
    // otherwise the place does not need to be translated!
    gsWarningMsg("Transition with id '%T' has type '%T' and will not be translated.\n", Aid, Atype);
    return NULL;
  }
  gsDebugMsg("    type: '%T'\n", Atype);
      }
      else {
  gsWarningMsg("Ignore an element named '%s'.\n", cur->Value().c_str());
      }
    }

    // argument order of returnvalue is id - name
    return ATmakeAppl3(ATmakeAFun("transition", 3, ATfalse), (ATerm)Aid, (ATerm)Aname, (ATerm)Trans_predicate);
  }

  //==================================================
  // pn2gsAterm_arc converts a pnml-arc to a usable ATerm
  //==================================================
  static ATermAppl pn2gsAterm_arc(ticpp::Element* cur) {
    // input: a pointer to the current arc
    // output: a usable translation of the arc,
    //         if the arc needs to be translated

    gsDebugMsg("> Start examining an arc...  \n");
    // first, we want to retrieve the id of the arc
    ATermAppl Aid;
    if (char const* id = cur->GetAttribute("id", false).c_str()) {
      // the arc has an id, put it in Aid
      Aid = ATmakeAppl0((pn2gsCheckAFun(ATmakeAFunId(id))));
      gsDebugMsg("    id: '%T'\n", Aid);
    } else {
      // the arc has NO id, so translation should be aborted!
      context.Abort = ATtrue;
      gsErrorMsg("An arc has no ID. \n");
      return 0;
    }

    // second, we want to retrieve the source and the target of the arc
    ATermAppl Asource;
    if (char const* source = cur->GetAttribute("source", false).c_str()) {
      Asource = ATmakeAppl0((pn2gsCheckAFun(ATmakeAFunId(source))));
      gsDebugMsg("    source: '%T'\n", Asource);
    } else {
      // the arc has NO source, so the arc will not be translated!
      gsWarningMsg("Arc with id '%T' has no source and will not be translated.\n", Aid);
      return 0;
    }

    ATermAppl Atarget;
    if (char const* target = cur->GetAttribute("target", false).c_str()) {
      Atarget = ATmakeAppl0(pn2gsCheckAFun(ATmakeAFunId(target)));
      gsDebugMsg("    target: '%T'\n", Atarget);
    } else {
      // the arc has NO target, so the arc will not be translated!
      gsWarningMsg("Arc with id '%T' has no target and will not be translated.\n", Aid);
      return 0;
    }

    // third, we want to verify the arc needs translation (thus is of the correct type)

    // temporary variables that contain data of the current arctype
    ATermAppl Atype = Appl0;

    ATermAppl Arc_name=Appl0;

    // this loop goes through all the children of the <arc>element
    // these children will be examined or ignored, this depends on the element name
    for (cur = cur->FirstChildElement(false); cur != 0; cur = cur->NextSiblingElement(false)) {
      // current elements that are conceivable for examination are:
      // <type>
      // all other elements will be ignored in the translation

      if (cur->Value() == "type") {
  // the arc contains a <type> element
  // this element contains a childelement <text> which contains the type of the transition
  if (!(Atype=pn2gsRetrieveText(cur))) {
    Atype = Appl0;
  }
  if (ATisEqual(Atype, gsString2ATermAppl("inhibitor"))) {
    // the type should be omitted
    // otherwise the arc does not need to be translated!
    // gsWarningMsg("Arc with id '%T' has type '%T' and will not be translated.\n", Aid, Atype);
    inhibitor_arcs=ATtrue;
  }
  else if (ATisEqual(Atype, gsString2ATermAppl("reset"))) {
    // the type should be omitted
    // otherwise the arc does not need to be translated!
    // gsWarningMsg("Arc with id '%T' has type '%T' and will not be translated.\n", Aid, Atype);
          reset_arcs=ATtrue;
  }
  else if (!ATisEqual(Atype, Appl0)) {
    // the type should be omitted
    // otherwise the arc does not need to be translated!
    gsWarningMsg("Arc with id '%T' has type '%T' and will not be translated.\n", Aid, Atype);
    return NULL;
  }
  gsDebugMsg("    type: '%T'\n", Atype);
      }
      else if (cur->Value() == "name") {
  // the arc contains a <name> element
  // this element contains a childelement <text> which contains the type of the transition
        std::string name=pn2gsGetText(cur->FirstChildElement());
        if(!name.empty()){
          colored=ATtrue;
          std::istringstream iss(name);

          ATermAppl Name = parse_data_expr(iss);
          if(!Name) {gsErrorMsg("Parsing of the name of arc %T failed\n", Aid); return NULL;}
          Arc_name = ATAgetArgument(Name,0); // get rid of Id(_).
        }
        else {
    // the name should be omitted
    // otherwise the arc does not need to be translated!
    gsErrorMsg("Arc with id '%T' has unparseable name and will not be translated.\n", Aid);
  }
  if (Arc_name) {
    gsDebugMsg("    name: '%T'\n", Arc_name);
        }
      }
      else {
  gsWarningMsg("Ignore an element named '%s'.\n", cur->Value().c_str());
      }
    }

    // argument order of returnvalue is id - source - target
    return ATmakeAppl5(ATmakeAFun("arc", 5, ATfalse), (ATerm)Aid, (ATerm)Asource, (ATerm)Atarget, (ATerm)Atype, (ATerm)Arc_name);
  }

  /*                        */
  /*                        */
  /*                        */
  /* Read-in phase function */
  /*                        */
  /*                        */
  /*                        */

  //==================================================
  // pn2gsAterm converts the pnml-input to a usable ATerm
  //==================================================
  static ATermAppl pn2gsAterm(ticpp::Document& doc) {
    // input: a pointer of the type ticpp::Document which points to the parsed XML-file
    // output: an ATermAppl, translated from the XML-file,
    //         in which only relevant elements/attributes are concluded

    //==================================================
    // initializations and initial checks.
    //==================================================
    ticpp::Element* cur = doc.FirstChildElement(false);

    // cur should point to the <pnml>element
    if (cur == 0 || cur->Value() != "pnml") {
      gsErrorMsg("File is not a usable PNML file!  \n");

      return 0;
    }

    // the first <net>element, if any present, is selected by cur
    try {
      for (cur = cur->FirstChildElement(); cur->Value() != "net"; cur = cur->NextSiblingElement()) {
        gsWarningMsg("Element '%s' is not a Petri net and will be ignored (including it's sub-elements).\n", cur->Value().c_str());
      }
    }
    catch (ticpp::Exception&) {
      gsErrorMsg("File does not contain a Petri net. \n");

      return 0;
    }

    // cur now points to the first <net>element
    gsDebugMsg("\nStart converting the Petri net to an ATerm...  \n \n");

    //==================================================
    // actual translation starts here
    //==================================================
    // retrieve the ID of the Petri net
    ATermAppl ANetID;
    if (char const* id = cur->GetAttribute("id", false).c_str()) {
      // the net has an id, put it in ANetID
      ANetID = ATmakeAppl0(ATprependAFun("Net_",pn2gsCheckAFun(ATmakeAFunId(id))));
    } else {
      ANetID = ATmakeAppl0(ATmakeAFun("Net_Petri_net", 0, ATtrue));
      gsWarningMsg("NO NET-ID FOUND!\n");
    }
    gsDebugMsg("NetID = '%T'\n",ANetID);

    // lists of the places, transitions and arcs that will be translated
    ATermList APlaces=ATmakeList0();
    ATermList ATransitions=ATmakeList0();
    ATermList AArcs=ATmakeList0();
    ATermAppl Net_prelude=Appl0;

    // temporary variables that contain data of the current place
    // so this data can be inserted in APlaces
    ATermAppl ACurrentPlace;
    // temporary variables that contain data of the current transition
    // so this data can be inserted in ATransitions
    ATermAppl ACurrentTransition;
    // temporary variables that contain data of the current arc
    // so this data can be inserted in AArcs
    ATermAppl ACurrentArc;

    // if an element has no ID, Abort is set to ATtrue
    // if Abort == ATtrue, the translation is aborted!
    context.Abort = ATfalse;

    // this loop goes through all the children of the <net>element
    // these children will be translated or ignored, this depends on the element name
    for (cur = cur->FirstChildElement(false); cur != NULL; cur = cur->NextSiblingElement(false)) {
      // current elements that are conceivable for translation are:
      // <place>  -  <transition>  -  <arc>
      // all other elements will be ignored in the translation

      if (cur->Value() == "place") {
  if (!(ACurrentPlace=pn2gsAterm_place(cur))) {
    // pn2gsAterm_place returns NULL, so the place will not be translated.
    if (context.Abort == ATtrue) {
      // pn2gsAterm_place has set context.Abort to ATtrue
      // this means the place had no ID
      // therefor the translation will be aborted!
      return NULL;
    }
  } else {
    APlaces = ATinsert(APlaces, (ATerm)ACurrentPlace);
    gsDebugMsg("  Translate this place: %T\n", (ATerm)ACurrentPlace);
  }
      }
      else if (cur->Value() == "transition") {
  if(!(ACurrentTransition=pn2gsAterm_trans(cur))) {
    // pn2gsAterm_trans returns NULL, so the transition will not be translated.
    if (context.Abort == ATtrue) {
      // pn2gsAterm_trans has set context.Abort to ATtrue
      // this means the transition had no ID
      // therefor the translation will be aborted!
      return NULL;
    }
  } else {
    ATransitions = ATinsert(ATransitions, (ATerm)ACurrentTransition);
    gsDebugMsg("  Translate this transition: %T\n", (ATerm)ACurrentTransition);
  }
      }
      else if (cur->Value() == "arc") {
  if(!(ACurrentArc=pn2gsAterm_arc(cur))) {
    // pn2gsAterm_arc returns NULL, so the arc will not be translated.
    if (context.Abort == ATtrue) {
      // pn2gsAterm_arc has set context.Abort to ATtrue
      // this means the arc had no ID
      // therefor the translation will be aborted!
      return NULL;
    }
  } else {
    AArcs = ATinsert(AArcs, (ATerm)ACurrentArc);
    gsDebugMsg("  Translate this arc: %T\n", (ATerm)ACurrentArc);
  }
      }
      else if (with_colors && cur->Value() == "toolspecific") {
        std::string prelude=pn2gsGetElement(cur,"mcrl2prelude");

  if(!prelude.empty()){
    colored=ATtrue;
    std::istringstream s(prelude);
          //std::istringstream iss(s+"init delta;");
    ATermAppl Prelude=parse_data_spec(s); //iss
          if(!Prelude) {gsErrorMsg("Parsing of the mCRL2 prelude failed\n"); return NULL;}
          Prelude=type_check_data_spec(Prelude);
          if(!Prelude) {gsErrorMsg("Type-checking of the mCRL2 prelude failed\n"); return NULL;}
          Net_prelude=Prelude;
  }
  else {
    gsWarningMsg("Ignore an element named 'toolspecific'\n");
  }
      }
      else {
  gsWarningMsg("An element named '%s' will be ignored in the translation (including it's sub-elements).\n", cur->Value().c_str());
      };
    };

    gsDebugMsg("\nConversion of PNML to ATerm succesfully completed. \n");

    // argument order of returnvalue is places - transitions - arcs - NetID
    return ATmakeAppl5(ATmakeAFun("PetriNet", 5, ATtrue), (ATerm)ATreverse(APlaces), (ATerm)ATreverse(ATransitions), (ATerm)ATreverse(AArcs), (ATerm)ANetID, (ATerm)Net_prelude);
  }

  /*                                    */
  /*                                    */
  /*                                    */
  /* Functions for the generation phase */
  /*                                    */
  /*                                    */
  /*                                    */

  //==================================================
  // pn2gsGenerateAction_trans generates the action t_..._mon for a given transition
  //==================================================
static ATermAppl pn2gsGenerateAction_trans(AFun CurrentId, AFun CurrentName){
  // input: access to the context + id and name of the transition
  // output: the action t_transid_mon or t_transid_transname_mon

  AFun ResAFun=ATprependAFun("t_",CurrentId);
  char * CurrentNameString=ATgetName(CurrentName);

    if(strcmp(CurrentNameString, "default_name")){
      // name of the transition is not "default_name"
      // name of the transition may be used
      ResAFun=ATappendAFun(ATappendAFun(ResAFun,"_"),CurrentNameString);
    }

    return ATmakeAppl0(ATappendAFun(ResAFun,"_mon"));
}


static ATermList pn2gsGenerateActionsTrans(){
  //returns names of all actions representing the firings of the transitions.
  ATermList r=ATmakeList0();
  for(ATermList l=ATtableKeys(context.trans_name); !ATisEmpty(l); l=ATgetNext(l)){
    ATermAppl e=ATAgetFirst(l);
    r=ATinsert(r,(ATerm)pn2gsGenerateAction_trans(ATgetAFun(e),ATgetAFun(ATtableGet(context.trans_name,(ATerm)e))));
  }
  return ATreverse(r);
}

  //==================================================
  // pn2gsGenerateActions generates all the mCRL2 actions
  //==================================================
  static ATermList pn2gsGenerateActions(void){
    // input: access to the context
    // output: an ATermList of <ActID>'s

    // #actions = 3x #arcs + 1x #transitions
    // the possible actions will be stored in ActionList
    ATermList ActionsList = ATmakeList0();

    // the possible actions are
    // for each arc:                                  arcid     >-<     _arcid     >-<     __arcid
    // for each transition (if a name is present):    t_transid_transname_mon
    // for each transition (if no name is present):   t_transid_mon

    // variables to store the Current Action to be inserted into ActionsList
    ATermAppl CurrentAction;

    // variable to go through all the arc-ids and next the transition-ids
    ATermList Ids;

    //==================================================
    // create actions from context.arc_in, context.arc_out, context.arc_inhibit and context.arc_reset
    //==================================================
    Ids = ATconcat(ATtableKeys(context.arc_in), ATtableKeys(context.arc_out));
    Ids = ATconcat(Ids, ATtableKeys(context.arc_inhibit));
    Ids = ATconcat(Ids, ATtableKeys(context.arc_reset));

    // ***** Changed by Yarick 29.05.2006 to add error action ****

    if(error) Ids = ATinsert(Ids,(ATerm)gsString2ATermAppl("error"));

    // ***** end changed by Yarick.

    while (ATisEmpty(Ids) == ATfalse) {
      // make the action: arcID
      ATermAppl Id=ATAgetFirst(Ids);
      ATermList SortList=ATmakeList0();
      ATermAppl Type=ATAtableGet(context.arc_type_mcrl2,(ATerm)Id);
      if(Type && !ATisEqual(Type,Appl0))
  SortList=ATmakeList1((ATerm)Type);

      CurrentAction = Id;
      ActionsList = ATinsert(ActionsList, (ATerm)gsMakeActId(CurrentAction, SortList));
      gsDebugMsg("Action: %T created.\n", CurrentAction);
      // make the action: _arcID
      CurrentAction = ATmakeAppl0(ATprependAFun("_", ATgetAFun(Id)));
      ActionsList = ATinsert(ActionsList, (ATerm)gsMakeActId(CurrentAction, SortList));
      gsDebugMsg("Action: %T created.\n", CurrentAction);
      // make the action: __arcID
      CurrentAction = ATmakeAppl0(ATprependAFun("__", ATgetAFun(Id)));
      ActionsList = ATinsert(ActionsList, (ATerm)gsMakeActId(CurrentAction, SortList));
      gsDebugMsg("Action: %T created.\n", CurrentAction);

      Ids = ATgetNext(Ids);
    }

    //==================================================
    // create actions from the transitions
    //==================================================
    // All transitions have a name. If no name is defined in PNML, it is "default_name"
    Ids = pn2gsGenerateActionsTrans();
    while (ATisEmpty(Ids) == ATfalse) {

      CurrentAction = ATAgetFirst(Ids);
      ActionsList = ATinsert(ActionsList,(ATerm)gsMakeActId(CurrentAction, ATmakeList0()));
      gsDebugMsg("Action: %T created.\n", CurrentAction);
      Ids = ATgetNext(Ids);
    }

    //now reverse the whole thing
    return ATreverse(ActionsList);
  }

  //==================================================
  // pn2gsSyncIn creates the mCRL2 syncronistation of all elements of the list
  //==================================================
  static ATermAppl pn2gsSyncIn(ATermList Ids){
    // input: a not-empty list with the ID's
    // output: an AtermAppl that is the SYNC of all the ID's in the input-list

    ATermAppl sync;
    if (ATgetLength(Ids) > 1) {
      sync = gsMakeSync(pn2gsSyncIn(ATgetNext(Ids)), gsMakeParamId(ATmakeAppl0(ATgetAFun(ATgetFirst(Ids))), ATmakeList0()));
    } else {
      // ATgetLength(Ids) == 1, since the input-list is not-empty!
      sync = gsMakeParamId(ATmakeAppl0(ATgetAFun(ATgetFirst(Ids))), ATmakeList0());
    }
    return sync;
  }

  //==================================================
  // pn2gsSyncOut creates the mCRL2 syncronistation of all elements of the list, preceded by an underscore
  //==================================================
  static ATermAppl pn2gsSyncOut(ATermList Ids){
    // input: a not-empty list with the ID's
    // output: an AtermAppl that is the SYNC of all the ID's in the input-list, each ID preceded by an underscore

    ATermAppl sync;
    if (ATgetLength(Ids) > 1) {
      sync = gsMakeSync(pn2gsSyncOut(ATgetNext(Ids)), gsMakeParamId(ATmakeAppl0(ATprependAFun("_", ATgetAFun(ATgetFirst(Ids)))), ATmakeList0()));
    } else {
      // ATgetLength(Ids) == 1, since the input-list is not-empty!
      sync = gsMakeParamId(ATmakeAppl0(ATprependAFun("_", ATgetAFun(ATgetFirst(Ids)))), ATmakeList0());
    }
    return sync;
  }

  //==================================================
  // pn2gsGenerateTransition generates the mCRL2 Process Equations belonging to one transition
  //==================================================
  static ATermList pn2gsGenerateTransition(ATerm TransID){
    // input: access to the context; ID of the transtion
    // output: an updated list of the processes: the processes belonging to the transition added to the list

    ATermList EquationList=ATmakeList0();

    // variables to store the name and id of the transition
    AFun CurrentTransId = ATgetAFun(TransID);
    char * CurrentTransName = ATgetName(ATgetAFun(ATtableGet(context.trans_name, TransID)));

    // variables to store the process names
    ATermAppl CurrentTrans;
    ATermAppl CurrentTransIn;
    ATermAppl CurrentTransOut;

    //==================================================
    // retrieve the process names
    //==================================================
    if (strcmp(CurrentTransName, "default_name")) {
      // name of the transition is not "default_name"
      // name of the transition will be used
      // make the processes: T_transid_transname     >-<     T_transid_transname_in     >-<     T_transid_transname_out
      CurrentTrans = ATmakeAppl0(ATprependAFun("T_", ATappendAFun(ATappendAFun(CurrentTransId, "_"), CurrentTransName)));
      CurrentTransIn = ATmakeAppl0(ATprependAFun("T_", ATappendAFun(ATappendAFun(ATappendAFun(CurrentTransId, "_"), CurrentTransName), "_in")));
      CurrentTransOut = ATmakeAppl0(ATprependAFun("T_", ATappendAFun(ATappendAFun(ATappendAFun(CurrentTransId, "_"), CurrentTransName), "_out")));
    } else {
      // name of the transition is "default_name"
      // name of the transition will not be used
      // make the processes: T_transid     >-<     T_transid_in     >-<     T_transid_out
      CurrentTrans = ATmakeAppl0(ATprependAFun("T_", CurrentTransId));
      CurrentTransIn = ATmakeAppl0(ATprependAFun("T_", ATappendAFun(CurrentTransId, "_in")));
      CurrentTransOut = ATmakeAppl0(ATprependAFun("T_", ATappendAFun(CurrentTransId, "_out")));
    }
    // insert the name of the process T_ti into context.transitions
    // this is needed for the generation of the general process Trans
    context.transitions = ATinsert(context.transitions, (ATerm)CurrentTrans);
    gsDebugMsg("context.transitions now contains the following transitions: %T\n", context.transitions);

    //==================================================
    // generate the processes
    //==================================================
    ATermAppl Process;
    ATermAppl MonitorAction = pn2gsGenerateAction_trans(CurrentTransId, ATmakeAFunId(CurrentTransName));
    if (!(ATtableGet(context.trans_in, TransID)) && !(ATtableGet(context.trans_out, TransID))) {
      // Transition has no incoming arcs and no outgoing arcs
      gsDebugMsg("No in and no out for trans: %T\n", (ATerm)CurrentTrans);
      // create T_ti
      Process = gsMakeParamId(MonitorAction, ATmakeList0());
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(CurrentTrans, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTrans);

    } else if (ATtableGet(context.trans_in, TransID) && !(ATtableGet(context.trans_out, TransID))) {
      // Transition has incoming arcs but no outgoing arcs
      gsDebugMsg("In and no out for trans: %T\n", (ATerm)CurrentTrans);
      // create T_ti
      Process = gsMakeSync(gsMakeParamId(CurrentTransIn, ATmakeList0()), gsMakeParamId(MonitorAction, ATmakeList0()));
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(CurrentTrans, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTrans);
      // create T_ti_in
      Process = pn2gsSyncIn((ATermList)ATtableGet(context.trans_in, TransID));
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(CurrentTransIn, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTransIn);

    } else if (!(ATtableGet(context.trans_in, TransID)) && ATtableGet(context.trans_out, TransID)) {
      // Transition has outgoing arcs but no incoming arcs
      gsDebugMsg("Out and no in for trans: %T\n", (ATerm)CurrentTrans);
      // create T_ti
      Process = gsMakeSync(gsMakeParamId(CurrentTransOut, ATmakeList0()), gsMakeParamId(MonitorAction, ATmakeList0()));
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(CurrentTrans, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTrans);
      // create T_ti_out
      Process = pn2gsSyncOut((ATermList)ATtableGet(context.trans_out, TransID));
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(CurrentTransOut, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTransOut);

    } else if (ATtableGet(context.trans_in, TransID) && ATtableGet(context.trans_out, TransID)) {
      // Transition has incoming arcs and outgoing arcs
      gsDebugMsg("In and out for trans: %T\n", (ATerm)CurrentTrans);
      // create T_ti
      Process = gsMakeSync(gsMakeSync(gsMakeParamId(CurrentTransIn, ATmakeList0()), gsMakeParamId(CurrentTransOut, ATmakeList0())), gsMakeParamId(MonitorAction, ATmakeList0()));
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(CurrentTrans, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTrans);
      // create T_ti_in
      Process = pn2gsSyncIn((ATermList)ATtableGet(context.trans_in, TransID));
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(CurrentTransIn, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTransIn);
      // create T_ti_out
      Process = pn2gsSyncOut((ATermList)ATtableGet(context.trans_out, TransID));
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(CurrentTransOut, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTransOut);
    }
    return EquationList;
  }

  //==================================================
  // pn2gsChoiceIn creates the mCRL2 choice of all elements of the list
  //==================================================
  static ATermAppl pn2gsChoiceIn(ATermList Ids){
    // input: a not-empty list with the ID's
    // output: an AtermAppl that is the CHOICE of all the ID's in the input-list

    ATermAppl choice;
    if (ATgetLength(Ids) > 1) {
      choice = gsMakeChoice(gsMakeParamId(ATmakeAppl0(ATgetAFun(ATgetFirst(Ids))), ATmakeList0()), pn2gsChoiceIn(ATgetNext(Ids)));
    } else {
      // ATgetLength(Ids) == 1, since the input-list is not-empty!
      choice = gsMakeParamId(ATmakeAppl0(ATgetAFun(ATgetFirst(Ids))), ATmakeList0());
    }
    return choice;
  }

  //==================================================
  // pn2gsChoiceOut creates the mCRL2 choice of all elements of the list, preceded by an underscore
  //==================================================
  static ATermAppl pn2gsChoiceOut(ATermList Ids){
    // input: a not-empty list with the ID's
    // output: an AtermAppl that is the CHOICE of all the ID's in the input-list, each ID preceded by an underscore

    ATermAppl choice;
    if (ATgetLength(Ids) > 1) {
      choice = gsMakeChoice(gsMakeParamId(ATmakeAppl0(ATprependAFun("_", ATgetAFun(ATgetFirst(Ids)))), ATmakeList0()), pn2gsChoiceOut(ATgetNext(Ids)));
    } else {
      // ATgetLength(Ids) == 1, since the input-list is not-empty!
      choice = gsMakeParamId(ATmakeAppl0(ATprependAFun("_", ATgetAFun(ATgetFirst(Ids)))), ATmakeList0());
    }
    return choice;
  }

  //==================================================
  // pn2gsPlaceParameter generates the parameter for a given place
  //==================================================
static ATermAppl pn2gsPlaceParameter(ATermAppl Place) {
  // input: the name of a place
  // output: the parameter of that place, or type Nat
  //         named: pm_PlaceName
  return ATmakeAppl0(ATprependAFun("pm_", ATgetAFun(Place)));
}

  //==================================================
  // pn2gsGeneratePlace generates the mCRL2 Process Equations belonging to one place
  //==================================================
  static ATermList pn2gsGeneratePlace(ATerm PlaceID){
    // input: access to the context; ID of the place
    // output: an updated list of the processes: the processes belonging to the place added to the list

    ATermList EquationList=ATmakeList0();

    // variables to store the name and id of the place
    AFun CurrentPlaceId = ATgetAFun(PlaceID);
    char * CurrentPlaceName = ATgetName(ATgetAFun(ATtableGet(context.place_name, PlaceID)));

    // variables to store the process names
    ATermAppl CurrentPlace;
    ATermAppl CurrentPlaceAdd;
    ATermAppl CurrentPlaceIn;
    ATermAppl CurrentPlaceRem;
    ATermAppl CurrentPlaceOut;

    //==================================================
    // retrieve the process names
    //==================================================
    if (strcmp(CurrentPlaceName, "default_name")) {
      // name of the place is not "default_name"
      // name of the place may be used
      // make the processes: P_placeid_placename     >-<     P_placeid_placename_add     >-<     P_placeid_placename_in     >-<
      //                     P_placeid_placename_rem     >-<     P_placeid_placename_out
      CurrentPlace = ATmakeAppl0(ATprependAFun("P_", ATappendAFun(ATappendAFun(CurrentPlaceId, "_"), CurrentPlaceName)));
      CurrentPlaceAdd = ATmakeAppl0(ATprependAFun("P_", ATappendAFun(ATappendAFun(ATappendAFun(CurrentPlaceId, "_"), CurrentPlaceName), "_add")));
      CurrentPlaceIn = ATmakeAppl0(ATprependAFun("P_", ATappendAFun(ATappendAFun(ATappendAFun(CurrentPlaceId, "_"), CurrentPlaceName), "_in")));
      CurrentPlaceRem = ATmakeAppl0(ATprependAFun("P_", ATappendAFun(ATappendAFun(ATappendAFun(CurrentPlaceId, "_"), CurrentPlaceName), "_rem")));
      CurrentPlaceOut = ATmakeAppl0(ATprependAFun("P_", ATappendAFun(ATappendAFun(ATappendAFun(CurrentPlaceId, "_"), CurrentPlaceName), "_out")));
    } else {
      // name of the transition is "default_name"
      // name of the transition will not be used
      // make the processes: P_placeid     >-<     P_placeid_add     >-<     P_placeid_in     >-<     P_placeid_rem     >-<     P_placeid_out
      CurrentPlace = ATmakeAppl0(ATprependAFun("P_", CurrentPlaceId));
      CurrentPlaceAdd = ATmakeAppl0(ATprependAFun("P_", ATappendAFun(CurrentPlaceId, "_add")));
      CurrentPlaceIn = ATmakeAppl0(ATprependAFun("P_", ATappendAFun(CurrentPlaceId, "_in")));
      CurrentPlaceRem = ATmakeAppl0(ATprependAFun("P_", ATappendAFun(CurrentPlaceId, "_rem")));
      CurrentPlaceOut = ATmakeAppl0(ATprependAFun("P_", ATappendAFun(CurrentPlaceId, "_out")));
    }

    //added by Yarick: we need a table to relate PlaceId and CurrentPlace.
    ATtablePut(context.place_process_name,PlaceID,(ATerm)CurrentPlace);

    //==================================================
    // retrieve the maximum concurrency
    //==================================================
    int MaxConcIn_int;
    if (!(ATtableGet(context.place_in, PlaceID))) {
      MaxConcIn_int = 0;
    } else {
      MaxConcIn_int = ATgetLength((ATermList)ATtableGet(context.place_in, PlaceID));
    }
    ATermAppl MaxConcIn;
    MaxConcIn = gsMakeId(ATmakeAppl0(ATmakeAFunInt0(MaxConcIn_int)));
    if (gsIsNumericString(gsATermAppl2String(MaxConcIn))) {
     gsDebugMsg("Parameter %T is a Number\n", MaxConcIn);
    } else {
      gsDebugMsg("Parameter %T is not a Number\n", MaxConcIn);
    }

    int MaxConcOut_int;
    if (!(ATtableGet(context.place_out, PlaceID))) {
      MaxConcOut_int = 0;
    } else {
      MaxConcOut_int = ATgetLength((ATermList)ATtableGet(context.place_out, PlaceID));
    }
    ATermAppl MaxConcOut;
    MaxConcOut = gsMakeId(ATmakeAppl0(ATmakeAFunInt0(MaxConcOut_int)));
    if (gsIsNumericString(gsATermAppl2String(MaxConcOut))) {
      gsDebugMsg("Parameter %T is a Number\n", MaxConcOut);
    } else {
      gsDebugMsg("Parameter %T is not a Number\n", MaxConcOut);
    }
    gsDebugMsg("Place %T has maximum concurrency in: '%d' and out: '%d'\n", PlaceID, MaxConcIn_int, MaxConcOut_int);
    gsDebugMsg("Place %T has maximum concurrency in: '%T' and out: '%T'\n", PlaceID, MaxConcIn, MaxConcOut);

    //==================================================
    // generate the processes
    //==================================================

    {
      /* Creation of P_pi */
      ATermAppl ProcVar;
      ATermAppl Process;
      ATermAppl SubProcess0;
      ATermAppl CondIf0;
      ATermAppl CondThen0;
      ATermAppl SubProcess1;
      ATermAppl CondIf1;
      ATermAppl CondThen1;
      ATermAppl SubProcess2;
      ATermAppl CondIf2;
      ATermAppl CondThen2;
      ATermAppl SumVar0 = gsMakeDataVarId(ATmakeAppl0(ATmakeAFun("y", 0, ATtrue)), static_cast<ATermAppl>(mcrl2::data::sort_pos::pos()));
      ATermAppl SumVar1 = gsMakeDataVarId(ATmakeAppl0(ATmakeAFun("z", 0, ATtrue)), static_cast<ATermAppl>(mcrl2::data::sort_pos::pos()));
      ATermList SumVars;
      gsDebugMsg("Parameter %T is %d a DataVarId\n", SumVar0, mcrl2::data::data_expression(SumVar0).is_variable());
      gsDebugMsg("Parameter %T is %d a DataVarId\n", SumVar1, mcrl2::data::data_expression(SumVar1).is_variable());

      // create P_pi parameter
      ProcVar = gsMakeDataVarId(pn2gsPlaceParameter(CurrentPlace),static_cast<ATermAppl>(mcrl2::data::sort_nat::nat()));
      gsDebugMsg("Parameter %T is %d a DataVarId\n", ProcVar, mcrl2::data::data_expression(ProcVar).is_variable());

      // create first sum-sub-process
      SumVars = ATmakeList1((ATerm)SumVar0);
      CondIf0 = pn2gsMakeDataApplProd2(OpLTE,SumVar0,MaxConcIn);
      CondThen0 = gsMakeSeq(gsMakeParamId(CurrentPlaceAdd, ATmakeList1((ATerm)SumVar0)), gsMakeParamId(CurrentPlace, ATmakeList1((ATerm)pn2gsMakeDataApplProd2(OpAdd,SumVar0,ProcVar))));
      SubProcess0 = gsMakeSum(SumVars, pn2gsMakeIfThenUntimed(CondIf0, CondThen0));
      gsDebugMsg("Parameter %T is %d a Sum\n", SubProcess0, gsIsSum(SubProcess0));

      // create second sum-sub-process
      SumVars = ATmakeList1((ATerm)SumVar1);
      CondIf1 = pn2gsMakeDataApplProd2(OpLTE,SumVar1, pn2gsMakeDataApplProd2(OpMin, MaxConcOut, ProcVar));
      CondThen1 = gsMakeSeq(gsMakeParamId(CurrentPlaceRem, ATmakeList1((ATerm)SumVar1)), gsMakeParamId(CurrentPlace, ATmakeList1((ATerm)pn2gsMakeDataApplProd2(OpMax,Number0, pn2gsMakeDataApplProd2(OpSubt,ProcVar,SumVar1)))));
      SubProcess1 = gsMakeSum(SumVars, pn2gsMakeIfThenUntimed(CondIf1, CondThen1));
      gsDebugMsg("Parameter %T is %d a Sum\n", SubProcess1, gsIsSum(SubProcess1));

      // create third sum-sub-process
      SumVars = ATmakeList2((ATerm)SumVar0, (ATerm)SumVar1);
      CondIf2 = pn2gsMakeDataApplProd2(OpAnd, CondIf0, CondIf1);
      CondThen2 = gsMakeSeq(gsMakeSync(gsMakeParamId(CurrentPlaceAdd, ATmakeList1((ATerm)SumVar0)), gsMakeParamId(CurrentPlaceRem, ATmakeList1((ATerm)SumVar1))), gsMakeParamId(CurrentPlace, ATmakeList1((ATerm)pn2gsMakeDataApplProd2(OpMax,Number0,pn2gsMakeDataApplProd2(OpSubt,pn2gsMakeDataApplProd2(OpAdd,SumVar0,ProcVar), SumVar1)))));
      SubProcess2 = gsMakeSum(SumVars, pn2gsMakeIfThenUntimed(CondIf2, CondThen2));
      gsDebugMsg("Parameter %T is %d a Sum\n", SubProcess2, gsIsSum(SubProcess2));

      // create P_pi
      Process = gsMakeChoice(gsMakeChoice(SubProcess0, SubProcess1), SubProcess2);
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(CurrentPlace, ATmakeList1((ATerm)static_cast<ATermAppl>(mcrl2::data::sort_nat::nat()))), ATmakeList1((ATerm)ProcVar), Process));
      gsDebugMsg("Process: %T created.\n", CurrentPlace);
    }

    {
      /* Creation of P_pi_add */
      ATermAppl ProcVar = gsMakeDataVarId(pn2gsPlaceParameter(CurrentPlaceAdd),static_cast<ATermAppl>(mcrl2::data::sort_pos::pos()));
      ATermAppl CondIf = pn2gsMakeDataApplProd2(OpGT,ProcVar, Number1);
      ATermAppl CondThen = gsMakeSync(gsMakeParamId(CurrentPlaceIn, ATmakeList0()), gsMakeParamId(CurrentPlaceAdd, ATmakeList1((ATerm)pn2gsMakeDataApplProd2(OpMax,Number1, pn2gsMakeDataApplProd2(OpSubt,ProcVar, Number1)))));
      ATermAppl CondElse = gsMakeParamId(CurrentPlaceIn, ATmakeList0());
      ATermAppl Process = gsMakeIfThenElse(CondIf, CondThen, CondElse);
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(CurrentPlaceAdd, ATmakeList1((ATerm)static_cast<ATermAppl>(mcrl2::data::sort_pos::pos()))), ATmakeList1((ATerm)ProcVar), Process));
      gsDebugMsg("Process: %T created.\n", CurrentPlaceAdd);
    }

    {
      /* Creation of P_pi_in */
      ATermAppl Process;
      if (MaxConcIn_int == 0) {
  Process = gsMakeTau();
      } else {
  Process = pn2gsChoiceIn((ATermList)ATtableGet(context.place_in, PlaceID));
      }
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(CurrentPlaceIn, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentPlaceIn);
    }

    {
      /* Creation of P_pi_rem */
      ATermAppl ProcVar = gsMakeDataVarId(pn2gsPlaceParameter(CurrentPlaceRem),static_cast<ATermAppl>(mcrl2::data::sort_pos::pos()));
      ATermAppl CondIf = pn2gsMakeDataApplProd2(OpGT,ProcVar, Number1);
      ATermAppl CondThen = gsMakeSync(gsMakeParamId(CurrentPlaceOut, ATmakeList0()), gsMakeParamId(CurrentPlaceRem, ATmakeList1((ATerm)pn2gsMakeDataApplProd2(OpMax, Number1, pn2gsMakeDataApplProd2(OpSubt,ProcVar, Number1)))));
      ATermAppl CondElse = gsMakeParamId(CurrentPlaceOut, ATmakeList0());
      ATermAppl Process = gsMakeIfThenElse(CondIf, CondThen, CondElse);
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(CurrentPlaceRem, ATmakeList1((ATerm)static_cast<ATermAppl>(mcrl2::data::sort_pos::pos()))), ATmakeList1((ATerm)ProcVar), Process));
      gsDebugMsg("Process: %T created.\n", CurrentPlaceRem);
    }

    {
      /* Creation of P_pi_out */
      ATermAppl Process;
      if (MaxConcOut_int == 0) {
  Process = gsMakeTau();
      } else {
  Process = pn2gsChoiceOut((ATermList)ATtableGet(context.place_out, PlaceID));
      }
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(CurrentPlaceOut, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentPlaceOut);
    }

    // return the updated list of the processes: the processes belonging to the place added to the input-list
    return EquationList;
  }

  //==================================================
  // pn2gsHideList generates a list with all the actions that should be hidden
  //==================================================
  static ATermList pn2gsHideList(ATermList List) {
    // input: a list
    // output: a list in which all the elements of the inputed list are present, preceded by two underscores.

    ATermList ReturnList = ATmakeList0();
    AFun CurrentId;
    while (ATisEmpty(List) == ATfalse) {
      CurrentId = ATprependAFun("__", ATgetAFun(ATgetFirst(List)));
      ReturnList = ATinsert(ReturnList, (ATerm)ATmakeAppl0(CurrentId));
      List = ATgetNext(List);
    }
    return ATreverse(ReturnList);
  }

  //==================================================
  // pn2gsBlockList generates a list with all the actions that should be
  // blocked
  //==================================================
  static ATermList pn2gsBlockList(ATermList List) {
    // input: a list
    // output: a list in which all the elements of the inputed list are present twice, once preceded by an underscore.

    ATermList ReturnList = ATmakeList0();
    AFun CurrentId;
    while (ATisEmpty(List) == ATfalse) {
      CurrentId = ATgetAFun(ATgetFirst(List));
      ReturnList = ATinsert(ReturnList, (ATerm)ATmakeAppl0(CurrentId));
      CurrentId = ATprependAFun("_", CurrentId);
      ReturnList = ATinsert(ReturnList, (ATerm)ATmakeAppl0(CurrentId));
      List = ATgetNext(List);
    }
    return ATreverse(ReturnList);
  }

  //==================================================
  // pn2gsCommList generates a list with all the actions that communicate
  //==================================================
  static ATermList pn2gsCommList(ATermList List) {
    // input: a list
    // output: a list in which all the elements of the inputed list
    //         communicate with itself, once preceded by an underscore,
    //         to itself, preceded by two underscores.

    ATermList ReturnList = ATmakeList0();
    AFun CurrentId;
    while (ATisEmpty(List) == ATfalse) {
      CurrentId = ATgetAFun(ATgetFirst(List));
      ATermAppl CommExpr = gsMakeCommExpr(gsMakeMultActName(ATmakeList2((ATerm)ATmakeAppl0(CurrentId), (ATerm)ATmakeAppl0(ATprependAFun("_", CurrentId)))), ATmakeAppl0(ATprependAFun("__", CurrentId)));
      ReturnList = ATinsert(ReturnList, (ATerm)CommExpr);
      List = ATgetNext(List);
    }
    return ATreverse(ReturnList);
  }

  //==================================================
  // pn2gsPlacesParameters generates a list of all the parameters of the places
  //==================================================
  static ATermList pn2gsPlacesParameters(ATermList Places) {
    // input: a list containg all the places
    // output: a list in which all the elements of the inputed list
    //         are preceded by "pm_"

    ATermList ReturnList = ATmakeList0();
    while (ATisEmpty(Places) == ATfalse){
      ATermAppl Place=ATAgetFirst(Places);
      ATermAppl PlaceName=ATAtableGet(context.place_process_name,(ATerm)ATAgetFirst(Places));
      ATermAppl PlaceParameter=gsMakeDataVarId(pn2gsPlaceParameter(PlaceName),static_cast<ATermAppl>(mcrl2::data::sort_nat::nat()));
      //colored
      ATermAppl Type=ATAtableGet(context.place_type_mcrl2,(ATerm)Place);
      if(Type && !ATisEqual(Type,Appl0)){
  Type=static_cast<ATermAppl>(mcrl2::data::sort_bag::bag(mcrl2::data::sort_expression(Type)));
  PlaceParameter=ATsetArgument(PlaceParameter,(ATerm)Type,1);
      }
      ReturnList = ATinsert(ReturnList, (ATerm)PlaceParameter);
      Places = ATgetNext(Places);
    }
    return ATreverse(ReturnList);
  }

  static ATermList pn2gsPlacesParameterTypes(ATermList Places) {
    // input: a list containg all the places
    // output: a list of types for these places

    ATermList ReturnList = ATmakeList0();
    while (ATisEmpty(Places) == ATfalse) {
      ATermAppl Place=ATAgetFirst(Places);
      //colored
      ATermAppl Type=ATAtableGet(context.place_type_mcrl2,(ATerm)Place);
      if(Type && !ATisEqual(Type,Appl0)){
  Type=static_cast<ATermAppl>(mcrl2::data::sort_bag::bag(mcrl2::data::sort_expression(Type)));
      }
      else{
  Type=static_cast<ATermAppl>(mcrl2::data::sort_nat::nat());
      }
      ReturnList = ATinsert(ReturnList, (ATerm)Type);
      Places = ATgetNext(Places);
    }
    return ATreverse(ReturnList);
  }

  //==================================================
  // pn2gsMerge creates the mCRL2 merge of all elements of the list, with a parameter!
  //==================================================
  static ATermAppl pn2gsMerge(ATermList Ids) {
    // input: a not-empty list with the ID's
    // output: an ATermAppl that is the MERGE of all the ID's in the input-list, each with its parameter!

    ATermAppl merge;
    ATermAppl Id=ATAtableGet(context.place_process_name,(ATerm)ATAgetFirst(Ids));
    if (ATgetLength(Ids) > 1) {
      merge = gsMakeMerge(gsMakeParamId(Id, ATmakeList1((ATerm)gsMakeId(pn2gsPlaceParameter(Id)))), pn2gsMerge(ATgetNext(Ids)));
    } else {
      // ATgetLength(Ids) == 1, since the input-list is not-empty!
      merge = gsMakeParamId(Id, ATmakeList1((ATerm)gsMakeId(pn2gsPlaceParameter(Id))));
    }
    return merge;
  }

  //==================================================
  // pn2gsInitialMarkings creates a list with all the initial markings
  //==================================================
  static ATermList pn2gsInitialMarkings(ATermList Places) {
    // input: a list with the places and a table with the markings
    // output: a list which contains all the initial markings of the places

    ATermList ReturnList = ATmakeList0();
    while (ATisEmpty(Places) == ATfalse) {
      ATermAppl Place=ATAgetFirst(Places);
      ATermAppl Marking = ATAtableGet(context.place_mark, (ATerm)Place);
      if(!Marking) Marking = gsMakeId(gsString2ATermAppl("0"));

      gsDebugMsg("Initial Marking = %T\n", Marking);

      //colored
      ATermAppl Type=ATAtableGet(context.place_type_mcrl2,(ATerm)Place);
      if(Type && !ATisEqual(Type,Appl0)){
  Marking = ATAtableGet(context.place_mark_mcrl2, (ATerm)Place);
  if(!Marking || ATisEqual(Marking,Appl0)) Marking = EmptyBag;
      }

      ReturnList = ATinsert(ReturnList,(ATerm)Marking);
      Places = ATgetNext(Places);
    }
    return ATreverse(ReturnList);
  }

  //==================================================
  // pn2gsGenerateProcEqns generates all the mCRL2 Process Equations
  //==================================================
  static ATermList pn2gsGenerateProcEqns(ATermAppl NetID){
    // input: access to the context and the ID of the Petri net
    // output: an ATermList of <ProcEqn>'s

    // #processes = 5x #places + 3x #transitions + ~3
    // the actual number of processes might be less than the number stated here.
    // the possible processes will be stored in ProcessList
    ATermList ProcessList = ATmakeList0();

    //==================================================
    // first, we generate the transition processes.
    //==================================================

    // In the comment below, ti will refer to 'transid' or to 'transid_transname'.
    // This depends on whether or not a transition has a name, and if it does, if it's not too long.

    // Each transition creates the following processes:
    // T_ti     = T_ti_in | T_ti_out | t_ti_mon;
    // T_ti_in  = "synchronisation of the incoming arcs"
    // T_ti_out = "synchronisation of the outgoing arcs"

    // If there are no incoming or outgoing arcs, T_ti_in respectively T_ti_out are left out!

    gsDebugMsg("\n\nStart creation of processes belonging to transitions.\n\n");
    if(rec_par){
      for(ATermList Ids = ATtableKeys(context.trans_name);!ATisEmpty(Ids);Ids = ATgetNext(Ids))
  ProcessList = ATconcat(pn2gsGenerateTransition(ATgetFirst(Ids)),ProcessList);
    }
    else{
      for(ATermList Ids = ATtableKeys(context.trans_name);!ATisEmpty(Ids);Ids = ATgetNext(Ids))
  ProcessList = ATconcat(pn2gsGenerateTransitionAlternative(ATgetFirst(Ids)),ProcessList);
    }

    //==================================================
    // second, we generate the place processes.
    //==================================================

    // In the comment below, pi will refer to 'placeid' or to 'placeid_placename'.
    // This depends on whether or not a place has a name, and if it does, if it's not too long.

    // Each place creates the following processes:

    // the values n and m indicate the maximum concurrency of the place
    // P_pi(X:Nat)     =  sum y:Pos. (y<=n) -> P_pi_add(y) . P_pi(X+y)
    //                  + sum z:Pos. (z<=min(m,X)) -> P_pi_rem(z) . P_pi(max(0,X-z))
    //                  + sum y,z:Pos. (y<=n && z<=min(m,X)) -> (P_pi_add(y) | P_pi_rem(z)) . P_pi(max(0,X+y-z))
    //
    // P_pi_add(y:Pos) = block({"set of synchronisation of each incoming arc with itself"}, (y>1) -> (P_pi_in | P_pi_add(max(1, y-1))) : P_pi_in)
    //
    // P_pi_in = "choice of the incoming arcs"
    //
    // P_pi_rem(z:Pos) = block({"set of synchronisation of each outgoing arc with itself"}, (z>1) -> (P_pi_out | P_pi_rem(max(1, z-1))) : P_pi_out)
    //
    // P_pi_out = "choice of the outgoing arcs"

    // If there are no incoming or outgoing arcs, P_pi_add and P_pi_in respectively P_pi_add and P_pi_out are will never be executed
    // because of the fact that the maximum concurrency == 0. Since 'y' and 'z' are of type Pos, they must be > 0 and thus execution
    // of the named processes is prevented.
    // Therefore, in that case it doesn't matter what P_pi_add and P_pi_in respectively P_pi_add and P_pi_out look like.
    // For ease of implementation, the parts in between " " will be defined as tau.

    gsDebugMsg("\n\nStart creation of processes belonging to places.\n\n");
    if(rec_par){
      for(ATermList Ids = ATtableKeys(context.place_name);!ATisEmpty(Ids);Ids = ATgetNext(Ids))
  ProcessList = ATconcat(pn2gsGeneratePlace(ATgetFirst(Ids)),ProcessList);
    }
    else{
      for(ATermList Ids = ATtableKeys(context.place_name);!ATisEmpty(Ids);Ids = ATgetNext(Ids))
  ProcessList = ATconcat(pn2gsGeneratePlaceAlternative(ATgetFirst(Ids)),ProcessList);
    }

    //==================================================
    // third, we generate the three general Petri net processes.
    //==================================================

    gsDebugMsg("\n\nStart creation of general processes.\n\n");
    // Trans =  "..." . Trans
    // "..." is the choice of all transitions. This guarantees interleaving.

    // If no interleaving is desired, instead of a choice of all transitions,
    // this should be the parallelisation of all transitions.
    // Also, the transition-processes should be extended with: " . T_ti "
    if (ATisEmpty(context.transitions) == ATtrue) {
      // there are no transitions
      ProcessList = ATinsert(ProcessList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(gsString2ATermAppl("Trans"), ATmakeList0()), ATmakeList0(), gsMakeDelta()));
    } else {
      // there are transitions



      // ***** Changed by Yarick 29.05.2006 to add error action ****
      if(!error){
  ProcessList = ATinsert(ProcessList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(gsString2ATermAppl("Trans"), ATmakeList0()), ATmakeList0(), gsMakeSeq(pn2gsChoiceIn(context.transitions), gsMakeParamId(gsString2ATermAppl("Trans"), ATmakeList0()))));
      }
      else {
  ProcessList = ATinsert(ProcessList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(gsString2ATermAppl("Trans"), ATmakeList0()), ATmakeList0(), gsMakeSeq(pn2gsChoiceIn(ATinsert(context.transitions,(ATerm)gsString2ATermAppl("error"))), gsMakeParamId(gsString2ATermAppl("Trans"), ATmakeList0()))));
      }
      // ***** end changed by Yarick.
    }
    gsDebugMsg("Process Trans created.\n");

    // PetriNet("...") = hide(I, block(H, comm(C, ("..." || Trans))));
    // the first "..." are the parameters of PetriNet; one for every place
    // the second "..." is the parallelisation of all the places in the PetriNet
    ATermAppl Process;

    //Added by Yarick: alternative main process.
    if(rec_par){
      ATermList AllArcs=ATconcat(ATtableKeys(context.arc_in), ATtableKeys(context.arc_out));
      Process = gsMakeHide(pn2gsHideList(AllArcs),
         gsMakeBlock(pn2gsBlockList(AllArcs),
            gsMakeComm(pn2gsCommList(AllArcs),
                 gsMakeMerge(
                   gsMakeParamId(gsString2ATermAppl("Trans"),ATmakeList0()),
                   pn2gsMerge(ATtableKeys(context.place_process_name))
                 ))));
    }
    else{
      //hide(In,block(Hn,comm(Cn,
      //....
      //  hide(I2,block(H2,comm(C2,
      //    hide(I1,block(H1,comm(C1,Trans||P1)))||P2)))||...Pn)))
      Process=gsMakeParamId(gsString2ATermAppl("Trans"), ATmakeList0());
      for(ATermList Places=ATtableKeys(context.place_process_name);!ATisEmpty(Places);Places=ATgetNext(Places)){
  ATermAppl PlaceID=ATAgetFirst(Places);
  ATermAppl Place=ATAtableGet(context.place_process_name,(ATerm)PlaceID);
  Process=gsMakeMerge(Process,gsMakeParamId(Place,ATmakeList1((ATerm)gsMakeId(pn2gsPlaceParameter(Place)))));

  ATermList AssocArcs=ATmakeList0(); // in and out arcs of the place
  {
    ATermList MoreArcs=ATLtableGet(context.place_in,(ATerm)PlaceID);
    if(MoreArcs) AssocArcs=ATconcat(AssocArcs,MoreArcs);

    MoreArcs=ATLtableGet(context.place_out,(ATerm)PlaceID);
    if(MoreArcs) AssocArcs=ATconcat(AssocArcs,MoreArcs);

    MoreArcs=ATLtableGet(context.place_inhibit,(ATerm)PlaceID);
    if(MoreArcs) AssocArcs=ATconcat(AssocArcs,MoreArcs);

    MoreArcs=ATLtableGet(context.place_reset,(ATerm)PlaceID);
    if(MoreArcs) AssocArcs=ATconcat(AssocArcs,MoreArcs);
  }

  if(ATisEmpty(AssocArcs)) continue;

  // ***** Changed by Yarick 29.05.2006 to add error action ****
  if(!error){
    Process=gsMakeHide(pn2gsHideList(AssocArcs),
           gsMakeBlock(pn2gsBlockList(AssocArcs),
           gsMakeComm(pn2gsCommList(AssocArcs),
                Process)));
  }
  else {
    Process=gsMakeHide(pn2gsHideList(AssocArcs),
           gsMakeBlock(ATinsert(pn2gsBlockList(AssocArcs),(ATerm)gsString2ATermAppl("_error")),
           gsMakeComm(pn2gsCommList(ATinsert(AssocArcs,(ATerm)gsString2ATermAppl("error"))),
                Process)));
  }
  // ***** end changed by Yarick.
      }
      // ***** Changed by Yarick 29.05.2006 to add error action + hide all actions ****
      if(error)
  Process=gsMakeBlock(ATmakeList1((ATerm)gsString2ATermAppl("error")),Process);
      if(hide)
  Process=gsMakeHide(pn2gsGenerateActionsTrans(),Process);
      // ***** end changed by Yarick.
    }

    ATermList Place_procs=ATtableKeys(context.place_process_name);
    ProcessList =
      ATinsert(ProcessList,
         (ATerm)gsMakeProcEqn(
            gsMakeProcVarId(gsString2ATermAppl("PetriNet"),
                pn2gsPlacesParameterTypes(Place_procs)),
            pn2gsPlacesParameters(Place_procs),
            Process)
         );
    gsDebugMsg("Process PetriNet added (the whole list: %T).\n",ProcessList);

    // reminder: NetID == "Net_'ID of the Petri net'"
    // the Net_ is preceded to the ID of the Petri net to prevent that two processes would have the same name!
    //
    // NetID = PetriNet("...")
    // "..." is the initial marking of all places

    Process = gsMakeParamId(gsString2ATermAppl("PetriNet"), pn2gsInitialMarkings(Place_procs));
    ProcessList = ATinsert(ProcessList, (ATerm)gsMakeProcEqn(gsMakeProcVarId(NetID, ATmakeList0()), ATmakeList0(), Process));
    gsDebugMsg("Process %T added (the whole list: %T).\n", NetID, ProcessList);

    //Now reverse the whole thing
    return ATreverse(ProcessList);
  }

  /*                           */
  /*                           */
  /*                           */
  /* Generation-phase function */
  /*                           */
  /*                           */
  /*                           */

  //==================================================
  // pn2gsTranslate converts the ATerm delivered by pn2gsAterm to a mCRL2 ATerm.
  //==================================================
  static ATermAppl pn2gsTranslate(ATermAppl Spec){
    // input: an ATermAppl that contains the translated PNML-file
    // output: another ATermAppl, which is the mCRL2 translation
    //         of the PNML-ATerm.

    //==================================================
    // initializations of tables and some initial checks.
    //==================================================

    gsDebugMsg("\n====================\n\nStart generating the necessary data. \n \n");

    // put the places, transitions and arcs in the lists again
    ATermList APlaces = ATLgetArgument(Spec, 0);
    ATermList ATransitions = ATLgetArgument(Spec, 1);
    ATermList AArcs = ATLgetArgument(Spec, 2);

    // temporary variable to store the current key
    // used for Places, Transitions and Arcs!!!
    ATerm CurrentKey;
    ATermAppl Prelude=ATAgetArgument(Spec,4);
    if(ATisEqual(Prelude,Appl0)) Prelude=gsMakeEmptyDataSpec(); //NULL;
    assert(gsIsDataSpec(Prelude));

    gsDebugMsg("> Insert the data of places that will be translated into tables...  \n");
    while (ATisEmpty(APlaces) == ATfalse) {
      // this loop itterates all places that will be translated
      gsDebugMsg("    examining %T\n", ATgetFirst(APlaces));
      CurrentKey = ATgetArgument(ATgetFirst(APlaces), 0);
      // it is sufficient to check whether a key appears in place_name OR place_mark
      // since a value is inserted in both tables at the same time!
      if (ATtableGet(context.place_name, CurrentKey)) {
  // the ID of the current places appears more than once in the places.
  // this is an error in the input, and thus termination takes place!
  gsErrorMsg("The id: '%T' appears more than once!\n", CurrentKey);
  return NULL;
      } else {
  // insert the data into context.place_name
  // key = id
  // value = name
  ATtablePut(context.place_name, CurrentKey , ATgetArgument(ATgetFirst(APlaces), 1));

        //typechecking of the place-related data and putting it into the tables:
        ATermAppl Value=ATAgetArgument(ATAgetFirst(APlaces), 2);
        ATermAppl Type=type_check_data_expr(Value, static_cast<ATermAppl>(mcrl2::data::sort_nat::nat()), gsMakeEmptyDataSpec());
        if(!Type) {gsErrorMsg("Type-checking of the initial marking of place %T failed (%T is not a natural number)\n",CurrentKey,Value); return NULL;}
  ATtablePut(context.place_mark, CurrentKey, (ATerm)Value);

        Value=ATAgetArgument(ATAgetFirst(APlaces), 3);
        if(!ATisEqual(Value,Appl0)){
          Type=type_check_sort_expr(Value,Prelude);
          if(!Type) {gsErrorMsg("Type-checking of sort expression %P as a type of place %T failed \n",Value,CurrentKey); return NULL;}
        }
        ATtablePut(context.place_type_mcrl2, CurrentKey, (ATerm)Value);

        ATermAppl SortValue=Value;

        Value=ATAgetArgument(ATAgetFirst(APlaces), 4);
        if(!ATisEqual(Value,Appl0)){
          Type=type_check_data_expr(Value,static_cast<ATermAppl>(mcrl2::data::sort_bag::bag(mcrl2::data::sort_expression(SortValue))),Prelude);
          if(!Type) {gsErrorMsg("Type-checking of data expression %T as an initial mCRL2 marking of place %T failed \n",Value,CurrentKey); return NULL;}
  }
        ATtablePut(context.place_mark_mcrl2, CurrentKey, (ATerm)Value);
      }

      // remove the entry from the list
      APlaces = ATgetNext(APlaces);
    }
    gsDebugMsg("\n  Places that are not inserted into the tables: %T\n", (ATerm)APlaces);
    gsDebugMsg("  ID's of the two read-in place tables.\n  THESE TWO LISTS SHOULD BE EXACTLY THE SAME!\n  ");
    gsDebugMsg("%T \n  %T \n", (ATerm)ATtableKeys(context.place_name), (ATerm)ATtableKeys(context.place_mark));

    gsDebugMsg("> Insert the the data of transitions that will be translated into tables...  \n");
    while (ATisEmpty(ATransitions) == ATfalse) {
      // this loop itterates all transitions that will be translated
      gsDebugMsg("    examining %T\n", ATgetFirst(ATransitions));
      CurrentKey = ATgetArgument(ATgetFirst(ATransitions), 0);
      if (ATtableGet(context.place_name, CurrentKey)) {
  // the ID of the current transition appeared already in the places.
  // this is an error in the input, and thus termination takes place!
  gsErrorMsg("The id: '%T' appears more than once!\n", CurrentKey);
  return NULL;
      } else if (ATtableGet(context.trans_name, CurrentKey)) {
  // the ID of the current transition appears more than once in the transitions.
  // this is an error in the input, and thus termination takes place!
  gsErrorMsg("The id: '%T' appears more than once!\n", CurrentKey);
  return NULL;
      } else {
  // insert the data into context.trans_name
  // key = id
  // value = name
  ATtablePut(context.trans_name, CurrentKey , ATgetArgument(ATgetFirst(ATransitions), 1));

        //typechecking later
        ATtablePut(context.trans_predicate, CurrentKey , ATgetArgument(ATgetFirst(ATransitions), 2));
      }
      // remove the entry from the list ATransitions
      ATransitions = ATgetNext(ATransitions);
    }
    gsDebugMsg("\n  Transitions that are not inserted into the tables: %T\n", (ATerm)ATransitions);
    gsDebugMsg("  ID's of the read-in transition table.\n  %T\n", (ATerm)ATtableKeys(context.trans_name));

    gsDebugMsg("> Insert the data of the arcs that will be translated into tables...  \n");
    while (ATisEmpty(AArcs) == ATfalse) {
      // this loop itterates all arcs that will be translated
      gsDebugMsg("    examining %T\n", ATgetFirst(AArcs));
      CurrentKey = ATgetArgument(ATgetFirst(AArcs), 0);
      if (ATtableGet(context.place_name, CurrentKey)) {
  // the ID of the current arc appeared already in the places.
  // this is an error in the input, and thus termination takes place!
  gsErrorMsg("The id: '%T' appears more than once!\n", CurrentKey);
  return NULL;
      } else if (ATtableGet(context.trans_name, CurrentKey)) {
  // the ID of the current arc appeared already in the transitions.
  // this is an error in the input, and thus termination takes place!
  gsErrorMsg("The id: '%T' appears more than once!\n", CurrentKey);
  return NULL;
      } else if (ATtableGet(context.arc_in, CurrentKey)) {
  // the ID of the current arc appeared already in the in_arcs.
  // this is an error in the input, and thus termination takes place!
  gsErrorMsg("The id: '%T' appears more than once!\n", CurrentKey);
  return NULL;
      } else if (ATtableGet(context.arc_out, CurrentKey)) {
  // the ID of the current arc appeared already in the out_arcs.
  // this is an error in the input, and thus termination takes place!
  gsErrorMsg("The id: '%T' appears more than once!\n", CurrentKey);
  return NULL;
      } else if (ATtableGet(context.arc_inhibit, CurrentKey)) {
  // the ID of the current arc appeared already in the inhibitor_arcs.
  // this is an error in the input, and thus termination takes place!
  gsErrorMsg("The id: '%T' appears more than once!\n", CurrentKey);
  return NULL;
      } else if (ATtableGet(context.arc_reset, CurrentKey)) {
  // the ID of the current arc appeared already in the reset_arcs.
  // this is an error in the input, and thus termination takes place!
  gsErrorMsg("The id: '%T' appears more than once!\n", CurrentKey);
  return NULL;
      } else {
  // the arc's ID did not appear in the transitions, places or arcs that will be translated
  // check the source and the target from the arc to see if the arc is used in the translation!

  // temporary variables to store the current source and target
  ATermAppl CurrentSource = ATAgetArgument(ATAgetFirst(AArcs),1);
  ATermAppl CurrentTarget = ATAgetArgument(ATAgetFirst(AArcs),2);
  ATermAppl CurrentType = ATAgetArgument(ATAgetFirst(AArcs),3);
  if (ATtableGet(context.place_name, (ATerm)CurrentSource) && ATtableGet(context.trans_name, (ATerm)CurrentTarget)) {
    if(ATisEqual(CurrentType,Appl0)){
      // The arc is an arc_out; it goes from a place to a transition
      // insert the data into context.arc_out
      // key = id
      // value = arc_out(source, target)
      ATtablePut(context.arc_out, (ATerm)CurrentKey, (ATerm)ATmakeAppl2(ATmakeAFun("arc_out",2,ATfalse),(ATerm)CurrentSource,(ATerm)CurrentTarget));
    }
    else if(ATisEqual(CurrentType,gsString2ATermAppl("inhibitor"))){
      // The arc is an arc_inhibitor; it goes from a place to a transition
      // insert the data into context.arc_out
      // key = id
      // value = arc_out(source, target)
      ATtablePut(context.arc_inhibit, (ATerm)CurrentKey, (ATerm)ATmakeAppl2(ATmakeAFun("arc_out",2,ATfalse),(ATerm)CurrentSource,(ATerm)CurrentTarget));
    }
    else if(ATisEqual(CurrentType,gsString2ATermAppl("reset"))){
      // The arc is an arc_reset; it goes from a place to a transition
      // insert the data into context.arc_out
      // key = id
      // value = arc_out(source, target)
      gsWarningMsg("The reset arc %T going from place %T to transition %T is reversed (buggy pnml?)\n", CurrentKey, CurrentSource, CurrentTarget);
      ATtablePut(context.arc_reset, (ATerm)CurrentKey, (ATerm)ATmakeAppl2(ATmakeAFun("arc_out",2,ATfalse),(ATerm)CurrentSource,(ATerm)CurrentTarget));
    }
  }
  else if (ATtableGet(context.place_name, (ATerm)CurrentTarget) && ATtableGet(context.trans_name, (ATerm)CurrentSource)) {
    if(ATisEqual(CurrentType,Appl0)){
      // The arc is an arc_in; it goes from a transition to a place
      // insert the data into context.arc_in
      // key = id
      // value = arc_in(source, target)
      ATtablePut(context.arc_in, (ATerm)CurrentKey, (ATerm)ATmakeAppl2(ATmakeAFun("arc_in",2,ATfalse),(ATerm)CurrentSource,(ATerm)CurrentTarget));
    }
    else if(ATisEqual(CurrentType,gsString2ATermAppl("inhibitor"))){
      // The arc is an arc_inhibitor; it goes from a transition to a place (which is wrong)
      // insert the data into context.arc_in
      // key = id
      // value = arc_in(source, target)
      gsWarningMsg("The inhibitor arc %T going from transition %T to place %T is reversed (buggy pnml?)\n", CurrentKey, CurrentSource, CurrentTarget);
      ATtablePut(context.arc_inhibit, (ATerm)CurrentKey, (ATerm)ATmakeAppl2(ATmakeAFun("arc_in",2,ATfalse),(ATerm)CurrentTarget,(ATerm)CurrentSource));
    }
    else if(ATisEqual(CurrentType,gsString2ATermAppl("reset"))){
      // The arc is an arc_reset; it goes from a transition to a place (which is wrong)
      // insert the data into context.arc_in
      // key = id
      // value = arc_in(source, target)
      ATtablePut(context.arc_reset, (ATerm)CurrentKey, (ATerm)ATmakeAppl2(ATmakeAFun("arc_in",2,ATfalse),(ATerm)CurrentSource,(ATerm)CurrentTarget));
    }
  }
  else {
    // either the source or the target (or both) of the arc will not be translated
    // therefore the arc will not be translated either!
    gsWarningMsg("The source or target of arc with id '%T' will not be translated, and thus this arc will not be translated either.\n", CurrentKey);
  }
  //not to forget, the name of the arc:
  ATtablePut(context.arc_name, (ATerm)CurrentKey, (ATerm)ATAgetArgument(ATAgetFirst(AArcs),4));
      }
      // remove the entry from the list ATransitions
      AArcs = ATgetNext(AArcs);
    }
    gsDebugMsg("\n  Arcs that are not inserted into the tables: %T\n", (ATerm)AArcs);

    gsDebugMsg("  ID's of the two read-in arc tables. The first list contains the in-arcs, the second one the out-arcs. \n");
    gsDebugMsg("  NO ID SHOULD BE IN BOTH TABLES!\n");
    gsDebugMsg("%T\n  %T\n\n", (ATerm)ATtableKeys(context.arc_in), (ATerm)ATtableKeys(context.arc_out));

    //==================================================
    // generation of some necessary data
    //==================================================

    // Temporary variables used for generations
    ATermList Arcs;
    ATerm CurrentArc;
    ATerm CurrentPlace;
    ATerm CurrentTrans;
    ATermList ArcValueList;

    // Generate context.place_in - context.trans_out
    Arcs = ATtableKeys(context.arc_in);
    while (ATisEmpty(Arcs) == ATfalse) {
      CurrentArc = ATgetFirst(Arcs);
      CurrentPlace = ATgetArgument(ATtableGet(context.arc_in, CurrentArc), 1);
      CurrentTrans = ATgetArgument(ATtableGet(context.arc_in, CurrentArc), 0);
      // insert CurrentPlace and CurrentArc in context.place_in
      if (!(ATtableGet(context.place_in, CurrentPlace))) {
  // if the CurrentPlace was not yet present in context.place_in, insert it
  // key = CurrentPlace.id
  // value = [CurrentArc.id]
  ATtablePut(context.place_in, CurrentPlace, (ATerm)ATmakeList1(CurrentArc));
      } else {
  // if the CurrentPlace was already present in context.place_in, insert CurrentArc.id in the value-list
  ArcValueList = (ATermList)ATtableGet(context.place_in, CurrentPlace);
  ArcValueList = ATinsert(ArcValueList, CurrentArc);
  ATtablePut(context.place_in, CurrentPlace, (ATerm)ArcValueList);
      }

      // insert CurrentTrans and CurrentArc in context.trans_out
      if (!(ATtableGet(context.trans_out, CurrentTrans))) {
  // if the CurrentTrans was not yet present in context.trans_out, insert it
  // key = CurrentTrans.id
  // value = [CurrentArc.id]
  ATtablePut(context.trans_out, CurrentTrans, (ATerm)ATmakeList1(CurrentArc));
      } else {
  // if the CurrentTrans was already present in context.trans_out, insert CurrentArc.id in the value-list
  ArcValueList = (ATermList)ATtableGet(context.trans_out, CurrentTrans);
  ArcValueList = ATinsert(ArcValueList, CurrentArc);
  ATtablePut(context.trans_out, CurrentTrans, (ATerm)ArcValueList);
      }
      Arcs = ATgetNext(Arcs);
    }
    Arcs = ATtableKeys(context.place_in);
    gsDebugMsg("context.place_in contains the following keys: %T\n", Arcs);
    while (ATisEmpty(Arcs) == ATfalse) {
      gsDebugMsg("Place '%T' has the following incoming arcs: %T\n", ATgetFirst(Arcs), ATtableGet(context.place_in, ATgetFirst(Arcs)));
      Arcs = ATgetNext(Arcs);
    }
    Arcs = ATtableKeys(context.trans_out);
    gsDebugMsg("context.trans_out contains the following keys: %T\n", Arcs);
    while (ATisEmpty(Arcs) == ATfalse) {
      gsDebugMsg("Transition '%T' has the following outgoing arcs: %T\n", ATgetFirst(Arcs), ATtableGet(context.trans_out, ATgetFirst(Arcs)));
      Arcs = ATgetNext(Arcs);
    }

    // Generate context.trans_in - context.place_out
    Arcs = ATtableKeys(context.arc_out);
    while (ATisEmpty(Arcs) == ATfalse) {
      CurrentArc = ATgetFirst(Arcs);
      CurrentPlace = ATgetArgument(ATtableGet(context.arc_out, CurrentArc), 0);
      CurrentTrans = ATgetArgument(ATtableGet(context.arc_out, CurrentArc), 1);
      // insert CurrentPlace and CurrentArc in context.place_out
      if (!(ATtableGet(context.place_out, CurrentPlace))) {
  // if the CurrentPlace was not yet present in context.place_out, insert it
  // key = CurrentPlace.id
  // value = [CurrentArc.id]
  ATtablePut(context.place_out, CurrentPlace, (ATerm)ATmakeList1(CurrentArc));
      } else {
  // if the CurrentPlace was already present in context.place_out, insert CurrentArc.id in the value-list
  ArcValueList = (ATermList)ATtableGet(context.place_out, CurrentPlace);
  ArcValueList = ATinsert(ArcValueList, CurrentArc);
  ATtablePut(context.place_out, CurrentPlace, (ATerm)ArcValueList);
      }

      // insert CurrentTrans and CurrentArc in context.trans_in
      if (!(ATtableGet(context.trans_in, CurrentTrans))) {
  // if the CurrentTrans was not yet present in context.trans_in, insert it
  // key = CurrentTrans.id
  // value = [CurrentArc.id]
  ATtablePut(context.trans_in, CurrentTrans, (ATerm)ATmakeList1(CurrentArc));
      } else {
  // if the CurrentTrans was already present in context.trans_in, insert CurrentArc.id in the value-list
  ArcValueList = (ATermList)ATtableGet(context.trans_in, CurrentTrans);
  ArcValueList = ATinsert(ArcValueList, CurrentArc);
  ATtablePut(context.trans_in, CurrentTrans, (ATerm)ArcValueList);
      }
      Arcs = ATgetNext(Arcs);
    }
    Arcs = ATtableKeys(context.place_out);
    gsDebugMsg("context.place_out contains the following keys: %T\n", Arcs);
    while (ATisEmpty(Arcs) == ATfalse) {
      gsDebugMsg("Place '%T' has the following outgoing arcs: %T\n", ATgetFirst(Arcs), ATtableGet(context.place_out, ATgetFirst(Arcs)));
      Arcs = ATgetNext(Arcs);
    }
    Arcs = ATtableKeys(context.trans_in);
    gsDebugMsg("context.trans_in contains the following keys: %T\n", Arcs);
    while (ATisEmpty(Arcs) == ATfalse) {
      gsDebugMsg("Transition '%T' has the following incoming arcs: %T\n", ATgetFirst(Arcs), ATtableGet(context.trans_in, ATgetFirst(Arcs)));
      Arcs = ATgetNext(Arcs);
    }

    // Generate context.trans_in - context.place_inhibit
    Arcs = ATtableKeys(context.arc_inhibit);
    while (ATisEmpty(Arcs) == ATfalse) {
      CurrentArc = ATgetFirst(Arcs);
      CurrentPlace = ATgetArgument(ATAtableGet(context.arc_inhibit, CurrentArc), 0);
      CurrentTrans = ATgetArgument(ATAtableGet(context.arc_inhibit, CurrentArc), 1);
      // insert CurrentPlace and CurrentArc in context.place_inhibit
      if (!(ATtableGet(context.place_inhibit, CurrentPlace))) {
  // if the CurrentPlace was not yet present in context.place_inhibit, insert it
  // key = CurrentPlace.id
  // value = [CurrentArc.id]
  ATtablePut(context.place_inhibit, CurrentPlace, (ATerm)ATmakeList1(CurrentArc));
      } else {
  // if the CurrentPlace was already present in context.place_inhibit, insert CurrentArc.id in the value-list
  ArcValueList = (ATermList)ATtableGet(context.place_inhibit, CurrentPlace);
  ArcValueList = ATinsert(ArcValueList, CurrentArc);
  ATtablePut(context.place_inhibit, CurrentPlace, (ATerm)ArcValueList);
      }

      // insert CurrentTrans and CurrentArc in context.trans_in
      if (!(ATtableGet(context.trans_in, CurrentTrans))) {
  // if the CurrentTrans was not yet present in context.trans_in, insert it
  // key = CurrentTrans.id
  // value = [CurrentArc.id]
  ATtablePut(context.trans_in, CurrentTrans, (ATerm)ATmakeList1(CurrentArc));
      } else {
  // if the CurrentTrans was already present in context.trans_in, insert CurrentArc.id in the value-list
  ArcValueList = (ATermList)ATtableGet(context.trans_in, CurrentTrans);
  ArcValueList = ATinsert(ArcValueList, CurrentArc);
  ATtablePut(context.trans_in, CurrentTrans, (ATerm)ArcValueList);
      }
      Arcs = ATgetNext(Arcs);
    }
    Arcs = ATtableKeys(context.place_inhibit);
    gsDebugMsg("context.place_inhibit contains the following keys: %T\n", Arcs);
    while (ATisEmpty(Arcs) == ATfalse) {
      gsDebugMsg("Place '%T' has the following outgoing arcs: %T\n", ATgetFirst(Arcs), ATtableGet(context.place_inhibit, ATgetFirst(Arcs)));
      Arcs = ATgetNext(Arcs);
    }

    // Generate context.trans_in - context.place_reset
    Arcs = ATtableKeys(context.arc_reset);
    while (ATisEmpty(Arcs) == ATfalse) {
      CurrentArc = ATgetFirst(Arcs);
      CurrentTrans = ATgetArgument(ATAtableGet(context.arc_reset, CurrentArc), 0);
      CurrentPlace = ATgetArgument(ATAtableGet(context.arc_reset, CurrentArc), 1);
      // insert CurrentPlace and CurrentArc in context.place_reset
      if (!(ATtableGet(context.place_reset, CurrentPlace))) {
  // if the CurrentPlace was not yet present in context.place_reset, insert it
  // key = CurrentPlace.id
  // value = [CurrentArc.id]
  ATtablePut(context.place_reset, CurrentPlace, (ATerm)ATmakeList1(CurrentArc));
      } else {
  // if the CurrentPlace was already present in context.place_reset, insert CurrentArc.id in the value-list
  ArcValueList = (ATermList)ATtableGet(context.place_reset, CurrentPlace);
  ArcValueList = ATinsert(ArcValueList, CurrentArc);
  ATtablePut(context.place_reset, CurrentPlace, (ATerm)ArcValueList);
      }

      // insert CurrentTrans and CurrentArc in context.trans_out
      if (!(ATtableGet(context.trans_out, CurrentTrans))) {
  // if the CurrentTrans was not yet present in context.trans_out, insert it
  // key = CurrentTrans.id
  // value = [CurrentArc.id]
  ATtablePut(context.trans_out, CurrentTrans, (ATerm)ATmakeList1(CurrentArc));
      } else {
  // if the CurrentTrans was already present in context.trans_out, insert CurrentArc.id in the value-list
  ArcValueList = (ATermList)ATtableGet(context.trans_out, CurrentTrans);
  ArcValueList = ATinsert(ArcValueList, CurrentArc);
  ATtablePut(context.trans_out, CurrentTrans, (ATerm)ArcValueList);
      }      Arcs = ATgetNext(Arcs);
    }
    Arcs = ATtableKeys(context.place_reset);
    gsDebugMsg("context.place_reset contains the following keys: %T\n", Arcs);
    while (ATisEmpty(Arcs) == ATfalse) {
      gsDebugMsg("Place '%T' has the following outgoing arcs: %T\n", ATgetFirst(Arcs), ATtableGet(context.place_reset, ATgetFirst(Arcs)));
      Arcs = ATgetNext(Arcs);
    }

    //Filling in the color data for arcs
    for(ATermList Arcs=ATtableKeys(context.arc_in);!ATisEmpty(Arcs);Arcs=ATgetNext(Arcs)){
      ATermAppl Arc=ATAgetFirst(Arcs);
      ATermAppl Place=ATAgetArgument(ATAtableGet(context.arc_in,(ATerm)Arc),1);
      ATermAppl Type=ATAtableGet(context.place_type_mcrl2,(ATerm)Place);
      ATtablePut(context.arc_type_mcrl2,(ATerm)Arc,(ATerm)Type);
    }
    for(ATermList Arcs=ATtableKeys(context.arc_reset);!ATisEmpty(Arcs);Arcs=ATgetNext(Arcs)){
      ATermAppl Arc=ATAgetFirst(Arcs);
      ATermAppl Place=ATAgetArgument(ATAtableGet(context.arc_reset,(ATerm)Arc),1);
      ATermAppl Type=ATAtableGet(context.place_type_mcrl2,(ATerm)Place);
      ATtablePut(context.arc_type_mcrl2,(ATerm)Arc,(ATerm)Type);
    }
    for(ATermList Arcs=ATtableKeys(context.arc_out);!ATisEmpty(Arcs);Arcs=ATgetNext(Arcs)){
      ATermAppl Arc=ATAgetFirst(Arcs);
      ATermAppl Place=ATAgetArgument(ATAtableGet(context.arc_out,(ATerm)Arc),0);
      ATermAppl Type=ATAtableGet(context.place_type_mcrl2,(ATerm)Place);
      ATtablePut(context.arc_type_mcrl2,(ATerm)Arc,(ATerm)Type);
    }
    for(ATermList Arcs=ATtableKeys(context.arc_inhibit);!ATisEmpty(Arcs);Arcs=ATgetNext(Arcs)){
      ATermAppl Arc=ATAgetFirst(Arcs);
      ATermAppl Place=ATAgetArgument(ATAtableGet(context.arc_inhibit,(ATerm)Arc),0);
      ATermAppl Type=ATAtableGet(context.place_type_mcrl2,(ATerm)Place);
      ATtablePut(context.arc_type_mcrl2,(ATerm)Arc,(ATerm)Type);
    }

    //Typechecking the transition predicates
    //for each transition create a table of variables
    //taking the names of the arcs as the names of the variables
    //for uncolored arcs the names are ignored
    //2 arcs with the same name are not allowed at 1 transition
    //then typecheck the predicate w.r.t the variables and the prelude

    ATermTable Vars=ATtableCreate(63,50);
    for(ATermList Trans=ATtableKeys(context.trans_name);!ATisEmpty(Trans);Trans=ATgetNext(Trans)){
      ATermAppl Tran=ATAgetFirst(Trans);

      ATermList Arcs=ATLtableGet(context.trans_in,(ATerm)Tran);
      if(!Arcs) Arcs=ATLtableGet(context.trans_out,(ATerm)Tran);
      else {
  ATermList Arcs2=ATLtableGet(context.trans_out,(ATerm)Tran);
  if(Arcs2) Arcs=ATconcat(Arcs,Arcs2);
      }

      if(!Arcs) continue; //this transition has no ajusent arcs...

      for(;!ATisEmpty(Arcs);Arcs=ATgetNext(Arcs)){
  ATermAppl Arc=ATAgetFirst(Arcs);

  ATermAppl ArcName=ATAtableGet(context.arc_name,(ATerm)Arc);
  if(!ArcName || ATisEqual(ArcName,Appl0)) continue;

  if(ATAtableGet(Vars,(ATerm)ArcName)){
    ATtableDestroy(Vars);
    gsErrorMsg("transition %T has more than 1 adjucent arc with name %T.\n",Tran,ArcName);
    return NULL;
  }

  ATermAppl ArcType=ATAtableGet(context.arc_type_mcrl2,(ATerm)Arc);
  if(ATisEqual(ArcType,Appl0)) continue;

  ATtablePut(Vars, (ATerm)ArcName, (ATerm)ArcType);
      }


      ATermAppl Value=ATAtableGet(context.trans_predicate,(ATerm)Tran);
      if(!ATisEqual(Value,Appl0)){
  ATermAppl Type=type_check_data_expr(Value,static_cast<ATermAppl>(mcrl2::data::sort_bool::bool_()),Prelude,Vars);
  if(!Type) {
    ATtableDestroy(Vars);
    gsErrorMsg("Type-checking of the mCRL2 predicate %P of transition %T failed\n",Value,Tran);
    return NULL;
  }
      }
      ATtableReset(Vars);
    }
    ATtableDestroy(Vars);


    gsDebugMsg("\n====================\n\n");

    //==================================================
    // creation of mCRL2 ATerms
    //==================================================

    if(!ATgetLength(ATtableKeys(context.arc_in))&&
       !ATgetLength(ATtableKeys(context.arc_out))&&
       !ATgetLength(ATtableKeys(context.arc_inhibit))&&
       !ATgetLength(ATtableKeys(context.arc_reset))
      )
      {gsErrorMsg("specification contains no arcs.\n"); return NULL;}

    ATermList Actions=pn2gsGenerateActions();       // store all the mCRL2 Actions
    ATermList ProcEqns=pn2gsGenerateProcEqns(ATAgetArgument(Spec, 3));      // store all the mCRL2 Process Equations

    gsDebugMsg("\n\n====================\n\n");
    gsDebugMsg("Conversion Succesful!");
    gsDebugMsg("\n\n====================\n\n");

    ATermAppl Result=gsMakeProcSpec(Prelude,
          gsMakeActSpec(Actions),
          gsMakeGlobVarSpec(ATmakeList0()),
          gsMakeProcEqnSpec(ProcEqns),
          gsMakeProcessInit(gsMakeParamId(ATAgetArgument(Spec, 3), ATmakeList0()))
          );

    // ***** Changed by Yarick 29.05.2006 to add error action ****

    if(error) {
      //add nMaxTockens
      ATermAppl ExtraMap=mcrl2::data::function_symbol(ATAgetArgument(nMaxTokens,0),mcrl2::data::sort_pos::pos());
      ATermAppl ExtraEqn=gsMakeDataEqn(ATmakeList0(),sort_bool::true_(),nMaxTokens,gsMakeId(ATmakeAppl0(ATmakeAFunInt0(error))));
      ATermAppl DataSpec=ATAgetArgument(Result,0);
      ATermAppl NewMapSpec=gsMakeMapSpec(ATinsert(ATLgetArgument(ATAgetArgument(DataSpec,2),0),(ATerm)ExtraMap));
      ATermAppl NewDataEqnSpec=gsMakeDataEqnSpec(ATinsert(ATLgetArgument(ATAgetArgument(DataSpec,3),0),(ATerm)ExtraEqn));
      DataSpec=ATsetArgument(DataSpec,(ATerm)NewMapSpec,2);
      DataSpec=ATsetArgument(DataSpec,(ATerm)NewDataEqnSpec,3);
      Result=ATsetArgument(Result,(ATerm)DataSpec,0);
    }

    // ***** end changed by Yarick.


    return Result;
  }

  /*               */
  /*               */
  /*               */
  /* Main function */
  /*               */
  /*               */
  /*               */


  //==================================================
  // PrintHelp performs actual conversion by calling more specialised functions
  //==================================================
  bool do_pnml2mcrl2(char const* InFileName, std::ostream& output_stream)
  { ATprotectList(&(context.transitions));
    ATprotectAppl(&Appl0);
    ATprotectAppl(&IdX);
    ATprotectAppl(&Number0);
    ATprotectAppl(&Number1);
    ATprotectAppl(&OpAnd);
    ATprotectAppl(&OpAdd);
    ATprotectAppl(&OpSubt);
    ATprotectAppl(&OpMax);
    ATprotectAppl(&OpMin);
    ATprotectAppl(&OpGT);
    ATprotectAppl(&OpLTE);
    ATprotectAppl(&OpEq);
    ATprotectAppl(&OpInt2Nat);
    ATprotectAppl(&EmptyBag);
    ATprotectAppl(&OpBagEnum);
    ATprotectAppl(&nMaxTokens);
    ATprotectAppl(&ErrorAction);

    ticpp::Document doc(InFileName);

    try {
      if (std::strcmp(InFileName,"\0") == 0) {
        std::cin >> doc;
      }
      else {
        doc.LoadFile();
      }
    }
    catch (...) {
      gsErrorMsg("Document not parsed succesfully. \n");

      return false;
    }


    Appl0=gsString2ATermAppl("_");
    IdX=gsMakeId(gsString2ATermAppl("x"));
    Number0=gsMakeId(gsString2ATermAppl("0"));
    Number1=gsMakeId(gsString2ATermAppl("1"));
    OpAnd=gsMakeId(mcrl2::data::sort_bool::and_().name());
    OpAdd=gsMakeId(mcrl2::data::sort_pos::plus().name());
    OpSubt=gsMakeId(mcrl2::data::sort_int::minus(mcrl2::data::sort_int::int_(),mcrl2::data::sort_int::int_()).name());
    OpMax=gsMakeId(mcrl2::data::sort_pos::maximum().name());
    OpMin=gsMakeId(mcrl2::data::sort_pos::minimum().name());
    OpGT=gsMakeId(mcrl2::data::greater(mcrl2::data::sort_pos::pos()).name());
    OpLTE=gsMakeId(mcrl2::data::less_equal(mcrl2::data::sort_pos::pos()).name());
    OpEq=gsMakeId(mcrl2::data::equal_to(mcrl2::data::sort_pos::pos()).name());
    OpInt2Nat=gsMakeId(mcrl2::data::sort_int::int2nat().name());
    EmptyBag=gsMakeId(mcrl2::data::sort_bag::emptybag(mcrl2::data::sort_pos::pos()).name());
    OpBagEnum=gsMakeId(gsMakeOpIdNameBagEnum());
    nMaxTokens=gsMakeId(gsString2ATermAppl("nMaxTokens"));
    ErrorAction=gsMakeParamId(ATmakeAppl0(ATmakeAFunId("_error")), ATmakeList0());

    ATermAppl Spec=pn2gsAterm(doc);

    if(!Spec){
      gsErrorMsg("Error while converting PNML to ATerm, conversion stopped!  \n");
      return false;
    }

    context.place_name=ATtableCreate(63,50);
    context.place_mark=ATtableCreate(63,50);
    context.place_mark_mcrl2=ATtableCreate(63,50);
    context.place_type_mcrl2=ATtableCreate(63,50);
    context.trans_name=ATtableCreate(63,50);
    context.trans_predicate=ATtableCreate(63,50);
    context.arc_in=ATtableCreate(63,50);
    context.arc_out=ATtableCreate(63,50);
    context.arc_inhibit=ATtableCreate(63,50);
    context.arc_reset=ATtableCreate(63,50);
    context.arc_name=ATtableCreate(63,50);

    context.place_in=ATtableCreate(63,50);
    context.trans_in=ATtableCreate(63,50);
    context.place_out=ATtableCreate(63,50);
    context.trans_out=ATtableCreate(63,50);
    context.place_inhibit=ATtableCreate(63,50);
    context.place_reset=ATtableCreate(63,50);
    context.arc_type_mcrl2=ATtableCreate(63,50);

    context.transitions=ATmakeList0();
    context.place_process_name=ATtableCreate(63,50);

    //ATprintf("Spec %t\n\n", Spec);
    Spec=pn2gsTranslate(Spec);

    ATunprotectList(&(context.transitions));
    ATunprotectAppl(&Appl0);
    ATunprotectAppl(&IdX);
    ATunprotectAppl(&Number0);
    ATunprotectAppl(&Number1);
    ATunprotectAppl(&OpAnd);
    ATunprotectAppl(&OpAdd);
    ATunprotectAppl(&OpSubt);
    ATunprotectAppl(&OpMax);
    ATunprotectAppl(&OpMin);
    ATunprotectAppl(&OpGT);
    ATunprotectAppl(&OpLTE);
    ATunprotectAppl(&OpEq);
    ATunprotectAppl(&OpInt2Nat);
    ATunprotectAppl(&EmptyBag);
    ATunprotectAppl(&OpBagEnum);
    ATunprotectAppl(&nMaxTokens);
    ATunprotectAppl(&ErrorAction);
    ATtableDestroy(context.place_name);
    ATtableDestroy(context.place_mark);
    ATtableDestroy(context.place_mark_mcrl2);
    ATtableDestroy(context.place_type_mcrl2);
    ATtableDestroy(context.trans_name);
    ATtableDestroy(context.trans_predicate);
    ATtableDestroy(context.arc_in);
    ATtableDestroy(context.arc_out);
    ATtableDestroy(context.arc_inhibit);
    ATtableDestroy(context.arc_reset);
    ATtableDestroy(context.arc_name);

    ATtableDestroy(context.place_in);
    ATtableDestroy(context.trans_in);
    ATtableDestroy(context.place_out);
    ATtableDestroy(context.trans_out);
    ATtableDestroy(context.place_inhibit);
    ATtableDestroy(context.place_reset);
    ATtableDestroy(context.place_process_name);
    ATtableDestroy(context.arc_type_mcrl2);

    if(!Spec) {
      gsErrorMsg("Error while converting PNML ATerm to mCRL2 ATerm, conversion stopped!  \n");
      return false;
    }

    gsDebugMsg("The result of conversion is: %T\n",Spec);

    Spec = type_check_proc_spec(Spec);

    if(Spec){
      PrintPart_CXX(output_stream, (ATerm) Spec, ppDefault);
    }

    return (true);
  }



  //==================================================
  // main
  //==================================================
  int main(int argc, char **argv)
  {
    MCRL2_ATERM_INIT(argc, argv)
    return pnml2mcrl2_tool().execute(argc,argv);
  }

// Added by Yarick: alternative generation of Places:
//  static ATermAppl pn2gsGenerateP_pi_a(ATermList InActionLists, ATermList OutActionLists, ATermList ResetActionLists);
static ATermList pn2gsGetActionLists(unsigned int n, ATermList ActList);
static ATermAppl pn2gsMakeMultiAction(ATermList ActionList, ATermList ParamList=NULL);
static ATermList pn2gsMakeSendActions(ATermList ReadActions);

//==================================================
// pn2gsGeneratePlaceAlternative generates the mCRL2 Process Equations belonging to one place
// without recursive parallelism
//==================================================
static ATermList pn2gsGeneratePlaceAlternative(ATerm PlaceID){
  // input: access to the context; ID of the place
  // output: an updated list of the processes: the processes belonging to the place added to the list

  //==================================================
  // retrieve the maximum concurrency
  //==================================================
  ATermList ActsIn=ATLtableGet(context.place_in, PlaceID);
  if(!ActsIn) ActsIn=ATmakeList0();
  int n=ATgetLength(ActsIn);

  ATermList ActsOut=ATLtableGet(context.place_out, PlaceID);
  if(!ActsOut) ActsOut=ATmakeList0();
  else ActsOut=pn2gsMakeSendActions(ActsOut);
  int m=ATgetLength(ActsOut);

  ATermList ActsInhibit=ATLtableGet(context.place_inhibit, PlaceID);
  if(!ActsInhibit) ActsInhibit=ATmakeList0();
  else ActsInhibit=pn2gsMakeSendActions(ActsInhibit);
  int k=ATgetLength(ActsInhibit);

  ATermList ActsReset=ATLtableGet(context.place_reset, PlaceID);
  if(!ActsReset) ActsReset=ATmakeList0();
  int l=ATgetLength(ActsReset);

  gsDebugMsg("Place %T has maximum concurrency in: '%d', out: '%d', inhibit: '%d', and reset: '%d'\n", PlaceID, n,m,k,l);

  //==================================================
  // generate the processes
  //==================================================

  //==================================================
  // second, we generate the place processes.
  //==================================================

    // In the comment below, pi will refer to 'placeid' or to 'placeid_placename'.
    // This depends on whether or not a place has a name, and if it does, if it's not too long.

    // Each place creates the following processes:

    // the values n and m indicate the maximum in/out concurrency of the place
    // we can add 0 to n tokens and remove 0 to min(m,X) token at one time.
    // we don't know X at generation time, so we have to generate for 0 to m possibilities.
    //
    // P_pi(x:Nat)     =
    //                    0<=x ->(0,0 case we skip
    //                   (skip)  +P_pi_ar_1_0.P_pi(max(x+1-0,0)) we skip max and -0.
    //                           +P_pi_ar_2_0.P_pi(max(x+2-0,0))
    //                           ...
    //                           +P_pi_ar_n_0.P_pi(max(x+n-0,0))
    //                           )
    //                   +1<=x ->(P_pi_ar_0_1.P_pi(max(x+0-1,0))
    //                           +P_pi_ar_1_1.P_pi(max(x+1-1,0))
    //                           +P_pi_ar_2_1.P_pi(max(x+2-1,0))
    //                           ...
    //                           +P_pi_ar_n_1.P_pi(max(x+n-1,0))
    //                           )
    //                   +
    //                   ....
    //                   +m<=x ->(P_pi_ar_0_m.P_pi(max(x+0-m,0))
    //                           +P_pi_ar_1_m.P_pi(max(x+1-m,0))
    //                           +P_pi_ar_2_m.P_pi(max(x+2-m,0))
    //                           ...
    //                           +P_pi_ar_n_m.P_pi(max(x+n-m,0))
    //                           )
    //
    // P_pi_ar_i_j = all possible multiactions with i incoming and j outgoing arcs.
    // need to be generated for all 0=<i<=n,0<=j<=m,i+j>0.
    // variables to store the name and id of the place
    //
    // For the case with k inhibitor arcs (k>0):
    // additional summands for P_pi:
    //                   +x==0 ->(P_pi_ai_0.P_pi(x)
    //                           +P_pi_ai_1.P_pi(x+1)
    //                           ...
    //                           +P_pi_ai_n.P_pi(x+n))
    //
    // P_pi_ai_i = all possible multiactions with i incoming and inhibitor arcs.
    // need to be generated for all 0=<i<=n
    // variables to store the name and id of the place


    //Calculate the name of the process
  AFun CurrentPlaceId = ATgetAFun(PlaceID);

  {
    char * CurrentPlaceName = ATgetName(ATgetAFun(ATtableGet(context.place_name, PlaceID)));

    if(strcmp(CurrentPlaceName, "default_name"))
      // name of the place is not "default_name"
      // name of the place may be used
      CurrentPlaceId=ATappendAFun(ATappendAFun(CurrentPlaceId,"_"),CurrentPlaceName);

    //Prepend "P_"
    CurrentPlaceId=ATprependAFun("P_",CurrentPlaceId);
  }
  // variables to store the process names
  ATermAppl CurrentPlace=ATmakeAppl0(CurrentPlaceId);
  //ATermAppl CurrentPlaceAdd=ATmakeAppl0(ATappendAFun(CurrentPlaceId,"_add"));
  //ATermAppl CurrentPlaceIn=ATmakeAppl0(ATappendAFun(CurrentPlaceId,"_in"));
  //ATermAppl CurrentPlaceRem=ATmakeAppl0(ATappendAFun(CurrentPlaceId,"_rem"));
  //ATermAppl CurrentPlaceOut=ATmakeAppl0(ATappendAFun(CurrentPlaceId,"_out"));

  //added by Yarick: we need a table to relate PlaceId and CurrentPlace.
  ATtablePut(context.place_process_name,PlaceID,(ATerm)CurrentPlace);

  ATermList EquationList=ATmakeList0(); //the result
  ATermAppl Body=NULL;                  //the body of the main equation

  // ++++++++++++++++++ begin generation of minimal number of summands
  // This is yet another alternative method. We try to only generate
  // the full multiactions between a place and a transition.

  //colored: Type
  ATermAppl Type=ATAtableGet(context.place_type_mcrl2,(ATerm)PlaceID);
  if(Type && ATisEqual(Type,Appl0)) Type=NULL;

  // foreach transition t : find all arks between p and t
  // calculate its value n input arcs - m out arcs, take into account the inhibitor and reset arcs.
  for(ATermList Lt=ATtableKeys(context.trans_name);!ATisEmpty(Lt);Lt=ATgetNext(Lt)){
    ATermAppl TransID=ATAgetFirst(Lt);

    //find all arcs connecting PlaceId and TransId
    ATermList mult_c=ATmakeList0(); //current multiactions
    for(ATermList La=ATLtableGet(context.place_in,(ATerm)PlaceID);La && !ATisEmpty(La);La=ATgetNext(La)){
      ATermAppl ArcID=ATAgetFirst(La);
      ATermAppl Arc=ATAtableGet(context.arc_in,(ATerm)ArcID);
      if(!ATisEqual(ATAgetArgument(Arc,1),PlaceID)) continue;
      if(!ATisEqual(ATAgetArgument(Arc,0),TransID)) continue;
      mult_c=ATinsert(mult_c,(ATerm)ArcID);
    }
    int nIn=ATgetLength(mult_c); //the number of tokens that adds/removes
    ATermList mult=ATreverse(mult_c); //the resulting list of actions (in,_out,inhibit,_reset)

    mult_c=ATmakeList0();
    for(ATermList La=ATLtableGet(context.place_out,(ATerm)PlaceID);La && !ATisEmpty(La);La=ATgetNext(La)){
      ATermAppl ArcID=ATAgetFirst(La);
      ATermAppl Arc=ATAtableGet(context.arc_out,(ATerm)ArcID);
      if(!ATisEqual(ATAgetArgument(Arc,0),PlaceID)) continue;
      if(!ATisEqual(ATAgetArgument(Arc,1),TransID)) continue;
      mult_c=ATinsert(mult_c,(ATerm)ArcID);
    }
    int nOut=ATgetLength(mult_c);
    mult=ATconcat(mult,pn2gsMakeSendActions(ATreverse(mult_c)));

    mult_c=ATmakeList0();
    bool inhib=false;
    for(ATermList La=ATLtableGet(context.place_inhibit,(ATerm)PlaceID);La && !ATisEmpty(La);La=ATgetNext(La)){
      ATermAppl ArcID=ATAgetFirst(La);
      ATermAppl Arc=ATAtableGet(context.arc_inhibit,(ATerm)ArcID);
      if(!ATisEqual(ATAgetArgument(Arc,0),PlaceID)) continue;
      if(!ATisEqual(ATAgetArgument(Arc,1),TransID)) continue;
      mult_c=ATinsert(mult_c,(ATerm)ArcID);
      inhib=true;
    }
    mult=ATconcat(mult,pn2gsMakeSendActions(ATreverse(mult_c)));

    mult_c=ATmakeList0();
    bool reset=false;
    for(ATermList La=ATLtableGet(context.place_reset,(ATerm)PlaceID);La && !ATisEmpty(La);La=ATgetNext(La)){
      ATermAppl ArcID=ATAgetFirst(La);
      ATermAppl Arc=ATAtableGet(context.arc_reset,(ATerm)ArcID);
      if(!ATisEqual(ATAgetArgument(Arc,1),PlaceID)) continue;
      if(!ATisEqual(ATAgetArgument(Arc,0),TransID)) continue;
      mult_c=ATinsert(mult_c,(ATerm)ArcID);
      reset=true;
    }
    mult=ATconcat(mult,ATreverse(mult_c));

    if(!nIn && !nOut && !inhib && !reset) continue;

    if(nOut>0 && inhib){
      gsWarningMsg("Both output and inhibitor arcs connect place %T with transition %T. This transition can never fire.\n");
      continue;
    }

    //summand
    AFun AR=ATappendAFun(CurrentPlaceId,"_ar_");
    if(inhib && reset) AR=ATappendAFun(CurrentPlaceId,"_arir_");
    else{
      if(inhib) AR=ATappendAFun(CurrentPlaceId,"_ari_");
      else if(reset) AR=ATappendAFun(CurrentPlaceId,"_arr_");
    }

    ATermAppl LeftName=ATmakeAppl0(ATappendAFun(AR,ATgetName(ATgetAFun(TransID))));

    //colored (make variables list)
    ATermList VarNames=ATmakeList0();
    if(Type){
      for(int i=0; i<nIn+nOut; i++){
  ATermAppl Name=ATmakeAppl0(ATappendAFun(ATappendAFun(ATprependAFun("vp_",ATgetAFun(PlaceID)),"_"),ATgetName(ATmakeAFunInt0(i))));
  VarNames=ATinsert(VarNames,(ATerm)Name);
      }
      VarNames=ATreverse(VarNames);
    }

    ATermAppl Left=gsMakeParamId(LeftName,pn2gsMakeIds(VarNames)); //make name P_pi_ar_i_j
    ATermAppl RightExpr=IdX;  //x;
    int d=nIn-nOut;
    if(!reset) {
      if(d>0) RightExpr=pn2gsMakeDataApplProd2(OpAdd,RightExpr,gsMakeId(ATmakeAppl0(ATmakeAFunInt0(d))));//RightExpr=x+d;
      else if(d<0) RightExpr=gsMakeDataAppl(OpInt2Nat,ATmakeList1((ATerm)pn2gsMakeDataApplProd2(OpSubt,RightExpr,gsMakeId(ATmakeAppl0(ATmakeAFunInt0(-d))))));//RightExpr=Int2Nat(x-d);
    }
    else
      RightExpr=gsMakeId(ATmakeAppl0(ATmakeAFunInt0(nIn)));//RightExpr=nIn;

    //colored
    if(Type){
      RightExpr=IdX;
      if(!reset){
  if(nIn)
    RightExpr=pn2gsMakeDataApplProd2(OpAdd,RightExpr,pn2gsMakeBagVars(pn2gsMakeIds(ATgetSlice(VarNames,0,nIn))));//RightExpr+={|in|};
  if(nOut)
    RightExpr=pn2gsMakeDataApplProd2(OpSubt,RightExpr,pn2gsMakeBagVars(pn2gsMakeIds(ATgetTail(VarNames,nIn))));//RightExpr-={|out|};
      }
      else
  RightExpr=pn2gsMakeBagVars(ATgetTail(VarNames,nIn));//RightExpr={|in|};
    }

    ATermAppl Right=gsMakeParamId(CurrentPlace,ATmakeList1((ATerm)RightExpr));//make P_pi(max(x+i-j,0))
    ATermAppl Summand=gsMakeSeq(Left,Right);

    //condition
    ATermAppl Cond=NULL;
    if(nOut>0){
      Cond=pn2gsMakeDataApplProd2(OpLTE,gsMakeId(ATmakeAppl0(ATmakeAFunInt0(nOut))),IdX);//make nOut<=x
    }
    if(inhib){
      ATermAppl Cond1=pn2gsMakeDataApplProd2(OpEq,IdX,Number0);//make x==0
      if(Cond) Cond=pn2gsMakeDataApplProd2(OpAnd,Cond,Cond1);
      else Cond=Cond1;
    }

    //colored
    if(Type){
      Cond=NULL;
      if(nOut>0){
  Cond=pn2gsMakeDataApplProd2(OpLTE,pn2gsMakeBagVars(pn2gsMakeIds(ATgetTail(VarNames,nIn))),IdX);//subbag
      }
      if(inhib){
  ATermAppl Cond1=pn2gsMakeDataApplProd2(OpEq,IdX,EmptyBag);//make x=={}
  if(Cond) Cond=pn2gsMakeDataApplProd2(OpAnd,Cond,Cond1);
  else Cond=Cond1;
      }
    }

    if(Cond)
      Summand=pn2gsMakeIfThenUntimed(Cond,Summand);

    //colored (sums)
    if(Type){
      ATermList SumList=ATmakeList0();
      for(ATermList l=VarNames; !ATisEmpty(l); l=ATgetNext(l)){
  SumList=ATinsert(SumList,(ATerm)gsMakeDataVarId(ATAgetFirst(l),Type));
      }
      Summand=gsMakeSum(ATreverse(SumList),Summand);
    }

    if(Body) Body=gsMakeChoice(Summand,Body);
    else Body=Summand;

    // colored:
    ATermList LeftType=ATmakeList0();
    if(Type && nIn+nOut>0) {
      // all input and output parameters (not reset and inhibitor)
      for(int i=0; i<nIn+nOut; i++)
  LeftType=ATinsert(LeftType,(ATerm)Type);
    }

    // extra equation
    EquationList = ATinsert(EquationList,
          (ATerm)gsMakeProcEqn(
             gsMakeProcVarId(LeftName, LeftType),
             (Type)?pn2gsMakeDataVarIds(VarNames,Type):ATmakeList0(),
             pn2gsMakeMultiAction(mult,(Type)?pn2gsMakeListOfLists(pn2gsMakeIds(VarNames)):ATmakeList0())));

  } //For loop

    // ++++++++++++++++++ end generation of minimal number of summands


//     // +++++++++++++++++ begin normal alternative generation

//     //calculate the reset multiactions (if any)
//     ATermList ResetActionLists=ATmakeList0();
//     for(int i=1; i<=l; i++)
//       ResetActionLists=ATconcat(ResetActionLists,pn2gsGetActionLists(i,ActsReset));


//     //generate the main process
//     ATermAppl VarX=gsMakeDataVarId(ATmakeAppl0(ATmakeAFunId("x")),static_cast<ATermAppl>(mcrl2::data::sort_nat::nat()));;
//     //ATermAppl Number0=gsMakeId(gsString2ATermAppl("0"));
//     AFun CurrentPlaceARId=ATappendAFun(CurrentPlaceId,"_ar_");
//     AFun CurrentPlaceAIId=ATappendAFun(CurrentPlaceId,"_ai_");
//     AFun CurrentPlaceARRId=ATappendAFun(CurrentPlaceId,"_arr_");
//     AFun CurrentPlaceAIRId=ATappendAFun(CurrentPlaceId,"_air_");
//     ATermAppl OpAdd=gsMakeId(gsMakeOpIdNameAdd());
//     ATermAppl OpSubt=gsMakeId(gsMakeOpIdNameSubt());
//     //ATermAppl OpMax=gsMakeId(gsMakeOpIdNameMax());
//     ATermAppl OpLTE=gsMakeId(gsMakeOpIdNameLTE());
//     ATermAppl OpEq=gsMakeId(gsMakeOpIdNameEq());
//     ATermAppl OpInt2Nat=gsMakeId(gsMakeOpIdNameInt2Nat());
//     ATermAppl Number0=gsMakeId(gsString2ATermAppl("0"));

//     for(int j=m;j>-1;j--){
//       ATermAppl Summand=NULL;
//       AFun NumberJId=ATmakeAFunInt0(j);
//       ATermAppl NumberJ=gsMakeId(ATmakeAppl0(NumberJId)));
//       ATermList OutActionLists=NULL;
//       if(j>0) OutActionLists=pn2gsGetActionLists(j,ActsOut);
//       for(int i=n;i>-1;i--){
// 	if(j==0 && i==0 && l==0) continue;
// 	AFun NumberIId=ATmakeAFunInt0(i);
// 	ATermAppl LeftName=ATmakeAppl0(ATappendAFun(ATappendAFun(ATappendAFun(CurrentPlaceARId,
// 									      ATgetName(NumberIId)),
// 								 "_"),
// 						    ATgetName(NumberJId)));
// 	ATermAppl LeftNameResets=ATmakeAppl0(ATappendAFun(ATappendAFun(ATappendAFun(CurrentPlaceARRId,
// 										    ATgetName(NumberIId)),
// 								       "_"),
// 							  ATgetName(NumberJId)));
// 	ATermAppl Left=gsMakeParamId(LeftName,ATmakeList0());//make name P_pi_ar_i_j
// 	ATermAppl LeftResets=gsMakeParamId(LeftNameResets,ATmakeList0());//make name P_pi_arr_i_j

// 	ATermAppl RightExpr=VarX;  //x;
// 	{
// 	  int d=i-j;
// 	  if(d>0) RightExpr=pn2gsMakeDataApplProd2(OpAdd,RightExpr,gsMakeId(ATmakeAppl0(ATmakeAFunInt0(d))));//RightExpr=RightExpr+d;
// 	  else if(d<0) RightExpr=gsMakeDataAppl(OpInt2Nat,ATmakeList1((ATerm)pn2gsMakeDataApplProd2(OpSubt,RightExpr,gsMakeId(ATmakeAppl0(ATmakeAFunInt0(-d))))));//RightExpr=max(RightExpr-d,0);
// 	}
// 	ATermAppl Right=gsMakeParamId(CurrentPlace,ATmakeList1((ATerm)RightExpr));//make P_pi(max(x+i-j,0))
// 	ATermAppl RightResets=gsMakeParamId(CurrentPlace,ATmakeList1((ATerm)Number0));//make P_pi(0)
// 	ATermAppl Sec=gsMakeSeq(Left,Right);
// 	ATermAppl SecResets=gsMakeSeq(LeftResets,RightResets);
// 	if(i>0 || j>0){
// 	  if(Summand) Summand=gsMakeChoice(Sec,Summand);
// 	  else Summand=Sec;
// 	}

// 	//in case there are resets
// 	if(l>0){
// 	  if(Summand) Summand=gsMakeChoice(SecResets,Summand);
// 	  else Summand=SecResets;
// 	}

// 	ATermList InActionLists=NULL;
// 	if(i>0) InActionLists=pn2gsGetActionLists(i,ActsIn);
// 	//generate the additional process
// 	if(i>0 || j>0)
// 	  EquationList = ATinsert(EquationList,
// 				    (ATerm)gsMakeProcEqn(
// 							 gsMakeProcVarId(LeftName, ATmakeList0()),
// 							 ATmakeList0(),
// 							 pn2gsGenerateP_pi_a(InActionLists,OutActionLists,NULL)));
// 	//in case there are resets
// 	if(l>0)
// 	  EquationList = ATinsert(EquationList,
// 				  (ATerm)gsMakeProcEqn(
// 						       gsMakeProcVarId(LeftNameResets, ATmakeList0()),
// 						       ATmakeList0(),
// 						       pn2gsGenerateP_pi_a(InActionLists,OutActionLists,ResetActionLists)));
//       }

//       if(j>0){ //generate the condition
// 	ATermAppl Cond=pn2gsMakeDataApplProd2(OpLTE,NumberJ,VarX);//make j<=x
// 	Summand=pn2gsMakeIfThenUntimed(Cond,Summand);
//       }

//       if(Body){
// 	if(Summand) Body=gsMakeChoice(Summand,Body);
//       }
//       else Body=Summand;
//     }

//     //add inhibitor arcs
//     if(k>0){
//       ATermAppl Summand=NULL;

//       //Calculate a list of all inhibirot multiactions
//       ATermList InhibitorActionLists=ATmakeList0();
//       for(int i=1; i<=k; i++)
// 	InhibitorActionLists=ATconcat(InhibitorActionLists,pn2gsGetActionLists(i,ActsInhibit));

//       for(int i=n;i>-1;i--){
// 	AFun NumberIId=ATmakeAFunInt0(i);
// 	ATermAppl LeftName=ATmakeAppl0(ATappendAFun(CurrentPlaceAIId,ATgetName(NumberIId)));
// 	ATermAppl LeftNameResets=ATmakeAppl0(ATappendAFun(CurrentPlaceAIRId,ATgetName(NumberIId)));
// 	ATermAppl Left=gsMakeParamId(LeftName,ATmakeList0());//make name P_pi_ai_i
// 	ATermAppl LeftResets=gsMakeParamId(LeftNameResets,ATmakeList0());//make name P_pi_ai_i

// 	ATermAppl RightExpr=VarX;  //x;
// 	if(i>0) RightExpr=pn2gsMakeDataApplProd2(OpAdd,RightExpr,gsMakeId(ATmakeAppl0(ATmakeAFunInt0(i))));//RightExpr=RightExpr+i;

// 	ATermAppl Right=gsMakeParamId(CurrentPlace,ATmakeList1((ATerm)RightExpr));//make P_pi(x+i)
// 	ATermAppl RightResets=gsMakeParamId(CurrentPlace,ATmakeList1((ATerm)Number0));//make P_pi(x+i)
// 	ATermAppl Sec=gsMakeSeq(Left,Right);
// 	ATermAppl SecResets=gsMakeSeq(LeftResets,RightResets);
// 	if(Summand) Summand=gsMakeChoice(Sec,Summand);
// 	else Summand=Sec;
// 	//in case there are resets
// 	if(l>0) Summand=gsMakeChoice(SecResets,Summand);

// 	ATermList InActionLists=NULL;
// 	if(i>0) InActionLists=pn2gsGetActionLists(i,ActsIn);
// 	//generate the additional process
// 	EquationList = ATinsert(EquationList,
// 				(ATerm)gsMakeProcEqn(
// 						     gsMakeProcVarId(LeftName, ATmakeList0()),
// 						     ATmakeList0(),
// 						     pn2gsGenerateP_pi_a(InActionLists,InhibitorActionLists,NULL)));
// 	if(l>0)
// 	  EquationList = ATinsert(EquationList,
// 				  (ATerm)gsMakeProcEqn(
// 						       gsMakeProcVarId(LeftNameResets, ATmakeList0()),
// 						       ATmakeList0(),
// 						       pn2gsGenerateP_pi_a(InActionLists,InhibitorActionLists,ResetActionLists)));
//       }

//       //generate the condition
//       ATermAppl Cond=pn2gsMakeDataApplProd2(OpEq,VarX,Number0);//make j<=x
//       Summand=pn2gsMakeIfThenUntimed(Cond,Summand);

//       if(Body){
// 	if(Summand) Body=gsMakeChoice(Summand,Body);
//       }
//       else Body=Summand;
//     }
//     // +++++++++++++++++ end normal alternative generation



  // ***** Changed by Yarick 29.05.2006 to add error action ****

  // handle the case m+n+k+l=0.
  if(!error){
    if(!Body) Body=gsMakeDelta();
  }
  else {
    ATermAppl ErrorCond=pn2gsMakeDataApplProd2(OpLTE,nMaxTokens,IdX);

    //colored
    //not possible because the cardinality of bags is not built-in
    //if(Type) ErrorCond=pn2gsMakeDataApplProd2(OpLTE,nMaxTokens,pn2gsMakeDataAppl(OpCard(IdX)));

    ATermAppl ExtraSummand=gsMakeIfThenElse(ErrorCond,
              gsMakeSeq(ErrorAction,
            gsMakeParamId(CurrentPlace,ATmakeList1((ATerm)IdX))),
              gsMakeDelta());
    if(Body)
      Body=gsMakeChoice(ExtraSummand,Body);
    else Body=ExtraSummand;
  }
  // ***** end changed by Yarick.


  //make process P_pi and add it
  ATermAppl VarX=gsMakeDataVarId(ATAgetArgument(IdX,0),(Type)?static_cast<ATermAppl>(mcrl2::data::sort_bag::bag(mcrl2::data::sort_expression(Type))):static_cast<ATermAppl>(mcrl2::data::sort_nat::nat()));
  EquationList = ATinsert(EquationList,
        (ATerm)gsMakeProcEqn(
                 gsMakeProcVarId(CurrentPlace,
                     ATmakeList1((ATerm)((Type)?static_cast<ATermAppl>(mcrl2::data::sort_bag::bag(mcrl2::data::sort_expression(Type))):static_cast<ATermAppl>(mcrl2::data::sort_nat::nat())))),
                 ATmakeList1((ATerm)VarX),
                 Body));

  return EquationList;
}

// static ATermList concat_lists(ATermList l, ATermList m){
//   //concats the elements of the lists of lists
//   //no checks
//   ATermList r=ATmakeList0();
//   for(;!ATisEmpty(l);l=ATgetNext(l)){
//     ATermList el=ATLgetFirst(l);
//     for(ATermList k=m;!ATisEmpty(k);k=ATgetNext(k))
//       r=ATinsert(r,(ATerm)ATconcat(el,ATLgetFirst(k)));
//   }
//   return ATreverse(r);
// }

// static ATermAppl pn2gsGenerateP_pi_a(ATermList InActionLists, ATermList OutActionLists, ATermList ResetActionLists){
//   //input: sets of multiactions to be combined
//   //output: a process that is the choice of all multiactions (order not important)

//   ATermList BigList=NULL;
//   ATermList BigList1=NULL;

//   BigList1=InActionLists;
//   if(!BigList) BigList=BigList1;
//   else if(BigList1) BigList=concat_lists(BigList,BigList1);

//   BigList1=OutActionLists;
//   if(!BigList) BigList=BigList1;
//   else if(BigList1) BigList=concat_lists(BigList,BigList1);

//   BigList1=ResetActionLists;
//   if(!BigList) BigList=BigList1;
//   else if(BigList1) BigList=concat_lists(BigList,BigList1);

//   if(!BigList) return gsMakeDelta();

//   ATermAppl Body=NULL;
//   for(;!ATisEmpty(BigList);BigList=ATgetNext(BigList)){
//     ATermAppl Res=pn2gsMakeMultiAction(ATLgetFirst(BigList));
//     if(Body) Body=gsMakeChoice(Res,Body);
//     else Body=Res;
//   }
//   return Body;
// }

static ATermList pn2gsGetActionLists(unsigned int n, ATermList ActList){
    //returns all sublists (not necessarily consecutive) of length n

    //cannot
    if(n>ATgetLength(ActList)) return ATmakeList0();

    //one way
    if(n==ATgetLength(ActList)) return ATmakeList1((ATerm)ActList);

    //the only sublist of length 0 is []
    if(!n) return ATmakeList1((ATerm)ATmakeList0());

    //0<n<len(list); therefore len(list) > 0: 2 possibilities:
    //- take the first element, get the recursive possibilities with n-1, combine.
    ATermAppl FirstAction=ATAgetFirst(ActList);
    ActList=ATgetNext(ActList);

    ATermList RestN1=pn2gsGetActionLists(n-1,ActList);

    //- not take the first element, get the recursive possibilities with n.
    ATermList RestN=pn2gsGetActionLists(n,ActList);

    //add the first Action to all the elements of RestN1
    {
      ATermList NewRestN1=ATmakeList0();
      for(;!ATisEmpty(RestN1);RestN1=ATgetNext(RestN1)){
  ATermList CurAList=ATLgetFirst(RestN1);
  CurAList=ATinsert(CurAList,(ATerm)FirstAction);
  NewRestN1=ATinsert(NewRestN1,(ATerm)CurAList);
      }
      RestN1=ATreverse(NewRestN1);
    }
    return ATconcat(RestN1,RestN);
  }

static ATermAppl pn2gsMakeMultiAction(ATermList ActionList, ATermList ParamList){
  //Make a process term a_1(p_1)|...|a_n(p_n)
  //input : list of action names, parameters
  //if the list of parameters is shorter, empty parameter lists are used.
  ATermAppl Res=NULL;
  for(;!ATisEmpty(ActionList);ActionList=ATgetNext(ActionList)){
    if(ParamList && ATisEmpty(ParamList)) ParamList=NULL;
    ATermAppl CurAct=gsMakeParamId(ATAgetFirst(ActionList),(ParamList)?ATLgetFirst(ParamList):ATmakeList0());
    if(Res) Res=gsMakeSync(CurAct,Res);
    else Res=CurAct;
    if(ParamList)
      ParamList=ATgetNext(ParamList);
  }

  if(!Res) Res=gsMakeTau();

  return Res;
}

  //==================================================
  // pn2gsGenerateTransitionAlternative generates the mCRL2 Process Equations belonging to one transition
  //==================================================
  static ATermList pn2gsGenerateTransitionAlternative(ATerm TransID){
    // input: access to the context; ID of the transtion
    // output: an updated list of the processes: the processes belonging to the transition added to the list

    //Calculate the name of the process
    AFun CurrentTransId = ATgetAFun(TransID);
    {
      char * CurrentTransName = ATgetName(ATgetAFun(ATtableGet(context.trans_name, TransID)));

      if(strcmp(CurrentTransName, "default_name")){
  // name of the trans is not "default_name"
  // name of the trans may be used
  CurrentTransId=ATappendAFun(ATappendAFun(CurrentTransId,"_"),CurrentTransName);
      }
    }

    // variables to store the process names
    ATermAppl CurrentTrans=ATmakeAppl0(ATprependAFun("T_",CurrentTransId));

    // insert the name of the process T_ti into context.transs
    // this is needed for the generation of the general process PetriNet
    context.transitions = ATinsert(context.transitions, (ATerm)CurrentTrans);
    gsDebugMsg("context.trans now contains the following trans: %T\n", context.transitions);

    //==================================================
    // generate the processes
    //==================================================
    ATermList Arcs    = ATmakeList0();
    ATermList Actions = ATmakeList0();
    {
      ATermList MoreActions=ATLtableGet(context.trans_in, TransID);
      if(MoreActions){
  Actions=ATconcat(Actions,MoreActions);
  Arcs=ATconcat(Arcs,MoreActions);
      }

      MoreActions=ATLtableGet(context.trans_out, TransID);
      if(MoreActions) {
  Actions=ATconcat(Actions,pn2gsMakeSendActions(MoreActions));
  Arcs=ATconcat(Arcs,MoreActions);
      }
    }

    //taking care of the colors
    //need to
    //1) add parameters to actions (variables)
    //after adding the monitor action
    //2) add predicate (if any)
    //3) add sums

    //1) add parameters to actions (variables)
    ATermList ParamList=ATmakeList0();
    ATermList SumList=ATmakeList0();
    int n=0;
    for(ATermList l=Arcs;!ATisEmpty(l);l=ATgetNext(l)){
      ATermAppl Arc=ATAgetFirst(l);
      ATermAppl Type=ATAtableGet(context.arc_type_mcrl2,(ATerm)Arc);
      if(!Type || ATisEqual(Type,Appl0)){
  //not colored
  ParamList=ATinsert(ParamList,(ATerm)ATmakeList0());
      }
      else if(ATAtableGet(context.arc_inhibit,(ATerm)Arc) || ATAtableGet(context.arc_reset,(ATerm)Arc)) {
  //inhibitor and reset arcs : no parameters
  ParamList=ATinsert(ParamList,(ATerm)ATmakeList0());
      }
      else {
  ATermAppl VarName=ATAtableGet(context.arc_name,(ATerm)Arc);
  if(!VarName || ATisEqual(VarName,Appl0)){
    //generate a fresh variable
    VarName=ATmakeAppl0(ATappendAFun(ATappendAFun(ATprependAFun("vt_",CurrentTransId),"_"),ATgetName(ATmakeAFunInt0(n))));
    n++;
  }
  ParamList=ATinsert(ParamList,(ATerm)ATmakeList1((ATerm)gsMakeId(VarName)));
  SumList=ATinsert(SumList,(ATerm)gsMakeDataVarId(VarName,Type));
      }
    }
    ParamList=ATreverse(ParamList);
    SumList=ATreverse(SumList);

    //add the monitor action
    Actions=ATinsert(Actions,(ATerm)ATmakeAppl0(ATappendAFun(ATprependAFun("t_",CurrentTransId),"_mon"))); //MonitorAction
    ParamList=ATinsert(ParamList,(ATerm)ATmakeList0());

    //make the multiaction
    ATermAppl Body=pn2gsMakeMultiAction(Actions,ParamList);
    // 2) add predicate (if any)
    ATermAppl Predicate=ATAtableGet(context.trans_predicate,(ATerm)TransID);
    if(!ATisEqual(Predicate,Appl0)){
      Body=pn2gsMakeIfThenUntimed(Predicate,Body);
    }
    // 3) add sums if any
    if(ATgetLength(SumList)){
      Body=gsMakeSum(SumList,Body);
    }

    //gsVerboseMsg("Body: %T\n\n",Body);

    return ATmakeList1((ATerm)gsMakeProcEqn(gsMakeProcVarId(CurrentTrans, ATmakeList0()), ATmakeList0(), Body));
  }

static ATermList pn2gsMakeSendActions(ATermList ReadActions){
  //prepend all of these actions with "_".
  ATermList SendActions=ATmakeList0();
  for(;!ATisEmpty(ReadActions);ReadActions=ATgetNext(ReadActions)){
    ATermAppl Act=ATAgetFirst(ReadActions);
    Act=ATmakeAppl0(ATprependAFun("_",ATgetAFun(Act)));
    SendActions=ATinsert(SendActions,(ATerm)Act);
  }
  return ATreverse(SendActions);
}

static ATermList pn2gsMakeIds(ATermList l){
  ATermList r=ATmakeList0();
  for(;!ATisEmpty(l);l=ATgetNext(l))
    r=ATinsert(r,(ATerm)gsMakeId(ATAgetFirst(l)));
  return ATreverse(r);
}

static ATermAppl pn2gsMakeBagVars(ATermList l){
  //makes a bag from the list of variables.
  if(ATisEmpty(l))
    return EmptyBag;

  ATermList l1 = ATmakeList0();
  for(l=ATreverse(l);!ATisEmpty(l);l=ATgetNext(l)){
    l1=ATinsert(l1,(ATerm)Number1);
    l1=ATinsert(l1,(ATerm)ATAgetFirst(l));
  }

  return gsMakeDataAppl(OpBagEnum, l1);
}

static ATermList pn2gsMakeListOfLists(ATermList l){
  //for [e_1,...,e_n] returns the [[e_1],...,[e_n]]
  ATermList r=ATmakeList0();
  for(;!ATisEmpty(l);l=ATgetNext(l))
    r=ATinsert(r,(ATerm)ATmakeList1(ATgetFirst(l)));
  return ATreverse(r);
}

static ATermList pn2gsMakeDataVarIds(ATermList l, ATermAppl Type){
  //for [e_1,...,e_n],t returns the [DataVarId(e_1,t),...,DataVarId(e_n,t)]
  ATermList r=ATmakeList0();
  for(;!ATisEmpty(l);l=ATgetNext(l))
    r=ATinsert(r,(ATerm)gsMakeDataVarId(ATAgetFirst(l),Type));
  return ATreverse(r);
}
