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

NAN_MODULE_INIT(Int64::Init) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Int64").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

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
  Nan::SetPrototypeMethod(tpl, "and", And);
  Nan::SetPrototypeMethod(tpl, "or", Or);
  Nan::SetPrototypeMethod(tpl, "xor", Xor);
  Nan::SetPrototypeMethod(tpl, "add", Add);
  Nan::SetPrototypeMethod(tpl, "sub", Sub);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("Int64").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

Int64::Int64() {
  mValue = 0;
}

Int64::Int64(const Local<Number>& n) {
  mValue = static_cast<uint64_t>(Nan::To<int64_t>(n).ToChecked());
}

Int64::Int64(const Local<Number>& hi, const Local<Number>& lo) {
  uint32_t highBits = static_cast<uint32_t>(Nan::To<uint32_t>(hi).ToChecked());
  uint32_t lowBits = static_cast<uint32_t>(Nan::To<uint32_t>(lo).ToChecked());
  mValue =
    (static_cast<uint64_t>(highBits) << 32) |
    (static_cast<uint64_t>(lowBits));
}

Int64::Int64(const Local<String>& s) {
  Nan::Utf8String utf8(s);
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

NAN_METHOD(Int64::New) {
  if (info.IsConstructCall()) {
    Int64* obj = NULL;
    if (info.Length() == 0) {
      obj = new Int64();
    } else if (info.Length() == 1) {
      if (info[0]->IsNumber()) {
        obj = new Int64(Nan::To<v8::Number>(info[0]).ToLocalChecked());
      } else if (info[0]->IsString()) {
        obj = new Int64(Nan::To<String>(info[0]).ToLocalChecked());
      }
    } else if (info.Length() == 2) {
      if (info[0]->IsNumber() && info[1]->IsNumber()) {
        obj = new Int64(
          Nan::To<v8::Number>(info[0]).ToLocalChecked(),
          Nan::To<v8::Number>(info[1]).ToLocalChecked()
        );
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
      v8::Local<v8::Value> argv[0] = {};
      info.GetReturnValue().Set(
        Nan::NewInstance(cons, 0, argv).ToLocalChecked()
      );
    } else if (info.Length() == 1) {
      v8::Local<v8::Value> argv[1] = {info[0]};
      info.GetReturnValue().Set(
        Nan::NewInstance(cons, 1, argv).ToLocalChecked()
      );
    } else if (info.Length() == 2) {
      v8::Local<v8::Value> argv[2] = {info[0], info[1]};
      info.GetReturnValue().Set(
        Nan::NewInstance(cons, 2, argv).ToLocalChecked()
      );
    }
  }
}

NAN_METHOD(Int64::ToNumber) {
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  double value = obj->mValue;
  if (value >= 1ull << 53) {
    value = numeric_limits<double>::infinity();
  }
  info.GetReturnValue().Set(Nan::New(value));
}

NAN_METHOD(Int64::ValueOf) {
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  double value = obj->mValue;
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
  o << obj->mValue;
  info.GetReturnValue().Set(Nan::New(o.str().c_str()).ToLocalChecked());
}

NAN_METHOD(Int64::ToSignedDecimalString) {
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());

  std::ostringstream o;
  o << (static_cast<int64_t>(obj->mValue));
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
  Int64* otherObj = ObjectWrap::Unwrap<Int64>(Nan::To<Object>(info[0]).ToLocalChecked());
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
  Int64* otherObj = ObjectWrap::Unwrap<Int64>(Nan::To<Object>(info[0]).ToLocalChecked());
  int32_t cmp = 0;
  if (obj->mValue < otherObj->mValue) {
    cmp = -1;
  } else if (obj->mValue > otherObj->mValue) {
    cmp = 1;
  }
  info.GetReturnValue().Set(Nan::New(cmp));
}

NAN_METHOD(Int64::High32) {
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  uint32_t highBits = static_cast<uint32_t>(obj->mValue >> 32);
  info.GetReturnValue().Set(Nan::New(highBits));
}

NAN_METHOD(Int64::Low32) {
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  uint32_t lowBits = static_cast<uint32_t>(obj->mValue & 0xffffffffull);
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
  uint64_t shiftBy = static_cast<uint64_t>(
    Nan::To<int64_t>(info[0]).ToChecked()
  );
  uint64_t value = obj->mValue << shiftBy;
  Local<Value> argv[2] = {
    Nan::New(static_cast<uint32_t>(value >> 32)),
    Nan::New(static_cast<uint32_t>(value & 0xffffffffull))
  };
  v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
  Local<Object> instance = Nan::NewInstance(cons, 2, argv).ToLocalChecked();
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
  uint64_t shiftBy = static_cast<uint64_t>(
    Nan::To<int64_t>(info[0]).ToChecked()
  );
  uint64_t value = obj->mValue >> shiftBy;
  Local<Value> argv[2] = {
    Nan::New(static_cast<uint32_t>(value >> 32)),
    Nan::New(static_cast<uint32_t>(value & 0xffffffffull))
  };
  v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
  Local<Object> instance = Nan::NewInstance(cons, 2, argv).ToLocalChecked();
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::And) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  uint64_t value;
  if (info[0]->IsNumber()) {
    value = obj->mValue & Nan::To<int64_t>(info[0]).ToChecked();
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(Nan::To<Object>(info[0]).ToLocalChecked());
    value = obj->mValue & otherObj->mValue;
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }
  Local<Value> argv[2] = {
    Nan::New(static_cast<uint32_t>(value >> 32)),
    Nan::New(static_cast<uint32_t>(value & 0xffffffffull))
  };
  v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
  Local<Object> instance = Nan::NewInstance(cons, 2, argv).ToLocalChecked();
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::Or) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  uint64_t value;
  if (info[0]->IsNumber()) {
    value = obj->mValue | Nan::To<int64_t>(info[0]).ToChecked();
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(Nan::To<Object>(info[0]).ToLocalChecked());
    value = obj->mValue | otherObj->mValue;
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }
  Local<Value> argv[2] = {
    Nan::New(static_cast<uint32_t>(value >> 32)),
    Nan::New(static_cast<uint32_t>(value & 0xffffffffull))
  };
  v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
  Local<Object> instance = Nan::NewInstance(cons, 2, argv).ToLocalChecked();
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::Xor) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  uint64_t value;
  if (info[0]->IsNumber()) {
    value = obj->mValue ^ Nan::To<int64_t>(info[0]).ToChecked();
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(Nan::To<Object>(info[0]).ToLocalChecked());
    value = obj->mValue ^ otherObj->mValue;
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }
  Local<Value> argv[2] = {
    Nan::New(static_cast<uint32_t>(value >> 32)),
    Nan::New(static_cast<uint32_t>(value & 0xffffffffull))
  };
  v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
  Local<Object> instance = Nan::NewInstance(cons, 2, argv).ToLocalChecked();
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::Add) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  uint64_t value;
  if (info[0]->IsNumber()) {
    value = obj->mValue + Nan::To<int64_t>(info[0]).ToChecked();
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(Nan::To<Object>(info[0]).ToLocalChecked());
    value = obj->mValue + otherObj->mValue;
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }
  Local<Value> argv[2] = {
    Nan::New(static_cast<uint32_t>(value >> 32)),
    Nan::New(static_cast<uint32_t>(value & 0xffffffffull))
  };
  v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
  Local<Object> instance = Nan::NewInstance(cons, 2, argv).ToLocalChecked();
  info.GetReturnValue().Set(instance);
}

NAN_METHOD(Int64::Sub) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Argument required");
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(info.Holder());
  uint64_t value;
  if (info[0]->IsNumber()) {
    value = obj->mValue - Nan::To<int64_t>(info[0]).ToChecked();
  } else if (info[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(Nan::To<Object>(info[0]).ToLocalChecked());
    value = obj->mValue - otherObj->mValue;
  } else {
    Nan::ThrowTypeError("Object or number expected");
    return;
  }
  Local<Value> argv[2] = {
    Nan::New(static_cast<uint32_t>(value >> 32)),
    Nan::New(static_cast<uint32_t>(value & 0xffffffffull))
  };
  v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
  Local<Object> instance = Nan::NewInstance(cons, 2, argv).ToLocalChecked();
  info.GetReturnValue().Set(instance);
}
