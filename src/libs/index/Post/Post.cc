#include "src/libs/index/Post/Post.h"


using namespace BBG;

// Post

Post::Post() : location( 0 ) { }

Post::Post( uint32_t _location, uint32_t tag ) 
    {
    location = ( _location & LocationMask ) + ( tag << HtmlShift );
    }

uint32_t Post::getAbsoluteLocation( ) const 
    { 
    return location & LocationMask;
    }

HtmlTag Post::getHtmlTag( ) const 
    { 
    // demask word priority type 
    return HtmlTag( location >> HtmlShift );
    }

Post& Post::operator=( const Post& other ) 
    {
    location = other.location;
    return *this;
    }

bool Post::operator==( const Post& other ) const 
    {
    return location == other.location;
    }

void Post::setLocation( uint32_t _location ) 
    {
    location = ( location & HtmlMask ) + _location;
    }

void Post::setTag( uint32_t tag ) 
    {
    location = ( location & LocationMask ) + ( tag << HtmlShift );
    }

char * Post::encode( char* buf ) const 
    {
    buf = byteEncode( buf, location );    
    return buf;
    }

const char * Post::decode( const char * buf ) 
    {
    buf = byteDecode( buf, location );
    return buf;
    }

size_t Post::byteSize( ) const
    {
    return byteEncodingSize( location );
    }

// EndDocPost

EndDocPost::EndDocPost( ) : docFeatures( ), bytesToUrl( 0 ), 
    location( 0 ), docId( 0 )  { }

EndDocPost::EndDocPost( uint32_t _location, uint32_t doc ) : 
    docFeatures( ), bytesToUrl( 0 ),
    docId( doc ) 
    {
    location = ( _location & Post::LocationMask );
    }

EndDocPost::EndDocPost( uint32_t _location, uint32_t doc, DocFeatures _features ) : 
    docFeatures( _features ), bytesToUrl( 0 ), docId( doc )
    {
    location = ( _location & Post::LocationMask );
    }

const string EndDocPost::getUrl( ) const 
    {
    char * urlStart = ( char * ) this + bytesToUrl;
    return string( urlStart );
    }

const string EndDocPost::getTitle( ) const 
    {
    char * titleStart = (char *) this + bytesToUrl;
    // Move passed url
    while ( *( titleStart++ ) );
    return string( titleStart );
    }


uint32_t EndDocPost::getAbsoluteLocation( ) const 
    { 
    return location & Post::LocationMask;
    }

void EndDocPost::setLocation( uint32_t _location ) 
    {
    location = ( location & Post::HtmlMask ) + _location;
    }

bool EndDocPost::operator==( const EndDocPost& other ) const
    {
    return ( location == other.location &&
            docId == other.docId &&
            bytesToUrl == other.bytesToUrl &&
            docFeatures == other.docFeatures );
    }


EndDocPost& EndDocPost::operator=( const EndDocPost& other ) 
    {
    location = other.location;
    docId = other.docId;
    bytesToUrl = other.bytesToUrl;
    docFeatures = other.docFeatures;
    return *this;
    }

uint32_t EndDocPost::docStart( ) const 
    {
    return getAbsoluteLocation( ) - docFeatures.numWords;
    }

char * EndDocPost::encode( char* buf ) const 
    {
    buf = byteEncode( buf, location );
    buf = byteEncode( buf, docId );
    buf = byteEncode( buf, bytesToUrl );
    buf = docFeatures.encode( buf );
    return buf;
}

const char * EndDocPost::decode( const char * buf ) 
    {
    buf = byteDecode( buf, location );
    buf = byteDecode( buf, docId );
    buf = byteDecode( buf, bytesToUrl );
    buf = docFeatures.decode( buf );
    return buf;
    }

size_t EndDocPost::byteSize( ) const
    {
    return byteEncodingSize( location ) + 
        byteEncodingSize( docId ) +
        byteEncodingSize( bytesToUrl ) + 
        docFeatures.byteSize( );
    }

