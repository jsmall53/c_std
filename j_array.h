#include "j_types.h"

#define _ArrayHeader_ struct { u64 count; u64 capacity; };

struct ArrayHeader { u64 count; u64 capacity; };

void* ArrayGrow();
void ArrayShift(ArrayHeader* header, void* array, u64 itemSize, u64 fromIndex);

