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

// Static functions declaration
static ATermAppl gstcReadIn (ATermAppl);
static ATermAppl gstcCheck (ATermAppl);
static ATermAppl gstcTransform (ATermAppl);

// Main function
ATermAppl
gsTypeCheck (ATermAppl input)
{	
	ATermAppl Result = input;
	gsVerboseMsg ("type checking fase started\n");

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
    return Result;
}

// Static functions
static ATermAppl
gstcReadIn (ATermAppl input)
{
	ATermAppl Result = input;
	gsDebugMsg ("type checking read-in fase started\n");
	
	Result=NULL;
	
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
