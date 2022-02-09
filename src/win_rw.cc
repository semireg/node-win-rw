#include "win_rw.hpp"
#include <node_buffer.h>

void initNode(v8::Local<v8::Object> exports) {
    MY_MODULE_SET_METHOD(exports, "readPath", ReadPath);
    MY_MODULE_SET_METHOD(exports, "writePath", WritePath);
}

NODE_MODULE(win_rw, initNode);

bool getStringOrBufferFromV8Value(v8::Local<v8::Value> iV8Value, std::string &oData)
{
    v8::Isolate *isolate = v8::Isolate::GetCurrent();

    if (iV8Value->IsString())
    {
        v8::String::Utf8Value data_str_v8(isolate, iV8Value);
        oData.assign(*data_str_v8, data_str_v8.length());
        return true;
    }
    if (iV8Value->IsObject() && node::Buffer::HasInstance(iV8Value))
    {
        oData.assign(node::Buffer::Data(iV8Value), node::Buffer::Length(iV8Value));
        return true;
    }
    return false;
}

bool getStringOrBufferFromV8MaybeLocalString(v8::MaybeLocal<v8::String> iV8MaybeLocal, std::string &oData)
{
    v8::Local<v8::String> localString = iV8MaybeLocal.ToLocalChecked();
    return getStringOrBufferFromV8Value(localString, oData);
}
