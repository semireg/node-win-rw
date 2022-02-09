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

#endif