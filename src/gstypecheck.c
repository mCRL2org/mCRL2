#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
}
#endif

#include "gstypecheck.h"
#include "gsfunc.h"

// Static data 
typedef struct { 
     ATermIndexedSet basic_sorts;	
     ATermTable defined_sorts;		//name -> sort expression
     ATermTable constructors;		//name -> Set(sort expression)
     ATermTable mappings;		    //name -> Set(sort expression)
     ATermTable actions;	        //name -> Set(List(sort expression)) because of action polymorphism
     ATermTable processes;	        //name -> List(sort expression) process names are unique.
} Context;
static Context context;

// Static functions declaration
void gstcContextDataInit(void);
void gstcContextDataDestroy(void);
static ATermAppl gstcReadIn (ATermAppl);
static ATermAppl gstcCheck (ATermAppl);
static ATermAppl gstcTransform (ATermAppl);

// Main function
ATermAppl
gsTypeCheck (ATermAppl input)
{	
	ATermAppl Result = input;
	gsVerboseMsg ("type checking fase started\n");

	gstcContextDataInit();
	
	Result = gstcReadIn(Result);
    if (Result == NULL) {
      throw;
    }

 	Result = gstcCheck(Result);
    if (Result == NULL) {
      throw;
    }

 	Result = gstcTransform(Result);
    if (Result == NULL) {
      throw;
    }

	gsVerboseMsg ("type checking fase finished\n");
	
finally:
    if (Result != NULL) {
      gsDebugMsg("return %t\n", Result);
    } else {
      gsDebugMsg("return NULL\n");
    }
	gstcContextDataDestroy();
    return Result;
}

// Static functions
void gstcContextDataInit(void){
	ATprotect((ATerm* )&context.basic_sorts);
    ATprotect((ATerm* )&context.defined_sorts);	
    ATprotect((ATerm* )&context.constructors);
    ATprotect((ATerm* )&context.mappings);
    ATprotect((ATerm* )&context.actions);	
    ATprotect((ATerm* )&context.processes);
	
	context.basic_sorts=ATindexedSetCreate(63,50);
    context.defined_sorts=ATtableCreate(63,50);
    context.constructors=ATtableCreate(63,50);
    context.mappings=ATtableCreate(63,50);
    context.actions=ATtableCreate(63,50);
    context.processes=ATtableCreate(63,50);
}

void gstcContextDataDestroy(void){
	ATindexedSetDestroy(context.basic_sorts);
    ATtableDestroy(context.defined_sorts);
    ATtableDestroy(context.constructors);
    ATtableDestroy(context.mappings);
    ATtableDestroy(context.actions);
    ATtableDestroy(context.processes);
}

static ATermAppl
gstcReadIn (ATermAppl input)
{
	ATermAppl Result = input;
	gsDebugMsg ("type checking read-in fase started\n");
	
	
	
	gsDebugMsg ("type checking read-in fase finished\n");
finally:
    if (Result != NULL) {
      gsDebugMsg("return %t\n", Result);
    } else {
      gsDebugMsg("return NULL\n");
    }
    return Result;
}

static ATermAppl
gstcCheck (ATermAppl input)
{
	ATermAppl Result = input;
	gsDebugMsg ("type checking check fase started\n");


	gsDebugMsg ("type checking check fase finished\n");
finally:
    if (Result != NULL) {
      gsDebugMsg("return %t\n", Result);
    } else {
      gsDebugMsg("return NULL\n");
    }
    return Result;
}

static ATermAppl
gstcTransform (ATermAppl input)
{
	ATermAppl Result = input;
	gsDebugMsg ("type checking transform fase started\n");


	gsDebugMsg ("type checking transform fase finished\n");
finally:
    if (Result != NULL) {
      gsDebugMsg("return %t\n", Result);
    } else {
      gsDebugMsg("return NULL\n");
    }
    return Result;
}
