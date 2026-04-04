#ifndef LB_UTILS_HPP
#define LB_UTILS_HPP

#include <cstdint>
#include <cstddef>

#define TODO __builtin_trap()

namespace lb {
   template<typename T> struct Arr {
      T*     buf;
      size_t cap;
      size_t len;
   };
};

#endif
