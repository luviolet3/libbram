#ifndef LB_UTILS_HPP
#define LB_UTILS_HPP

#include <cstdint>

namespace lb {
   struct Str {
      char* buf;
      size_t size;
   };
   template<typename T> struct Arr {
      T*     buf;
      size_t cap;
      size_t len;
   };
};

#endif
