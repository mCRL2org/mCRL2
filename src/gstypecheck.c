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
     ATermTable constants;			//name -> Set(sort expression)
     ATermTable functions;		    //name -> Set(sort expression)
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
static inline ATbool gstcInTypes(ATermAppl, ATermList);
static ATbool gstcEqTypes(ATermAppl, ATermAppl);

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
    ATprotect((ATerm* )&context.constants);
    ATprotect((ATerm* )&context.functions);
    ATprotect((ATerm* )&context.actions);	
    ATprotect((ATerm* )&context.processes);
	
	context.basic_sorts=ATindexedSetCreate(63,50);
    context.defined_sorts=ATtableCreate(63,50);
    context.constants=ATtableCreate(63,50);
    context.functions=ATtableCreate(63,50);
    context.actions=ATtableCreate(63,50);
    context.processes=ATtableCreate(63,50);
}

void gstcContextDataDestroy(void){
	ATindexedSetDestroy(context.basic_sorts);
    ATtableDestroy(context.defined_sorts);
    ATtableDestroy(context.constants);
    ATtableDestroy(context.functions);
    ATtableDestroy(context.actions);
    ATtableDestroy(context.processes);
}

static ATermAppl
gstcReadIn (ATermAppl input){	
	ATbool new;
	ATermAppl Result = input;
	gsDebugMsg ("type checking read-in fase started\n");
	
	// Read-in sorts
	ATermList Sorts = ATLgetArgument(ATAgetArgument(Result,0),0);
	for(;!ATisEmpty(Sorts);Sorts=ATgetNext(Sorts)){
		ATermAppl Sort=ATAgetFirst(Sorts);
		ATermAppl SortName=ATAgetArgument(Sort,0);
		if(ATindexedSetGetIndex(context.basic_sorts, (ATerm)SortName)>=0 
			|| ATAtableGet(context.defined_sorts, (ATerm)SortName)!=NULL){
				ThrowM("Double declaration of sort %t\n", SortName);
		}				
		if(gsIsSortId(Sort)) ATindexedSetPut(context.basic_sorts, (ATerm)SortName, &new);
		else
		if(gsIsSortRef(Sort)) ATtablePut(context.defined_sorts, (ATerm)SortName, (ATerm)ATAgetArgument(Sort,1));
	}
	
	// Check soorts for loops
	// Unwind sorts to enable equiv and subtype relations
	
	//gsWarningMsg("Basic: %t\n Defined: %t\n", ATindexedSetElements(context.basic_sorts), ATtableKeys(context.defined_sorts));

    // Read-in functions and constants
	ATermList Funcs = ATconcat(ATLgetArgument(ATAgetArgument(Result,1),0),ATLgetArgument(ATAgetArgument(Result,2),0));
	for(;!ATisEmpty(Funcs);Funcs=ATgetNext(Funcs)){
		ATermAppl Func=ATAgetFirst(Funcs);
		ATermAppl FuncName=ATAgetArgument(Func,0);
		ATermAppl FuncType=ATAgetArgument(Func,1);
		bool is_constant=!gsIsSortArrowProd(FuncType);
		
		if(ATAtableGet(context.constants, (ATerm)FuncName)!=NULL){
			ThrowM("Double declaration of constant %t\n", FuncName);
		}
		ATermList Types=ATLtableGet(context.functions, (ATerm)FuncName);
		if(is_constant && Types!=NULL){
			ThrowM("Double declaration of constant %t\n", FuncName);
		}
		if(is_constant){
			ATtablePut(context.constants, (ATerm)FuncName, (ATerm)FuncType);
		}
		else{
			// the table context.functions contains a list of types for each
			// function name. We need to check if there is already such a type 
			// in the list. If so -- error, otherwise -- add
			if (Types!=NULL && gstcInTypes(FuncType, Types)){
				ThrowM("Double declaration of constant %t\n", FuncName);
			}
			else{
				Types=(Types==NULL)?ATmakeList1((ATerm)FuncType):ATinsert(Types,(ATerm)FuncType);
				ATtablePut(context.functions, (ATerm)FuncName, (ATerm)Types);
			}
		}	
	}	
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

static inline ATbool gstcInTypes(ATermAppl Type, ATermList Types){
	for(;!ATisEmpty(Types);Types=ATgetNext(Types))
    	if(gstcEqTypes(ATAgetFirst(Types),Type)) return ATtrue;
	return ATfalse;
}

static ATbool gstcEqTypes(ATermAppl Type1, ATermAppl Type2){
	return ATfalse;
}
