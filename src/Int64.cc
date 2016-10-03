#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include <node.h>
#include <v8.h>

#include <iomanip>
#include <limits>
#include <sstream>

#include "Int64.h"

using namespace node;
using namespace std;
using namespace v8;

Nan::Persistent<Function> Int64::constructor;

void Int64::Init(Handle<Object> exports) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Int64").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "toBytes", ToBytes);
  Nan::SetPrototypeMethod(tpl, "toNumber", ToNumber);
  Nan::SetPrototypeMethod(tpl, "valueOf", ValueOf);
  Nan::SetPrototypeMethod(tpl, "toString", ToString);
  Nan::SetPrototypeMethod(tpl, "toUnsignedDecimalString", ToUnsignedDecimalString);
  Nan::SetPrototypeMethod(tpl, "toSignedDecimalString", ToSignedDecimalString);
  Nan::SetPrototypeMethod(tpl, "equals", Equals);
  Nan::SetPrototypeMethod(tpl, "compare", Compare);
  Nan::SetPrototypeMethod(tpl, "high32", High32);
  Nan::SetPrototypeMethod(tpl, "low32", Low32);
  Nan::SetPrototypeMethod(tpl, "shiftLeft", ShiftLeft);
  Nan::SetPrototypeMethod(tpl, "shiftRight", ShiftRight);
  Nan::SetPrototypeMethod(tpl, "unsignedShiftRight", UnsignedShiftRight);
  Nan::SetPrototypeMethod(tpl, "and", And);
  Nan::SetPrototypeMethod(tpl, "or", Or);
  Nan::SetPrototypeMethod(tpl, "xor", Xor);
  Nan::SetPrototypeMethod(tpl, "add", Add);
  Nan::SetPrototypeMethod(tpl, "mul", Mul);
  Nan::SetPrototypeMethod(tpl, "div", Div);
  Nan::SetPrototypeMethod(tpl, "mod", Mod);
  Nan::SetPrototypeMethod(tpl, "neg", Neg);
  Nan::SetPrototypeMethod(tpl, "sub", Sub);

  Nan::SetPrototypeMethod(tpl, "compare_un", CompareUnsigned);
  Nan::SetPrototypeMethod(tpl, "div_un", DivUn);
  Nan::SetPrototypeMethod(tpl, "mod_un", ModUn);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("Int64").ToLocalChecked(), tpl->GetFunction());
}

Int64::Int64() {
  mValue = 0;
}

Int64::Int64(const Local<Number>& n) {
  mValue = static_cast<int64_t>(n->NumberValue());
}

Int64::Int64(const Local<Array>& a) {
  int num_bytes = a->Length();
  if (num_bytes > 8) {
    Nan::ThrowTypeError("Wrong array size");
    return;
  }

  union {
    int8_t ch[8];
    int64_t lVal;
  } uValue;

  mValue = uValue.lVal = 0;

  for (int n = 0; n < num_bytes; ++n) {
    int8_t loVal = static_cast<int32_t>(Local<Number>::Cast(a->Get(n))->NumberValue());
    uValue.ch[n] = loVal;
  }

  mValue = uValue.lVal;
}

Int64::Int64(const Local<Number>& hi, const Local<Number>& lo) {
  int32_t highBits = static_cast<int32_t>(hi->NumberValue());
  int32_t lowBits = static_cast<int32_t>(lo->NumberValue());
  mValue =
    (static_cast<int64_t>(highBits) << 32) |
    (static_cast<int64_t>(lowBits));
}

Int64::Int64(const Local<String>& s) {
  String::Utf8Value utf8(s);
  stringstream ss;
  char* ps = *utf8;
  if (utf8.length() > 2 && ps[0] == '0' && ps[1] == 'x') {
    ss << hex << (ps + 2);
  } else {
    ss << ps;
  }
  ss >> mValue;
}

Int64::~Int64() {}

Local<Object> Int64::InitFromValue(Isolate* isolate, int64_t value) {
  Local<Array> result_bytes = Array::New(isolate);
  uint64_t val = static_cast<uint64_t>(value);
  int8_t n = 0;
  do {
    Local<Number> byte = Number::New(isolate, val & 0xff);
    result_bytes->Set(n++, byte);
  } while (val >>= 8);

  Local<Value> argv[1] = { result_bytes };
  v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
  return cons->NewInstance(1, argv);
}

NAN_METHOD(Int64::New) {
  if (info.IsConstructCall()) {
    Int64* obj = NULL;
    if (info.Length() == 0) {
      obj = new Int64();
    } else if (info.Length() == 1) {
      if (info[0]->IsNumber()) {
        obj = new Int64(info[0]->ToNumber());
      } else if (info[0]->IsString()) {
        obj = new Int64(info[0]->ToString());
      } else if (info[0]->IsArray()) {
        Local<Array> input = Local<Array>::Cast(info[0]);
        obj = new Int64(input);
      }
    } else if (info.Length() == 2) {
      if (info[0]->IsNumber() && info[1]->IsNumber()) {
        obj = new Int64(info[0]->ToNumber(), info[1]->ToNumber());
      }
    }
    if (obj == NULL) {
      Nan::ThrowTypeError("Wrong arguments");
      return;
    }
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    if (info.Length() == 0) {
      v8::Local<v8::Value> argv[1] = {Local<String>()};
      info.GetReturnValue().Set(cons->NewInstance(0, argv));
    } else if (info.Length() == 1) {
      v8::Local<v8::Value> argv[1] = {info[0]};
      info.GetReturnValue().Set(cons->NewInstance(1, argv));
    } else if (info.Length() == 2) {
      v8::Local<v8::Value> argv[2] = {info[0], info[1]};
      info.GetReturnValue().Set(cons->NewInstance(2, argv));
    }
  }
}

NAN_METHOD(Int64::ToBytes) {
  Isolate* isolate = info.GetIsolate();
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  Local<Array> result_bytes = Array::New(isolate);
  uint64_t val = static_cast<uint64_t>(obj->mValue);
  int8_t n = 0;
  do {
    Local<Number> byte = Number::New(isolate, val & 0xff);
    result_bytes->Set(n++, byte);
  } while (val >>= 8);

  info.GetReturnValue().Set(result_bytes);
}

NAN_METHOD(Int64::ToNumber) {
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  double value = static_cast<double>(obj->mValue);
  if (value >= 1ull << 53) {
    value = numeric_limits<double>::infinity();
  }
  info.GetReturnValue().Set(Nan::New(value));
}

NAN_METHOD(Int64::ValueOf) {
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  double value = static_cast<double>(obj->mValue);

  if (value >= 1ull << 53) {
    value = numeric_limits<double>::infinity();
  }
  info.GetReturnValue().Set(Nan::New(value));
}

NAN_METHOD(Int64::ToString) {
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());

  std::ostringstream o;
  o << "0x" << hex << setfill('0') << setw(16) << obj->mValue;
  info.GetReturnValue().Set(Nan::New(o.str().c_str()).ToLocalChecked());
}

NAN_METHOD(Int64::ToUnsignedDecimalString) {
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());

  std::ostringstream o;
  o <<  (static_cast<uint64_t>(obj->mValue));
  info.GetReturnValue().Set(Nan::New(o.str().c_str()).ToLocalChecked());
}

NAN_METHOD(Int64::ToSignedDecimalString) {
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());

  std::ostringstream o;
  o <<obj->mValue;
  info.GetReturnValue().Set(Nan::New(o.str().c_str()).ToLocalChecked());
}

NAN_METHOD(Int64::Equals) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
  }
  if (!info[0]->IsObject()) {
    Nan::ThrowTypeError("Object expected");
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  Int64* otherObj = ObjectWrap::Unwrap<Int64>(info[0]->ToObject());
  bool isEqual = obj->mValue == otherObj->mValue;
  info.GetReturnValue().Set(Nan::New(isEqual));
}

NAN_METHOD(Int64::Compare) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
  }
  if (!info[0]->IsObject()) {
    Nan::ThrowTypeError("Object expected");
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  Int64* otherObj = ObjectWrap::Unwrap<Int64>(info[0]->ToObject());
  int32_t cmp = 0;
  if (obj->mValue < otherObj->mValue) {
    cmp = -1;
  } else if (obj->mValue > otherObj->mValue) {
    cmp = 1;
  }
  info.GetReturnValue().Set(Nan::New(cmp));
}

NAN_METHOD(Int64::CompareUnsigned) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
  }
  if (!info[0]->IsObject()) {
    Nan::ThrowTypeError("Object expected");
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  Int64* otherObj = ObjectWrap::Unwrap<Int64>(info[0]->ToObject());
  int32_t cmp = 0;
  if (static_cast<uint64_t>(obj->mValue) < static_cast<uint64_t>(otherObj->mValue)) {
    cmp = -1;
  } else if (static_cast<uint64_t>(obj->mValue) > static_cast<uint64_t>(otherObj->mValue)) {
    cmp = 1;
  }
  info.GetReturnValue().Set(Nan::New(cmp));
}

NAN_METHOD(Int64::High32) {
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  int32_t highBits = static_cast<int32_t>(obj->mValue >> 32);
  info.GetReturnValue().Set(Nan::New(highBits));
}

NAN_METHOD(Int64::Low32) {
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  int32_t lowBits = static_cast<int32_t>(obj->mValue & 0xffffffffull);
  info.GetReturnValue().Set(Nan::New(lowBits));
}

NAN_METHOD(Int64::ShiftLeft) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
  }
  if (!info[0]->IsNumber()) {
    Nan::ThrowTypeError("Integer expected");
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  int64_t shiftBy = static_cast<int64_t>(info[0]->ToNumber()->NumberValue());
  int64_t value = obj->mValue << shiftBy;
  Isolate* isolate = info.GetIsolate();
  Local<Object> instance = InitFromValue(isolate, value);
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::ShiftRight) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  if (!info[0]->IsNumber()) {
    Nan::ThrowTypeError("Integer expected");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  int64_t shiftBy = static_cast<int64_t>(info[0]->ToNumber()->NumberValue());
  int64_t value = obj->mValue >> shiftBy;
  Isolate* isolate = info.GetIsolate();
  Local<Object> instance = InitFromValue(isolate, value);
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::UnsignedShiftRight) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  if (!info[0]->IsNumber()) {
    Nan::ThrowTypeError("Integer expected");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  uint64_t shiftBy = static_cast<uint64_t>(info[0]->ToNumber()->NumberValue());
  uint64_t value = static_cast<uint64_t>(obj->mValue) >> shiftBy;
  Isolate* isolate = info.GetIsolate();
  Local<Object> instance = InitFromValue(isolate, value);
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::And) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  int64_t value;
  if (info[0]->IsNumber()) {
    value = obj->mValue & info[0]->IntegerValue();
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(info[0]->ToObject());
    value = obj->mValue & otherObj->mValue;
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }
  Isolate* isolate = info.GetIsolate();
  Local<Object> instance = InitFromValue(isolate, value);
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::Or) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  int64_t value;
  if (info[0]->IsNumber()) {
    value = obj->mValue | info[0]->IntegerValue();
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(info[0]->ToObject());
    value = obj->mValue | otherObj->mValue;
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }
  Isolate* isolate = info.GetIsolate();
  Local<Object> instance = InitFromValue(isolate, value);
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::Xor) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  int64_t value;
  if (info[0]->IsNumber()) {
    value = obj->mValue ^ info[0]->IntegerValue();
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(info[0]->ToObject());
    value = obj->mValue ^ otherObj->mValue;
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }
  Isolate* isolate = info.GetIsolate();
  Local<Object> instance = InitFromValue(isolate, value);
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::Add) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());

  int64_t value;

  if (info[0]->IsNumber()) {
    value = obj->mValue + info[0]->IntegerValue();
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(info[0]->ToObject());
    value = obj->mValue + otherObj->mValue;
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }

  Isolate* isolate = info.GetIsolate();
  Local<Object> instance = InitFromValue(isolate, value);
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::Mul) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  int64_t value;
  if (info[0]->IsNumber()) {
    value = obj->mValue * info[0]->IntegerValue();
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(info[0]->ToObject());
    value = obj->mValue * otherObj->mValue;
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }
  Isolate* isolate = info.GetIsolate();
  Local<Object> instance = InitFromValue(isolate, value);
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::Div) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  int64_t value;
  if (info[0]->IsNumber()) {
    value = obj->mValue / info[0]->IntegerValue();
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(info[0]->ToObject());
    value = obj->mValue / otherObj->mValue;
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }
  Isolate* isolate = info.GetIsolate();
  Local<Object> instance = InitFromValue(isolate, value);
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::DivUn) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  uint64_t value;
  if (info[0]->IsNumber()) {
    value = static_cast<uint64_t>(obj->mValue) / static_cast<uint64_t>(info[0]->IntegerValue());
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(info[0]->ToObject());
    value = static_cast<uint64_t>(obj->mValue) / static_cast<uint64_t>(otherObj->mValue);
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }
  Isolate* isolate = info.GetIsolate();
  Local<Object> instance = InitFromValue(isolate, value);
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::Mod) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  int64_t value;
  if (info[0]->IsNumber()) {
    value = obj->mValue % info[0]->IntegerValue();
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(info[0]->ToObject());
    value = obj->mValue % otherObj->mValue;
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }
  Isolate* isolate = info.GetIsolate();
  Local<Object> instance = InitFromValue(isolate, value);
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::ModUn) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  uint64_t value;
  if (info[0]->IsNumber()) {
    value = static_cast<uint64_t>(obj->mValue) % static_cast<uint64_t>(info[0]->IntegerValue());
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(info[0]->ToObject());
    value = static_cast<uint64_t>(obj->mValue) % static_cast<uint64_t>(otherObj->mValue);
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }
  Isolate* isolate = info.GetIsolate();
  Local<Object> instance = InitFromValue(isolate, value);
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::Neg) {
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  int64_t value = -obj->mValue;
  Isolate* isolate = info.GetIsolate();
  Local<Object> instance = InitFromValue(isolate, value);
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::Sub) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  int64_t value;
  if (info[0]->IsNumber()) {
    value = obj->mValue - info[0]->IntegerValue();
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(info[0]->ToObject());
    value = obj->mValue - otherObj->mValue;
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }
  Isolate* isolate = info.GetIsolate();
  Local<Object> instance = InitFromValue(isolate, value);
  info.GetReturnValue().Set(instance);
}
