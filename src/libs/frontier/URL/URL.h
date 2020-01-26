#ifndef URL_H
#define URL_H


#include "src/libs/string/string.h"

namespace BBG {
   struct URL {
      string url;
      string root;

      URL();
      URL(const string& _url, const string & _root);
      URL& operator=(const URL& other);
   };

   void swap(URL &a, URL &b);
}

#endif