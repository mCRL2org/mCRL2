/* $Id: pnml2gs.c,v 1.0 2005/03/09 15:46:00 ysu Exp $ */

#define NAME "pnml2gs"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "aterm2.h"
#include "libgsparse.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsparse.h"

// the static context of the spec will be checked and used, not transformed
typedef struct {
  //read-in
  ATermTable place_name;	//place_id -> name
  ATermTable place_mark;	//place_id -> Nat
  ATermTable trans_name;	//trans_id -> name
  ATermTable arc_in;	        //arc_id -> trans_id x place_id
  ATermTable arc_out;	        //arc_id -> place_id x trans_id
  //generate
  ATermTable place_in;	        //place_id -> List(arc_id) (arc_in)
  ATermTable trans_in;	        //trans_id -> List(arc_id) (arc_out)
  ATermTable place_out;	        //place_id -> List(arc_id) (arc_out)
  ATermTable trans_out;	        //trans_id -> List(arc_id) (arc_in)
} Context;
static Context context;

//==================================================
// retrieve_text gets the contents of a child <text> element of cur
//==================================================
static ATerm retrieve_text(xmlNodePtr cur) {
  // input: a pointer to the current element
  // output: the contents of the first child <text> attribute 
  //         of the current element

  xmlChar * RV;
  cur=cur->xmlChildrenNode;
  while (cur != NULL) {
    if (!xmlNodeIsText(cur)) {
      if (!xmlStrcmp(cur->name, (const xmlChar *)"text")) {
	RV = xmlNodeGetContent(cur);
	return ATparse((const char *)RV);
      }
    }
    cur=cur->next;
  }
  return NULL;
}

//==================================================
// pnml2aterm_place converts a pnml-place to a usable ATerm
//==================================================
static ATermAppl pnml2aterm_place(xmlNodePtr cur) {
  // input: a pointer to the current place
  // output: a usable translation of the place,
  //         if the place needs to be translated

  fprintf(stderr, "> Start examining a place...  \n");
  // first, we want to retrieve the id of the place
  ATerm Aid = ATparse((const char *)xmlGetProp(cur, (const xmlChar *)"id"));
  fprintf(stderr, ATwriteToString(Aid));
  fprintf(stderr, "\n");

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
      if (!(Aname=retrieve_text(cur))) {
	Aname = ATparse("default_name");
      }
      fprintf(stderr, "    name: '");
      fprintf(stderr, ATwriteToString(Aname));
      fprintf(stderr, "' \n");
    } else if (!xmlStrcmp(cur->name, (const xmlChar *)"initialMarking")) {
      // the place contains an <initialMarking> element
      // this element contains a childelement <text> which contains the initial marking of the place
      // this marking is retrieved below and assigned to AinitialMarking

      if (!(AinitialMarking=retrieve_text(cur))) {
	AinitialMarking = ATparse("0");
      }
      if (atoi(ATwriteToString(AinitialMarking)) < 0) {
	// if the initial marking is less than zero, it is resetted to zero
	AinitialMarking = ATparse("0");
	fprintf(stderr, "Place with id '");
	fprintf(stderr, ATwriteToString(Aid));
	fprintf(stderr, "' has initial marking is less than 0, resetting initial marking to 0! \n");
      }
      fprintf(stderr, "    initialMarking: '");
      fprintf(stderr, ATwriteToString(AinitialMarking));
      fprintf(stderr, "' \n");
    } else if (!xmlStrcmp(cur->name, (const xmlChar *)"type")) {
      // the place contains an <type> element
      // this element contains a childelement <text> which contains the type of the place

      if (!(Atype=retrieve_text(cur))) {
	Atype = ATparse("channel");
      }
      if (!ATisEqual(Atype, ATparse("channel"))) {
	// the type should either be omitted or have the value "channel"
	// otherwise the place does not need to be translated!
	fprintf(stderr, "Place with id '");
	fprintf(stderr, ATwriteToString(Aid));
	fprintf(stderr, "' has type '");
	fprintf(stderr, ATwriteToString(Atype));
	fprintf(stderr, "' and will not be translated. \n");
	return NULL;
      }
      fprintf(stderr, "    type: '");
      fprintf(stderr, ATwriteToString(Atype));
      fprintf(stderr, "' \n");

    } else if (xmlNodeIsText(cur)) {
    } else {
      fprintf(stderr, "    Ignore an element named '");
      fprintf(stderr, (const char *)cur->name);
      fprintf(stderr, "'.\n");
    }
    cur = cur->next;
  }

  // argument order of returnvalue is id - name - initialMarking 
  return ATmakeAppl3(ATmakeAFun("place", 3, ATfalse), Aid, Aname, AinitialMarking);
}

//==================================================
// pnml2aterm_transition converts a pnml-transition to a usable ATerm
//==================================================
static ATermAppl pnml2aterm_transition(xmlNodePtr cur) {
  // input: a pointer to the current transition
  // output: a usable translation of the transition,
  //         if the transition needs to be translated

  fprintf(stderr, "> Start examining a transition...  \n");
  // first, we want to retrieve the id of the transition
  ATerm Aid = ATparse((const char *)xmlGetProp(cur, (const xmlChar *)"id"));
  fprintf(stderr, ATwriteToString(Aid));
  fprintf(stderr, "\n");

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
      fprintf(stderr, "    name: ");
      if (!(Aname=retrieve_text(cur))) {
	Aname = ATparse("default_name");
      }
      fprintf(stderr, "|[");
      fprintf(stderr, ATwriteToString(Aname));
      fprintf(stderr, "]| \n");
    } else if (!xmlStrcmp(cur->name, (const xmlChar *)"type")) {
      // the transition contains an <type> element
      // this element contains a childelement <text> which contains the type of the transition
      fprintf(stderr, "    type: ");
      if (!(Atype=retrieve_text(cur))) {
	Atype = ATparse("AND");
      }
      fprintf(stderr, "|[");
      fprintf(stderr, ATwriteToString(Atype));
      fprintf(stderr, "]| \n");
      if (!ATisEqual(Atype, ATparse("AND"))) {
	// the type should either be omitted or have the value "AND"
	// otherwise the place does not need to be translated!
	fprintf(stderr, "Type is not 'AND'! \n");
	return NULL;
      }
    } else if (xmlNodeIsText(cur)) {
    } else {
      fprintf(stderr, "    ignore ");
      fprintf(stderr, (const char *)cur->name);
      fprintf(stderr, "\n");
    }
    cur = cur->next;
  }

  // argument order of returnvalue is id - name  
  return ATmakeAppl2(ATmakeAFun("transition", 2, ATfalse), Aid, Aname);
}

//==================================================
// pnml2aterm_arc converts a pnml-arc to a usable ATerm
//==================================================
static ATermAppl pnml2aterm_arc(xmlNodePtr cur) {
  // input: a pointer to the current arc
  // output: a usable translation of the arc,
  //         if the arc needs to be translated

  fprintf(stderr, "> Start examining an arc...  \n");
  // first, we want to retrieve the id of the arc
  ATerm Aid = ATparse((const char *)xmlGetProp(cur, (const xmlChar *)"id"));
  fprintf(stderr, ATwriteToString(Aid));
  fprintf(stderr, "\n");

  // second, we want to retrieve the source and the target of the arc
  ATerm Asource = ATparse((const char *)xmlGetProp(cur, (const xmlChar *)"source"));
  fprintf(stderr, ATwriteToString(Asource));
  fprintf(stderr, "\n");

  ATerm Atarget = ATparse((const char *)xmlGetProp(cur, (const xmlChar *)"target"));
  fprintf(stderr, ATwriteToString(Atarget));
  fprintf(stderr, "\n");

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
      fprintf(stderr, "    type: ");
      if (!(Atype=retrieve_text(cur))) {
	Atype = ATparse("some_strange`type=that n0b0dy u5e5...");
      }
      fprintf(stderr, "|[");
      fprintf(stderr, ATwriteToString(Atype));
      fprintf(stderr, "]| \n");
      if (!ATisEqual(Atype, ATparse("some_strange`type=that n0b0dy u5e5..."))) {
	// the type should be omitted
	// otherwise the arc does not need to be translated!
	fprintf(stderr, "Arc has a type, and thus does not need translation! \n");
	return NULL;
      }
    } else if (xmlNodeIsText(cur)) {
    } else {
      fprintf(stderr, "    ignore ");
      fprintf(stderr, (const char *)cur->name);
      fprintf(stderr, "\n");
    }
    cur = cur->next;
  }

  // argument order of returnvalue is id - source - target
  return ATmakeAppl3(ATmakeAFun("arc", 3, ATfalse), Aid, Asource, Atarget);
}

//==================================================
// pnml2aterm converts the pnml-input to a usable ATerm
//==================================================
static ATermAppl pnml2aterm(xmlDocPtr doc) {
  // input: a pointer of the type xmlDocPtr which points to the parsed XML-file
  // output: an ATermAppl, translated from the XML-file,
  //         in which only relevant elements/attributes are concluded

  //==================================================
  // initializations and initial checks.
  //==================================================
  xmlNodePtr cur = xmlDocGetRootElement(doc);
  if (cur == NULL) {
    fprintf(stderr, "File is empty. \n");
    return NULL;
  }
  if (xmlStrcmp(cur->name, (const xmlChar *)"pnml")) {
    fprintf(stderr, "File is not a PNML file!  \n");
    return NULL;
  }
  // cur now points to the <pnml>element

  cur = cur->xmlChildrenNode;
  // the first <net>element, if any present, is selected by cur
  while (cur != NULL && xmlStrcmp(cur->name, (const xmlChar *)"net")) {
    if (!xmlNodeIsText(cur)) {
      fprintf(stderr, "Element: ");
      fprintf(stderr, (const char *)cur->name);
      fprintf(stderr, " is not a Petri net and will be ignored in the translation (including it's sub-elements).   \n");
    }
    cur = cur->next;
  }
  if (cur == NULL) {
    fprintf(stderr, "File does not contain a Petri net. \n");
    return NULL;
  }   

  // cur now points to the first <net>element
  fprintf(stderr, "\n");
  fprintf(stderr, "Start converting the Petri net to an ATerm...  \n");
  fprintf(stderr, "\n");

  //==================================================
  // actual translation starts here
  //==================================================
  // retrieve the ID of the Petri net
  ATerm ANetID;
  if (!xmlGetProp(cur, (const xmlChar *)"id")) {
    ANetID = ATparse("Petri_net");
    fprintf(stderr, "NO NET-ID FOUND!\n");
  } else {
    ANetID = ATparse((const char *)xmlGetProp(cur, (const xmlChar *)"id"));
  }
  fprintf(stderr, "NetID = ");
  fprintf(stderr, ATwriteToString(ANetID));
  fprintf(stderr, "\n");

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

  // this loop goes through all the children of the <net>element
  // these children will be translated or ignored, this depends on the element name
  while (cur != NULL) {
    // current elements that are conceivable for translation are:
    // <place>  -  <transition>  -  <arc>
    // all other elements will be ignored in the translation
    
    if (!xmlStrcmp(cur->name, (const xmlChar *)"place")) {
      if (!(ACurrentPlace=pnml2aterm_place(cur))) {
	// pnml2aterm_place returns NULL, no translation needed.
      } else {
	APlaces = ATinsert(APlaces, (ATerm)ACurrentPlace);
	fprintf(stderr, "Translate this place: ");
	fprintf(stderr, ATwriteToString((ATerm)ACurrentPlace));
	fprintf(stderr, "\n");
      }
   } else if (!xmlStrcmp(cur->name, (const xmlChar *)"transition")) {
      if(!(ACurrentTransition=pnml2aterm_transition(cur))) {
	fprintf(stderr, "This transition will not be translated \n");
      } else {
	ATransitions = ATinsert(ATransitions, (ATerm)ACurrentTransition);
	fprintf(stderr, "Translate this transition: ");
	fprintf(stderr, ATwriteToString((ATerm)ACurrentTransition));
	fprintf(stderr, "\n");
      }
   } else if (!xmlStrcmp(cur->name, (const xmlChar *)"arc")) {
      if(!(ACurrentArc=pnml2aterm_arc(cur))) {
	fprintf(stderr, "This arc will not be translated \n");
      } else {
	AArcs = ATinsert(AArcs, (ATerm)ACurrentArc);
	fprintf(stderr, "Translate this arc: ");
	fprintf(stderr, ATwriteToString((ATerm)ACurrentArc));
	fprintf(stderr, "\n");
      }
   } else if (xmlNodeIsText(cur)) {
   } else {
     fprintf(stderr, "> An element named ");
     fprintf(stderr, (const char *)cur->name);
     fprintf(stderr, " will be ignored in the translation (including it's sub-elements).  \n");
   };
   cur = cur->next;
  };

  // argument order of returnvalue is places - transitions - arcs 
  return ATmakeAppl3(ATmakeAFun(ATwriteToString(ANetID), 3, ATtrue), (ATerm)ATreverse(APlaces), (ATerm)ATreverse(ATransitions), (ATerm)ATreverse(AArcs));
}

//==================================================
// do_pnml2gs converts the ATerm delivered by pnml2aterm to a GenSpect ATerm.
//==================================================
static ATermAppl do_pnml2gs(ATermAppl Spec){
  // input: an ATermAppl that contains the translated PNML-file
  // output: another ATermAppl, which is the GenSpect translation
  //         of the PNML-ATerm.

  //==================================================
  // initializations of tables and some initial checks.
  //==================================================
  fprintf(stderr, "\n");
  fprintf(stderr, "====================\n");
  fprintf(stderr, ATwriteToString((ATerm)Spec));
  fprintf(stderr, "\n====================\n");
  fprintf(stderr, "\n");

  ATerm ANetID = ATparse(ATgetName(ATgetAFun(Spec)));
  
  fprintf(stderr, ATwriteToString(ANetID));
  fprintf(stderr, "\n");

  // put the places, transitions and arcs in the lists again
  ATermList APlaces = (ATermList)ATgetArgument(Spec, 0);
  ATermList ATransitions = (ATermList)ATgetArgument(Spec, 1);
  ATermList AArcs = (ATermList)ATgetArgument(Spec, 2);

  // temporary variable to store the current key
  // used for Places, Transitions and Arcs!!!
  ATerm CurrentKey;

  while (ATisEmpty(APlaces) == ATfalse) {
    // this loop itterates all places that will be translated
    fprintf(stderr, ATwriteToString(ATgetFirst(APlaces)));
    fprintf(stderr, "\n");

    CurrentKey = ATgetArgument(ATgetFirst(APlaces), 0);
    
    // it is sufficient to check whether a key appears in place_name OR place_mark
    // since a value is inserted in both tables at the same time!
    if (ATtableGet(context.place_name, CurrentKey)) {
      // the ID of the current places appears more than once in the places.
      // this is an error in the input, and thus termination takes place!
      fprintf(stderr, "The id: ");
      fprintf(stderr, ATwriteToString(CurrentKey));
      fprintf(stderr, " appears more than once! \n");
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
  fprintf(stderr, ATwriteToString((ATerm)APlaces));
  fprintf(stderr, "\n");
  fprintf(stderr, ATwriteToString((ATerm)ATtableKeys(context.place_name)));
  fprintf(stderr, "\n");
  fprintf(stderr, ATwriteToString((ATerm)ATtableKeys(context.place_mark)));
  fprintf(stderr, "\n");

  while (ATisEmpty(ATransitions) == ATfalse) {
    // this loop itterates all transitions that will be translated
    fprintf(stderr, ATwriteToString(ATgetFirst(ATransitions)));
    fprintf(stderr, "\n");

    CurrentKey = ATgetArgument(ATgetFirst(ATransitions), 0);
    
    if (ATtableGet(context.place_name, CurrentKey)) {
      // the ID of the current transition appeared already in the places.
      // this is an error in the input, and thus termination takes place!
      fprintf(stderr, "The id: ");
      fprintf(stderr, ATwriteToString(CurrentKey));
      fprintf(stderr, " appears more than once! \n");
      return NULL;
    } else if (ATtableGet(context.trans_name, CurrentKey)) {
      // the ID of the current transition appears more than once in the transitions.
      // this is an error in the input, and thus termination takes place!
      fprintf(stderr, "The id: ");
      fprintf(stderr, ATwriteToString(CurrentKey));
      fprintf(stderr, " appears more than once! \n");
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
  fprintf(stderr, ATwriteToString((ATerm)ATransitions));
  fprintf(stderr, "\n");
  fprintf(stderr, ATwriteToString((ATerm)ATtableKeys(context.trans_name)));
  fprintf(stderr, "\n");

  // temporary variables to store the current source and target
  ATerm CurrentSource;
  ATerm CurrentTarget;
  while (ATisEmpty(AArcs) == ATfalse) {
    // this loop itterates all arcs that will be translated
    fprintf(stderr, ATwriteToString(ATgetFirst(AArcs)));
    fprintf(stderr, "\n");

    CurrentKey = ATgetArgument(ATgetFirst(AArcs), 0);
    
    if (ATtableGet(context.place_name, CurrentKey)) {
      // the ID of the current arc appeared already in the places.
      // this is an error in the input, and thus termination takes place!
      fprintf(stderr, "The id: ");
      fprintf(stderr, ATwriteToString(CurrentKey));
      fprintf(stderr, " appears more than once! \n");
      return NULL;
    } else if (ATtableGet(context.trans_name, CurrentKey)) {
      // the ID of the current arc appeared already in the transitions.
      // this is an error in the input, and thus termination takes place!
      fprintf(stderr, "The id: ");
      fprintf(stderr, ATwriteToString(CurrentKey));
      fprintf(stderr, " appears more than once! \n");
      return NULL;
    } else if (ATtableGet(context.arc_in, CurrentKey)) {
      // the ID of the current arc appeared already in the in_arcs.
      // this is an error in the input, and thus termination takes place!
      fprintf(stderr, "The id: ");
      fprintf(stderr, ATwriteToString(CurrentKey));
      fprintf(stderr, " appears more than once! \n");
      return NULL;
    } else if (ATtableGet(context.arc_out, CurrentKey)) {
      // the ID of the current arc appeared already in the out_arcs.
      // this is an error in the input, and thus termination takes place!
      fprintf(stderr, "The id: ");
      fprintf(stderr, ATwriteToString(CurrentKey));
      fprintf(stderr, " appears more than once! \n");
      return NULL;
    } else {
      // the arc's ID did not appear in the transitions, places or arcs that will be translated
      // check the source and the target from the arc to see if the arc is used in the translation!
      CurrentSource = ATgetArgument(ATgetFirst(AArcs),1);
      CurrentTarget = ATgetArgument(ATgetFirst(AArcs),2);
      if (ATtableGet(context.place_name, CurrentSource) && ATtableGet(context.trans_name, CurrentTarget)) {
	// The arc is an arc_out; it goes from a place to a transition
	// insert the data into context.arc_out
	// key = id
	// value = arc_out(source, target)
	ATtablePut(context.arc_out, CurrentKey, (ATerm)ATmakeAppl2(ATmakeAFun("arc_out",2,ATfalse),CurrentSource,CurrentTarget));
      } else if (ATtableGet(context.place_name, CurrentTarget) && ATtableGet(context.trans_name, CurrentSource)) {
	// The arc is an arc_in; it goes from a transition to a place
	// insert the data into context.arc_in
	// key = id
	// value = arc_in(source, target)
	ATtablePut(context.arc_in, CurrentKey, (ATerm)ATmakeAppl2(ATmakeAFun("arc",2,ATfalse),CurrentSource,CurrentTarget));
      } else {
	// either the source or the target (or both) of the arc will not be translated
	// therefore the arc will not be translated either!
      }
    }
    // remove the entry from the list ATransitions
    AArcs = ATgetNext(AArcs);
  }

  fprintf(stderr, ATwriteToString((ATerm)AArcs));
  fprintf(stderr, "\n");
  fprintf(stderr, ATwriteToString((ATerm)ATtableKeys(context.arc_in)));
  fprintf(stderr, "\n");
  fprintf(stderr, ATwriteToString((ATerm)ATtableKeys(context.arc_out)));
  fprintf(stderr, "\n");



  /* GENERATE context.place_in - context.trans_in - context.place_out - context.trans_out */



  fprintf(stderr, "\n");
  fprintf(stderr, "====================\n");
  fprintf(stderr, "\n");

  return Spec;	
}


//==================================================
// main
//==================================================
int main(int argc, char **argv){
  ATerm stackbot;
  ATinit(0,NULL,&stackbot);
  
  #define sopts "ai"
  struct option lopts[] = {
    {"read-aterm"  , no_argument,      NULL, 'a'},
    {"debug"       , no_argument,      NULL, 'd'},
    {0, 0, 0, 0}
  };
  int opt,read_aterm;
  
  read_aterm = 0;
  while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 ){
    switch ( opt ){
    case 'a':
      read_aterm = 1;
      break;
    case 'd': 
      gsSetDebugMsg();
      break;
    default:
      break;
    }
  }
  
  char *SpecStream;
  if ( argc-optind < 1 ){
      SpecStream = "stdin";
  } else {
    if ( (SpecStream = argv[optind]) == NULL ){
      perror(NAME);
      return 1;
    }
  }
  
  fprintf(stderr,"\n");

  xmlDocPtr doc = xmlParseFile(SpecStream);
  if(!doc) {
    fprintf(stderr,"Document not parsed succesfully. \n");
    return 1;
  }
  
  ATermAppl Spec=pnml2aterm(doc);
  xmlFreeDoc(doc);

  if(!Spec){	
    fprintf(stderr, "Error while converting PNML to ATerm, conversion stopped!  \n");
    return 1;
  }

  context.place_name=ATtableCreate(63,50);
  context.place_mark=ATtableCreate(63,50); 
  context.trans_name=ATtableCreate(63,50); 
  context.arc_in=ATtableCreate(63,50);  
  context.arc_out=ATtableCreate(63,50);    
  
  context.place_in=ATtableCreate(63,50);   
  context.trans_in=ATtableCreate(63,50);   
  context.place_out=ATtableCreate(63,50);  
  context.trans_out=ATtableCreate(63,50);  

  Spec=do_pnml2gs(Spec);

  ATtableDestroy(context.place_name);
  ATtableDestroy(context.place_mark); 
  ATtableDestroy(context.trans_name); 
  ATtableDestroy(context.arc_in);  
  ATtableDestroy(context.arc_out);    
  
  ATtableDestroy(context.place_in);   
  ATtableDestroy(context.trans_in);   
  ATtableDestroy(context.place_out);  
  ATtableDestroy(context.trans_out);
 
  if(!Spec) {
    fprintf(stderr, "Error while converting PNML ATerm to GenSpect ATerm, conversion stopped!  \n");
    return 1;
  }
  fprintf(stderr, "\n");  
  fprintf(stderr, ATwriteToString((ATerm)Spec));
  fprintf(stderr, "\n \n");  
  gsPrintSpecification(stdout,Spec);        
  return 0;
}

