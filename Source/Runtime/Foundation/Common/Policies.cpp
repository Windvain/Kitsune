#include <initializer_list>
#include "Foundation/Iterators/Iterator.h"

using namespace Kitsune;

// We assume that std::initializer_list is implemented as a pair of pointers, or as a pointer
// and size pair.
// This assertion should not fail with the commonly used compilers.
static_assert(ForwardIterator<typename std::initializer_list<int>::iterator>,
              "std::initializer_list iterators do not satisfy ForwardIterator.");

static_assert(ForwardIterator<typename std::initializer_list<int>::const_iterator>,
              "std::initializer_list iterators do not satisfy ForwardIterator.");
