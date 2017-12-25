#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <node.h>
#include <node_buffer.h>
#include <v8.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "syscall/syscall.cc"

#define V8_RETURN_NUM(X) args.GetReturnValue().Set(Integer::New(args.GetIsolate(), X));
#define V8_RETURN_NUM64(X) args.GetReturnValue().Set(Int64ToArray(args.GetIsolate(), result));

namespace libsys {

    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Value;
    using v8::Handle;
    using v8::Array;
    using v8::Integer;
    using v8::Exception;
    using v8::ArrayBuffer;
    using v8::Uint8Array;
    using v8::TypedArray;


    uint64_t GetAddrBuffer(Local<Object> obj) {
        return (uint64_t) node::Buffer::Data(obj);
    }

    uint64_t GetAddrArrayBuffer(Local<Object> obj) {
        Local<ArrayBuffer> ab = obj.As<ArrayBuffer>();
        ArrayBuffer::Contents ab_c = ab->GetContents();
        return (uint64_t)(ab_c.Data());
    }

//    uint64_t GetAddrUint8Array(Local<Object> obj) {
//        Local<Uint8Array> ui = obj.As<Uint8Array>();
//        ArrayBuffer::Contents ab_c = ui->Buffer()->GetContents();
//        return (uint64_t)(ab_c.Data()) + ui->ByteOffset();
//    }

    uint64_t GetAddrTypedArray(Local<Object> obj) {
        Local<TypedArray> ta = obj.As<TypedArray>();
        ArrayBuffer::Contents ab_c = ta->Buffer()->GetContents();
        return (uint64_t)(ab_c.Data()) + ta->ByteOffset();
    }

    // Transfrom different JavaScript objects to 64-bit integer.
    int64_t ArgToInt(Local<Value> arg) {
        if(arg->IsNumber()) {
            return (int64_t) arg->Int32Value();
        } else {
            if(arg->IsString()) {
                String::Utf8Value v8str(arg->ToString());
    //            String::AsciiValue  v8str(arg->ToString());
                std::string cppstr = std::string(*v8str);
                const char *cstr = cppstr.c_str();
                return (uint64_t) cstr;
            } else if(arg->IsArrayBuffer()) {
                return GetAddrArrayBuffer(arg->ToObject());
//            } else if(arg->IsUint8Array()) {
            } else if(arg->IsTypedArray()) {
//                return GetAddrUint8Array(arg->ToObject());
                return GetAddrTypedArray(arg->ToObject());
            } else if (arg->IsArray()) { // [lo, hi, offset]
                Local<Array> arr = arg.As<Array>();
                uint32_t arrlen = arr->Length();

                int32_t lo = (int32_t) arr->Get(0)->Int32Value();
                int32_t hi = (int32_t) arr->Get(1)->Int32Value();

                uint64_t addr = (uint64_t) ((((int64_t) hi) << 32) | ((int64_t) lo & 0xffffffff));

                if(arrlen == 3) {
                    int32_t offset = (int32_t) arr->Get(2)->Int32Value();
                    addr += offset;
                }

                return addr;
            } else {
                // Assume it is `Buffer`.
                return GetAddrBuffer(arg->ToObject());
            }
        }
    }

    int64_t ExecSyscall(const FunctionCallbackInfo<Value>& args) {
        char len = (char) args.Length();

        int64_t cmd = (uint64_t) args[0]->Int32Value();
        if(len == 1) {
            return syscall0(cmd);
        }

        int64_t arg1 = ArgToInt(args[1]);
        if(len == 2) {
            return syscall1(cmd, arg1);
        }

        int64_t arg2 = ArgToInt(args[2]);
        if(len == 3) {
            return syscall2(cmd, arg1, arg2);
        }

        int64_t arg3 = ArgToInt(args[3]);
        if(len == 4) {
            return syscall3(cmd, arg1, arg2, arg3);
        }

        int64_t arg4 = ArgToInt(args[4]);
        if(len == 5) {
             return syscall4(cmd, arg1, arg2, arg3, arg4);
         }

        int64_t arg5 = ArgToInt(args[5]);
        if(len == 6) {
            return syscall5(cmd, arg1, arg2, arg3, arg4, arg5);
        }

        int64_t arg6 = ArgToInt(args[6]);
        if(len == 7) {
            return syscall6(cmd, arg1, arg2, arg3, arg4, arg5, arg6);
        }

        return -1;
    }

    void MethodSyscall(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        char len = (char) args.Length();
        if(len > 7) isolate->ThrowException(String::NewFromUtf8(isolate, "Syscall with over 6 arguments."));
        else args.GetReturnValue().Set(Integer::New(isolate, ExecSyscall(args)));
    }

    Handle<Array> Int64ToArray(Isolate* isolate, int64_t number) {
        int32_t lo = number & 0xffffffff;
        int32_t hi = number >> 32;
        Handle<Array> array = Array::New(isolate, 2);
        array->Set(0, Integer::New(isolate, lo));
        array->Set(1, Integer::New(isolate, hi));
        return array;
    }

    void MethodSyscall64(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        char len = (char) args.Length();
        if(len > 7) isolate->ThrowException(String::NewFromUtf8(isolate, "Syscall with over 6 arguments."));
        else {
            int64_t result = ExecSyscall(args);
            args.GetReturnValue().Set(Int64ToArray(isolate, result));
        }
    }

    void MethodSyscall_0(const FunctionCallbackInfo<Value>& args) {
        int64_t cmd = (int64_t) args[0]->Int32Value();
        int64_t result = syscall0(cmd);
        V8_RETURN_NUM(result);
    }

    void MethodSyscall64_0(const FunctionCallbackInfo<Value>& args) {
        int64_t cmd = (int64_t) args[0]->Int32Value();
        int64_t result = syscall0(cmd);
        V8_RETURN_NUM64(result);
    }

    void MethodSyscall_1(const FunctionCallbackInfo<Value>& args) {
        int64_t cmd = (int64_t) args[0]->Int32Value();
        int64_t arg1 = (int64_t) args[1]->Int32Value();
        int64_t result = syscall1(cmd, arg1);
        V8_RETURN_NUM(result);
    }

    void MethodSyscall64_1(const FunctionCallbackInfo<Value>& args) {
        int64_t cmd = (int64_t) args[0]->Int32Value();
        int64_t arg1 = (int64_t) args[1]->Int32Value();
        int64_t result = syscall1(cmd, arg1);
        V8_RETURN_NUM64(result);
    }

    void MethodSyscall_2(const FunctionCallbackInfo<Value>& args) {
        int64_t cmd = (int64_t) args[0]->Int32Value();
        int64_t arg1 = (int64_t) args[1]->Int32Value();
        int64_t arg2 = (int64_t) args[2]->Int32Value();
        int64_t result = syscall2(cmd, arg1, arg2);
        V8_RETURN_NUM(result);
    }

    void MethodSyscall64_2(const FunctionCallbackInfo<Value>& args) {
        int64_t cmd = (int64_t) args[0]->Int32Value();
        int64_t arg1 = (int64_t) args[1]->Int32Value();
        int64_t arg2 = (int64_t) args[2]->Int32Value();
        int64_t result = syscall2(cmd, arg1, arg2);
        V8_RETURN_NUM64(result);
    }

    void MethodSyscall_3(const FunctionCallbackInfo<Value>& args) {
        int64_t cmd = (int64_t) args[0]->Int32Value();
        int64_t arg1 = (int64_t) args[1]->Int32Value();
        int64_t arg2 = (int64_t) args[2]->Int32Value();
        int64_t arg3 = (int64_t) args[3]->Int32Value();
        int64_t result = syscall3(cmd, arg1, arg2, arg3);
        V8_RETURN_NUM(result);
    }

    void MethodSyscall64_3(const FunctionCallbackInfo<Value>& args) {
        int64_t cmd = (int64_t) args[0]->Int32Value();
        int64_t arg1 = (int64_t) args[1]->Int32Value();
        int64_t arg2 = (int64_t) args[2]->Int32Value();
        int64_t arg3 = (int64_t) args[3]->Int32Value();
        int64_t result = syscall3(cmd, arg1, arg2, arg3);
        V8_RETURN_NUM64(result);
    }

    void MethodSyscall_4(const FunctionCallbackInfo<Value>& args) {
        int64_t cmd = (int64_t) args[0]->Int32Value();
        int64_t arg1 = (int64_t) args[1]->Int32Value();
        int64_t arg2 = (int64_t) args[2]->Int32Value();
        int64_t arg3 = (int64_t) args[3]->Int32Value();
        int64_t arg4 = (int64_t) args[4]->Int32Value();
        int64_t result = syscall4(cmd, arg1, arg2, arg3, arg4);
        V8_RETURN_NUM(result);
    }

    void MethodSyscall64_4(const FunctionCallbackInfo<Value>& args) {
        int64_t cmd = (int64_t) args[0]->Int32Value();
        int64_t arg1 = (int64_t) args[1]->Int32Value();
        int64_t arg2 = (int64_t) args[2]->Int32Value();
        int64_t arg3 = (int64_t) args[3]->Int32Value();
        int64_t arg4 = (int64_t) args[4]->Int32Value();
        int64_t result = syscall4(cmd, arg1, arg2, arg3, arg4);
        V8_RETURN_NUM64(result);
    }

    void MethodSyscall_5(const FunctionCallbackInfo<Value>& args) {
        int64_t cmd = (int64_t) args[0]->Int32Value();
        int64_t arg1 = (int64_t) args[1]->Int32Value();
        int64_t arg2 = (int64_t) args[2]->Int32Value();
        int64_t arg3 = (int64_t) args[3]->Int32Value();
        int64_t arg4 = (int64_t) args[4]->Int32Value();
        int64_t arg5 = (int64_t) args[5]->Int32Value();
        int64_t result = syscall5(cmd, arg1, arg2, arg3, arg4, arg5);
        V8_RETURN_NUM(result);
    }

    void MethodSyscall64_5(const FunctionCallbackInfo<Value>& args) {
        int64_t cmd = (int64_t) args[0]->Int32Value();
        int64_t arg1 = (int64_t) args[1]->Int32Value();
        int64_t arg2 = (int64_t) args[2]->Int32Value();
        int64_t arg3 = (int64_t) args[3]->Int32Value();
        int64_t arg4 = (int64_t) args[4]->Int32Value();
        int64_t arg5 = (int64_t) args[5]->Int32Value();
        int64_t result = syscall5(cmd, arg1, arg2, arg3, arg4, arg5);
        V8_RETURN_NUM64(result);
    }

    void MethodSyscall_6(const FunctionCallbackInfo<Value>& args) {
        int64_t cmd = (int64_t) args[0]->Int32Value();
        int64_t arg1 = (int64_t) args[1]->Int32Value();
        int64_t arg2 = (int64_t) args[2]->Int32Value();
        int64_t arg3 = (int64_t) args[3]->Int32Value();
        int64_t arg4 = (int64_t) args[4]->Int32Value();
        int64_t arg5 = (int64_t) args[5]->Int32Value();
        int64_t arg6 = (int64_t) args[6]->Int32Value();
        int64_t result = syscall6(cmd, arg1, arg2, arg3, arg4, arg5, arg6);
        V8_RETURN_NUM(result);
    }

    void MethodSyscall64_6(const FunctionCallbackInfo<Value>& args) {
        int64_t cmd = (int64_t) args[0]->Int32Value();
        int64_t arg1 = (int64_t) args[1]->Int32Value();
        int64_t arg2 = (int64_t) args[2]->Int32Value();
        int64_t arg3 = (int64_t) args[3]->Int32Value();
        int64_t arg4 = (int64_t) args[4]->Int32Value();
        int64_t arg5 = (int64_t) args[5]->Int32Value();
        int64_t arg6 = (int64_t) args[6]->Int32Value();
        int64_t result = syscall6(cmd, arg1, arg2, arg3, arg4, arg5, arg6);
        V8_RETURN_NUM64(result);
    }

    void MethodAddrArrayBuffer(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        uint64_t addr = GetAddrArrayBuffer(args[0]->ToObject());
        args.GetReturnValue().Set(Integer::New(isolate, addr));
    }

    void MethodAddrArrayBuffer64(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        uint64_t addr = GetAddrArrayBuffer(args[0]->ToObject());
        if(args.Length() == 2) {
            int32_t offset = (int32_t) args[1]->Int32Value();
            addr += offset;
        }
        args.GetReturnValue().Set(Int64ToArray(isolate, addr));
    }

    void MethodAddrTypedArray(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        uint64_t addr = GetAddrTypedArray(args[0]->ToObject());
        args.GetReturnValue().Set(Integer::New(isolate, addr));
    }

    void MethodAddrTypedArray64(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        uint64_t addr = GetAddrTypedArray(args[0]->ToObject());
        args.GetReturnValue().Set(Int64ToArray(isolate, addr));
    }

    void MethodAddrBuffer(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        uint64_t addr = GetAddrBuffer(args[0]->ToObject());
        args.GetReturnValue().Set(Integer::New(isolate, addr));
    }

    void MethodAddrBuffer64(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        int64_t addr = GetAddrBuffer(args[0]->ToObject());
        args.GetReturnValue().Set(Int64ToArray(isolate, addr));
    }

    void MethodFrame(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        void* addr = (void*) ArgToInt(args[0]);
        size_t size = (size_t) args[1]->Int32Value();

        Local<ArrayBuffer> buf = ArrayBuffer::New(isolate, addr, size);
        args.GetReturnValue().Set(buf);
    }

    typedef int64_t number; // JavaScript number.
    typedef number (*callback)();
    typedef number (*callback1)(number arg1);
    typedef number (*callback2)(number arg1, number arg2);
    typedef number (*callback3)(number arg1, number arg2, number arg3);
    typedef number (*callback4)(number arg1, number arg2, number arg3, number arg4);
    typedef number (*callback5)(number arg1, number arg2, number arg3, number arg4, number arg5);
    typedef number (*callback6)(number arg1, number arg2, number arg3, number arg4, number arg5, number arg6);
    typedef number (*callback7)(number arg1, number arg2, number arg3, number arg4, number arg5, number arg6, number arg7);
    typedef number (*callback8)(number arg1, number arg2, number arg3, number arg4, number arg5, number arg6, number arg7, number arg8);
    typedef number (*callback9)(number arg1, number arg2, number arg3, number arg4, number arg5, number arg6, number arg7, number arg8, number arg9);
    typedef number (*callback10)(number arg1, number arg2, number arg3, number arg4, number arg5, number arg6, number arg7, number arg8, number arg9, number arg10);

    int64_t call_method(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        uint64_t addr = ArgToInt(args[0]);
        char len = (char) args.Length();

        int32_t offset;
        if(len > 1) {
            offset = (int32_t) args[1]->Int32Value();
            addr += offset;
        }

        if(len <= 2) {
            return ((callback) addr)();
        }

        Local<Array> arr = args[2].As<Array>();
        uint32_t arrlen = arr->Length();

        switch(arrlen) {
            case 0:
                return ((callback) addr)();
            case 1:
                return ((callback1) addr)(ArgToInt(arr->Get(0)));
            case 2:
                return ((callback2) addr)(
                    ArgToInt(arr->Get(0)), ArgToInt(arr->Get(1))
                );
            case 3:
                return ((callback3) addr)(
                    ArgToInt(arr->Get(0)), ArgToInt(arr->Get(1)), ArgToInt(arr->Get(2))
                );
            case 4:
                return ((callback4) addr)(
                    ArgToInt(arr->Get(0)), ArgToInt(arr->Get(1)), ArgToInt(arr->Get(2)), ArgToInt(arr->Get(3))
                );
            case 5:
                return ((callback5) addr)(
                    ArgToInt(arr->Get(0)), ArgToInt(arr->Get(1)), ArgToInt(arr->Get(2)), ArgToInt(arr->Get(3)), ArgToInt(arr->Get(4))
                );
            case 6:
                return ((callback6) addr)(
                    ArgToInt(arr->Get(0)), ArgToInt(arr->Get(1)), ArgToInt(arr->Get(2)), ArgToInt(arr->Get(3)), ArgToInt(arr->Get(4)),
                    ArgToInt(arr->Get(5))
                );
            case 7:
                return ((callback7) addr)(
                    ArgToInt(arr->Get(0)), ArgToInt(arr->Get(1)), ArgToInt(arr->Get(2)), ArgToInt(arr->Get(3)), ArgToInt(arr->Get(4)),
                    ArgToInt(arr->Get(5)), ArgToInt(arr->Get(6))
                );
            case 8:
                return ((callback8) addr)(
                    ArgToInt(arr->Get(0)), ArgToInt(arr->Get(1)), ArgToInt(arr->Get(2)), ArgToInt(arr->Get(3)), ArgToInt(arr->Get(4)),
                    ArgToInt(arr->Get(5)), ArgToInt(arr->Get(6)), ArgToInt(arr->Get(7))
                );
            case 9:
                return ((callback9) addr)(
                    ArgToInt(arr->Get(0)), ArgToInt(arr->Get(1)), ArgToInt(arr->Get(2)), ArgToInt(arr->Get(3)), ArgToInt(arr->Get(4)),
                    ArgToInt(arr->Get(5)), ArgToInt(arr->Get(6)), ArgToInt(arr->Get(7)), ArgToInt(arr->Get(8))
                );
            case 10:
                return ((callback10) addr)(
                    ArgToInt(arr->Get(0)), ArgToInt(arr->Get(1)), ArgToInt(arr->Get(2)), ArgToInt(arr->Get(3)), ArgToInt(arr->Get(4)),
                    ArgToInt(arr->Get(5)), ArgToInt(arr->Get(6)), ArgToInt(arr->Get(7)), ArgToInt(arr->Get(8)), ArgToInt(arr->Get(9))
                );
            default:
                isolate->ThrowException(String::NewFromUtf8(isolate, "Too many arguments."));
                return -1;
        }
    }

    void MethodCall(const FunctionCallbackInfo<Value>& args) {
        int64_t result = call_method(args);
        V8_RETURN_NUM(result);
    }

    void MethodCall64(const FunctionCallbackInfo<Value>& args) {
        int64_t result = call_method(args);
        V8_RETURN_NUM64(result);
    }

    void MethodCall_0(const FunctionCallbackInfo<Value>& args) {
        uint64_t addr = ArgToInt(args[0]);
        int64_t result = ((callback) addr)();
        V8_RETURN_NUM(result);
    }

    void MethodCall64_0(const FunctionCallbackInfo<Value>& args) {
        uint64_t addr = ArgToInt(args[0]);
        int64_t result = ((callback) addr)();
        V8_RETURN_NUM64(result);
    }

    void MethodCall_1(const FunctionCallbackInfo<Value>& args) {
        uint64_t addr = ArgToInt(args[0]);
        int64_t arg1 = ArgToInt(args[1]);
        int64_t result = ((callback1) addr)(arg1);
        V8_RETURN_NUM(result);
    }

    void MethodCall64_1(const FunctionCallbackInfo<Value>& args) {
        uint64_t addr = ArgToInt(args[0]);
        int64_t arg1 = ArgToInt(args[1]);
        int64_t result = ((callback1) addr)(arg1);
        V8_RETURN_NUM64(result);
    }

    void init(Local<Object> exports) {
        NODE_SET_METHOD(exports, "syscall",                 MethodSyscall);
        NODE_SET_METHOD(exports, "syscall64",               MethodSyscall64);
        NODE_SET_METHOD(exports, "syscall_0",               MethodSyscall_0);
        NODE_SET_METHOD(exports, "syscall_1",               MethodSyscall_1);
        NODE_SET_METHOD(exports, "syscall_2",               MethodSyscall_2);
        NODE_SET_METHOD(exports, "syscall_3",               MethodSyscall_3);
        NODE_SET_METHOD(exports, "syscall_4",               MethodSyscall_4);
        NODE_SET_METHOD(exports, "syscall_5",               MethodSyscall_5);
        NODE_SET_METHOD(exports, "syscall_6",               MethodSyscall_6);
        NODE_SET_METHOD(exports, "syscall64_0",             MethodSyscall64_0);
        NODE_SET_METHOD(exports, "syscall64_1",             MethodSyscall64_1);
        NODE_SET_METHOD(exports, "syscall64_2",             MethodSyscall64_2);
        NODE_SET_METHOD(exports, "syscall64_3",             MethodSyscall64_3);
        NODE_SET_METHOD(exports, "syscall64_4",             MethodSyscall64_4);
        NODE_SET_METHOD(exports, "syscall64_5",             MethodSyscall64_5);
        NODE_SET_METHOD(exports, "syscall64_6",             MethodSyscall64_6);
        NODE_SET_METHOD(exports, "addressArrayBuffer",      MethodAddrArrayBuffer);
        NODE_SET_METHOD(exports, "addressArrayBuffer64",    MethodAddrArrayBuffer64);
        NODE_SET_METHOD(exports, "addressTypedArray",       MethodAddrTypedArray);
        NODE_SET_METHOD(exports, "addressTypedArray64",     MethodAddrTypedArray64);
        NODE_SET_METHOD(exports, "addressBuffer",           MethodAddrBuffer);
        NODE_SET_METHOD(exports, "addressBuffer64",         MethodAddrBuffer64);
        NODE_SET_METHOD(exports, "frame",                   MethodFrame);
        NODE_SET_METHOD(exports, "call",                    MethodCall);
        NODE_SET_METHOD(exports, "call64",                  MethodCall64);
        NODE_SET_METHOD(exports, "call_0",                  MethodCall_0);
        NODE_SET_METHOD(exports, "call_1",                  MethodCall_1);
        NODE_SET_METHOD(exports, "call64_0",                MethodCall64_0);
        NODE_SET_METHOD(exports, "call64_1",                MethodCall64_1);
    }

    NODE_MODULE(addon, init)

}
