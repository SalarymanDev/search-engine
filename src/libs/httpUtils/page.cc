#include "page.h"

#include <cstring>

Page::Page(const char* Url){
  pathBuffer = new char[ strlen( Url ) + 1 ];
  const char *f;
  char *t;
  for ( t = pathBuffer, f = Url; (*t++ = *f++); )
        ;

  this->service = pathBuffer;

  const char Colon = ':', Slash = '/';
  char *p;
  for ( p = pathBuffer; *p && *p != Colon; p++ )
        ;

  if (*p) {
    // Mark the end of the Service.
    *p++ = 0;

    if ( *p == Slash )
      p++;
    if ( *p == Slash )
      p++;

    host = p;

    for ( ; *p && *p != Slash && *p != Colon; p++ )
        ;

    if (*p == Colon){
      // Port specified.  Skip over the colon and
      // the port number.
      *p++ = 0;
      port = +p;
      for ( ; *p && *p != Slash; p++ )
        ;
    } else
        this->port = p;

    if ( *p )
        // Mark the end of the Host and Port.
        *p++ = 0;

    // Whatever remains is the Path.
    this->path = p;
  } else
    host = path = p;
}

Page::~Page(){ delete[] pathBuffer; }
