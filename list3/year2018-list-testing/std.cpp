#define _GLIBCXX_DEBUG 1
#include "counted.h"
#include "../list.h"
//using container = __gnu_debug::list<counted>;
using container = list<counted>;

#include "tests.inl"
