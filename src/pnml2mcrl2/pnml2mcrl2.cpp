#define NAME "pnml2mcrl2"
#define VERSION "1.1"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <aterm2.h>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "typecheck.h"

  typedef struct {
    ATbool Abort;                 // if an element has no ID, this boolean is used to grant abortion of the conversion

    // read-in
    ATermTable place_name;	// place_id -> name
    ATermTable place_mark;	// place_id -> Nat
    ATermTable trans_name;	// trans_id -> name
    ATermTable arc_in;	        // arc_id -> trans_id x place_id
    ATermTable arc_out;	        // arc_id -> place_id x trans_id
    ATermTable arc_inhibit;	// arc_id -> place_id x trans_id (always this way)
    ATermTable arc_reset;	// arc_id -> trans_id x place_id (always this way)

    // generate
    ATermTable place_in;	        // place_id -> List(arc_id) (arc_in)
    ATermTable trans_in;	        // trans_id -> List(arc_id) (arc_out+arc_inhibit)
    ATermTable place_out;	        // place_id -> List(arc_id) (arc_out)
    ATermTable trans_out;	        // trans_id -> List(arc_id) (arc_in+arc_reset)
    ATermTable place_inhibit;	        // place_inhibit -> List(arc_id) (arc_inhibit)
    ATermTable place_reset;	        // place_reset -> List(arc_id) (arc_reset)
    // not needed as thay are the same as arc_out 
    //ATermTable transition_inhibit;	// transition_inhibit -> List(arc_id) (arc_inhibit)
    // not needed as thay are the same as arc_in 
    //ATermTable transition_reset;	// transition_reset -> List(arc_id) (arc_reset)
    
    // needed for the creation of general mCRL2 processes
    ATermList transitions;      // store all the mCRL2 processes (involving transitions) needed for the Trans-process
    ATermList places;           // store all the mCRL2 processes (involving places) needed for the PetriNet-process
    ATermTable place_process_name; // place_id -> name of the corresponding process
  } Context;
  static Context context;

  static ATbool rec_par=ATtrue;
  static ATermList pn2gsGeneratePlaceAlternative(ATerm PlaceID);
  static ATermList pn2gsGenerateTransitionAlternative(ATerm TransID);
  static ATermList pn2gsListNNats(int n);
  static inline ATermAppl pn2gsMakeDataApplProd2(ATermAppl Op, ATermAppl Left, ATermAppl Right){
    return gsMakeDataApplProd(Op,ATmakeList2((ATerm)Left,(ATerm)Right));
  }

  bool perform_task(char const* InFileName, FILE* OutStream);

  bool perform_task(char const* InFileName, char const* OutFileName) {
    FILE* OutStream = fopen(OutFileName,"wb");

    if (OutStream == 0) {
      gsErrorMsg("cannot open file '%s' for writing\n", OutFileName);
      return false;
    }
    
    bool result = perform_task(InFileName, OutStream);

    fclose(OutStream);

    return (result);
  }

// Squadt protocol interface and utility pseudo-library
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <squadt_utility.h>

class squadt_interactor : public squadt_tool_interface {

  private:

    enum input_files {
      pnml_file_for_input   = 0,  ///< file containing an LTS that can be imported using the LTS library
      mcrl2_file_for_output = 1,  ///< file used to write the output to
    };

  public:

    /** \brief constructor */
    squadt_interactor();

    /** \brief configures tool capabilities */
    void set_capabilities(sip::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(sip::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(sip::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(sip::configuration&);
};

squadt_interactor::squadt_interactor() {
}

void squadt_interactor::set_capabilities(sip::tool::capabilities& c) const {
  c.add_input_combination(pnml_file_for_input, "pnml", "Transformation");
}

void squadt_interactor::user_interactive_configuration(sip::configuration& c) {
  /* set the squadt configuration to be sent back, such
   * that mcrl22lpe can be restarted later with exactly
   * the same parameters
   */
  if (c.is_fresh()) {
    c.add_output(mcrl2_file_for_output, "mcrl2", c.get_output_name(".mcrl2"));
  }
}

bool squadt_interactor::check_configuration(sip::configuration const& c) const {
  bool result = true;

  result |= c.object_exists(pnml_file_for_input);
  result |= c.object_exists(mcrl2_file_for_output);

  return (result);
}

bool squadt_interactor::perform_task(sip::configuration& c) {
  using namespace boost;
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::datatype;
  using namespace sip::layout::elements;

  bool result = true;

  rec_par=ATfalse;

  result = ::perform_task(c.get_object(pnml_file_for_input)->get_location().c_str(),
                          c.get_object(mcrl2_file_for_output)->get_location().c_str());

  return (result);
}

#endif

  //====================================
  //ADDED BY YARICK: AFun extensions.
  //====================================

  //==================================================
  // ATmakeAFunInt functions as ATmakeAFun, except that the name is an int and not a (char *)
  //==================================================
  static AFun ATmakeAFunInt(int name, int arity, ATbool quoted) {
    // input: an integer value (name), it's arity and whether it is quoted or not
    // output: an AFun, as in ATmakeAFun, but now with a name from an integer value

    // on 128 bit architecture int cannot ocupy more than 128/3+1=43 8-ary digits, even less 10-ary
    char buf[50];
    sprintf(buf, "%d", name);
    return ATmakeAFun(buf, arity, quoted);
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
  static inline AFun ATmakeAFunId(char *name){
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
    
    char *String=ATgetName(id);

    // check if the first character is of format [a-zA-Z_]
    if(!(isalpha(String[0])||String[0]=='_')) {
      // first character does not follow the format
      // put 'c_' in front of the String
      id=ATprependAFun("c_",id);
    }

    String=strdup(ATgetName(id));

    for(int i=0; i< (int)strlen(String); i++){
      if(!(isalnum(String[i]))) {
	// if a character in the string is not [a-zA-Z0-9_], replace it by an '_'
	String[i]='_';
      }
    }

    id=ATmakeAFun(String,ATgetArity(id),ATisQuoted(id));
    free(String);
    return id;
  }

  //==================================================
  // pn2gsRetrieveTextWithCheck gets the contents of a child <text> element of cur, and checks it to format [a-zA-Z_][a-zA-Z0-9_]*
  //==================================================
  static ATermAppl pn2gsRetrieveTextWithCheck(xmlNodePtr cur) {
    // input: a pointer to the current element
    // output: the contents of the first child <text> attribute 
    //         of the current element, following format [a-zA-Z_][a-zA-Z0-9_]*

    // this function is used for the retrieval of names and ids
    
    cur=cur->xmlChildrenNode;
    while (cur != NULL) {
      if (!xmlNodeIsText(cur)) {
	if (!xmlStrcmp(cur->name, (const xmlChar *)"text")) {
	  return ATmakeAppl0(pn2gsCheckAFun(ATmakeAFunId((char *)xmlNodeGetContent(cur))));
	  //	  return (ATerm)ATmakeAppl0(ATmakeAFun(RV,0,ATfalse));
	}
      }
      cur=cur->next;
    }
    return NULL;
  }

  //==================================================
  // pn2gsRetrieveText gets the contents of a child <text> element of cur
  //==================================================
  static ATerm pn2gsRetrieveText(xmlNodePtr cur) {
    // input: a pointer to the current element
    // output: the contents of the first child <text> attribute 
    //         of the current element

    // this function is used for the retrieval of types, initial markings, etc.
    
    char * RV;
    cur=cur->xmlChildrenNode;
    while (cur != NULL) {
      if (!xmlNodeIsText(cur)) {
	if (!xmlStrcmp(cur->name, (const xmlChar *)"text")) {
	  RV = (char *)xmlNodeGetContent(cur);
	  return ATparse(RV);
	}
      }
      cur=cur->next;
    }
    return NULL;
  }
  
  //==================================================
  // pn2gsAterm_place converts a pnml-place to a usable ATerm
  //==================================================
  static ATermAppl pn2gsAterm_place(xmlNodePtr cur) {
    // input: a pointer to the current place
    // output: a usable translation of the place,
    //         if the place needs to be translated
    
    gsDebugMsg("> Start examining a place...  \n");
    // first, we want to retrieve the id of the place
    ATermAppl Aid;
    if (!xmlGetProp(cur, (const xmlChar *)"id")) {
      // the place has NO id, so translation should be aborted!
      context.Abort = ATtrue;
      gsErrorMsg("A place has no ID. \n");
      return NULL;
    } else {
      // the place has an id, put it in Aid
      Aid = ATmakeAppl0((pn2gsCheckAFun(ATmakeAFunId((char *)xmlGetProp(cur, (const xmlChar *)"id")))));
    }
    gsDebugMsg("    id: '%T'\n", Aid); 
    
    // second, we want to retrieve the necessary attributes of the place
    cur = cur->xmlChildrenNode;
    // cur now points to the first child of the place  
    
    // temporary variables that contain data of the current place so this data can be returned
    // default values are assigned here
    ATerm Aname = ATparse("default_name");
    ATerm AinitialMarking = ATparse("0");
    ATerm Atype = ATparse("channel");
    
    // this loop goes through all the children of the <place>element
    // these children will be translated or ignored, this depends on the element name
    while (cur != NULL) {
      // current elements that are conceivable for translation are:
      // <name>  -  <initialMarking>  -  <type>
      // all other elements will be ignored in the translation
      
      if (!xmlStrcmp(cur->name, (const xmlChar *)"name")) {
	// the place contains a <name> element
	// a <name> element contains a childelement <text> which contains the name of the place
	// the name is retrieved below and assigned to Aname
	if (!(Aname=(ATerm)pn2gsRetrieveTextWithCheck(cur))) {
	  Aname = ATparse("default_name");
	}
	gsDebugMsg("    name: '%T'\n", Aname);
      } else if (!xmlStrcmp(cur->name, (const xmlChar *)"initialMarking")) {
	// the place contains an <initialMarking> element
	// this element contains a childelement <text> which contains the initial marking of the place
	// this marking is retrieved below and assigned to AinitialMarking
	
	if (!(AinitialMarking=pn2gsRetrieveText(cur))) {
	  AinitialMarking = ATparse("0");
	}
	if (atoi(ATwriteToString(AinitialMarking)) < 0) {
	  // if the initial marking is less than zero, it is resetted to zero
	  AinitialMarking = ATparse("0");
	  
	  gsWarningMsg("Place with id '%T' has initial marking is less than 0, resetting initial marking to 0! \n", Aid);
	}
	gsDebugMsg("    initialMarking: '%T'\n", AinitialMarking);
      } else if (!xmlStrcmp(cur->name, (const xmlChar *)"type")) {
	// the place contains an <type> element
	// this element contains a childelement <text> which contains the type of the place
	
	if (!(Atype=pn2gsRetrieveText(cur))) {
	  Atype = ATparse("channel");
	}
	if (!ATisEqual(Atype, ATparse("channel"))) {
	  // the type should either be omitted or have the value "channel"
	  // otherwise the place does not need to be translated!
	  gsWarningMsg("Place with id '%T' has type '%T' and will not be translated.\n", Aid, Atype);
	  return NULL;
	}
	gsDebugMsg("    type: '%T'\n", Atype);
      } else if (xmlNodeIsText(cur)) {
      } else {
	gsWarningMsg("Ignore an element named '%s'.\n", (const char *)cur->name);
      }
      cur = cur->next;
    }
    
    // argument order of returnvalue is id - name - initialMarking 
    return ATmakeAppl3(ATmakeAFun("place", 3, ATfalse), (ATerm)Aid, Aname, AinitialMarking);
  }
  
  //==================================================
  // pn2gsAterm_trans converts a pnml-transition to a usable ATerm
  //==================================================
  static ATermAppl pn2gsAterm_trans(xmlNodePtr cur) {
    // input: a pointer to the current transition
    // output: a usable translation of the transition,
    //         if the transition needs to be translated
    
    gsDebugMsg("> Start examining a transition...  \n");
    // first, we want to retrieve the id of the transition
    ATermAppl Aid;
    if (!xmlGetProp(cur, (const xmlChar *)"id")) {
      // the transition has NO id, so translation should be aborted!
      context.Abort = ATtrue;
      gsErrorMsg("A transition has no ID. \n");
      return NULL;
    } else {
      // the transition has an id, put it in Aid
      Aid = ATmakeAppl0(pn2gsCheckAFun(ATmakeAFunId((char *)xmlGetProp(cur, (const xmlChar *)"id"))));
    }
    gsDebugMsg("    id: '%T'\n", Aid); 
    
    // second, we want to retrieve the necessary attributes of the transition
    cur = cur->xmlChildrenNode;
    // cur now points to the first child of the transition
    
    // temporary variables that contain data of the current transition so this data can be returned
    // default values are assigned here
    ATerm Aname = ATparse("default_name");
    ATerm Atype = ATparse("AND");
    
    // this loop goes through all the children of the <transition>element
    // these children will be translated or ignored, this depends on the element name
    while (cur != NULL) {
      // current elements that are conceivable for translation are:
      // <name>  -  <type>
      // all other elements will be ignored in the translation
      
      if (!xmlStrcmp(cur->name, (const xmlChar *)"name")) {
	// the transition contains a <name> element
	// a <name> element contains a childelement <text> which contains the name of the transition
	// the name is retrieved below and assigned to Aname
	if (!(Aname=(ATerm)pn2gsRetrieveTextWithCheck(cur))) {
	  Aname = ATparse("default_name");
	}
	gsDebugMsg("    name: '%T'\n", Aname);
      } else if (!xmlStrcmp(cur->name, (const xmlChar *)"type")) {
	// the transition contains an <type> element
	// this element contains a childelement <text> which contains the type of the transition
	if (!(Atype=pn2gsRetrieveText(cur))) {
	  Atype = ATparse("AND");
	}
	if (!ATisEqual(Atype, ATparse("AND"))) {
	  // the type should either be omitted or have the value "AND"
	  // otherwise the place does not need to be translated!
	  gsWarningMsg("Transition with id '%T' has type '%T' and will not be translated.\n", Aid, Atype);
	  return NULL;
	}
	gsDebugMsg("    type: '%T'\n", Atype);
      } else if (xmlNodeIsText(cur)) {
      } else {
	gsWarningMsg("Ignore an element named '%s'.\n", (const char *)cur->name);
      }
      cur = cur->next;
    }
    
    // argument order of returnvalue is id - name  
    return ATmakeAppl2(ATmakeAFun("transition", 2, ATfalse), (ATerm)Aid, Aname);
  }
  
  //==================================================
  // pn2gsAterm_arc converts a pnml-arc to a usable ATerm
  //==================================================
  static ATermAppl pn2gsAterm_arc(xmlNodePtr cur) {
    // input: a pointer to the current arc
    // output: a usable translation of the arc,
    //         if the arc needs to be translated
    
    gsDebugMsg("> Start examining an arc...  \n");
    // first, we want to retrieve the id of the arc
    ATermAppl Aid;
    if (!xmlGetProp(cur, (const xmlChar *)"id")) {
      // the arc has NO id, so translation should be aborted!
      context.Abort = ATtrue;
      gsErrorMsg("An arc has no ID. \n");
      return NULL;
    } else {
      // the arc has an id, put it in Aid
      Aid = ATmakeAppl0((pn2gsCheckAFun(ATmakeAFunId((char *)xmlGetProp(cur, (const xmlChar *)"id")))));
    }
    gsDebugMsg("    id: '%T'\n", Aid); 
    
    // second, we want to retrieve the source and the target of the arc
    ATermAppl Asource;
    if (!xmlGetProp(cur, (const xmlChar *)"source")) {
      // the arc has NO source, so the arc will not be translated!
      gsWarningMsg("Arc with id '%T' has no source and will not be translated.\n", Aid);
      return NULL;
    } else {
      Asource = ATmakeAppl0((pn2gsCheckAFun(ATmakeAFunId((char *)xmlGetProp(cur, (const xmlChar *)"source")))));
    }
    gsDebugMsg("    source: '%T'\n", Asource);
    
    ATermAppl Atarget;
    if (!xmlGetProp(cur, (const xmlChar *)"target")) {
      // the arc has NO target, so the arc will not be translated!
      gsWarningMsg("Arc with id '%T' has no target and will not be translated.\n", Aid);
      return NULL;
    } else {
      Atarget = ATmakeAppl0(pn2gsCheckAFun(ATmakeAFunId((char *)xmlGetProp(cur, (const xmlChar *)"target"))));
    }
    gsDebugMsg("    target: '%T'\n", Atarget);
    
    // third, we want to verify the arc needs translation (thus is of the correct type)
    cur = cur->xmlChildrenNode;
    // cur now points to the first child of the arc
    
    // temporary variables that contain data of the current arctype
    ATerm Atype = ATparse("some_strange`type=that n0b0dy u5e5...");
    
    // this loop goes through all the children of the <arc>element
    // these children will be examined or ignored, this depends on the element name
    while (cur != NULL) {
      // current elements that are conceivable for examination are:
      // <type>
      // all other elements will be ignored in the translation
      
      if (!xmlStrcmp(cur->name, (const xmlChar *)"type")) {
	// the arc contains a <type> element
	// this element contains a childelement <text> which contains the type of the transition
	
	if (!(Atype=pn2gsRetrieveText(cur))) {
	  Atype = ATparse("some_strange`type=that n0b0dy u5e5...");
	}
	if (ATisEqual(Atype, ATparse("inhibitor"))) {
	  // the type should be omitted
	  // otherwise the arc does not need to be translated!
	  // gsWarningMsg("Arc with id '%T' has type '%T' and will not be translated.\n", Aid, Atype);
	  return ATmakeAppl4(ATmakeAFun("arc", 4, ATfalse), (ATerm)Aid, (ATerm)Asource, (ATerm)Atarget, (ATerm)gsString2ATermAppl("inhibitor"));
	}
	else if (ATisEqual(Atype, ATparse("reset"))) {
	  // the type should be omitted
	  // otherwise the arc does not need to be translated!
	  // gsWarningMsg("Arc with id '%T' has type '%T' and will not be translated.\n", Aid, Atype);
	  return ATmakeAppl4(ATmakeAFun("arc", 4, ATfalse), (ATerm)Aid, (ATerm)Asource, (ATerm)Atarget, (ATerm)gsString2ATermAppl("reset"));
	}	
	else if (!ATisEqual(Atype, ATparse("some_strange`type=that n0b0dy u5e5..."))) {
	  // the type should be omitted
	  // otherwise the arc does not need to be translated!
	  gsWarningMsg("Arc with id '%T' has type '%T' and will not be translated.\n", Aid, Atype);
	  return NULL;
	}
	gsDebugMsg("    type: '%T'\n", Atype);
      } else if (xmlNodeIsText(cur)) {
      } else {
	gsWarningMsg("Ignore an element named '%s'.\n", (const char *)cur->name);
      }
      cur = cur->next;
    }
    
    // argument order of returnvalue is id - source - target
    return ATmakeAppl4(ATmakeAFun("arc", 4, ATfalse), (ATerm)Aid, (ATerm)Asource, (ATerm)Atarget, (ATerm)gsString2ATermAppl("default"));
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
  static ATermAppl pn2gsAterm(xmlDocPtr doc) {
    // input: a pointer of the type xmlDocPtr which points to the parsed XML-file
    // output: an ATermAppl, translated from the XML-file,
    //         in which only relevant elements/attributes are concluded
    
    //==================================================
    // initializations and initial checks.
    //==================================================
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (cur == NULL) {
      gsErrorMsg("File is empty. \n");
      return NULL;
    }
    if (xmlStrcmp(cur->name, (const xmlChar *)"pnml")) {
      gsErrorMsg("File is not a PNML file!  \n");
      return NULL;
    }
    // cur now points to the <pnml>element
    
    cur = cur->xmlChildrenNode;
    // the first <net>element, if any present, is selected by cur
    while (cur != NULL && xmlStrcmp(cur->name, (const xmlChar *)"net")) {
      if (!xmlNodeIsText(cur)) {
	gsWarningMsg("Element '%s' is not a Petri net and will be ignored (including it's sub-elements).\n",(const char *)cur->name);
      }
      cur = cur->next;
    }
    if (cur == NULL) {
      gsErrorMsg("File does not contain a Petri net. \n");
      return NULL;
    }   
    
    // cur now points to the first <net>element
    gsDebugMsg("\nStart converting the Petri net to an ATerm...  \n \n");
    
    //==================================================
    // actual translation starts here
    //==================================================
    // retrieve the ID of the Petri net
    ATermAppl ANetID;
    if (!xmlGetProp(cur, (const xmlChar *)"id")) {
      ANetID = ATmakeAppl0(ATmakeAFun("Net_Petri_net", 0, ATtrue));
      gsWarningMsg("NO NET-ID FOUND!\n");
    } else {
      // the net has an id, put it in ANetID
      AFun NetID = pn2gsCheckAFun(ATmakeAFunId((char *)xmlGetProp(cur, (const xmlChar *)"id")));
      NetID = ATprependAFun("Net_",NetID);
      ANetID = ATmakeAppl0(NetID);
    }
    gsDebugMsg("NetID = '%T'\n",ANetID);
    
    cur = cur->xmlChildrenNode;
    // cur now points to the first element in the Petri net
    
    // lists of the places, transitions and arcs that will be translated
    ATermList APlaces=ATmakeList0();
    ATermList ATransitions=ATmakeList0();
    ATermList AArcs=ATmakeList0();
    
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
    while (cur != NULL) {
      // current elements that are conceivable for translation are:
      // <place>  -  <transition>  -  <arc>
      // all other elements will be ignored in the translation
      
      if (!xmlStrcmp(cur->name, (const xmlChar *)"place")) {
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
      } else if (!xmlStrcmp(cur->name, (const xmlChar *)"transition")) {
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
      } else if (!xmlStrcmp(cur->name, (const xmlChar *)"arc")) {
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
      } else if (xmlNodeIsText(cur)) {
      } else {
	gsWarningMsg("An element named '%s' will be ignored in the translation (including it's sub-elements).\n",(const char *)cur->name);
      };
      cur = cur->next;
    };
    
    gsDebugMsg("\nConversion of PNML to ATerm succesfully completed. \n");
    
    // argument order of returnvalue is places - transitions - arcs - NetID
    return ATmakeAppl4(ATmakeAFun("PetriNet", 4, ATtrue), (ATerm)ATreverse(APlaces), (ATerm)ATreverse(ATransitions), (ATerm)ATreverse(AArcs), (ATerm)ANetID);
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

    if (strcmp(CurrentNameString, "default_name")){
      // name of the transition is not "default_name" 
      // name of the transition may be used 
      ResAFun=ATappendAFun(ATappendAFun(ResAFun,"_"),CurrentNameString);
    }

    return ATmakeAppl0(ATappendAFun(ResAFun,"_mon"));
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
    while (ATisEmpty(Ids) == ATfalse) {
      // make the action: arcID
      CurrentAction = ATmakeAppl0(ATgetAFun(ATgetFirst(Ids)));
      ActionsList = ATinsert(ActionsList, (ATerm)gsMakeActId(CurrentAction, ATmakeList0()));
      gsDebugMsg("Action: %T created.\n", CurrentAction);
      // make the action: _arcID
      CurrentAction = ATmakeAppl0(ATprependAFun("_", ATgetAFun(ATgetFirst(Ids))));
      ActionsList = ATinsert(ActionsList, (ATerm)gsMakeActId(CurrentAction, ATmakeList0()));
      gsDebugMsg("Action: %T created.\n", CurrentAction);
      // make the action: __arcID
      CurrentAction = ATmakeAppl0(ATprependAFun("__", ATgetAFun(ATgetFirst(Ids))));
      ActionsList = ATinsert(ActionsList, (ATerm)gsMakeActId(CurrentAction, ATmakeList0()));
      gsDebugMsg("Action: %T created.\n", CurrentAction);
      
      Ids = ATgetNext(Ids);
    }
    
    //==================================================
    // create actions from the transitions
    //==================================================
    // All transitions have a name. If no name is defined in PNML, it is "default_name"
    Ids = ATtableKeys(context.trans_name);
    while (ATisEmpty(Ids) == ATfalse) {
      
      CurrentAction = pn2gsGenerateAction_trans(ATgetAFun(ATgetFirst(Ids)), ATgetAFun(ATtableGet(context.trans_name, ATgetFirst(Ids))));
      ActionsList = ATinsert(ActionsList, (ATerm)gsMakeActId(CurrentAction, ATmakeList0()));
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
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(CurrentTrans, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTrans);
      
    } else if (ATtableGet(context.trans_in, TransID) && !(ATtableGet(context.trans_out, TransID))) {
      // Transition has incoming arcs but no outgoing arcs
      gsDebugMsg("In and no out for trans: %T\n", (ATerm)CurrentTrans);
      // create T_ti
      Process = gsMakeSync(gsMakeParamId(CurrentTransIn, ATmakeList0()), gsMakeParamId(MonitorAction, ATmakeList0()));
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(CurrentTrans, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTrans);
      // create T_ti_in
      Process = pn2gsSyncIn((ATermList)ATtableGet(context.trans_in, TransID));
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(CurrentTransIn, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTransIn);
      
    } else if (!(ATtableGet(context.trans_in, TransID)) && ATtableGet(context.trans_out, TransID)) {
      // Transition has outgoing arcs but no incoming arcs
      gsDebugMsg("Out and no in for trans: %T\n", (ATerm)CurrentTrans);
      // create T_ti
      Process = gsMakeSync(gsMakeParamId(CurrentTransOut, ATmakeList0()), gsMakeParamId(MonitorAction, ATmakeList0()));
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(CurrentTrans, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTrans);
      // create T_ti_out
      Process = pn2gsSyncOut((ATermList)ATtableGet(context.trans_out, TransID));
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(CurrentTransOut, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTransOut);
      
    } else if (ATtableGet(context.trans_in, TransID) && ATtableGet(context.trans_out, TransID)) {
      // Transition has incoming arcs and outgoing arcs
      gsDebugMsg("In and out for trans: %T\n", (ATerm)CurrentTrans);
      // create T_ti
      Process = gsMakeSync(gsMakeSync(gsMakeParamId(CurrentTransIn, ATmakeList0()), gsMakeParamId(CurrentTransOut, ATmakeList0())), gsMakeParamId(MonitorAction, ATmakeList0()));
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(CurrentTrans, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTrans);
      // create T_ti_in
      Process = pn2gsSyncIn((ATermList)ATtableGet(context.trans_in, TransID));
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(CurrentTransIn, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentTransIn);
      // create T_ti_out
      Process = pn2gsSyncOut((ATermList)ATtableGet(context.trans_out, TransID));
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(CurrentTransOut, ATmakeList0()), ATmakeList0(), Process));
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
  // pn2gsPlaceParameterNat generates the parameter for a given place, of type Nat
  //==================================================
  static ATermAppl pn2gsPlaceParameterNat(char * PlaceName) {
    // input: the name of a place
    // output: the parameter of that place, or type Nat
    //         named: pm_PlaceName

    AFun CurrentParameter = ATprependAFun("pm_", ATmakeAFun(PlaceName, 0, ATtrue));
    return gsMakeDataVarId(ATmakeAppl0(CurrentParameter), gsMakeSortIdNat());
  }

  //==================================================
  // pn2gsPlaceParameterPos generates the parameter for a given place, of type Pos
  //==================================================
  static ATermAppl pn2gsPlaceParameterPos(char * PlaceName) {
    // input: the name of a place
    // output: the parameter of that place, or type Pos
    //         named: pm_PlaceName

    AFun CurrentParameter = ATprependAFun("pm_", ATmakeAFun(PlaceName, 0, ATtrue));
    return gsMakeDataVarId(ATmakeAppl0(CurrentParameter), gsMakeSortIdPos());
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

    // insert the name of the process P_pi into context.places
    // this is needed for the generation of the general process PetriNet
    context.places = ATinsert(context.places, (ATerm)CurrentPlace);

    //added by Yarick: we need a table to relate PlaceId and CurrentPlace.
    ATtablePut(context.place_process_name,PlaceID,(ATerm)CurrentPlace);

    gsDebugMsg("context.places now contains the following places: %T\n", context.places);

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
    MaxConcIn = gsMakeNumber(ATmakeAppl0(ATmakeAFunInt0(MaxConcIn_int)), gsMakeUnknown());
    if (gsIsNumber(MaxConcIn)) {
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
    MaxConcOut = gsMakeNumber(ATmakeAppl0(ATmakeAFunInt0(MaxConcOut_int)), gsMakeUnknown());
    if (gsIsNumber(MaxConcIn)) {
      gsDebugMsg("Parameter %T is a Number\n", MaxConcOut);
    } else {
      gsDebugMsg("Parameter %T is not a Number\n", MaxConcOut);
    }
    gsDebugMsg("Place %T has maximum concurrency in: '%d' and out: '%d'\n", PlaceID, MaxConcIn_int, MaxConcOut_int);
    gsDebugMsg("Place %T has maximum concurrency in: '%T' and out: '%T'\n", PlaceID, MaxConcIn, MaxConcOut);

    //==================================================
    // generate the processes
    //==================================================
 
    ATermAppl OpAdd=gsMakeId(gsMakeOpIdNameAdd());
    ATermAppl OpSubt=gsMakeId(gsMakeOpIdNameSubt());
    ATermAppl OpMax=gsMakeId(gsMakeOpIdNameMax());
    ATermAppl OpMin=gsMakeId(gsMakeOpIdNameMin());
    ATermAppl OpGT=gsMakeId(gsMakeOpIdNameGT());
    ATermAppl OpLTE=gsMakeId(gsMakeOpIdNameLTE());
    ATermAppl OpAnd=gsMakeId(gsMakeOpIdNameAnd());
    ATermAppl Number0=gsMakeNumber(gsString2ATermAppl("0"),gsMakeSortIdNat());
    ATermAppl Number1=gsMakeNumber(gsString2ATermAppl("1"),gsMakeSortIdPos());
    
    {
      /* Creation of P_pi */
      ATermAppl ProcVar;
      ATermAppl Process;
      ATermAppl SubProcess0;
      ATermAppl CondIf0;
      ATermAppl CondThan0;
      ATermAppl SubProcess1;
      ATermAppl CondIf1;
      ATermAppl CondThan1;
      ATermAppl SubProcess2;
      ATermAppl CondIf2;
      ATermAppl CondThan2;
      ATermAppl SumVar0 = gsMakeDataVarId(ATmakeAppl0(ATmakeAFun("y", 0, ATtrue)), gsMakeSortIdPos());
      ATermAppl SumVar1 = gsMakeDataVarId(ATmakeAppl0(ATmakeAFun("z", 0, ATtrue)), gsMakeSortIdPos());
      ATermList SumVars;
      gsDebugMsg("Parameter %T is %d a DataVarId\n", SumVar0, gsIsDataVarId(SumVar0));
      gsDebugMsg("Parameter %T is %d a DataVarId\n", SumVar1, gsIsDataVarId(SumVar1));

      // create P_pi parameter
      ProcVar = pn2gsPlaceParameterNat(gsATermAppl2String(CurrentPlace));
      gsDebugMsg("Parameter %T is %d a DataVarId\n", ProcVar, gsIsDataVarId(ProcVar));

      // create first sum-sub-process
      SumVars = ATmakeList1((ATerm)SumVar0);
      CondIf0 = pn2gsMakeDataApplProd2(OpLTE,SumVar0,MaxConcIn);
      CondThan0 = gsMakeSeq(gsMakeParamId(CurrentPlaceAdd, ATmakeList1((ATerm)SumVar0)), gsMakeParamId(CurrentPlace, ATmakeList1((ATerm)pn2gsMakeDataApplProd2(OpAdd,SumVar0,ProcVar))));
      SubProcess0 = gsMakeSum(SumVars, gsMakeCond(CondIf0, CondThan0, gsMakeDelta()));
      gsDebugMsg("Parameter %T is %d a Sum\n", SubProcess0, gsIsSum(SubProcess0));

      // create second sum-sub-process
      SumVars = ATmakeList1((ATerm)SumVar1);
      CondIf1 = pn2gsMakeDataApplProd2(OpLTE,SumVar1, pn2gsMakeDataApplProd2(OpMin, MaxConcOut, ProcVar));
      CondThan1 = gsMakeSeq(gsMakeParamId(CurrentPlaceRem, ATmakeList1((ATerm)SumVar1)), gsMakeParamId(CurrentPlace, ATmakeList1((ATerm)pn2gsMakeDataApplProd2(OpMax,Number0, pn2gsMakeDataApplProd2(OpSubt,ProcVar,SumVar1)))));
      SubProcess1 = gsMakeSum(SumVars, gsMakeCond(CondIf1, CondThan1, gsMakeDelta()));
      gsDebugMsg("Parameter %T is %d a Sum\n", SubProcess1, gsIsSum(SubProcess1));

      // create third sum-sub-process
      SumVars = ATmakeList2((ATerm)SumVar0, (ATerm)SumVar1);
      CondIf2 = pn2gsMakeDataApplProd2(OpAnd, CondIf0, CondIf1);
      CondThan2 = gsMakeSeq(gsMakeSync(gsMakeParamId(CurrentPlaceAdd, ATmakeList1((ATerm)SumVar0)), gsMakeParamId(CurrentPlaceRem, ATmakeList1((ATerm)SumVar1))), gsMakeParamId(CurrentPlace, ATmakeList1((ATerm)pn2gsMakeDataApplProd2(OpMax,Number0,pn2gsMakeDataApplProd2(OpSubt,pn2gsMakeDataApplProd2(OpAdd,SumVar0,ProcVar), SumVar1)))));
      SubProcess2 = gsMakeSum(SumVars, gsMakeCond(CondIf2, CondThan2, gsMakeDelta()));
      gsDebugMsg("Parameter %T is %d a Sum\n", SubProcess2, gsIsSum(SubProcess2));

      // create P_pi
      Process = gsMakeChoice(gsMakeChoice(SubProcess0, SubProcess1), SubProcess2);
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(CurrentPlace, ATmakeList1((ATerm)gsMakeSortIdNat())), ATmakeList1((ATerm)ProcVar), Process));
      gsDebugMsg("Process: %T created.\n", CurrentPlace);
    }

    {
      /* Creation of P_pi_add */
      ATermAppl ProcVar = pn2gsPlaceParameterPos(gsATermAppl2String(CurrentPlaceAdd));
      ATermAppl CondIf = pn2gsMakeDataApplProd2(OpGT,ProcVar, Number1);
      ATermAppl CondThan = gsMakeSync(gsMakeParamId(CurrentPlaceIn, ATmakeList0()), gsMakeParamId(CurrentPlaceAdd, ATmakeList1((ATerm)pn2gsMakeDataApplProd2(OpMax,Number1, pn2gsMakeDataApplProd2(OpSubt,ProcVar, Number1)))));
      ATermAppl CondElse = gsMakeParamId(CurrentPlaceIn, ATmakeList0());
      ATermAppl Process = gsMakeCond(CondIf, CondThan, CondElse);
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(CurrentPlaceAdd, ATmakeList1((ATerm)gsMakeSortIdPos())), ATmakeList1((ATerm)ProcVar), Process));
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
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(CurrentPlaceIn, ATmakeList0()), ATmakeList0(), Process));
      gsDebugMsg("Process: %T created.\n", CurrentPlaceIn);
    }

    {
      /* Creation of P_pi_rem */
      ATermAppl ProcVar = pn2gsPlaceParameterPos(gsATermAppl2String(CurrentPlaceRem));
      ATermAppl CondIf = pn2gsMakeDataApplProd2(OpGT,ProcVar, Number1);
      ATermAppl CondThan = gsMakeSync(gsMakeParamId(CurrentPlaceOut, ATmakeList0()), gsMakeParamId(CurrentPlaceRem, ATmakeList1((ATerm)pn2gsMakeDataApplProd2(OpMax, Number1, pn2gsMakeDataApplProd2(OpSubt,ProcVar, Number1)))));
      ATermAppl CondElse = gsMakeParamId(CurrentPlaceOut, ATmakeList0());
      ATermAppl Process = gsMakeCond(CondIf, CondThan, CondElse);
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(CurrentPlaceRem, ATmakeList1((ATerm)gsMakeSortIdPos())), ATmakeList1((ATerm)ProcVar), Process));
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
      EquationList = ATinsert(EquationList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(CurrentPlaceOut, ATmakeList0()), ATmakeList0(), Process));
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
    return ReturnList;
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
    return ReturnList;
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
    return ReturnList;
  }

  //==================================================
  // pn2gsPlacesParameters generates a list of all the parameters of the places
  //==================================================
  static ATermList pn2gsPlacesParameters(ATermList Places) {
    // input: a list containg all the places
    // output: a list in which all the elements of the inputed list
    //         are preceded by "pm_"

    ATermList ReturnList = ATmakeList0();
    while (ATisEmpty(Places) == ATfalse) {
      ReturnList = ATinsert(ReturnList, (ATerm)pn2gsPlaceParameterNat(ATgetName(ATgetAFun(ATgetFirst(Places)))));
      Places = ATgetNext(Places);
    }
    return ReturnList;
  }

  //==================================================
  // pn2gsMerge creates the mCRL2 merge of all elements of the list, with a parameter!
  //==================================================
  static ATermAppl pn2gsMerge(ATermList Ids) {
    // input: a not-empty list with the ID's
    // output: an AtermAppl that is the MERGE of all the ID's in the input-list, each with its parameter!

    ATermAppl merge;
    char * CurrentElement = ATgetName(ATgetAFun(ATgetFirst(Ids)));
    if (ATgetLength(Ids) > 1) {
      merge = gsMakeMerge(gsMakeParamId(ATmakeAppl0(ATmakeAFun(CurrentElement, 0, ATtrue)), ATmakeList1((ATerm)pn2gsPlaceParameterNat(CurrentElement))), pn2gsMerge(ATgetNext(Ids)));
    } else {
      // ATgetLength(Ids) == 1, since the input-list is not-empty!
      merge = gsMakeParamId(ATmakeAppl0(ATmakeAFun(CurrentElement, 0, ATtrue)), ATmakeList1((ATerm)pn2gsPlaceParameterNat(CurrentElement)));
    }
    return merge;
  }

  //==================================================
  // pn2gsInitialMarkings creates a list with all the initial markings
  //==================================================
  static ATermList pn2gsInitialMarkings(ATermList Places, ATermTable Markings) {
    // input: a list with the places and a table with the markings
    // output: a list which contains all the initial markings of the places

    ATermList ReturnList = ATmakeList0();
    while (ATisEmpty(Places) == ATfalse) {
      ATerm MarkingUnQuoted = ATtableGet(Markings, ATgetFirst(Places));
      gsDebugMsg("Initial Marking unquoted = %T\n", MarkingUnQuoted);
      ATermAppl MarkingQuoted = ATmakeAppl0(ATmakeAFun(ATwriteToString(MarkingUnQuoted), 0, ATtrue));
      gsDebugMsg("Initial Marking quoted= %T\n", MarkingQuoted);
      ReturnList = ATinsert(ReturnList, (ATerm)gsMakeNumber(MarkingQuoted, gsMakeUnknown()));
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
      ProcessList = ATinsert(ProcessList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(gsString2ATermAppl("Trans"), ATmakeList0()), ATmakeList0(), gsMakeDelta()));
    } else {
      // there are transitions
      ProcessList = ATinsert(ProcessList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(gsString2ATermAppl("Trans"), ATmakeList0()), ATmakeList0(), gsMakeSeq(pn2gsChoiceIn(context.transitions), gsMakeParamId(gsString2ATermAppl("Trans"), ATmakeList0()))));
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
							     pn2gsMerge(context.places) 
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
	Process=gsMakeMerge(Process,gsMakeParamId(Place,ATmakeList1((ATerm)pn2gsPlaceParameterNat(ATgetName(ATgetAFun(Place))))));

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

	Process=gsMakeHide(pn2gsHideList(AssocArcs), 
			   gsMakeBlock(pn2gsBlockList(AssocArcs), 
					  gsMakeComm(pn2gsCommList(AssocArcs), 
						     Process)));
      }
    }

    ATermList ParameterList = pn2gsPlacesParameters(context.places);
    ProcessList = ATinsert(ProcessList, (ATerm)gsMakeProcEqn(ATmakeList0(),gsMakeProcVarId(gsString2ATermAppl("PetriNet"),pn2gsListNNats(ATgetLength(ParameterList))), ParameterList, Process));
    gsDebugMsg("Process PetriNet created.\n");

    // reminder: NetID == "Net_'ID of the Petri net'"
    // the Net_ is preceded to the ID of the Petri net to prevent that two processes would have the same name!
    //
    // NetID = PetriNet("...")
    // "..." is the initial marking of all places

    Process = gsMakeParamId(gsString2ATermAppl("PetriNet"), pn2gsInitialMarkings(ATtableKeys(context.place_mark), context.place_mark));
    ProcessList = ATinsert(ProcessList, (ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(NetID, ATmakeList0()), ATmakeList0(), Process));
    gsDebugMsg("Process %T created.\n", NetID);

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
	
	// insert the data into context.place_mark
	// key = id
	// value = initialMarking
	ATtablePut(context.place_mark, CurrentKey , ATgetArgument(ATgetFirst(APlaces), 2));
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
	  if(ATisEqual(CurrentType,gsString2ATermAppl("default"))){
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
	  if(ATisEqual(CurrentType,gsString2ATermAppl("default"))){
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
      }
      Arcs = ATgetNext(Arcs);
    }
    Arcs = ATtableKeys(context.place_reset);
    gsDebugMsg("context.place_reset contains the following keys: %T\n", Arcs);
    while (ATisEmpty(Arcs) == ATfalse) {
      gsDebugMsg("Place '%T' has the following outgoing arcs: %T\n", ATgetFirst(Arcs), ATtableGet(context.place_reset, ATgetFirst(Arcs)));
      Arcs = ATgetNext(Arcs);
    }
    
    gsDebugMsg("\n====================\n\n");
    
    //==================================================
    // creation of mCRL2 ATerms
    //==================================================

    ATermList Actions=pn2gsGenerateActions();       // store all the mCRL2 Actions
    ATermList ProcEqns=pn2gsGenerateProcEqns(ATAgetArgument(Spec, 3));      // store all the mCRL2 Process Equations
    
    gsDebugMsg("\n\n====================\n\n");
    gsDebugMsg("Conversion Succesful!");
    gsDebugMsg("\n\n====================\n\n");

    return gsMakeSpecV1(gsMakeSortSpec(ATmakeList0()), gsMakeConsSpec(ATmakeList0()), gsMakeMapSpec(ATmakeList0()), gsMakeDataEqnSpec(ATmakeList0()), gsMakeActSpec(Actions), gsMakeProcEqnSpec(ProcEqns), gsMakeInit(ATmakeList0(),gsMakeParamId(ATAgetArgument(Spec, 3), ATmakeList0())));
  }

  /*               */
  /*               */
  /*               */
  /* Main function */
  /*               */
  /*               */
  /*               */


  //==================================================
  // PrintHelp prints and contains the Help-text.
  //==================================================
  void PrintHelp(char *Name){
    fprintf(stderr,
	    "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
	    "Convert a Petri net in INFILE to an mCRL2 specification, and write it to\n"
	    "OUTFILE. If INFILE is not present, stdin is used. If OUTFILE is not present,\n"
	    "stdout is used. INFILE is supposed to conform to the EPNML 1.1 standard.\n"
	    "\n"
	    "Only classical Petri nets are translated, i.e. places, transitions and arcs.\n"
	    "Other constructs such as timing, coloring, inhibitor arcs and hierarchy are\n"
	    "not taken into account.\n"
	    "With the -p option turned on, more functionality is supported.\n"
	    "\n"
	    "  -h, --help            display help information\n"
	    "  -q, --quiet           do not display warning messages\n"
	    "  -d, --debug           turn on the display of detailed intermediate messages\n"
	    "  -p, --no_rec_par      generate places in which the result is non-recursive\n"
	    "                        with this flag turned on, inhibitor and reset arcs\n"
	    "                        are translated!\n"
	    "  -v, --verbose         turn on the display of short intermediate messages\n"
	    "      --version         display version information\n",
	    Name);
  }

  //==================================================
  // PrintHelp performs actual conversion by calling more specialised functions
  //==================================================
  bool perform_task(char const* InFileName, FILE* OutStream) {
    xmlDocPtr doc = xmlParseFile(InFileName);
   
    if(!doc) {
      gsErrorMsg("Document not parsed succesfully. \n");
      return false;
    }
   
    gsEnableConstructorFunctions();
    
    ATermAppl Spec=pn2gsAterm(doc);
    xmlFreeDoc(doc);
    
    if(!Spec){	
      gsErrorMsg("Error while converting PNML to ATerm, conversion stopped!  \n");
      return false;
    }
    
    context.place_name=ATtableCreate(63,50);
    context.place_mark=ATtableCreate(63,50); 
    context.trans_name=ATtableCreate(63,50); 
    context.arc_in=ATtableCreate(63,50);  
    context.arc_out=ATtableCreate(63,50);    
    context.arc_inhibit=ATtableCreate(63,50);    
    context.arc_reset=ATtableCreate(63,50);    
    
    context.place_in=ATtableCreate(63,50);   
    context.trans_in=ATtableCreate(63,50);   
    context.place_out=ATtableCreate(63,50);  
    context.trans_out=ATtableCreate(63,50);  
    context.place_inhibit=ATtableCreate(63,50);   
    context.place_reset=ATtableCreate(63,50);   
   
    context.transitions=ATmakeList0();
    context.places=ATmakeList0();
    context.place_process_name=ATtableCreate(63,50);  
    
    Spec=pn2gsTranslate(Spec);
    
    ATtableDestroy(context.place_name);
    ATtableDestroy(context.place_mark); 
    ATtableDestroy(context.trans_name); 
    ATtableDestroy(context.arc_in);  
    ATtableDestroy(context.arc_out);    
    ATtableDestroy(context.arc_inhibit);    
    ATtableDestroy(context.arc_reset);    
    
    ATtableDestroy(context.place_in);   
    ATtableDestroy(context.trans_in);   
    ATtableDestroy(context.place_out);  
    ATtableDestroy(context.trans_out);
    ATtableDestroy(context.place_inhibit);   
    ATtableDestroy(context.place_reset);   
    ATtableDestroy(context.place_process_name);
    
    if(!Spec) {
      gsErrorMsg("Error while converting PNML ATerm to mCRL2 ATerm, conversion stopped!  \n");
      return false;
    }
    
    gsDebugMsg("The result of conversion is: %T\n",Spec);
   
    Spec = type_check_spec(Spec);
   
    if(Spec){
      PrintPart_C(OutStream, (ATerm) Spec, ppDefault);
    }

    return (true);
  }

  //==================================================
  // main
  //==================================================
  int main(int argc, char **argv){
    ATerm stackbot;
    ATinit(0,NULL,&stackbot);
    
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (!squadt_tool_interface::free_activation< squadt_interactor >(argc, argv)) {
#endif

      #define sopts "adhpqv"
      struct option lopts[] = {
        {"read-aterm"  , no_argument,      NULL, 'a'},
        {"debug"       , no_argument,      NULL, 'd'},
        {"help"        , no_argument,      NULL, 'h'},
        {"no_rec_par"  , no_argument,      NULL, 'p'},
        {"quiet"       , no_argument,      NULL, 'q'},
        {"verbose"     , no_argument,      NULL, 'v'},
        {"version"     , no_argument,      NULL, 0},
        {0, 0, 0, 0}
      };
      int opt;
      
      while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 ){
        switch ( opt ){
        case 'd': /* debug */
          gsSetDebugMsg();
          break;
        case 'h': /* help */
          PrintHelp(argv[0]);
          return 0;
        case 'p': /* no_rec_par */
          rec_par=ATfalse;
          break;
        case 'q': /* quiet */
          gsSetQuietMsg();
          break;
        case 'v': /* verbose */
          gsSetVerboseMsg();
          break;
        case 0: /* version */
          fprintf(stderr, "%s %s (revision %d)\n", NAME, VERSION, REVISION);
          return 0;
        default:
          break;
        }
      }
      
      char *InFileName;
      if ( argc-optind < 1 ){
        InFileName = "-";
      } else {
        if ( (InFileName = argv[optind]) == NULL ){ 
          perror(NAME);
          return 1;
        }
      }

      if ( optind+1 < argc ) {
        return (perform_task(InFileName, argv[optind+1]));
      }
      else {
        return (perform_task(InFileName, stdout));
      }
#ifdef ENABLE_SQUADT_CONNECTIVITY
    }
#endif

    return 0;
  }

  // Added by Yarick: alternative generation of Places:
//  static ATermAppl pn2gsGenerateP_pi_a(ATermList InActionLists, ATermList OutActionLists, ATermList ResetActionLists);
  static ATermList pn2gsGetActionLists(int n, ATermList ActList);
  static ATermAppl pn2gsMakeMultiAction(ATermList ActionList);
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
    
    // insert the name of the process P_pi into context.places
    // this is needed for the generation of the general process PetriNet
    context.places = ATinsert(context.places, (ATerm)CurrentPlace);
    gsDebugMsg("context.places now contains the following places: %T\n", context.places);

    //added by Yarick: we need a table to relate PlaceId and CurrentPlace.
    ATtablePut(context.place_process_name,PlaceID,(ATerm)CurrentPlace);
    
    ATermList EquationList=ATmakeList0(); //the result
    ATermAppl Body=NULL;                  //the body of the main equation

    // ++++++++++++++++++ begin generation of minimal number of summands
    // This is yet another alternative method. We try to only generate 
    // the full multiactions between a place and a transition.

    // foreach transition t : find all arks between p and t
    // calculate its value n input arcs - m out arcs, take into account the inhibitor and reset arcs.
    ATermAppl VarX=gsMakeDataVarId(ATmakeAppl0(ATmakeAFunId("x")),gsMakeSortIdNat());;
    ATermAppl Number0=gsMakeNumber(gsString2ATermAppl("0"),gsMakeSortIdNat());
    ATermAppl OpAdd=gsMakeId(gsMakeOpIdNameAdd());
    ATermAppl OpSubt=gsMakeId(gsMakeOpIdNameSubt());
    //ATermAppl OpMax=gsMakeId(gsMakeOpIdNameMax());
    ATermAppl OpLTE=gsMakeId(gsMakeOpIdNameLTE());
    ATermAppl OpEq=gsMakeId(gsMakeOpIdNameEq());
    ATermAppl OpInt2Nat=gsMakeId(gsMakeOpIdNameInt2Nat());
    for(ATermList Lt=ATtableKeys(context.trans_name);!ATisEmpty(Lt);Lt=ATgetNext(Lt)){
      ATermAppl TransID=ATAgetFirst(Lt);
      
      //find all arcs connecting PlaceId and TransId
      ATermList mult_i=ATmakeList0(); //current multiactions
      ATermList mult_o=ATmakeList0(); //current multiactions
      for(ATermList La=ATLtableGet(context.place_in,(ATerm)PlaceID);La && !ATisEmpty(La);La=ATgetNext(La)){
	ATermAppl ArcID=ATAgetFirst(La);
	ATermAppl Arc=ATAtableGet(context.arc_in,(ATerm)ArcID);
	if(!ATisEqual(ATAgetArgument(Arc,1),PlaceID)) continue;
	if(!ATisEqual(ATAgetArgument(Arc,0),TransID)) continue;
	mult_i=ATinsert(mult_i,(ATerm)ArcID);
      }
      int nIn=ATgetLength(mult_i); //the number of tokens that adds/removes
      
      for(ATermList La=ATLtableGet(context.place_out,(ATerm)PlaceID);La && !ATisEmpty(La);La=ATgetNext(La)){
	ATermAppl ArcID=ATAgetFirst(La);
	ATermAppl Arc=ATAtableGet(context.arc_out,(ATerm)ArcID);
	if(!ATisEqual(ATAgetArgument(Arc,0),PlaceID)) continue;
	if(!ATisEqual(ATAgetArgument(Arc,1),TransID)) continue;
	mult_o=ATinsert(mult_o,(ATerm)ArcID);
      }
      int nOut=ATgetLength(mult_o);
      
      bool inhib=false;
      for(ATermList La=ATLtableGet(context.place_inhibit,(ATerm)PlaceID);La && !ATisEmpty(La);La=ATgetNext(La)){
	ATermAppl ArcID=ATAgetFirst(La);
	ATermAppl Arc=ATAtableGet(context.arc_inhibit,(ATerm)ArcID);
	if(!ATisEqual(ATAgetArgument(Arc,0),PlaceID)) continue;
	if(!ATisEqual(ATAgetArgument(Arc,1),TransID)) continue;
	mult_o=ATinsert(mult_o,(ATerm)ArcID);
	inhib=true;
      }
      
      bool reset=false;
      for(ATermList La=ATLtableGet(context.place_reset,(ATerm)PlaceID);La && !ATisEmpty(La);La=ATgetNext(La)){
	ATermAppl ArcID=ATAgetFirst(La);
	ATermAppl Arc=ATAtableGet(context.arc_reset,(ATerm)ArcID);
	if(!ATisEqual(ATAgetArgument(Arc,1),PlaceID)) continue;
	if(!ATisEqual(ATAgetArgument(Arc,0),TransID)) continue;
	mult_i=ATinsert(mult_i,(ATerm)ArcID);
	reset=true;
      }
      
      mult_i=ATreverse(mult_i);
      mult_o=ATreverse(mult_o);

      if(!nIn && !nOut && !inhib && !reset) continue;

      if(nOut>0 && inhib){
	gsWarningMsg("Both output and inhibitor arcs connect place %T with transition %T. This transition can never fire.\n");
	continue;
      }

      mult_i=ATreverse(mult_i);
      mult_o=ATreverse(mult_o);

      ATermList mult=ATconcat(mult_i,pn2gsMakeSendActions(mult_o));
      
      //summand
      AFun AR=ATappendAFun(CurrentPlaceId,"_ar_");
      if(inhib && reset) AR=ATappendAFun(CurrentPlaceId,"_air_");
      else{
	if(inhib) AR=ATappendAFun(CurrentPlaceId,"_ai_");
	else if(reset) AR=ATappendAFun(CurrentPlaceId,"_arr_");
      }
      
      ATermAppl LeftName=ATmakeAppl0(ATappendAFun(AR,ATgetName(ATgetAFun(TransID))));
      ATermAppl Left=gsMakeParamId(LeftName,ATmakeList0());//make name P_pi_ar_i_j
      ATermAppl RightExpr=VarX;  //x;
      int d=nIn-nOut;
      if(!reset) {
	if(d>0) RightExpr=pn2gsMakeDataApplProd2(OpAdd,RightExpr,gsMakeNumber(ATmakeAppl0(ATmakeAFunInt0(d)),gsMakeSortIdPos()));//RightExpr=RightExpr+d;
	else if(d<0) RightExpr=gsMakeDataApplProd(OpInt2Nat,ATmakeList1((ATerm)pn2gsMakeDataApplProd2(OpSubt,RightExpr,gsMakeNumber(ATmakeAppl0(ATmakeAFunInt0(-d)),gsMakeSortIdPos()))));//RightExpr=max(RightExpr-d,0);
      }
      else {
	if(d>0) RightExpr=gsMakeNumber(ATmakeAppl0(ATmakeAFunInt0(d)),gsMakeSortIdPos());//RightExpr=d;
	else RightExpr=Number0;
      }

      ATermAppl Right=gsMakeParamId(CurrentPlace,ATmakeList1((ATerm)RightExpr));//make P_pi(max(x+i-j,0))
      ATermAppl Summand=gsMakeSeq(Left,Right);
      
      //condition
      if(nOut>0){
	ATermAppl Cond=pn2gsMakeDataApplProd2(OpLTE,gsMakeNumber(ATmakeAppl0(ATmakeAFunInt0(nOut)),gsMakeSortIdPos()),VarX);//make nOut<=x
	Summand=gsMakeCond(Cond,Summand,gsMakeDelta());
      }
      else /*never together */ if(inhib){
	ATermAppl Cond=pn2gsMakeDataApplProd2(OpEq,VarX,Number0);//make x==0
	Summand=gsMakeCond(Cond,Summand,gsMakeDelta());
      }	
      
      if(Body) Body=gsMakeChoice(Summand,Body);
      else Body=Summand; 
    
      // extra equation
      EquationList = ATinsert(EquationList, 
			      (ATerm)gsMakeProcEqn(ATmakeList0(), 
						   gsMakeProcVarId(LeftName, ATmakeList0()), 
						   ATmakeList0(), 
						   pn2gsMakeMultiAction(mult)));
    } //For loop

    // ++++++++++++++++++ end generation of minimal number of summands


//     // +++++++++++++++++ begin normal alternative generation 
   
//     //calculate the reset multiactions (if any)
//     ATermList ResetActionLists=ATmakeList0();
//     for(int i=1; i<=l; i++)
//       ResetActionLists=ATconcat(ResetActionLists,pn2gsGetActionLists(i,ActsReset));
    
    
//     //generate the main process
//     ATermAppl VarX=gsMakeDataVarId(ATmakeAppl0(ATmakeAFunId("x")),gsMakeSortIdNat());;
//     //ATermAppl Number0=gsMakeNumber(gsString2ATermAppl("0"),gsMakeSortIdNat());
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
//     ATermAppl Number0=gsMakeNumber(gsString2ATermAppl("0"),gsMakeSortIdNat());
    
//     for(int j=m;j>-1;j--){
//       ATermAppl Summand=NULL;
//       AFun NumberJId=ATmakeAFunInt0(j);
//       ATermAppl NumberJ=gsMakeNumber(ATmakeAppl0(NumberJId),(j)?gsMakeSortIdPos():gsMakeSortIdNat());
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
// 	  if(d>0) RightExpr=pn2gsMakeDataApplProd2(OpAdd,RightExpr,gsMakeNumber(ATmakeAppl0(ATmakeAFunInt0(d)),gsMakeSortIdPos()));//RightExpr=RightExpr+d;
// 	  else if(d<0) RightExpr=gsMakeDataApplProd(OpInt2Nat,ATmakeList1((ATerm)pn2gsMakeDataApplProd2(OpSubt,RightExpr,gsMakeNumber(ATmakeAppl0(ATmakeAFunInt0(-d)),gsMakeSortIdPos()))));//RightExpr=max(RightExpr-d,0);
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
// 				    (ATerm)gsMakeProcEqn(ATmakeList0(), 
// 							 gsMakeProcVarId(LeftName, ATmakeList0()), 
// 							 ATmakeList0(), 
// 							 pn2gsGenerateP_pi_a(InActionLists,OutActionLists,NULL)));
// 	//in case there are resets
// 	if(l>0) 
// 	  EquationList = ATinsert(EquationList, 
// 				  (ATerm)gsMakeProcEqn(ATmakeList0(), 
// 						       gsMakeProcVarId(LeftNameResets, ATmakeList0()), 
// 						       ATmakeList0(), 
// 						       pn2gsGenerateP_pi_a(InActionLists,OutActionLists,ResetActionLists)));
//       }
      
//       if(j>0){ //generate the condition
// 	ATermAppl Cond=pn2gsMakeDataApplProd2(OpLTE,NumberJ,VarX);//make j<=x
// 	Summand=gsMakeCond(Cond,Summand,gsMakeDelta());
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
// 	if(i>0) RightExpr=pn2gsMakeDataApplProd2(OpAdd,RightExpr,gsMakeNumber(ATmakeAppl0(ATmakeAFunInt0(i)),gsMakeSortIdPos()));//RightExpr=RightExpr+i;
	
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
// 				(ATerm)gsMakeProcEqn(ATmakeList0(), 
// 						     gsMakeProcVarId(LeftName, ATmakeList0()), 
// 						     ATmakeList0(), 
// 						     pn2gsGenerateP_pi_a(InActionLists,InhibitorActionLists,NULL)));
// 	if(l>0)
// 	  EquationList = ATinsert(EquationList, 
// 				  (ATerm)gsMakeProcEqn(ATmakeList0(), 
// 						       gsMakeProcVarId(LeftNameResets, ATmakeList0()), 
// 						       ATmakeList0(), 
// 						       pn2gsGenerateP_pi_a(InActionLists,InhibitorActionLists,ResetActionLists)));
//       }
      
//       //generate the condition
//       ATermAppl Cond=pn2gsMakeDataApplProd2(OpEq,VarX,Number0);//make j<=x
//       Summand=gsMakeCond(Cond,Summand,gsMakeDelta());
      
//       if(Body){
// 	if(Summand) Body=gsMakeChoice(Summand,Body);
//       }
//       else Body=Summand;	
//     }
//     // +++++++++++++++++ end normal alternative generation 
    

    // handle the case m+n+k+l=0.
    if(!Body) Body=gsMakeDelta();
    
    //make process P_pi and add it
    EquationList = ATinsert(EquationList, 
			    (ATerm)gsMakeProcEqn(ATmakeList0(), 
						 gsMakeProcVarId(CurrentPlace, 
								 ATmakeList1((ATerm)gsMakeSortIdNat())), 
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

static ATermList pn2gsGetActionLists(int n, ATermList ActList){
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

  static ATermAppl pn2gsMakeMultiAction(ATermList ActionList){
    //Make a process term a_1|...|a_n
    //input : list of action names
    ATermAppl Res=NULL;
    for(;!ATisEmpty(ActionList);ActionList=ATgetNext(ActionList)){
      ATermAppl CurAct=gsMakeParamId(ATAgetFirst(ActionList),ATmakeList0());
      if(Res) Res=gsMakeSync(CurAct,Res);
      else Res=CurAct;
    }

    if(!Res) Res=gsMakeTau();

    return Res;
  }
 
  static ATermList pn2gsListNNats(int n){
    ATermList Res=ATmakeList0();
    ATermAppl SortNat=gsMakeSortIdNat();

    for(int i=0;i<n;i++) Res=ATinsert(Res,(ATerm)SortNat);
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
    ATermList Actions = ATmakeList1((ATerm)ATmakeAppl0(ATappendAFun(ATprependAFun("t_",CurrentTransId),"_mon"))); //MonitorAction
    {
      ATermList MoreActions=ATLtableGet(context.trans_in, TransID);
      if(MoreActions) Actions=ATconcat(Actions,MoreActions);

      MoreActions=ATLtableGet(context.trans_out, TransID);
      if(MoreActions) Actions=ATconcat(Actions,pn2gsMakeSendActions(MoreActions));
    }

    return ATmakeList1((ATerm)gsMakeProcEqn(ATmakeList0(), gsMakeProcVarId(CurrentTrans, ATmakeList0()), ATmakeList0(), pn2gsMakeMultiAction(Actions)));    
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
