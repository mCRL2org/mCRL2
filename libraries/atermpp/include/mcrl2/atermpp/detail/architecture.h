#if !defined(AT_64BIT) && !defined(AT_32BIT)
#pragma message ( "Missing architecture: Guessing architecture..." )
#pragma message ( "* Define AT_32BIT for i386 architecture." )
#pragma message ( "* Define AT_64BIT for x86_x64 architecture." )

/* Covers gcc, icc, msvc and Solaris cc */  	 	 
#if defined(__LP64__) || defined(_LP64) || defined(__lp64) || \
     defined(_ADDR64) || defined(__arch64__) || defined(_M_X64) || \
     defined(_M_IA64) || defined(WIN64)  	 	 
#pragma message ( "Detected x86_x64 architecture." )
#  define AT_64BIT
#else
#pragma message ( "Detected i386 architecture." )
#  define AT_32BIT
#endif    	 	 

#endif
