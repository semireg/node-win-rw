#include "win_rw.hpp"
#include <node_buffer.h>

NAN_MODULE_INIT(Init) {
    MY_MODULE_SET_METHOD(target, "readPath", ReadPath);
    MY_MODULE_SET_METHOD(target, "writePath", WritePath);
}

#if NODE_MAJOR_VERSION >= 10
NAN_MODULE_WORKER_ENABLED(win_rw, Init)
#else
NODE_MODULE(win_rw, Init)
#endif

bool getStringOrBufferFromV8Value(v8::Local<v8::Value> iV8Value, std::string &oData)
{
    if(iV8Value->IsString())
    {
        Nan::Utf8String data_str_v8(V8_LOCAL_STRING_FROM_VALUE(iV8Value));
        oData.assign(*data_str_v8, data_str_v8.length());
        return true;
    }
    if(iV8Value->IsObject() && node::Buffer::HasInstance(iV8Value))
    {
        oData.assign(node::Buffer::Data(iV8Value), node::Buffer::Length(iV8Value));
        return true;
    }
    return false;
}