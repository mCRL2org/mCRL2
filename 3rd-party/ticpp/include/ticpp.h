/*
http://code.google.com/p/ticpp/
Copyright (c) 2006 Ryan Pusztai, Ryan Mulder

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
@copydoc ticpp
@file
@author		Ryan Pusztai
@author		Ryan Mulder
@date		04/11/2006

@version	0.04 Added NodeImp class. Also made all the classes inherit from NodeImp.
@version	0.03 Added Declaration class
@version	0.02 Added Element class
@version	0.01 Added Exception class, Document class

@todo add UNKNOWN support. See ticpp::NodeFactory.
@todo add TYPECOUNT support. See ticpp::NodeFactory.
@todo Add better documentation to explain the different constructors in the ticpp::Iterator class.
@todo Add a quick reference
*/
#ifdef TIXML_USE_TICPP

#ifndef TICPP_INCLUDED
#define TICPP_INCLUDED

#include "tinyxml.h"
#include <sstream>
#include <vector>
#include <memory>

/**
@subpage ticpp is a TinyXML wrapper that uses a lot more C++ ideals.
It throws exceptions, uses templates, is in its own name space, and
<b>requires</b> STL (Standard Template Library). This is done to ease the use
of getting values in and out of the xml.

If you don't prefer to use some of the concepts just don't use it.
It is just a wrapper that extends TinyXML. It doesn't actually change
any of TinyXML.
*/
namespace ticpp
{
    /**
	This is a ticpp exception class
	*/
	class Exception
	{
	public:
		Exception( const std::string& details );

		std::string m_details; /**< Exception Details */
	};

	/**
	This allows you to stream your exceptions in.
	It will take care of the conversion	and throwing the exception.
	*/
	#define THROW( message )																					\
		std::stringstream hopefullyThisNameWontConflictWithOtherVariables;										\
		std::string file( __FILE__ );																			\
		file = file.substr( file.find_last_of( "\\/" ) + 1 );													\
		hopefullyThisNameWontConflictWithOtherVariables << message << " <" << file << "@" << __LINE__ << ">";	\
		throw Exception( hopefullyThisNameWontConflictWithOtherVariables.str() );

	/** Wrapper around TiXmlBase */
	class Base
	{
	public:

		/**
		Converts any class with a proper overload of the << operator to a std::string
		@param value The value to be converted
		@throws Exception When value cannot be converted to a std::string
		*/
		template < class T >
			std::string ToString( const T& value )
		{
			std::stringstream convert;
			convert << value;
			if ( convert.fail() )
			{
				THROW( "Could not convert value to text" );
			}
			return convert.str();
		}

		std::string ToString( const std::string& value )
		{
			return value;
		}

		/**
		Converts a std::string to any class with a proper overload of the >> opertor
		@param temp			The string to be converted
		@param out	[OUT]	The container for the returned value
		@throws Exception When temp cannot be converted to the target type
		*/
		template < class T >
			void FromString( const std::string& temp, T* out )
		{
			std::istringstream val( temp );
			val >> *out;

			if ( val.fail() )
			{
				THROW( "Could not convert \"" << temp << "\" to target type" );
			}
		}

		/**
		Specialization for bool
		*/
			void FromString( const std::string& temp, bool* out )
		{
                        *out = (temp == "1" || temp == "yes" || temp == "true" || temp == "on");
		}

		/**
		Specialization for std::string
		*/
		void FromString( const std::string& temp, std::string* out )
		{
			*out = temp;
		}

		/**
		Return the position, in the original source file, of this node or attribute.
		Wrapper around TiXmlBase::Row()
		*/
		int Row()
		{
			return GetBasePointer()->Row();
		}

		/**
		Return the position, in the original source file, of this node or attribute.
		Wrapper around TiXmlBase::Row()
		*/
		int Column()
		{
			return GetBasePointer()->Column();
		}

		/**
		Destructor
		*/
		virtual ~Base()
		{
			DeleteSpawnedWrappers();
		}

	protected:
		TiCppRCImp* m_impRC;	/**< Holds status of internal TiXmlPointer - use this to determine if object has been deleted already */

		std::vector< Base* > m_spawnedWrappers; /**< Remember all wrappers that we've created with 'new' - ( e.g. NodeFactory, FirstChildElement, etc. )*/

		/**
		@internal
		Updates the pointer to the reference counter to point at the counter in the new node.

		@param node TiXmlBase containing the new reference counter
		*/
		void SetImpRC( TiXmlBase* node )
		{
			m_impRC = node->m_tiRC;
		}

		void ValidatePointer() const
		{
			if ( m_impRC->IsNull() )
			{
				THROW( "Internal TiXml Pointer is NULL" );
			}
		}

		/**
		@internal
		Delete all container objects we've spawned with 'new'.
		*/
		void DeleteSpawnedWrappers()
		{
			std::vector< Base* >::reverse_iterator wrapper;
			for ( wrapper = m_spawnedWrappers.rbegin(); wrapper != m_spawnedWrappers.rend(); ++wrapper )
			{
				delete *wrapper;
			}
			m_spawnedWrappers.clear();
		}

		/**
		@internal
		Get internal TiXmlBase*
		*/
		virtual TiXmlBase* GetBasePointer() = 0;
	};

	/**
	Wrapper around TiXmlAttribute
	*/
	class Attribute : public Base
	{
	private:
		TiXmlAttribute* m_tiXmlPointer;
		TiXmlBase* GetBasePointer()
		{
			ValidatePointer();
			return m_tiXmlPointer;
		}

	public:
		/**
		Construct an empty attribute.
		*/
		Attribute();

		/**
		Construct an attribute with @a name and @a value

		@param name The name of the attribute
		@param value The value of the attribute
		*/
		Attribute( const std::string& name, const std::string& value );

		/**
		@internal
		Construct an attribute with the internal pointer

		@param attribute The internal pointer
		*/
		Attribute( TiXmlAttribute* attribute );

		/**
		Get the value of this attribute
		Uses Base::FromString to convert TiXmlAttribute::ValueStr from a std::string,
		and puts it in the passed pointer.

		@param value [OUT] A pointer to fill with the value
		*/
		template < class T >
			void GetValue( T* value )
		{
			ValidatePointer();
			FromString( m_tiXmlPointer->ValueStr(), value );
		}

		/**
		Get the value of this attribute.
		Simple wrapper for TiXmlAttribute::ValueStr.

		@see GetValue
		*/
		std::string Value();

		/**
		Set the value of this node.
		Uses Base::ToString to convert value to a std::string, then calls TiXmlAttribute::SetValue.

		@param value The value to set
		*/
		template < class T >
			void SetValue( const T& value )
		{
			ValidatePointer();
			m_tiXmlPointer->SetValue( ToString( value ) );
		}

		/**
		Get the value of this attribute
		Uses Base::FromString to convert TiXmlAttribute::Name from a std::string,
		and puts it in the passed pointer.

		@param name [OUT] A pointer to fill with the name
		*/
		template < class T >
			void GetName( T* name )
		{
			ValidatePointer();
			FromString( m_tiXmlPointer->Name(), name );
		}

		/**
		Get the value of this attribute.
		Simple wrapper for TiXmlAttribute::Name.

		@see GetName
		*/
		std::string Name();

		/**
		Set the value of this attribute.
		Uses Base::ToString to convert @a name to a std::string, then calls TiXmlAttribute::SetName.

		@param name The name to set
		*/
		template < class T >
			void SetName( const T& name )
		{
			ValidatePointer();
			m_tiXmlPointer->SetName( ToString( name ) );
		}

		/**
		@internal
		Updates the reference count for the old and new pointers.
		*/
		void operator=( const Attribute& copy );

		/**
		@internal
		Updates the reference count for the old and new pointers.
		*/
		Attribute( const Attribute& copy );

		/*
		Decrements reference count.
		*/
		~Attribute();

		/**
		Get the next sibling attribute in the DOM.
		*/
		Attribute* Next( bool throwIfNoAttribute = true );

		/**
		Get the previous sibling attribute in the DOM.
		*/
		Attribute* Previous( bool throwIfNoAttribute = true );

		/**
		@internal
		Just for Iterator<>

		@param next [OUT] The pointer to the next valid attribute
		@return true if there is a next attribute, false if not
		*/
		void IterateNext( const std::string&, Attribute** next );

		/**
		@internal
		Just for Iterator<>

		@param previous [OUT] The pointer to the previous valid attribute
		@return true if there is a previous attribute, false if not
		*/
		void IteratePrevious( const std::string&, Attribute** previous );

		/**
		All TinyXml classes can print themselves to a filestream.
		*/
		virtual void Print( FILE* file, int depth );

	private:

		/**
		@internal
		Sets the internal pointer.
		Saves a copy of the pointer to the RC object.

		@param newPointer TiXmlAttribute* to set.
		*/
		void SetTiXmlPointer( TiXmlAttribute* newPointer );
	};

	// Forward Declaration so Node can return Element* and Document*
	class Element;
	class Document;
	class Text;
	class Comment;
	class Declaration;

	/**
	Wrapper around TiXmlNode
	*/
	class Node : public Base
	{
          friend std::istream& operator>> (std::istream & in, Node& node);
          friend std::ostream& operator<< (std::ostream & out, const Node& node);

	public:

		/**
		Get the value of this node
		Uses Base::FromString to convert TiXmlNode::ValueStr from a std::string,
		and puts it in the passed pointer.

		@param value [OUT] A pointer to fill with the value
		*/
		template < class T >
			void GetValue( T* value)
		{
			FromString( GetTiXmlPointer()->ValueStr(), value );
		}

		/**
		Get the value of this node.
		Simple wrapper for TiXmlNode::ValueStr.

		@see GetValue
		*/
		std::string Value();

		/**
		Set the value of this node.
		Uses Base::ToString to convert value to a std::string, then calls TiXmlNode::SetValue.

		@param value The value to set
		*/
		template < class T >
			void SetValue( const T& value )
		{
			GetTiXmlPointer()->SetValue( ToString( value ) );
		}

		/**
		Clear all Nodes below this.
		Simple wrapper for TiXmlNode::Clear.
		*/
		void Clear();

		/**
		The Parent of this Node.
		Simple wrapper for TiXmlNode::Parent.

		@param throwIfNoParent [DEF] If true, throws when Parent = NULL.
		@return The parent of this node, NULL if there is no Parent.
		@throws Exception When throwIfNoParent is true, and TiXmlNode::Parent returns Null.
		*/
		Node* Parent( bool throwIfNoParent = true );

		/**
		The first child of this node.

		@param throwIfNoChildren [DEF] If true, will throw an exception if there are no children.
		@return Pointer to child, Null if no children and 'throwIfNoChildren' is false.
		@throws Exception When throwIfNoChildren is true, and TiXmlNode::FirstChild returns Null.

		@see TiXmlNode::FirstChild
		*/
		Node* FirstChild( bool throwIfNoChildren = true );

		/**
		@internal
		The first child of this node with the matching @a value.

		@overload
		@param value				Value to match.
		@param throwIfNoChildren	[DEF] If true, will throw an exception if there are no children.

		@see FirstChild( bool throwIfNoChildren = true )
		*/
		Node* FirstChild( const char* value, bool throwIfNoChildren = true );

		/**
		The first child of this node with the matching @a value.

		@overload
		@param value				Value to match.
		@param throwIfNoChildren	[DEF] If true, will throw an exception if there are no children.

		@see FirstChild( const char* value, bool throwIfNoChildren = true )
		*/
		Node* FirstChild( const std::string& value, bool throwIfNoChildren = true );

		/**
		The last child of this node.

		@param throwIfNoChildren [DEF] If true, will throw an exception if there are no children.
		@return Pointer to child, Null if no children and 'throwIfNoChildren' is false.
		@throws Exception When throwIfNoChildren is true, and TiXmlNode::LastChild returns Null.

		@see TiXmlNode::LastChild
		*/
		Node* LastChild( bool throwIfNoChildren = true );

		/**
		@internal
		The last child of this node with the matching @a value.

		@overload
		@param value				Value to match.
		@param throwIfNoChildren	[DEF] If true, will throw an exception if there are no children.

		@see LastChild( bool throwIfNoChildren = true )
		*/
		Node* LastChild( const char* value, bool throwIfNoChildren = true );

		/**
		The last child of this node with the matching @a value.

		@overload
		@param value				Value to match.
		@param throwIfNoChildren	[DEF] If true, will throw an exception if there are no children.

		@see LastChild( const char* value, bool throwIfNoChildren = true )
		*/
		Node* LastChild( const std::string& value, bool throwIfNoChildren = true );

		/**
		An alternate way to walk the children of a node.
		Simple wrapper for TiXmlNode::IterateChildren.

		@param previous The previous Node* that was returned from IterateChildren.
		@return NULL When there are no more children.
		*/
		Node* IterateChildren( Node* previous );

		/**
		This flavor of IterateChildren searches for children with a particular @a value.
		Simple wrapper for TiXmlNode::IterateChildren.

		@param value	The value you want to search for.
		@param previous The previous Node* that was returned from IterateChildren.
		@return NULL When there are no more children.
		*/
		Node* IterateChildren( const std::string& value, Node* previous );

		/**
		Adds a child past the LastChild.
		Throws if you try to insert a document.

		@note This takes a copy of @a addThis so it is not as efficiant as LinkEndChild.
		@param addThis Node to insert.
		@throws Exception When TiXmlNode::InsertEndChild returns Null

		@see LinkEndChild
		@see TiXmlNode::InsertEndChild
		*/
		Node* InsertEndChild( Node& addThis );

		/**
		Adds a child past the LastChild.
		Throws if you try to link a document.

		@param childNode Node to link.
		@throws Exception When TiXmlNode::LinkEndChild returns Null.

		@see InsertEndChild
		@see TiXmlNode::LinkEndChild
		*/
		Node* LinkEndChild( Node* childNode );

		/**
		Adds a child before the specified child.
		Throws if you try to insert a document.

		@param beforeThis	Node that will have @a addThis linked before.
		@param addThis		Node to insert before.
		@throws Exception When TiXmlNode::InsertBeforeChild returns Null.

		@see InsertAfterChild
		@see TiXmlNode::InsertBeforeChild
		*/
		Node* InsertBeforeChild( Node* beforeThis, Node& addThis );

		/**
		Adds a child after the specified child.
		Throws if you try to insert a document.

		@param afterThis	Node that will have @a addThis linked after.
		@param addThis		Node to insert after.
		@throws Exception When TiXmlNode::InsertAfterChild returns Null.

		@see InsertBeforeChild
		@see TiXmlNode::InsertAfterChild
		*/
		Node* InsertAfterChild( Node* afterThis, Node& addThis );

		/**
		Replace a child of this node.
		Throws if you try to replace with a document.

		@param replaceThis	Node to replace.
		@param withThis		Node that is replacing @a replaceThis.
		@throws Exception When TiXmlNode::ReplaceChild returns Null.

		@see TiXmlNode::ReplaceChild
		*/
		Node* ReplaceChild( Node* replaceThis, Node& withThis );

		/**
		Delete a child of this node.

		@param removeThis Node to delete.
		@throws Exception When removeThis is not a child of this Node.

		@see TiXmlNode::RemoveChild
		*/
		void RemoveChild( Node* removeThis );

		/**
		Navigate to a sibling node.
		Wrapper around TiXmlNode::PreviousSibling.

		@param throwIfNoSiblings [DEF] If true, will throw an exception if there are no siblings.
		@return Pointer to sibling, Null if no siblings and 'throwIfNoSiblings' is false.
		@throws Exception When TiXmlNode::PreviousSibling returns Null and 'throwIfNoSiblings' is true.
		*/
		Node* PreviousSibling( bool throwIfNoSiblings = true );

		/**
		Navigate to a sibling node with the given @a value.

		@overload
		@param value The value of the node to look for.
		@param throwIfNoSiblings [DEF] If true, will throw an exception if there are no siblings.

		@see PreviousSibling( bool throwIfNoSiblings )
		*/
		Node* PreviousSibling( const std::string& value, bool throwIfNoSiblings = true );

		/**
		@internal
		Navigate to a sibling node with the given @a value.

		@overload
		@param value The value of the node to look for.
		@param throwIfNoSiblings [DEF] If true, will throw an exception if there are no siblings.

		@see PreviousSibling( const std::string& value, bool throwIfNoSiblings )
		*/
		Node* PreviousSibling( const char* value, bool throwIfNoSiblings = true );

		/**
		Navigate to a sibling node.
		Wrapper around TiXmlNode::NextSibling.

		@param throwIfNoSiblings [DEF] If true, will throw an exception if there are no siblings.
		@return Pointer to sibling, Null if no siblings and 'throwIfNoSiblings' is false.
		@throws Exception When TiXmlNode::NextSibling returns Null and 'throwIfNoSiblings' is true.
		*/
		Node* NextSibling( bool throwIfNoSiblings = true );

		/**
		Navigate to a sibling node with the given @a value.

		@overload
		@param value The value of the node to look for.
		@param throwIfNoSiblings [DEF] If true, will throw an exception if there are no siblings.

		@see NextSibling( bool throwIfNoSiblings )
		*/
		Node* NextSibling( const std::string& value, bool throwIfNoSiblings = true );

		/**
		@internal
		Navigate to a sibling node with the given @a value.

		@overload
		@param value The value of the node to look for.
		@param throwIfNoSiblings [DEF] If true, will throw an exception if there are no siblings.

		@see NextSibling( const std::string& value, bool throwIfNoSiblings )
		*/
		Node* NextSibling( const char* value, bool throwIfNoSiblings = true );

		/**
		@internal
		Just for Iterator<>

		@param value The value of nodes to iterate through
		@param next [OUT] The pointer to the next valid node
		*/
		template < class T >
			void IterateNext( const std::string& value, T** next )
		{
			Node* sibling = NextSibling( value, false );
			*next = dynamic_cast< T* >( sibling );

			while ( ( NULL != sibling ) && ( NULL == *next ) )
			{
				sibling = sibling->NextSibling( value, false );
				*next = dynamic_cast< T* >( sibling );
			}
		}

		/**
		@internal
		Just for Iterator<>

		@param value The value of nodes to iterate through
		@param previous [OUT] The pointer to the previous valid node
		*/
		template < class T >
			void IteratePrevious( const std::string& value, T** previous  )
		{
			Node* sibling = PreviousSibling( value, false );
			*previous = dynamic_cast< T* >( sibling );

			while ( ( NULL != sibling ) && ( NULL == *previous ) )
			{
				sibling = sibling->PreviousSibling( value, false );
				*previous = dynamic_cast< T* >( sibling );
			}
		}

		/**
		Navigate to a sibling element.
		Wrapper around TiXmlNode::NextSibling.

		@param throwIfNoSiblings [DEF] If true, will throw an exception if there are no sibling element.
		@return Pointer to sibling, Null if no siblings and 'throwIfNoSiblings' is false.
		@throws Exception When TiXmlNode::NextSibling returns Null and 'throwIfNoSiblings' is true.
		*/
		Element* NextSiblingElement( bool throwIfNoSiblings = true );

		/**
		Navigate to a sibling element with the given @a value.

		@overload
		@param value The value of the element to look for.
		@param throwIfNoSiblings [DEF] If true, will throw an exception if there are no sibling elements.
		@see NextSiblingElement( bool throwIfNoSiblings )
		*/
		Element* NextSiblingElement( const std::string& value, bool throwIfNoSiblings = true );

		/**
		@internal
		Navigate to a sibling element with the given @a value.

		@overload
		@param value The value of the element to look for.
		@param throwIfNoSiblings [DEF] If true, will throw an exception if there are no sibling elements.

		@see NextSiblingElement( const std::string& value, bool throwIfNoSiblings )
		*/
		Element* NextSiblingElement( const char* value, bool throwIfNoSiblings = true );

		/**
		The first child element of this node.

		@param throwIfNoChildren [DEF] If true, will throw an exception if there are no element children.
		@return Pointer to child, Null if no element children and 'throwIfNoChildren' is false.
		@throws Exception When throwIfNoChildren is true, and TiXmlNode::FirstChildElement returns Null.

		@see TiXmlNode::FirstChildElement
		*/
		Element* FirstChildElement( bool throwIfNoChildren = true );

		/**
		@internal
		The first child element of this node with the matching @a value.

		@overload
		@param value Value to match.
		@param throwIfNoChildren [DEF] If true, will throw an exception if there are no element children.

		@see FirstChildElement( bool throwIfNoChildren = true )
		*/
		Element* FirstChildElement( const char* value, bool throwIfNoChildren = true );

		/**
		The first child element of this node with the matching @a value.

		@overload
		@param value Value to match.
		@param throwIfNoChildren [DEF] If true, will throw an exception if there are no element children.

		@see FirstChildElement( const char* value, bool throwIfNoChildren = true )
		*/
		Element* FirstChildElement( const std::string& value, bool throwIfNoChildren = true );

		/**
		Query the type (as TiXmlNode::NodeType ) of this node.
		*/
		int Type();

		/**
		Return a pointer to the Document this node lives in.

		@param throwIfNoDocument [DEF] If true, will throw an exception if this node is not linked under a Document.
		@return A pointer to the Document this node lives in, NULL if not linked under a Document, and 'throwIfNoDocument' is false.
		@throws Exception When this node is not linked under a Document and 'throwIfNoDocument' is true.
		*/
		Document* GetDocument( bool throwIfNoDocument = true );

		/**
		Check if this node has no children.

		@return true if this node has no children.
		*/
		bool NoChildren();

		/**
		Pointer conversion ( NOT OBJECT CONVERSION ) - replaces TiXmlNode::ToElement, TiXmlNode::ToDocument, TiXmlNode::ToComment, etc.

		@throws Exception When the target is not an object of class T
		@warning Some ancient compilers do not support explicit specification of member template arguments, which this depends on ( e.g. VC6 ).
		*/
		template < class T >
			T* To()
		{
			T* pointer = dynamic_cast< T* >( this );
			if ( NULL == pointer )
			{
				std::string thisType = typeid( this ).name();
				std::string targetType = typeid( T ).name();
				std::string thatType = typeid( *this ).name();
				THROW( "The " << thisType.substr( 6 ) << " could not be casted to a " << targetType.substr( 6 )
					<< " *, because the target object is not a " << targetType.substr( 6 ) << ". (It is a " << thatType.substr( 6 ) << ")" );
			}
			return pointer;
		}

		/**
		Pointer conversion - replaces TiXmlNode::ToDocument.

		@throws Exception When this node is not a Document.
		*/
		Document* ToDocument();

		/**
		Pointer conversion - replaces TiXmlNode::ToElement.

		@throws Exception When this node is not a Element.
		*/
		Element* ToElement();

		/**
		Pointer conversion - replaces TiXmlNode::ToComment.

		@throws Exception When this node is not a Comment.
		*/
		Comment* ToComment();

		/**
		Pointer conversion - replaces TiXmlNode::ToText.

		@throws Exception When this node is not a Text.
		*/
		Text* ToText();

		/**
		Pointer conversion - replaces TiXmlNode::ToDeclaration.

		@throws Exception When this node is not a Declaration.
		*/
		Declaration* ToDeclaration();

		/**
		Create an exact duplicate of this node and return it.

		@note Using auto_ptr to manage the memory declared on the heap by TiXmlNode::Clone.
		@code
		// Now using clone
		ticpp::Document doc( "C:\\Test.xml" );
		ticpp::Node* sectionToClone;
		sectionToClone = doc.FirstChild( "settings" );
		std::auto_ptr< ticpp::Node > clonedNode = sectionToClone->Clone();
		// Now you can use the clone.
		ticpp::Node* node2 = clonedNode->FirstChildElement()->FirstChild();
		...
		// After the variable clonedNode goes out of scope it will automatically be cleaned up.
		@endcode
		@return Pointer the duplicate node.
		*/
		std::auto_ptr< Node > Clone();


	protected:
		/**
		@internal
		Allows NodeImp to use Node*'s.
		*/
		virtual TiXmlNode const* GetTiXmlPointer() const = 0;

		/**
		@internal
		Allows NodeImp to use Node*'s.
		*/
		virtual TiXmlNode* GetTiXmlPointer() = 0;

		TiXmlBase* GetBasePointer()
		{
			return GetTiXmlPointer();
		}

		/**
		@internal
		Constructs the correct child of Node, based on the Type of the TiXmlNode*.
		*/
		Node* NodeFactory( TiXmlNode* tiXmlNode, bool throwIfNull = true, bool rememberSpawnedWrapper = true );

	};

        inline std::istream& operator>> (std::istream & in, Node& node) {
          return (in >> *(node.GetTiXmlPointer()));
        }

        inline std::ostream& operator<< (std::ostream & out, const Node& node) {
          return (out << *(node.GetTiXmlPointer()));
        }

	/** Iterator for conveniently stepping through Nodes and Attributes */
	template < class T = Node >
		class Iterator
	{
	private:
		T* m_p;					/**< Internal Pointer */
		std::string m_value;	/**< Value for NextSibling  calls */

	public:

		/**
		For for loop comparisons.
		*/
		T* end()
		{
			return NULL;
		}

		/** Constructor */
		Iterator( const std::string& value = "" )
			: m_p( NULL ), m_value( value )
		{
		}

		/** Constructor */
		Iterator( T* node, const std::string& value = "" )
			: m_p( node ), m_value( value )
		{
		}

		/** Constructor */
		Iterator( const Iterator& it, const std::string& value  = "" )
			: m_p( it.m_p ), m_value( value )
		{
		}

		/**
		Gets internal pointer.
		@return The internal pointer.
		*/
		T* Get()
		{
			return m_p;
		}

		/** Sets internal pointer */
		Iterator& operator=( const Iterator& it )
		{
			m_p = it.m_p;
			return *this;
		}

		/** Sets internal pointer */
		Iterator& operator=( T* p )
		{
			m_p = p;
			return *this;
		}

		/** Sets internal pointer to the Next Sibling, or Iterator::END, if there are no more siblings */
		Iterator& operator++()
		{
			m_p->IterateNext( m_value, &m_p );
			return *this;
		}

		/** Sets internal pointer to the Next Sibling, or Iterator::END, if there are no more siblings */
		Iterator& operator++(int)
		{
			return this->operator ++();
		}

		/** Sets internal pointer to the Previous Sibling, or Iterator::END, if there are no prior siblings */
		Iterator& operator--()
		{
			m_p->IteratePrevious( m_value, &m_p );
			return *this;
		}

		/** Sets internal pointer to the Previous Sibling, or Iterator::END, if there are no prior siblings */
		Iterator& operator--(int)
		{
			return this->operator --();
		}

		/** Compares internal pointer */
		bool operator!=( T* p )
		{
			return m_p != p;
		}

		/** Compares internal pointer */
		bool operator!=( const Iterator& it )
		{
			return m_p != it.m_p;
		}

		/** Compares internal pointer* */
		bool operator==( T* p )
		{
			return m_p == p;
		}

		/** Compares internal pointer */
		bool operator==( const Iterator& it )
		{
			return m_p == it.m_p;
		}

		/** So Iterator behaves like a STL iterator */
		T* operator->()
		{
			return m_p;
		}

		/** So Iterator behaves like a STL iterator */
		T& operator*()
		{
			return *m_p;
		}
	};

	/** Implementation of Node wrapper */
	template < class T >
		class NodeImp : public Node
	{
	protected:

		T* m_tiXmlPointer;		/**< Internal pointer to the TiXml Class which is being wrapped */

		/**
		@internal
		Gets the internal TinyXML pointer.

		@returns The internal TiXmlNode*.
		*/
		TiXmlNode const* GetTiXmlPointer() const
		{
			ValidatePointer();
			return m_tiXmlPointer;
		}

		/**
		@internal
		Gets the internal TinyXML pointer.

		@returns The internal TiXmlNode*.
		*/
		TiXmlNode* GetTiXmlPointer()
		{
			ValidatePointer();
			return m_tiXmlPointer;
		}

		/**
		@internal
		Sets the internal pointer.
		Saves a copy of the pointer to the RC object.

		@param newPointer TiXmlNode* to set.
		*/
		void SetTiXmlPointer( T* newPointer )
		{
			m_tiXmlPointer = newPointer;
			SetImpRC( newPointer );
		}

		/**
		@internal
		Constructor used by child classes.
		*/
		NodeImp( T* tiXmlPointer )
		{
			// Check for NULL pointers
			if ( NULL == tiXmlPointer )
			{
				THROW( "Can not create a " << typeid( T ).name() );
			}
			SetTiXmlPointer( tiXmlPointer );
			m_impRC->IncRef();
		}

		/**
		@internal
		Updates the reference count for the old and new pointers.
		In addition, the spawnedWrappers must be cleared out before a new TiXml object is loaded in.
		*/
		virtual void operator=( const NodeImp<T>& copy )
		{
			DeleteSpawnedWrappers();

			// Dropping the reference to the old object
			this->m_impRC->DecRef();

			// Pointing to the new Object
			SetTiXmlPointer( copy.m_tiXmlPointer );

			// The internal tixml pointer changed in the above line
			this->m_impRC->IncRef();
		}

		/**
		@internal
		Updates the reference count for the old and new pointers.
		In addition, the spawnedWrappers must be cleared out before a new TiXml object is loaded in
		*/
		NodeImp( const NodeImp<T>& copy ) : Node()
		{
			DeleteSpawnedWrappers();

			// Dropping the reference to the old object
			this->m_impRC->DecRef();

			// Pointing to the new Object
			SetTiXmlPointer( copy.m_tiXmlPointer );

			// The internal tixml pointer changed in the above line
			this->m_impRC->IncRef();
		}

	public:

		/*
		Deletes the spawned wrapper objects.
		Decrements reference count.
		*/
		virtual ~NodeImp()
		{
			// The spawnedWrappers need to be deleted before m_tiXmlPointer
			DeleteSpawnedWrappers();
			m_impRC->DecRef();
		}
	};

	/** Wrapper around TiXmlComment */
	class Comment : public NodeImp< TiXmlComment >
	{
	public:

		/**
		Constructor.
		*/
		Comment();

		/**
		Constructor.
		*/
		Comment( TiXmlComment* comment );

		/**
		Constructor.
		*/
		Comment( const std::string& comment );
	};

	/** Wrapper around TiXmlText */
	class Text : public NodeImp< TiXmlText >
	{
	public:

		/**
		Constructor.
		*/
		Text();

		/**
		Constructor.
		@overload
		*/
		Text( TiXmlText* text );

		/**
		Constructor.
		@overload
		*/
		Text( const std::string& value );

		/**
		Streams value into a string and creates a Text with it.
		Uses ToString to covert the parameter to a string.

		@param value The value of the Text node.
		@throws Exception

		@see TiXmlText
        */
		template < class T >
			Text( const T& value )
				: NodeImp< TiXmlText >( new TiXmlText( ToString( value ) ) )
		{
			m_impRC->InitRef();
		}
	};

	/** Wrapper around TiXmlDocument */
	class Document : public NodeImp< TiXmlDocument >
	{
	public:
		/**
		Default	Constructor.
		Create an empty document, that has no name.
		*/
		Document();

		/**
		Constructor.
		*/
		Document( TiXmlDocument* document );

		/**
		Constructor.
		*/
		Document( const char* documentName );

		/**
		Constructor.
		Create a document with a name. The name of the document is also the filename of the xml.

		@param documentName Name to set in the Document.
		*/
		Document( const std::string& documentName );

		/**
		Write the document to a string using formatted printing ("pretty print").
		@return the document as a formatted standard string.
		*/
		void Print(std::ostream&);

		/**
		Load a file using the current document value. Throws if load is unsuccessful.

		@param encoding Sets the documents encoding.
		@see TiXmlEncoding
		@throws Exception
		*/
		void LoadFile( TiXmlEncoding encoding = TIXML_DEFAULT_ENCODING );

		/**
		Save a file using the current document value. Throws if it can't save the file.

		@throws Exception
		*/
		void SaveFile() const;

		/**
		Load a file using the given filename. Throws if load is unsuccessful.

		@param filename File to load.
		@param encoding Sets the documents encoding.
		@see TiXmlEncoding
		@throws Exception
		*/
		void LoadFile( const std::string& filename, TiXmlEncoding encoding = TIXML_DEFAULT_ENCODING );

		/**
		Save a file using the given filename. Throws if it can't save the file.

		@param filename File to save.
		@throws Exception
		*/
		void SaveFile( const std::string& filename ) const;

	};

	/** Wrapper around TiXmlElement */
	class Element : public NodeImp< TiXmlElement >
	{
	public:
		/**
		Default Constructor.
		*/
		Element();

		/**
		Default	Constructor. Initializes all the variables.
		*/
		Element( const std::string& value );

		/**
		Constructor.
		*/
		Element( TiXmlElement* element );

		/**
		Access the first attribute in this element.

		@param throwIfNoAttributes [DEF] If true, throws when there are no attributes
		@return The first attribute, NULL if there are none and @a throwIfNoAttributes is true
		*/
		Attribute* FirstAttribute( bool throwIfNoAttributes = true );

		/**
		Access the last attribute in this element.

		@param throwIfNoAttributes [DEF] If true, throws when there are no attributes
		@return The last attribute, NULL if there are none and @a throwIfNoAttributes is true
		*/
		Attribute* LastAttribute( bool throwIfNoAttributes = true );

		/**
		Sets an attribute of name to a given value.
		The attribute will be created if it does not exist, or changed if it does.
		Uses ToString to convert the @a value to a string, so there is no need to use any other SetAttribute methods.

		@see GetAttribute
		*/
		template < class T >
			void SetAttribute ( const std::string& name, const T& value )
		{
			ValidatePointer();
			m_tiXmlPointer->SetAttribute( name, ToString( value ) );
		}

		/**
		Gets the text of an Element.

		@param throwIfNotFound	[DEF]	If true, will throw an exception if there is no text in this element
		@note This only works if the Text is the FirstChild node
		@throws Exception When there is no text and throwIfNotFound is true

		@see GetText( T* value, bool throwIfNotFound = false )
		@see GetTextOrDefault
		@see GetTextOrDefault( T* value, const DefaultT& defaultValue )
		@see TiXmlElement::GetText
		*/
		std::string GetText( bool throwIfNotFound = true )
		{
			// Get the element's text value as a std::string
			std::string temp;
			if ( !GetTextImp( &temp ) )
			{
				if ( throwIfNotFound )
				{
					THROW( "Text does not exists in the current element" );
				}
			}

			return temp;
		}

		/**
		Gets the text of an Element, if it doesn't exist it will return the defaultValue.

		@param defaultValue			What to put in 'value' if there is no text in this element
		@note This only works if the Text is the FirstChild node

		@see GetText
		@see GetText( T* value, bool throwIfNotFound = false )
		@see GetTextOrDefault( T* value, const DefaultT& defaultValue )
		@see TiXmlElement::GetText
		*/
		std::string GetTextOrDefault( const std::string& defaultValue )
		{
			// Get the element's text value as a std::string
			std::string temp;
			if ( !GetTextImp( &temp ) )
			{
				return defaultValue;
			}

			return temp;
		}

		/**
		Gets the text value of an Element, if it doesn't exist it will return the defaultValue.
		Uses FromString to convert the string to the type of choice

		@param value		[OUT]	The container for the returned value
		@param defaultValue			What to put in 'value' if there is no text in this element
		@note This is different than GetText() in that it will covert the text to what ever type you want.
		@note This only works if the Text is the FirstChild node

		@see GetText
		@see GetText( T* value, bool throwIfNotFound = false )
		@see GetTextOrDefault( const std::string& defaultValue )
		@see TiXmlElement::GetText
		*/
		template < class T, class DefaultT >
			void GetTextOrDefault( T* value, const DefaultT& defaultValue )
		{
			// Get the element's text value as a std::string
			std::string temp;
			if ( !GetTextImp( &temp ) )
			{
				// The text value does not exist - set value to the default
				*value = defaultValue;
				return;
			}

			// Stream the value from the string to T
			FromString( temp, value );
		}

		/**
		Gets the text of an Element.
		Uses FromString to convert the string to the type of choice.

		@param value			[OUT]	The container for the returned value
		@param throwIfNotFound	[DEF]	If true, will throw an exception if there is no text in this element
		@note This is different than GetText() in that it will covert the text to what ever type you want
		@note This only works if the Text is the FirstChild node
		@throws Exception When there is no text and throwIfNotFound is true

		@see GetText
		@see GetTextOrDefault
		@see GetTextOrDefault( T* value, const DefaultT& defaultValue )
		@see TiXmlElement::GetText
		*/
		template< class T >
			void GetText( T* value, bool throwIfNotFound = true )
		{
			// Get the element's text value as a std::string
			std::string temp;
			if ( !GetTextImp( &temp ) )
			{
				if ( throwIfNotFound )
				{
					THROW( "Text does not exists in the current element" );
				}
				else
				{
					return;
				}
			}

			// Stream the value from the string to T
			FromString( temp, value );
		}

		/**
		Convenience function to set the text of an element.
		Creates a Text node and inserts it as the first child.
		Uses ToString to convert the parameter to a string.

		@param value The text to set.
		*/
		template < class T >
			void SetText( const T& value )
		{
			ValidatePointer();
			std::string temp = ToString( value );

			if ( m_tiXmlPointer->NoChildren() )
			{
				m_tiXmlPointer->LinkEndChild( new TiXmlText( temp ) );
			}
			else
			{
				if ( NULL == m_tiXmlPointer->GetText() )
				{
					m_tiXmlPointer->InsertBeforeChild( m_tiXmlPointer->FirstChild(), TiXmlText( temp ) );
				}
				else
				{
					// There already is text, so change it
					m_tiXmlPointer->FirstChild()->SetValue( temp );
				}
			}
		}

		/**
		Gets an attribute of @a name from an element, if it doesn't exist it will return the defaultValue.
		Uses FromString to convert the string to the type of choice.

		@param name			The name of the attribute you are querying.
		@param value		[OUT] The container for the returned value.
		@param defaultValue	What to put in @a value if there is no attribute in this element.
		@throws Exception

		@see GetAttribute
		*/
		template < class T, class DefaulT >
			void GetAttributeOrDefault( const std::string& name, T* value, const DefaulT& defaultValue )
		{
			// Get the attribute's value as a std::string
			std::string temp;
			if ( !GetAttributeImp( name, &temp ) )
			{
				// The attribute does not exist - set value to the default
				*value = defaultValue;
				return;
			}

			// Stream the value from the string to T
			FromString( temp, value );
		}

		/**
		Gets an attribute of @a name from an element.
		Uses FromString to convert the string to the type of choice.

		@param name				The name of the attribute you are querying.
		@param value			[OUT]	The container for the returned value
		@param throwIfNotFound	[DEF]	If true, will throw an exception if the attribute doesn't exist
		@throws Exception When the attribute doesn't exist and throwIfNotFound is true

		@see GetAttributeOrDefault
		*/
		template< class T >
			void GetAttribute( const std::string& name, T* value, bool throwIfNotFound = true )
		{
			// Get the attribute's value as a std::string
			std::string temp;
			if ( !GetAttributeImp( name, &temp ) )
			{
				if ( throwIfNotFound )
				{
					THROW( "Attribute does not exist" );
				}
				else
				{
					return;
				}
			}

			// Stream the value from the string to T
			FromString( temp, value );
		}

		/**
		Gets an attribute of @a name from an element.
		Uses FromString to convert the string to the type of choice.

		@param name				The name of the attribute you are querying.
		@param throwIfNotFound	[DEF]	If true, will throw an exception if the attribute doesn't exist
		@throws Exception When the attribute doesn't exist and throwIfNotFound is true

		*/
		std::string GetAttributeValue( const std::string& name, bool throwIfNotFound = true )
		{
			// Get the attribute's value as a std::string
			std::string temp;
			if ( !GetAttributeImp( name, &temp ) )
			{
				if ( throwIfNotFound )
				{
					THROW( "Attribute does not exist" );
				}
			}

                        return (temp);
		}

		/**
		Gets an attribute of @a name from an element.
		Uses FromString to convert the string to the type of choice.

		@param name				The name of the attribute you are querying.
		@param defaultValue	What to put in @a value if there is no attribute in this element.
		@throws Exception When the attribute doesn't exist and throwIfNotFound is true

		*/
		std::string GetAttributeValueOrDefault( const std::string& name, std::string defaultValue )
		{
			// Get the attribute's value as a std::string
			std::string temp;
			if ( !GetAttributeImp( name, &temp ) )
			{
                                return (defaultValue);
			}

                        return (temp);
		}
	private:

		/**
		@internal
		Implementation of the GetAttribute and GetAttributeOrDefault template methods.
		*/
		bool GetAttributeImp( const std::string& name, std::string* value );

		/**
		@internal
		Implementation of the GetText, GetTextOrDefault, GetTextValue, and GetTextValueOrDefault template methods.
		*/
		bool GetTextImp( std::string* value );
	};

	/** Wrapper around TiXmlDeclaration */
	class Declaration : public NodeImp< TiXmlDeclaration >
	{
	public:
		/**
		Default	Constructor. Construct an empty declaration.
		*/
		Declaration( void );

		/**
		Constructor.
		*/
		Declaration( TiXmlDeclaration* declaration );

		/**
		Constructor.
		*/
		Declaration( const std::string& version, const std::string& encoding, const std::string& standalone );

		/**
		Version. Will return an empty string if none was found.
		*/
		std::string Version( void );

		/**
		Encoding. Will return an empty string if none was found.
		*/
		std::string Encoding( void );

		/**
		StandAlone. Is this a standalone document?
		*/
		std::string Standalone( void );
	};
}

#endif	// TICPP_INCLUDED

#endif // TIXML_USE_TICPP
