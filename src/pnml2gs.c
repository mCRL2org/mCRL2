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
  ATermTable place_names;	//id -> name
  ATermTable place_mark;	//id -> Nat
  ATermTable trans_name;	//id -> name
  ATermTable arc_in;	        //id -> trans_id x place_id
  ATermTable arc_out;	        //id -> place_id x trans_id
  //generate
  ATermTable place_in;	        //id -> List(arc_id) (arc_in)
  ATermTable trans_in;	        //id -> List(arc_id) (arc_out)
  ATermTable place_out;	        //id -> List(arc_id) (arc_out)
  ATermTable trans_out;	        //id -> List(arc_id) (arc_in)
} Context;
static Context context;

//==================================================
// retrieve_text gets the contents of a child <text> element of cur
//==================================================
static ATerm * retrieve_text(xmlNodePtr cur) {
  // input: a pointer to the current element
  // output: the contents of the first child <text> attribute 
  //         of the current element

  xmlChar * RV;
  cur=cur->xmlChildrenNode;
  while (cur != NULL) {
    if (!xmlNodeIsText(cur)) {
      if (!xmlStrcmp(cur->name, (const xmlChar *) "text")) {
	RV = xmlNodeGetContent(cur);
	return ATparse(RV);
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
  ATerm Aid = ATparse(xmlGetProp(cur, (const xmlChar *) "id"));
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
    
    if (!xmlStrcmp(cur->name, (const xmlChar *) "name")) {
      // the place contains a <name> element
      // a <name> element contains a childelement <text> which contains the name of the place
      // the name is retrieved below and assigned to Aname
      fprintf(stderr, "    name: ");
      if (!(Aname=retrieve_text(cur))) {
	Aname = ATparse("default_name");
      }
      fprintf(stderr, "|[");
      fprintf(stderr, ATwriteToString(Aname));
      fprintf(stderr, "]| \n");
    } else if (!xmlStrcmp(cur->name, (const xmlChar *) "initialMarking")) {
      // the place contains an <initialMarking> element
      // this element contains a childelement <text> which contains the initial marking of the place
      // this marking is retrieved below and assigned to AinitialMarking
      fprintf(stderr, "    initialMarking: ");
      if (!(AinitialMarking=retrieve_text(cur))) {
	AinitialMarking = ATparse("0");
      }
      if (atoi(ATwriteToString(AinitialMarking)) < 0) {
	// if the initial marking is less than zero, it is resetted to zero
	AinitialMarking = ATparse("0");
	fprintf(stderr, "Initial marking is less than 0, resetting initial marking to 0! \n");
      }
      fprintf(stderr, "|[");
      fprintf(stderr, ATwriteToString(AinitialMarking));
      fprintf(stderr, "]| \n");
    } else if (!xmlStrcmp(cur->name, (const xmlChar *) "type")) {
      // the place contains an <type> element
      // this element contains a childelement <text> which contains the type of the place
      fprintf(stderr, "    type: ");
      if (!(Atype=retrieve_text(cur))) {
	Atype = ATparse("channel");
      }
      fprintf(stderr, "|[");
      fprintf(stderr, ATwriteToString(Atype));
      fprintf(stderr, "]| \n");
      if (!ATisEqual(Atype, ATparse("channel"))) {
	// the type should either be omitted or have the value "channel"
	// otherwise the place does not need to be translated!
	fprintf(stderr, "Type is not 'channel'! \n");
	return NULL;
      }
    } else if (xmlNodeIsText(cur)) {
    } else {
      fprintf(stderr, "    ignore ");
      fprintf(stderr, cur->name);
      fprintf(stderr, "\n");
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
  ATerm Aid = ATparse(xmlGetProp(cur, (const xmlChar *) "id"));
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
    
    if (!xmlStrcmp(cur->name, (const xmlChar *) "name")) {
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
    } else if (!xmlStrcmp(cur->name, (const xmlChar *) "type")) {
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
      fprintf(stderr, cur->name);
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
  ATerm Aid = ATparse(xmlGetProp(cur, (const xmlChar *) "id"));
  fprintf(stderr, ATwriteToString(Aid));
  fprintf(stderr, "\n");

  // second, we want to retrieve the source and the target of the arc
  ATerm Asource = ATparse(xmlGetProp(cur, (const xmlChar *) "source"));
  fprintf(stderr, ATwriteToString(Asource));
  fprintf(stderr, "\n");

  ATerm Atarget = ATparse(xmlGetProp(cur, (const xmlChar *) "target"));
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
    
    if (!xmlStrcmp(cur->name, (const xmlChar *) "type")) {
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
      fprintf(stderr, cur->name);
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
  if (xmlStrcmp(cur->name, (const xmlChar *) "pnml")) {
    fprintf(stderr, "File is not a PNML file!  \n");
    return NULL;
  }
  // cur now points to the <pnml>element

  cur = cur->xmlChildrenNode;
  // the first <net>element, if any present, is selected by cur
  while (cur != NULL && xmlStrcmp(cur->name, (const xmlChar *) "net")) {
    if (!xmlNodeIsText(cur)) {
      fprintf(stderr, "Element: ");
      fprintf(stderr, cur->name);
      fprintf(stderr, " is not a petri net and will be ignored in the translation (including it's sub-elements).   \n");
    }
    cur = cur->next;
  }
  if (cur == NULL) {
    fprintf(stderr, "File does not contain a petri net. \n");
    return NULL;
  }   

  // cur now points to the first <net>element
  fprintf(stderr, "\n");
  fprintf(stderr, "Start converting the petri net to an ATerm...  \n");
  fprintf(stderr, "\n");

  //==================================================
  // actual translation starts here
  //==================================================
  ATerm ANetID = ATparse(xmlGetProp(cur, (const xmlChar *) "id"));

  cur = cur->xmlChildrenNode;
  // cur now points to the first element in the petri net

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
    
    if (!xmlStrcmp(cur->name, (const xmlChar *) "place")) {
      if (!(ACurrentPlace=pnml2aterm_place(cur))) {
	fprintf(stderr, "This place will not be translated \n");
      } else {
	APlaces = ATinsert(APlaces, ACurrentPlace);
	fprintf(stderr, "Translate this place: ");
	fprintf(stderr, ATwriteToString(ACurrentPlace));
	fprintf(stderr, "\n");
      }
   } else if (!xmlStrcmp(cur->name, (const xmlChar *) "transition")) {
      if(!(ACurrentTransition=pnml2aterm_transition(cur))) {
	fprintf(stderr, "This transition will not be translated \n");
      } else {
	ATransitions = ATinsert(ATransitions, ACurrentTransition);
	fprintf(stderr, "Translate this transition: ");
	fprintf(stderr, ATwriteToString(ACurrentTransition));
	fprintf(stderr, "\n");
      }
   } else if (!xmlStrcmp(cur->name, (const xmlChar *) "arc")) {
      if(!(ACurrentArc=pnml2aterm_arc(cur))) {
	fprintf(stderr, "This arc will not be translated \n");
      } else {
	AArcs = ATinsert(AArcs, ACurrentArc);
	fprintf(stderr, "Translate this arc: ");
	fprintf(stderr, ATwriteToString(ACurrentArc));
	fprintf(stderr, "\n");
      }
   } else if (xmlNodeIsText(cur)) {
   } else {
     fprintf(stderr, "> An element named ");
     fprintf(stderr, cur->name);
     fprintf(stderr, " will be ignored in the translation (including it's sub-elements).  \n");
   };
   cur = cur->next;
  };

  // argument order of returnvalue is places - transitions - arcs  
  return ATmakeAppl3(ATmakeAFun(ATwriteToString(ANetID), 3, ATfalse), APlaces, ATransitions, AArcs);
}

//==================================================
// ATtableContainsKey checks if a specified key is already present in the given table.
//==================================================
static ATbool ATtableContainsKey(ATermTable table, ATerm key){
  // input: an ATermTable table and an ATerm key
  // output: ATtrue if key is a key in table
  //         ATfalse if key is not a key in table

  ATermList KeysList = ATtableKeys(table); 
  while (ATisEmpty(KeysList) == ATfalse) {
    // this loop itterates through all keys
    if (ATgetLast(KeysList) == key) {
      return ATtrue;
    }
    // remove the entry from the list
    KeysList = ATgetPrefix(KeysList);
  }
  return ATfalse;
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
  fprintf(stderr, "\n");

  ATerm ANetID = ATparse(ATgetName(ATgetAFun(Spec)));
  
  fprintf(stderr, ATwriteToString(ANetID));
  fprintf(stderr, "\n");

  // put the places, transitions and arcs in the lists again
  ATermList APlaces = ATgetArgument(Spec, 0);
  ATermList ATransitions = ATgetArgument(Spec, 1);
  ATermList AArcs = ATgetArgument(Spec, 2);

  // temporary variable to store the current key
  // used for Places, Transitions and Arcs!!!
  ATerm CurrentKey;

  // create table with the necessary data from the places
  // maximum concurrency is calculated and inserted later!
  ATermTable TablePlaces = ATtableCreate(10, 80);
  while (ATisEmpty(APlaces) == ATfalse) {
    // this loop itterates all places that will be translated
    fprintf(stderr, ATwriteToString(ATgetLast(APlaces)));
    fprintf(stderr, "\n");

    CurrentKey = ATgetArgument(ATgetLast(APlaces), 0);
    
    if (ATtableContainsKey(TablePlaces, CurrentKey) == ATtrue) {
      // the ID of the current places appears more than once in the places.
      // this is an error in the input, and thus termination takes place!
      fprintf(stderr, "The id: ");
      fprintf(stderr, ATwriteToString(CurrentKey));
      fprintf(stderr, " appears more than once! \n");
      return NULL;
    } else {
      // insert the data into TablePlaces
      // key = id
      // value = place(name, initialMarking)
      ATtablePut(TablePlaces, CurrentKey , ATmakeAppl2(ATmakeAFun("place", 2, ATfalse), ATgetArgument(ATgetLast(APlaces), 1),ATgetArgument(ATgetLast(APlaces),2)));
    }

    // remove the entry from the list
    APlaces = ATgetPrefix(APlaces);
  }
  fprintf(stderr, ATwriteToString(APlaces));
  fprintf(stderr, "\n");
  fprintf(stderr, ATwriteToString(ATtableKeys(TablePlaces)));
  fprintf(stderr, "\n");


  // create table with the necessary data from the transitions
  ATermTable TableTransitions = ATtableCreate(10, 80);
  while (ATisEmpty(ATransitions) == ATfalse) {
    // this loop itterates all transitions that will be translated
    fprintf(stderr, ATwriteToString(ATgetLast(ATransitions)));
    fprintf(stderr, "\n");

    CurrentKey = ATgetArgument(ATgetLast(ATransitions), 0);
    
    if (ATtableContainsKey(TablePlaces, CurrentKey) == ATtrue) {
      // the ID of the current transition appeared already in the places.
      // this is an error in the input, and thus termination takes place!
      fprintf(stderr, "The id: ");
      fprintf(stderr, ATwriteToString(CurrentKey));
      fprintf(stderr, " appears more than once! \n");
      return NULL;
    } else if (ATtableContainsKey(TableTransitions, CurrentKey) == ATtrue) {
      // the ID of the current transition appears more than once in the transitions.
      // this is an error in the input, and thus termination takes place!
      fprintf(stderr, "The id: ");
      fprintf(stderr, ATwriteToString(CurrentKey));
      fprintf(stderr, " appears more than once! \n");
      return NULL;
    } else {
      // insert the data into TableTransitions
      // key = id
      // value = transition(name)
      ATtablePut(TableTransitions, CurrentKey , ATmakeAppl1(ATmakeAFun("transition", 1, ATfalse), ATgetArgument(ATgetLast(ATransitions), 1)));
    }
    // remove the entry from the list ATransitions
    ATransitions = ATgetPrefix(ATransitions);
  }
  fprintf(stderr, ATwriteToString(ATransitions));
  fprintf(stderr, "\n");
  fprintf(stderr, ATwriteToString(ATtableKeys(TableTransitions)));
  fprintf(stderr, "\n");

  // temporary variables to store the current source and target
  ATerm CurrentSource;
  ATerm CurrentTarget;
  // create table with the necessary data from the arcs
  // also check the source and the target from the arc!
  ATermTable TableArcs = ATtableCreate(20, 80);
  while (ATisEmpty(AArcs) == ATfalse) {
    // this loop itterates all arcs that will be translated
    fprintf(stderr, ATwriteToString(ATgetLast(AArcs)));
    fprintf(stderr, "\n");

    CurrentKey = ATgetArgument(ATgetLast(AArcs), 0);
    
    if (ATtableContainsKey(TablePlaces, CurrentKey) == ATtrue) {
      // the ID of the current arc appeared already in the places.
      // this is an error in the input, and thus termination takes place!
      fprintf(stderr, "The id: ");
      fprintf(stderr, ATwriteToString(CurrentKey));
      fprintf(stderr, " appears more than once! \n");
      return NULL;
    } else if (ATtableContainsKey(TableTransitions, CurrentKey) == ATtrue) {
      // the ID of the current arc appeared already in the transitions.
      // this is an error in the input, and thus termination takes place!
      fprintf(stderr, "The id: ");
      fprintf(stderr, ATwriteToString(CurrentKey));
      fprintf(stderr, " appears more than once! \n");
      return NULL;
    } else if (ATtableContainsKey(TableArcs, CurrentKey) == ATtrue) {
      // the ID of the current arc appears more than once in the arcs.
      // this is an error in the input, and thus termination takes place!
      fprintf(stderr, "The id: ");
      fprintf(stderr, ATwriteToString(CurrentKey));
      fprintf(stderr, " appears more than once! \n");
      return NULL;
    } else {
      // the arc's ID is correct
      // check the source and the target from the arc to see if the arc is used in the translation!
      CurrentSource = ATgetArgument(ATgetLast(AArcs),1);
      CurrentTarget = ATgetArgument(ATgetLast(AArcs),2);

      if ((ATtableContainsKey(TablePlaces, CurrentSource) == ATtrue) && (ATtableContainsKey(TableTransitions, CurrentTarget) == ATtrue)) {
	// The arc goes from a place to a transition

	// insert the data into the TableArcs
	// key = id
	// value = arc(source, target)
	ATtablePut(TableArcs, CurrentKey, ATmakeAppl2(ATmakeAFun("arc",2,ATfalse),CurrentSource,CurrentTarget));

	/* update MC */
	
      } else if ((ATtableContainsKey(TablePlaces, CurrentTarget) == ATtrue) && (ATtableContainsKey(TableTransitions, CurrentSource) == ATtrue)) {
	// The arc goes from a transition to a place

	// insert the data into the TableArcs
	// key = id
	// value = arc(source, target)
	ATtablePut(TableArcs, CurrentKey, ATmakeAppl2(ATmakeAFun("arc",2,ATfalse),CurrentSource,CurrentTarget));

	/* update MC */
	
      } else {
	// either the source or the target (or both) of the arc will not be translated
	// therefore the arc will not be translated either!
	
      }
    }
    // remove the entry from the list ATransitions
    AArcs = ATgetPrefix(AArcs);
  }

  fprintf(stderr, ATwriteToString(AArcs));
  fprintf(stderr, "\n");
  fprintf(stderr, ATwriteToString(ATtableKeys(TableArcs)));
  fprintf(stderr, "\n");


  






  /* CALCULATE MAXIMUM CONCURRENCY */

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
    { "read-aterm",		no_argument,	NULL,	'a' },
    { 0, 0, 0, 0 }
  };
  int opt,read_aterm;
  
  read_aterm = 0;
  while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 ){
    switch ( opt ){
    case 'a':
      read_aterm = 1;
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

  context.place_names=ATtableCreate(63,50);
  context.place_mark=ATtableCreate(63,50); 
  context.trans_name=ATtableCreate(63,50); 
  context.arc_in=ATtableCreate(63,50);  
  context.arc_out=ATtableCreate(63,50);    
  
  context.place_in=ATtableCreate(63,50);   
  context.trans_in=ATtableCreate(63,50);   
  context.place_out=ATtableCreate(63,50);  
  context.trans_out=ATtableCreate(63,50);  

  Spec=do_pnml2gs(Spec);

  ATtableDestroy(context.place_names);
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

  gsPrintSpecification(stdout,Spec);        
  return 0;
}

