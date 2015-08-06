#ifndef INT64_H
#define INT64_H

#include <node.h>
#include <v8.h>
#include <node_object_wrap.h>

using namespace node;
using namespace v8;

class Int64 : public ObjectWrap {
 public:
  static void Init(Handle<Object> exports);

 private:
  Int64();
  Int64(const Local<Number>& n);
  Int64(const Local<Number>& hi, const Local<Number>& lo);
  Int64(const Local<String>& s);
  ~Int64();

  static Persistent<Function> constructor;
  static void New(const FunctionCallbackInfo<Value>& args);
  static void ToNumber(const FunctionCallbackInfo<Value>& args);
  static void ValueOf(const FunctionCallbackInfo<Value>& args);
  static void ToString(const FunctionCallbackInfo<Value>& args);
  static void ToUnsignedDecimalString(const FunctionCallbackInfo<Value>& args);
  static void ToSignedDecimalString(const FunctionCallbackInfo<Value>& args);
  static void Equals(const FunctionCallbackInfo<Value>& args);
  static void Compare(const FunctionCallbackInfo<Value>& args);
  static void High32(const FunctionCallbackInfo<Value>& args);
  static void Low32(const FunctionCallbackInfo<Value>& args);
  static void ShiftLeft(const FunctionCallbackInfo<Value>& args);
  static void ShiftRight(const FunctionCallbackInfo<Value>& args);
  static void And(const FunctionCallbackInfo<Value>& args);
  static void Or(const FunctionCallbackInfo<Value>& args);
  static void Xor(const FunctionCallbackInfo<Value>& args);
  static void Add(const FunctionCallbackInfo<Value>& args);
  static void Sub(const FunctionCallbackInfo<Value>& args);

  uint64_t mValue;
};

#endif
