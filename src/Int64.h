#ifndef INT64_H
#define INT64_H

#include <node.h>
#include <nan.h>
#include <v8.h>

using namespace node;
using namespace v8;

class Int64 : public Nan::ObjectWrap {
 public:
  static void Init(Handle<Object> exports);

 private:
  Int64();
  Int64(const Local<Number>& n);
  Int64(const Local<Array>& n);
  Int64(const Local<Number>& hi, const Local<Number>& lo);
  Int64(const Local<String>& s);
  ~Int64();

  int64_t mValue;

  static Nan::Persistent<Function> constructor;
  static NAN_METHOD(New);
  static NAN_METHOD(ToNumber);
  static NAN_METHOD(ValueOf);
  static NAN_METHOD(ToString);
  static NAN_METHOD(ToUnsignedDecimalString);
  static NAN_METHOD(ToSignedDecimalString);
  static NAN_METHOD(Equals);
  static NAN_METHOD(Compare);
  static NAN_METHOD(CompareUnsigned);
  static NAN_METHOD(High32);
  static NAN_METHOD(Low32);
  static NAN_METHOD(ShiftLeft);
  static NAN_METHOD(ShiftRight);
  static NAN_METHOD(UnsignedShiftRight);
  static NAN_METHOD(And);
  static NAN_METHOD(Or);
  static NAN_METHOD(Xor);
  static NAN_METHOD(Add);
  static NAN_METHOD(Mul);
  static NAN_METHOD(Div);
  static NAN_METHOD(Mod);
  static NAN_METHOD(DivUn);
  static NAN_METHOD(ModUn);
  static NAN_METHOD(Neg);
  static NAN_METHOD(Sub);
};

#endif
