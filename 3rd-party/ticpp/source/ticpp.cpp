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

#ifdef TIXML_USE_TICPP

#include "ticpp.h"
#include "ticpprc.h"
#include "tinyxml.h"
#include <sstream>

using namespace ticpp;

Attribute::Attribute()
{
	SetTiXmlPointer( new TiXmlAttribute() );
	m_impRC->InitRef();
}

Attribute::Attribute( TiXmlAttribute* attribute )
{
	SetTiXmlPointer( attribute );
	m_impRC->IncRef();
}

Attribute::Attribute( const std::string& name, const std::string& value )
{
	SetTiXmlPointer( new TiXmlAttribute( name, value ) );
	m_impRC->InitRef();
}

void Attribute::operator=( const Attribute& copy )
{
	// Dropping the reference to the old object
	this->m_impRC->DecRef();

	// Pointing to the new Object
	SetTiXmlPointer( copy.m_tiXmlPointer );

	// The internal tixml pointer changed in the above line
	this->m_impRC->IncRef();
}

Attribute::Attribute( const Attribute& copy ) : Base()
{
	// Dropping the reference to the old object
	this->m_impRC->DecRef();

	// Pointing to the new Object
	SetTiXmlPointer( copy.m_tiXmlPointer );

	// The internal tixml pointer changed in the above line
	this->m_impRC->IncRef();
}

Attribute::~Attribute()
{
	m_impRC->DecRef();
}

std::string Attribute::Value()
{
	ValidatePointer();
	return m_tiXmlPointer->ValueStr();
}

std::string Attribute::Name()
{
	ValidatePointer();
	return m_tiXmlPointer->Name();
}

Attribute* Attribute::Next( bool throwIfNoAttribute )
{
	ValidatePointer();
	TiXmlAttribute* attribute = m_tiXmlPointer->Next();
	if ( NULL == attribute )
	{
		if ( throwIfNoAttribute )
		{
			THROW( "No more attributes found" )
		}
		else
		{
			return NULL;
		}
	}

	Attribute* temp = new Attribute( attribute );
	m_spawnedWrappers.push_back( temp );

	return temp;
}

Attribute* Attribute::Previous( bool throwIfNoAttribute )
{
	ValidatePointer();
	TiXmlAttribute* attribute = m_tiXmlPointer->Previous();
	if ( NULL == attribute )
	{
		if ( throwIfNoAttribute )
		{
			THROW( "No more attributes found" )
		}
		else
		{
			return NULL;
		}
	}

	Attribute* temp = new Attribute( attribute );
	m_spawnedWrappers.push_back( temp );

	return temp;
}

void Attribute::IterateNext( const std::string&, Attribute** next )
{
	*next = Next( false );
}

void Attribute::IteratePrevious( const std::string&, Attribute** previous )
{
	*previous = Previous( false );
}

void Attribute::Print( FILE* file, int depth )
{
	ValidatePointer();
	m_tiXmlPointer->Print( file, depth );
}

void Attribute::SetTiXmlPointer( TiXmlAttribute* newPointer )
{
	m_tiXmlPointer = newPointer;
	SetImpRC( newPointer );
}

//*****************************************************************************

Node* Node::NodeFactory( TiXmlNode* tiXmlNode, bool throwIfNull, bool rememberSpawnedWrapper )
{
	if ( NULL == tiXmlNode )
	{
		if ( throwIfNull )
		{
			THROW( "tiXmlNode is NULL" )
		}
		else
		{
			return NULL;
		}
	}

	Node* temp;
	switch ( tiXmlNode->Type() )
	{
		case TiXmlNode::DOCUMENT:
			temp = new Document( tiXmlNode->ToDocument() );
			break;

		case TiXmlNode::ELEMENT:
			temp = new Element( tiXmlNode->ToElement() );
			break;

		case TiXmlNode::COMMENT:
			temp = new Comment( tiXmlNode->ToComment() );
			break;

		case TiXmlNode::TEXT:
			temp = new Text( tiXmlNode->ToText() );
			break;

		case TiXmlNode::DECLARATION:
			temp = new Declaration( tiXmlNode->ToDeclaration() );
			break;

		default:
			THROW( "Type is unsupported" )
	}

	if ( rememberSpawnedWrapper )
	{
		m_spawnedWrappers.push_back( temp );
	}
	return temp;
}


std::string Node::Value()
{
	return GetTiXmlPointer()->ValueStr();
}

void Node::Clear()
{
	GetTiXmlPointer()->Clear();
}

Node* Node::Parent( bool throwIfNoParent )
{
	TiXmlNode* parent = GetTiXmlPointer()->Parent();
	if ( ( NULL == parent ) && throwIfNoParent )
	{
		THROW( "No parent exists" );
	}

	return NodeFactory( parent, false );
}

Node* Node::FirstChild( bool throwIfNoChildren )
{
	return FirstChild( "", throwIfNoChildren );
}

Node* Node::FirstChild( const std::string& value, bool throwIfNoChildren )
{
	return FirstChild( value.c_str(), throwIfNoChildren );
}

Node* Node::FirstChild( const char* value, bool throwIfNoChildren )
{
	TiXmlNode* childNode;
	if ( 0 == strlen( value ) )
	{
		childNode = GetTiXmlPointer()->FirstChild();
	}
	else
	{
		childNode = GetTiXmlPointer()->FirstChild( value );
	}

	if ( ( NULL == childNode ) && throwIfNoChildren )
	{
		THROW( "Child with the value of \"" << value << "\" not found" );
	}

	return NodeFactory( childNode, false );
}

Node* Node::LastChild( bool throwIfNoChildren )
{
	return LastChild( "", throwIfNoChildren );
}

Node* Node::LastChild( const std::string& value, bool throwIfNoChildren )
{
	return LastChild( value.c_str(), throwIfNoChildren );
}

Node* Node::LastChild( const char* value, bool throwIfNoChildren )
{
	TiXmlNode* childNode;
	if ( 0 == strlen( value ) )
	{
		childNode = GetTiXmlPointer()->LastChild();
	}
	else
	{
		childNode = GetTiXmlPointer()->LastChild( value );
	}

	if ( ( NULL == childNode ) && throwIfNoChildren )
	{
		THROW( "Child with the value of \"" << value << "\" not found" );
	}

	return NodeFactory( childNode, false );
}

Node* Node::IterateChildren ( Node* previous )
{
	TiXmlNode* pointer;
	if ( NULL == previous )
	{
		pointer = GetTiXmlPointer()->IterateChildren( NULL );
	}
	else
	{
		pointer = GetTiXmlPointer()->IterateChildren( previous->GetTiXmlPointer() );
	}

	return NodeFactory( pointer, false );
}

Node* Node::IterateChildren( const std::string& value, Node* previous )
{
	TiXmlNode* pointer;
	if ( NULL == previous )
	{
		pointer = GetTiXmlPointer()->IterateChildren( value, NULL );
	}
	else
	{
		pointer = GetTiXmlPointer()->IterateChildren( value, previous->GetTiXmlPointer() );
	}

	return NodeFactory( pointer, false );
}

Node* Node::InsertEndChild( Node& addThis )
{
	if ( addThis.Type() == TiXmlNode::DOCUMENT )
	{
		THROW( "Node is a Document and can't be inserted" );
	}

	// Increment reference count when adding to the tree
	addThis.m_impRC->IncRef();

	TiXmlNode* pointer = GetTiXmlPointer()->InsertEndChild( *addThis.GetTiXmlPointer() );
	if ( NULL == pointer )
	{
		THROW( "Node can't be inserted" );
	}

	return NodeFactory( pointer );
}

Node* Node::LinkEndChild( Node* childNode )
{
	if ( childNode->Type() == TiXmlNode::DOCUMENT )
	{
		THROW( "Node is a Document and can't be linked" );
	}

	// Increment reference count when adding to the tree
	childNode->m_impRC->IncRef();

	if ( NULL == GetTiXmlPointer()->LinkEndChild( childNode->GetTiXmlPointer() ) )
	{
		THROW( "Node can't be linked" );
	}

	return childNode;
}

Node* Node::InsertBeforeChild( Node* beforeThis, Node& addThis )
{
	if ( addThis.Type() == TiXmlNode::DOCUMENT )
	{
		THROW( "Node is a Document and can't be inserted" );
	}

	// Increment reference count when adding to the tree
	addThis.m_impRC->IncRef();

	TiXmlNode* pointer = GetTiXmlPointer()->InsertBeforeChild( beforeThis->GetTiXmlPointer(), *addThis.GetTiXmlPointer() );
	if ( NULL == pointer )
	{
		THROW( "Node can't be inserted" );
	}

	return NodeFactory( pointer );
}

Node* Node::InsertAfterChild( Node* afterThis, Node& addThis )
{
	if ( addThis.Type() == TiXmlNode::DOCUMENT )
	{
		THROW( "Node is a Document and can't be inserted" );
	}

	// Increment reference count when adding to the tree
	addThis.m_impRC->IncRef();

	TiXmlNode* pointer = GetTiXmlPointer()->InsertAfterChild( afterThis->GetTiXmlPointer(), *addThis.GetTiXmlPointer() );
	if ( NULL == pointer )
	{
		THROW( "Node can't be inserted" );
	}

	return NodeFactory( pointer );
}

Node* Node::ReplaceChild( Node* replaceThis, Node& withThis )
{
	if ( withThis.Type() == TiXmlNode::DOCUMENT )
	{
		THROW( "Node is a Document and can't be inserted" );
	}

	// Increment reference count when adding to the tree
	withThis.m_impRC->IncRef();

	TiXmlNode* pointer = GetTiXmlPointer()->ReplaceChild( replaceThis->GetTiXmlPointer(), *withThis.GetTiXmlPointer() );
	if ( NULL == pointer )
	{
		THROW( "Node can't be inserted" );
	}

	return NodeFactory( pointer );
}

void Node::RemoveChild( Node* removeThis )
{
	if  ( !GetTiXmlPointer()->RemoveChild( removeThis->GetTiXmlPointer() ) )
	{
		THROW( "Node to remove (" << removeThis->Value() << ") is not a child of this Node (" << Value() << ")" )
	}
}

Node* Node::PreviousSibling( bool throwIfNoSiblings )
{
	return PreviousSibling( "", throwIfNoSiblings );
}

Node* Node::PreviousSibling( const std::string& value, bool throwIfNoSiblings )
{
	return PreviousSibling( value.c_str(), throwIfNoSiblings );
}

Node* Node::PreviousSibling( const char* value, bool throwIfNoSiblings )
{
	TiXmlNode* sibling;
	if ( 0 == strlen( value ) )
	{
		sibling = GetTiXmlPointer()->PreviousSibling();
	}
	else
	{
		sibling = GetTiXmlPointer()->PreviousSibling( value );
	}

	if ( ( NULL == sibling ) && throwIfNoSiblings )
	{
		THROW( "No Siblings found with value, '" << value << "', Prior to this Node (" << Value() << ")" )
	}

	return NodeFactory( sibling, false );
}

Node* Node::NextSibling( bool throwIfNoSiblings )
{
	return NextSibling( "", throwIfNoSiblings );
}

Node* Node::NextSibling( const std::string& value, bool throwIfNoSiblings )
{
	return NextSibling( value.c_str(), throwIfNoSiblings );
}

Node* Node::NextSibling( const char* value, bool throwIfNoSiblings )
{
	TiXmlNode* sibling;
	if ( 0 == strlen( value ) )
	{
		sibling = GetTiXmlPointer()->NextSibling();
	}
	else
	{
		sibling = GetTiXmlPointer()->NextSibling( value );
	}

	if ( ( NULL == sibling ) && throwIfNoSiblings )
	{
		THROW( "No Siblings found with value, '" << value << "', After this Node (" << Value() << ")" )
	}

	return NodeFactory( sibling, false );
}

Element* Node::NextSiblingElement( bool throwIfNoSiblings )
{
	return NextSiblingElement( "", throwIfNoSiblings );
}

Element* Node::NextSiblingElement( const std::string& value, bool throwIfNoSiblings )
{
	return NextSiblingElement( value.c_str(), throwIfNoSiblings );
}

Element* Node::NextSiblingElement( const char* value, bool throwIfNoSiblings )
{
	TiXmlElement* sibling;
	if ( 0 == strlen( value ) )
	{
		sibling = GetTiXmlPointer()->NextSiblingElement();
	}
	else
	{
		sibling = GetTiXmlPointer()->NextSiblingElement( value );
	}

	if ( NULL == sibling )
	{
		if ( throwIfNoSiblings )
		{
			THROW( "No Element Siblings found with value, '" << value << "', After this Node (" << Value() << ")" )
		}
		else
		{
			return NULL;
		}
	}

	Element* temp = new Element( sibling );
	m_spawnedWrappers.push_back( temp );

	return temp;
}

Element* Node::FirstChildElement( bool throwIfNoChildren )
{
	return FirstChildElement( "", throwIfNoChildren );
}

Element* Node::FirstChildElement( const std::string& value, bool throwIfNoChildren )
{
	return FirstChildElement( value.c_str(), throwIfNoChildren );
}

Element* Node::FirstChildElement( const char* value, bool throwIfNoChildren )
{
	TiXmlElement* element;
	if ( 0 == strlen( value ) )
	{
		element = GetTiXmlPointer()->FirstChildElement();
	}
	else
	{
		element = GetTiXmlPointer()->FirstChildElement( value );
	}

	if ( NULL == element )
	{
		if( throwIfNoChildren )
		{
			THROW( "Element (" << Value() << ") does NOT contain a child with the value of '" << value << "'" )
		}
		else
		{
			return NULL;
		}
	}

	Element* temp = new Element( element );
	m_spawnedWrappers.push_back( temp );

	return temp;
}

int Node::Type()
{
	return GetTiXmlPointer()->Type();
}

Document* Node::GetDocument( bool throwIfNoDocument )
{
	TiXmlDocument* doc = GetTiXmlPointer()->GetDocument();
	if ( NULL == doc )
	{
		if( throwIfNoDocument )
		{
			THROW( "This node (" << Value() << ") is not linked under a document" )
		}
		else
		{
			return NULL;
		}
	}
	Document* temp = new Document( doc );
	m_spawnedWrappers.push_back( temp );

	return temp;
}

bool Node::NoChildren()
{
	return GetTiXmlPointer()->NoChildren();
}

Document* Node::ToDocument()
{
	TiXmlDocument* doc = GetTiXmlPointer()->ToDocument();
	if ( NULL == doc )
	{
		THROW( "This node (" << Value() << ") is not a Document" )
	}
	Document* temp = new Document( doc );
	m_spawnedWrappers.push_back( temp );

	return temp;
}

Element* Node::ToElement()
{
	TiXmlElement* doc = GetTiXmlPointer()->ToElement();
	if ( NULL == doc )
	{
		THROW( "This node (" << Value() << ") is not a Element" )
	}
	Element* temp = new Element( doc );
	m_spawnedWrappers.push_back( temp );

	return temp;
}

Comment* Node::ToComment()
{
	TiXmlComment* doc = GetTiXmlPointer()->ToComment();
	if ( NULL == doc )
	{
		THROW( "This node (" << Value() << ") is not a Comment" )
	}
	Comment* temp = new Comment( doc );
	m_spawnedWrappers.push_back( temp );

	return temp;
}

Text* Node::ToText()
{
	TiXmlText* doc = GetTiXmlPointer()->ToText();
	if ( NULL == doc )
	{
		THROW( "This node (" << Value() << ") is not a Text" )
	}
	Text* temp = new Text( doc );
	m_spawnedWrappers.push_back( temp );

	return temp;
}

Declaration* Node::ToDeclaration()
{
	TiXmlDeclaration* doc = GetTiXmlPointer()->ToDeclaration();
	if ( NULL == doc )
	{
		THROW( "This node (" << Value() << ") is not a Declaration" )
	}
	Declaration* temp = new Declaration( doc );
	m_spawnedWrappers.push_back( temp );

	return temp;
}

std::auto_ptr< Node > Node::Clone()
{
	TiXmlNode* node = GetTiXmlPointer()->Clone();
	if ( NULL == node )
	{
		THROW( "Node could not be cloned" );
	}
	std::auto_ptr< Node > temp( NodeFactory( node, false, false ) );

	// Take ownership of the memory from TiXml
	temp->m_impRC->InitRef();

	return temp;
}

//*****************************************************************************

Comment::Comment()
: NodeImp< TiXmlComment >( new TiXmlComment() )
{
	m_impRC->InitRef();
}

Comment::Comment( TiXmlComment* comment )
: NodeImp< TiXmlComment >( comment )
{
}

Comment::Comment( const std::string& comment )
: NodeImp< TiXmlComment >( new TiXmlComment() )
{
	m_impRC->InitRef();
	m_tiXmlPointer->SetValue( comment );
}

//*****************************************************************************

Text::Text()
: NodeImp< TiXmlText >( new TiXmlText("") )
{
	m_impRC->InitRef();
}


Text::Text( const std::string& value )
: NodeImp< TiXmlText >( new TiXmlText( value ) )
{
	m_impRC->InitRef();
}

Text::Text( TiXmlText* text )
: NodeImp< TiXmlText >( text )
{
}


//*****************************************************************************

Document::Document()
: NodeImp< TiXmlDocument >( new TiXmlDocument() )
{
	m_impRC->InitRef();
}

Document::Document( TiXmlDocument* document )
: NodeImp< TiXmlDocument >( document )
{
}

Document::Document( const char* documentName )
: NodeImp< TiXmlDocument >( new TiXmlDocument( documentName ) )
{
	m_impRC->InitRef();
}

Document::Document( const std::string& documentName )
: NodeImp< TiXmlDocument >( new TiXmlDocument( documentName ) )
{
	m_impRC->InitRef();
}

void Document::Print(std::ostream& o)
{
	TiXmlPrinter printer;
        printer.SetIndent(" ");
	GetTiXmlPointer()->Accept( &printer );

        o << printer.Str();
}

void Document::LoadFile( TiXmlEncoding encoding )
{
	if ( !m_tiXmlPointer->LoadFile( encoding ) )
	{
		THROW( "Couldn't load " << m_tiXmlPointer->Value() );
	}
}

void Document::SaveFile( void ) const
{
	if ( !m_tiXmlPointer->SaveFile() )
	{
		THROW( "Couldn't save " << m_tiXmlPointer->Value() );
	}
}

void Document::LoadFile( const std::string& filename, TiXmlEncoding encoding )
{
	if ( !m_tiXmlPointer->LoadFile( filename.c_str(), encoding ) )
	{
		THROW( "Couldn't load " << filename );
	}
}

void Document::SaveFile( const std::string& filename ) const
{
	if ( !m_tiXmlPointer->SaveFile( filename.c_str() ) )
	{
		THROW( "Couldn't save " << filename );
	}
}

//*****************************************************************************

Element::Element()
: NodeImp< TiXmlElement >( new TiXmlElement( "DefaultValueCausedByCreatingAnElementWithNoParameters" ) )
{
	m_impRC->InitRef();
}

Element::Element( const std::string& value )
: NodeImp< TiXmlElement >( new TiXmlElement( value ) )
{
	m_impRC->InitRef();
}

Element::Element( TiXmlElement* element )
: NodeImp< TiXmlElement >( element )
{
}

Attribute* Element::FirstAttribute( bool throwIfNoAttributes )
{
	ValidatePointer();
	TiXmlAttribute* attribute = m_tiXmlPointer->FirstAttribute();
	if ( ( NULL == attribute ) && throwIfNoAttributes )
	{
		THROW( "This Element (" << Value() << ") has no attributes" )
	}

	if ( NULL == attribute )
	{
		if( throwIfNoAttributes )
		{
			THROW( "Element (" << Value() << ") has no attributes" )
		}
		else
		{
			return NULL;
		}
	}

	Attribute* temp = new Attribute( attribute );
	m_spawnedWrappers.push_back( temp );

	return temp;
}

Attribute* Element::LastAttribute( bool throwIfNoAttributes )
{
	ValidatePointer();
	TiXmlAttribute* attribute = m_tiXmlPointer->LastAttribute();
	if ( ( NULL == attribute ) && throwIfNoAttributes )
	{
		THROW( "This Element (" << Value() << ") has no attributes" )
	}

	if ( NULL == attribute )
	{
		if( throwIfNoAttributes )
		{
			THROW( "Element (" << Value() << ") has no attributes" )
		}
		else
		{
			return NULL;
		}
	}

	Attribute* temp = new Attribute( attribute );
	m_spawnedWrappers.push_back( temp );

	return temp;
}

bool Element::GetAttributeImp( const std::string& name, std::string* value )
{
	ValidatePointer();

	// Get value from TinyXML, if the attribute exists
	const char* retVal = m_tiXmlPointer->Attribute( name.c_str() );

	// TinyXML returns NULL if the attribute doesn't exist
	if ( NULL == retVal )
	{
		return false;
	}
	else
	{
		*value = retVal;
		return true;
	}
}

bool Element::GetTextImp( std::string* value )
{
	ValidatePointer();

	// Get value from TinyXML, if the attribute exists
	const char* retVal = m_tiXmlPointer->GetText();

	// TinyXML returns NULL if the attribute doesn't exist
	if ( NULL == retVal )
	{
		return false;
	}
	else
	{
		*value = retVal;
		return true;
	}
}

//*****************************************************************************

Declaration::Declaration()
: NodeImp< TiXmlDeclaration >( new TiXmlDeclaration() )
{
	m_impRC->InitRef();
}

Declaration::Declaration( TiXmlDeclaration* declaration )
: NodeImp< TiXmlDeclaration >( declaration )
{
}

Declaration::Declaration( const std::string& version, const std::string& encoding, const std::string& standalone )
: NodeImp< TiXmlDeclaration >( new TiXmlDeclaration( version, encoding, standalone ) )
{
	m_impRC->InitRef();
}

std::string Declaration::Version( void )
{
	return m_tiXmlPointer->Version();
}

std::string Declaration::Encoding( void )
{
	return m_tiXmlPointer->Encoding();
}

std::string Declaration::Standalone( void )
{
	return m_tiXmlPointer->Standalone();
}

//*****************************************************************************

Exception::Exception(const std::string &details)
	: m_details( details )
{

}

//*****************************************************************************

TiCppRC::TiCppRC()
{
	// Spawn reference counter for this object
	m_tiRC = new TiCppRCImp( this );
}

TiCppRC::~TiCppRC()
{
	// Set pointer held by reference counter to NULL
	this->m_tiRC->Nullify();

	// Decrement reference - so reference counter will delete itself if necessary
	this->m_tiRC->DecRef();
}

//*****************************************************************************

TiCppRCImp::TiCppRCImp( TiCppRC* tiCppRC )
	: m_count( 1 ), m_tiCppRC ( tiCppRC )
{
}

void TiCppRCImp::IncRef()
{
	m_count++;
}

void TiCppRCImp::DecRef()
{
	m_count--;
	if ( 0 == m_count )
	{
		delete m_tiCppRC;
		delete this;
	}
}

void TiCppRCImp::InitRef()
{
	m_count = 1;
}

void TiCppRCImp::Nullify()
{
	m_tiCppRC = NULL;
}

TiCppRC* TiCppRCImp::Get()
{
	return m_tiCppRC;
}

bool TiCppRCImp::IsNull()
{
	return NULL == m_tiCppRC;
}

#endif // TIXML_USE_TICPP
