#include "win_rw.hpp"

#if _MSC_VER
#include <windows.h>
#include <Winspool.h>
#include <Wingdi.h>
#pragma  comment(lib, "Winspool.lib")
#else
#error "Unsupported compiler for windows. Feel free to add it."
#endif

#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <node_version.h>

// possibly remove
#include <node_buffer.h>

namespace{
    /**
     * Returns last error code and message string
     */
    std::string getLastErrorCodeAndMessage() {
    	std::ostringstream s;
    	DWORD erroCode = GetLastError();
    	s << "code: " << erroCode;
    	DWORD retSize;
    	LPTSTR pTemp = NULL;
    	retSize = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
                                FORMAT_MESSAGE_FROM_SYSTEM|
                                FORMAT_MESSAGE_ARGUMENT_ARRAY,
                                NULL,
                                erroCode,
                                LANG_NEUTRAL,
                                (LPTSTR)&pTemp,
                                0,
                                NULL );
        if (retSize && pTemp != NULL) {
	    //pTemp[strlen(pTemp)-2]='\0'; //remove cr and newline character
	    //TODO: check if it is needed to convert c string to std::string
	    std::string stringMessage(pTemp);
	    s << ", message: " << stringMessage;
	    LocalFree((HLOCAL)pTemp);
	}

    	return s.str();
    }
}

MY_NODE_MODULE_CALLBACK(WritePath)
{
    MY_NODE_MODULE_HANDLESCOPE;
    REQUIRE_ARGUMENTS(iArgs, 2);

    REQUIRE_ARGUMENT_STRING(iArgs, 0, path);

    std::string data;
    v8::Local<v8::Value> arg1(iArgs[1]);
    if (!getStringOrBufferFromV8Value(arg1, data))
    {
        RETURN_EXCEPTION_STR("Argument 1 must be a string or Buffer");
    }

    HANDLE handle = CreateFile(*path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (handle == INVALID_HANDLE_VALUE)
    {
        std::string error_str("error on write open: ");
        error_str += getLastErrorCodeAndMessage();
        printf("%s", error_str.c_str());
        RETURN_EXCEPTION_STR(error_str.c_str());
    }

    COMMTIMEOUTS cto;
    GetCommTimeouts(handle, &cto);
    cto.WriteTotalTimeoutConstant = 1500;
    cto.WriteTotalTimeoutMultiplier = 0;
    SetCommTimeouts(handle, &cto);

    DWORD dataSize = (DWORD)data.size();
    DWORD batchSize = 4294967295;
    DWORD totalWritten = 0;

    while (totalWritten < dataSize)
    {
        DWORD thisSize = (dataSize - totalWritten);
        if (thisSize > batchSize)
        {
            thisSize = batchSize; // Avoiding terrible C++ max syntax
        }

        DWORD written;
        // printf("DEBUG: about to write:%lu\n", thisSize);
        BOOL writeOK = WriteFile(handle, (LPVOID)(data.c_str() + totalWritten), thisSize, &written, NULL);
        // printf("DEBUG: written:%lu\n", written);

        if (!writeOK)
        {
            std::string error_str("error on write: ");
            error_str += getLastErrorCodeAndMessage();
            CloseHandle(handle);
            printf("%s", error_str.c_str());
            RETURN_EXCEPTION_STR(error_str.c_str());
        }

        totalWritten += written;
    }

    CloseHandle(handle);
}

MY_NODE_MODULE_CALLBACK(ReadPath)
{
    MY_NODE_MODULE_HANDLESCOPE;
    REQUIRE_ARGUMENTS(iArgs, 1);

    REQUIRE_ARGUMENT_STRING(iArgs, 0, path);

    HANDLE handle = CreateFile(*path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
    if (handle == INVALID_HANDLE_VALUE)
    {
        std::string error_str("error on read open: ");
        error_str += getLastErrorCodeAndMessage();
        printf("%s", error_str.c_str());
        RETURN_EXCEPTION_STR(error_str.c_str());
    }

    DWORD nRead;
    char readBuf[1000] = {0};

    OVERLAPPED oRead = {0};
    oRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    // printf("DEBUG: about to read\n");
    BOOL readOK = ReadFile(handle, readBuf, 1000, &nRead, &oRead);

    DWORD dwWaitRes = WaitForSingleObject(oRead.hEvent, 50);
    DWORD reason = dwWaitRes - WAIT_OBJECT_0;
    BOOL didTimeout = reason == WAIT_TIMEOUT;

    readOK = GetOverlappedResult(handle, &oRead, &nRead, false);

    // printf("DEBUG: closing\n");

    CancelIo(handle);
    CloseHandle(handle);
    if (nRead > 999)
    {
        RETURN_EXCEPTION_STR("FIXME: too much data");
    }

    // printf("DEBUG: read: %lu\n", nRead);

    if (!readOK && didTimeout)
    {
        RETURN_EXCEPTION_STR("Timeout");
    }

    v8::Local<v8::Object> js_buffer = node::Buffer::Copy(isolate, (const char *)readBuf, nRead).ToLocalChecked();
    MY_NODE_MODULE_RETURN_VALUE(js_buffer);
}
