#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include <node.h>
#include <v8.h>

#include "Int64.h"
#include "UInt64.h"

using namespace node;
using namespace v8;

void InitAll(Handle<Object> exports) {
  UInt64::Init(exports);
}

NODE_MODULE(UInt64, InitAll)
