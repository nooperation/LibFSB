#pragma once
#define DllExport   __declspec( dllexport )
#include <sal.h>

extern "C"
{
    DllExport bool SaveFsbAsWav(
        _In_ const char* input,
        _In_ const unsigned int input_length,
        _In_ const char* output_path);

    DllExport const char *GetError();
}
