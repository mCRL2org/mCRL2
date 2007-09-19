// ---------------------------------------------------------------------------
// CArray.h - C++ File
// Copyright ©2005 Michael B. Comet  All Rights Reserved
// ---------------------------------------------------------------------------
//
// DESCRIPTION:
//	The header for template for dynamic arrays like Maya...
//
// USAGE:
//	Usually it is a good idea to simplify using templates by using a typedef.
//	For example if you wanted to make an array of ints....
//
//			typedef CArray<int> CIntArray ;
//
//  Then you can make variables like:      CIntArray theArr ;		 // Create object
//										   theArr.setLength(100) ;   // grow the array
//
//  and so on.  Of course Maya already has an MIntArray, but this way you can 
//  create similar array objects of your own classes, or of classes that Maya didn't
//	provide it for, like matrices:
//
//			typedef CArray<MMatrix> MMatrixArray ;
//
// ***note if you use this with your own classes, you will have to make sure your
//	class implements the "friend ostream& operator<<" proc so that the array
//	template can properly do a cout or such on the element.
//
//
// AUTHOR:
//	Michael B. Comet - comet@comet-cartoons.com
//
// VERSIONS:
//	10/15/05 - comet - Initial Rev
// 
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------

#ifndef CCARRAY_H
#define CCARRAY_H 1

#include <iostream>
using namespace::std ;

// ---------------------------------------------------------------------------

template <class TYPE>
class CArray
{
private:
	TYPE		*ptr ;		// ptr to array of data
	unsigned	uEleAlloc ;	// actual size we currently are 
	unsigned	uEle ;		// number of elements that appears to user
	unsigned	uGrowSize ;	// How much to grow each time min

public:
	CArray() 							// Blank constructor
		{
		ptr = NULL ;  uEleAlloc = uEle = 0 ;  uGrowSize = 64 ;
		}

	CArray(CArray &other)				// Copy constructor
		{ 
		ptr = NULL ;  uEleAlloc = uEle = 0 ;  uGrowSize = 64 ;
		copy(other); 
		}
	
	CArray( const TYPE *src, unsigned count)	// Copy by pointer constructor
		{
		ptr = NULL ;  uEleAlloc = uEle = 0 ;  uGrowSize = 64 ;
		setLength(count) ;
		unsigned u ;
		for (u=0; u < uEle; ++u)
			ptr[u] = src[u] ;
		}
	CArray( unsigned initialSize, TYPE initialValue) 	// Initialization constructor
		{
		ptr = NULL ;  uEleAlloc = uEle = 0 ;  uGrowSize = 64 ;
		setLength(initialSize) ;
		unsigned u ;
		for (u=0; u < uEle; ++u)
			ptr[u] = initialValue ;
		}
	~CArray()							// Destructor
		{
		clear() ; 
		}


	void clear(void) ;		// free stuff

	bool setLength(unsigned length) ;		// Set to new size
	unsigned length(void)					// Return cur length
		{ 
		return uEle; 
		} 

	bool set(TYPE data, unsigned index) ;		// Set a given element.  Does bounds checking but not growing.
	bool get(TYPE &data, unsigned index) ;		// Get a given element.  Does bounds checking but not growing.
	bool get(TYPE *data) ;						// Copies all elements to the given pointer of type.
	bool remove(unsigned index) ;				// Remove a given element at the provided index
	bool insert(TYPE data, unsigned index) ;	// Insert a given element at the provided index
	bool append(TYPE data) ;					// Append data to end of array, growing array by 1.

	bool copy(CArray &source)			// Copy all the elements from source to this array
		{
		clear() ;
		unsigned uLen = source.length() ;
		setLength( uLen );
		unsigned u;
		for (u=0; u < uEle; ++u)
			ptr[u] = source.ptr[u] ;
		return true ;
		}

	void setSizeIncrement( unsigned newIncrement) // Set grow size
		{
		uGrowSize = newIncrement ;
		if (uGrowSize < 1)
			uGrowSize = 1 ;
		}
	unsigned sizeIncrement(void) const			// Get grow size
		{
		return uGrowSize ;
		}

	const TYPE& operator[](unsigned index) const 	// No bounds check, get element
		{ 
		return ptr[index]; 
		}
	
	TYPE& operator[](unsigned index)				// No bounds check, set element
		{ 
		return ptr[index]; 
		}	

	CArray& operator=(CArray &other)		// Copy elements from other to this array
		{
		copy(other) ;
		return *this ;
		}

	friend ostream& operator<<(ostream &os, const CArray &arr)
		{
		os << "(length=" << arr.uEle << ") [" ;
		unsigned u ;
		for (u=0; u < arr.uEle; ++u)
			{
			os << arr.ptr[u] ;
			if (u < arr.uEle-1)
				os << ", " ;
			else
				os << "]" ;
			}
		return os ;
		}

} ;

// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//	CArray Class Implementation
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------

/*
 * void CArray<TYPE>::clear() - free mem
 */
template <class TYPE>
void CArray<TYPE>::clear(void)
{
	if (ptr != NULL)
		delete [] ptr ;
	ptr = NULL ;
	uEleAlloc = 0 ;
	uEle = 0 ;
}


// ---------------------------------------------------------------------------

/*
 * CArray<type>::setLength() - Set size of array.  Returns true on success
 */
template <class TYPE>
bool CArray<TYPE>::setLength(unsigned length)
{
	if (length == 0)
		{
		clear() ;
		return true ;
		}

	// Alloc new storage
	TYPE *newptr=NULL ;
	unsigned uNewEleAlloc = (( length / uGrowSize) + 1) * uGrowSize ;

	// Now see if we need to alloc new, and do so only if needed!
	// Needed means either the amt we need is greater than what we have already
	// ...or if the amt is half or less than half, to free RAM we'll realloc smaller
	//
	if (uNewEleAlloc > uEleAlloc || uNewEleAlloc <= (uEleAlloc / 2) )
		{
		newptr = new TYPE [uNewEleAlloc] ;

		if (newptr == NULL)		// memory alloc error!
			return false ;

		// Now copy old elements over to new array.
		// Up the the old amt or user set new length, whichever is smaller.
		unsigned u;
		for (u=0; u < uEle && u < length; ++u)
			{
			newptr[u] = ptr[u] ;
			}

		// Update all the current info...
		if (ptr != NULL)
			delete [] ptr ;
		ptr = newptr ;
		uEleAlloc = uNewEleAlloc ;
		}

	uEle = length ;

	return true ;
}


// ---------------------------------------------------------------------------

/*
 * CArray<TYPE>::set() - Set a given element.  Does bounds checking but not growing.
 */
template <class TYPE>
bool CArray<TYPE>::set(TYPE data, unsigned index)
{
	if (index >= uEle || ptr == NULL)
		return false ;

	ptr[index] = data ;

	return true ;
}

// ---------------------------------------------------------------------------

/*
 * CArray<TYPE>::get() - Get a given element.  Does bounds checking but not growing.
 */
template <class TYPE>
bool CArray<TYPE>::get(TYPE &data, unsigned index)
{
	if (index >= uEle || ptr == NULL)
		return false ;

	data = ptr[index] ;

	return true ;
}

// ---------------------------------------------------------------------------

/*
 * CArray<TYPE>::get() - Copies all elements to the given pointer of type.  Ptr should be big enough to hold arr.
 */
template <class TYPE>
bool CArray<TYPE>::get(TYPE *data) 
{
	if (data == NULL)
		return false ;

	unsigned u ;
	for (u=0; u < uEle; ++u)
		data[u] = ptr[u] ;

	return true ;

}


// ---------------------------------------------------------------------------

/*
 * CArray<TYPE>::remove() - Remove a given element at the provided index
 */
template <class TYPE>
bool CArray<TYPE>::remove(unsigned index) 
{
	if (uEle == 0 || ptr == NULL)
		return false ;

	// Now starting with the element we are removing, work up
	// coping each next value down to the current spot...
	//
	unsigned u ;
	for (u=index; u < uEle-1 ; ++u)
		{
		ptr[u] = ptr[u+1] ;	
		}

	// Now just setLength to 1 less than we had.  This will either
	// simply change the uEle value, or if we shrunk enough, realloc and 
	// free some ram.
	setLength( uEle-1 ) ;

	return true ;
}

// ---------------------------------------------------------------------------

/*
 * CArray<TYPE>::insert() - insert a given element at the provided index
 */
template <class TYPE>
bool CArray<TYPE>::insert(TYPE data, unsigned index) 
{
	// First setLength up one so we have room
	setLength( uEle+1 ) ;

	// Now starting with the last element work back 
	// until we get to the one we are inserting at and copy forward.
	//
	unsigned u ;
	for (u=uEle-1; u > index; --u)
		{
		ptr[u] = ptr[u-1] ;	
		}

	// Finally insert new value
	ptr[index] = data ;

	return true ;
}

// ---------------------------------------------------------------------------


/*
 * CArray<TYPE>::append() - Append data to end of array, growing array by 1.
 */
template <class TYPE>
bool CArray<TYPE>::append(TYPE data) 
{
	// First setLength up one so we have room
	setLength( uEle+1 ) ;

	// Finally insert new value
	ptr[uEle-1] = data ;

	return true ;
}

// ---------------------------------------------------------------------------

#endif // CCARRAY_H



