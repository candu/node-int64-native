#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include <node.h>
#include <v8.h>
#include <nan.h>

#include <iomanip>
#include <limits>
#include <sstream>

#include "Int64.h"

using namespace node;
using namespace std;
using namespace v8;

Persistent<Function> Int64::constructor;

void Int64::Init( Handle<Object> exports ) {
  
  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>( Int64::New );
  
  tpl->SetClassName( NanNew<String>( "Int64" ) );
  tpl->InstanceTemplate()->SetInternalFieldCount( 1 );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "toNumber" ),
    NanNew<FunctionTemplate>( Int64::ToNumber )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "valueOf" ),
    NanNew<FunctionTemplate>( Int64::ToNumber )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "toString" ),
    NanNew<FunctionTemplate>( Int64::ToString )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "toUnsignedDecimalString" ),
    NanNew<FunctionTemplate>( Int64::ToUnsignedDecimalString )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "toSignedDecimalString" ),
    NanNew<FunctionTemplate>( Int64::ToSignedDecimalString )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "equals" ),
    NanNew<FunctionTemplate>( Int64::Equals )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "compare" ),
    NanNew<FunctionTemplate>( Int64::Compare )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "high32" ),
    NanNew<FunctionTemplate>( Int64::High32 )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "low32" ),
    NanNew<FunctionTemplate>( Int64::Low32 )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "shiftLeft" ),
    NanNew<FunctionTemplate>( Int64::ShiftLeft )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "shiftRight" ),
    NanNew<FunctionTemplate>( Int64::ShiftRight )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "and" ),
    NanNew<FunctionTemplate>( Int64::And )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "or" ),
    NanNew<FunctionTemplate>( Int64::Or )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "xor" ),
    NanNew<FunctionTemplate>( Int64::Xor )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "add" ),
    NanNew<FunctionTemplate>( Int64::Add )->GetFunction()
  );
  
  tpl->PrototypeTemplate()->Set(
    NanNew<String>( "sub" ),
    NanNew<FunctionTemplate>( Int64::Sub )->GetFunction()
  );
  
  // Create constructor function from template
  NanAssignPersistent( Int64::constructor, tpl->GetFunction() );
  // Set module.exports to constructor
  exports->Set( NanNew<String>( "Int64" ), NanNew( Int64::constructor ) );
  
}

Int64::Int64() {
  mValue = 0;
}

Int64::Int64( const Local<Number>& n ) {
  mValue = static_cast<uint64_t> ( n->NumberValue() );
}

Int64::Int64( const Local<Number>& hi, const Local<Number>& lo ) {
  uint32_t highBits = static_cast<uint32_t> ( hi->NumberValue() );
  uint32_t lowBits = static_cast<uint32_t> ( lo->NumberValue() );
  mValue =
    ( static_cast<uint64_t> ( highBits ) << 32 ) |
    ( static_cast<uint64_t> ( lowBits ) );
}

Int64::Int64( const Local<String>& str ) {
  
  NanUtf8String utf8( str );
  stringstream stream;
  
  char* ps = *utf8;
  
  if( utf8.length() > 2 && ps[0] == '0' && ps[1] == 'x' ) {
    stream << hex << ( ps + 2 );
  } else {
    stream << ps;
  }
  
  stream >> mValue;
  
}

Int64::~Int64() {}

NAN_METHOD( Int64::New ) {
  
  NanScope();
  Int64* obj = NULL;
  
  if( args.Length() == 0 ) {
    obj = new Int64();
  }
  
  if( args.Length() == 1 ) {
    if( args[0]->IsNumber() ) {
      obj = new Int64( args[0]->ToNumber() );
    } else if( args[0]->IsString() ) {
      obj = new Int64( args[0]->ToString() );
    }
  }
  
  if( args.Length() == 2 ) {
    if( args[0]->IsNumber() && args[1]->IsNumber() ) {
      obj = new Int64( args[0]->ToNumber(), args[1]->ToNumber() );
    }
  }
  
  if( obj == NULL ) {
    NanThrowTypeError( "Arguments must be of type String or Number" );
  }
  
  obj->Wrap( args.This() );
  
  NanReturnThis();
  
}

NAN_METHOD( Int64::ToNumber ) {
  
  NanScope();
  Int64* obj = ObjectWrap::Unwrap<Int64>( args.This() );
  double value;
  
  if( obj->mValue >= 1ull << 53 ) {
    value = numeric_limits<double>::infinity();
  } else {
    value = static_cast<double>( obj->mValue );
  }
  
  NanReturnValue( NanNew<Number>( value ) );
  
}

NAN_METHOD( Int64::ToString ) {
  
  NanScope();
  Int64* obj = ObjectWrap::Unwrap<Int64>( args.This() );
  
  std::ostringstream out;
  
  out << "0x" << hex << setfill( '0' ) << setw( 16 ) << obj->mValue;
  
  NanReturnValue( NanNew<String>( out.str().c_str() ) );
  
}

NAN_METHOD( Int64::ToUnsignedDecimalString ) {
  
  NanScope();
  Int64* obj = ObjectWrap::Unwrap<Int64>( args.This() );
  
  std::ostringstream out;
  
  out << obj->mValue;
  
  NanReturnValue( NanNew<String>( out.str().c_str() ) );
  
}

NAN_METHOD( Int64::ToSignedDecimalString ) {
  
  NanScope();
  Int64* obj = ObjectWrap::Unwrap<Int64>( args.This() );
  
  std::ostringstream out;
  
  out << ( static_cast<int64_t> ( obj->mValue ) );
  
  NanReturnValue( NanNew<String>( out.str().c_str() ) );
  
}

NAN_METHOD( Int64::Equals ) {
  
  NanScope();
  
  if( args.Length() < 1 ) {
    NanThrowError( "Missing argument" );
  }
  
  if( !args[0]->IsObject() ) {
    NanThrowTypeError( "Argument must be an Object" );
  }
  
  Int64* a = ObjectWrap::Unwrap<Int64>( args.This() );
  Int64* b = ObjectWrap::Unwrap<Int64>( args[0]->ToObject() );
  
  bool isEqual = a->mValue == b->mValue;
  
  NanReturnValue( NanNew<Boolean>( isEqual ) );
  
}

NAN_METHOD( Int64::Compare ) {
  
  NanScope();
  
  if( args.Length() < 1 ) {
    NanThrowError( "Missing argument" );
  }
  
  if( !args[0]->IsObject() ) {
    NanThrowTypeError( "Argument must be an Object" );
  }
  
  Int64* a = ObjectWrap::Unwrap<Int64>( args.This() );
  Int64* b = ObjectWrap::Unwrap<Int64>( args[0]->ToObject() );
  
  int32_t cmp = 0;
  
  if( a->mValue < b->mValue ) {
    cmp = -1;
  } else if( a->mValue > b->mValue ) {
    cmp = 1;
  }
  
  NanReturnValue( NanNew<Number>( cmp ) );
  
}

NAN_METHOD( Int64::High32 ) {
  NanScope();
  Int64* obj = ObjectWrap::Unwrap<Int64>( args.This() );
  uint32_t highBits = static_cast<uint32_t> ( obj->mValue >> 32 );
  NanReturnValue( NanNew<Number>( highBits ) );
}

NAN_METHOD( Int64::Low32 ) {
  NanScope();
  Int64* obj = ObjectWrap::Unwrap<Int64>( args.This() );
  uint32_t lowBits = static_cast<uint32_t> ( obj->mValue & 0xffffffffull );
  NanReturnValue( NanNew<Number>( lowBits ) );
}

NAN_METHOD( Int64::ShiftLeft ) {
  
  NanScope();
  Int64* obj = ObjectWrap::Unwrap<Int64>( args.This() );
  
  if( args.Length() < 1 ) {
    NanThrowError( "Missing argument" );
  }
  
  if( !args[0]->IsNumber() ) {
    NanThrowTypeError( "Argument must be a Number" );
  }
  
  uint64_t shiftBy = static_cast<uint64_t> ( args[0]->ToNumber()->NumberValue() );
  uint64_t value = obj->mValue << shiftBy;
  
  Local<Value> argv[2] = {
    NanNew<Number>( static_cast<uint32_t> ( value >> 32 ) ),
    NanNew<Number>( static_cast<uint32_t> ( value & 0xffffffffull ) ),
  };
  
  Local<Object> instance = NanNew( Int64::constructor )->NewInstance( 2, argv );
  
  NanReturnValue( instance );
  
}

NAN_METHOD( Int64::ShiftRight ) {
  
  NanScope();
  Int64* obj = ObjectWrap::Unwrap<Int64>( args.This() );
  
  if( args.Length() < 1 ) {
    NanThrowError( "Missing argument" );
  }
  
  if( !args[0]->IsNumber() ) {
    NanThrowTypeError( "Argument must be a Number" );
  }
  
  uint64_t shiftBy = static_cast<uint64_t> ( args[0]->ToNumber()->NumberValue() );
  uint64_t value = obj->mValue >> shiftBy;
  
  Local<Value> argv[2] = {
    NanNew<Int32>( static_cast<uint32_t> ( value >> 32 ) ),
    NanNew<Int32>( static_cast<uint32_t> ( value & 0xffffffffull ) ),
  };
  
  Local<Object> instance = NanNew( Int64::constructor )->NewInstance( 2, argv );
  
  NanReturnValue( instance );
  
}

NAN_METHOD( Int64::And ) {
  
  NanScope();
  Int64* obj = ObjectWrap::Unwrap<Int64>( args.This() );
  
  if( args.Length() < 1 ) {
    NanThrowError( "Missing argument" );
  }
  
  uint64_t value = 0;
  
  if( args[0]->IsNumber() ) {
    value = obj->mValue & args[0]->IntegerValue();
  } else if( args[0]->IsObject() ) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>( args[0]->ToObject() );
    value = obj->mValue & otherObj->mValue;
  } else {
    NanThrowTypeError( "Argument must be a Number or Object" );
  }
  
  Local<Value> argv[2] = {
    NanNew<Number>( static_cast<uint32_t> ( value >> 32 ) ),
    NanNew<Number>( static_cast<uint32_t> ( value & 0xffffffffull ) ),
  };
  
  Local<Object> instance = NanNew( Int64::constructor )->NewInstance( 2, argv );
  
  NanReturnValue( instance );
  
}

NAN_METHOD( Int64::Or ) {
  
  NanScope();
  Int64* obj = ObjectWrap::Unwrap<Int64>( args.This() );
  
  if( args.Length() < 1 ) {
    NanThrowError( "Missing argument" );
  }
  
  uint64_t value = 0;
  
  if( args[0]->IsNumber() ) {
    value = obj->mValue | args[0]->IntegerValue();
  } else if( args[0]->IsObject() ) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>( args[0]->ToObject() );
    value = obj->mValue | otherObj->mValue;
  } else {
    NanThrowTypeError( "Argument must be a Number or Object" );
  }
  
  Local<Value> argv[2] = {
    NanNew<Number>( static_cast<uint32_t> ( value >> 32 ) ),
    NanNew<Number>( static_cast<uint32_t> ( value & 0xffffffffull ) ),
  };
  
  Local<Object> instance = NanNew( Int64::constructor )->NewInstance( 2, argv );
  
  NanReturnValue( instance );
  
}

NAN_METHOD( Int64::Xor ) {
  
  NanScope();
  Int64* obj = ObjectWrap::Unwrap<Int64>( args.This() );
  
  if( args.Length() < 1 ) {
    NanThrowError( "Missing argument" );
  }
  
  uint64_t value = 0;
  
  if( args[0]->IsNumber() ) {
    value = obj->mValue ^ args[0]->IntegerValue();
  } else if( args[0]->IsObject() ) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>( args[0]->ToObject() );
    value = obj->mValue ^ otherObj->mValue;
  } else {
    NanThrowTypeError( "Argument must be a Number or Object" );
  }
  
  Local<Value> argv[2] = {
    NanNew<Number>( static_cast<uint32_t> ( value >> 32 ) ),
    NanNew<Number>( static_cast<uint32_t> ( value & 0xffffffffull ) ),
  };
  
  Local<Object> instance = NanNew( Int64::constructor )->NewInstance( 2, argv );
  
  NanReturnValue( instance );
  
}

NAN_METHOD( Int64::Add ) {
  
  NanScope();
  Int64* obj = ObjectWrap::Unwrap<Int64>( args.This() );
  
  if( args.Length() < 1 ) {
    NanThrowError( "Missing argument" );
  }
  
  uint64_t value = 0;
  
  if( args[0]->IsNumber() ) {
    value = obj->mValue + args[0]->IntegerValue();
  } else if( args[0]->IsObject() ) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>( args[0]->ToObject() );
    value = obj->mValue + otherObj->mValue;
  } else {
    NanThrowTypeError( "Argument must be a Number or Object" );
  }
  
  Local<Value> argv[2] = {
    NanNew<Number>( static_cast<uint32_t> ( value >> 32 ) ),
    NanNew<Number>( static_cast<uint32_t> ( value & 0xffffffffull ) ),
  };
  
  Local<Object> instance = NanNew( Int64::constructor )->NewInstance( 2, argv );
  
  NanReturnValue( instance );
  
}

NAN_METHOD( Int64::Sub ) {
  
  NanScope();
  Int64* obj = ObjectWrap::Unwrap<Int64>( args.This() );
  
  if( args.Length() < 1 ) {
    NanThrowError( "Missing argument" );
  }
  
  uint64_t value = 0;
  
  if( args[0]->IsNumber() ) {
    value = obj->mValue - args[0]->IntegerValue();
  } else if( args[0]->IsObject() ) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>( args[0]->ToObject() );
    value = obj->mValue - otherObj->mValue;
  } else {
    NanThrowTypeError( "Argument must be a Number or Object" );
  }
  
  Local<Value> argv[2] = {
    NanNew<Number>( static_cast<uint32_t> ( value >> 32 ) ),
    NanNew<Number>( static_cast<uint32_t> ( value & 0xffffffffull ) ),
  };
  
  Local<Object> instance = NanNew( Int64::constructor )->NewInstance( 2, argv );
  
  NanReturnValue( instance );
  
}
