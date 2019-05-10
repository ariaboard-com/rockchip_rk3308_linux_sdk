#include "cae_lib.h"

CAE_LIBHANDLE cae_LoadLibrary(const char* lib_name)
{
#if defined(WIN32)
    return LoadLibrary( lib_name );
#else
    return dlopen(lib_name, RTLD_LAZY);
#endif
}

int cae_FreeLibrary(CAE_LIBHANDLE lib_handle)
{
#if defined(WIN32) 
    return FreeLibrary( lib_handle );
#else
    dlclose(lib_handle);
    return 1;
#endif
}

void* cae_GetProcAddress(CAE_LIBHANDLE lib_handle, const char* fun_name)
{
#if defined(WIN32)
    return (void *)GetProcAddress( lib_handle, fun_name );
#else
    return dlsym(lib_handle, fun_name);
#endif
}

