#ifndef WIN_RW_HPP
#define WIN_RW_HPP

#include "macros.hh"

#include <node.h>
#include <v8.h>

#include <string>

/** communicate directly with device using path 
 */
MY_NODE_MODULE_CALLBACK(ReadPath);
MY_NODE_MODULE_CALLBACK(WritePath);

template<typename Type>
class MemValueBase
{
public:
    MemValueBase(): _value(NULL) {}

    /** Destructor. The allocated memory will be deallocated
    */
    virtual ~MemValueBase() {}

    Type * get() {return _value; }
    Type * operator ->() { return &_value; }
    operator bool() const { return (_value != NULL); }
protected:
    Type *_value;

    virtual void free() {};
};

/**
 * try to extract String or buffer from v8 value
 * @param iV8Value - source v8 value
 * @param oData - destination data
 * @return TRUE if value is String or Buffer, FALSE otherwise
 */
bool getStringOrBufferFromV8Value(v8::Local<v8::Value> iV8Value, std::string &oData);


#endif