#pragma once

//#define CSARCX_API_USE_DLLS
//#define CSARCX_API_EXPORTS

#ifdef CSARCX_DEF
#error CSARCX_DEF was previously defined
#endif

#if defined(CSARCX_USE_DLLS) && defined(_MSC_VER)
# if defined(CSARCX_EXPORTS)
#  define CSARCX_DEF __declspec(dllexport)
# else
#  define CSARCX_DEF __declspec(dllimport)
# endif  // defined(CSARCX_EXPORTS)
#elif defined(CSARCX_USE_DLLS) && defined(CSARCX_EXPORTS)
# define CSARCX_DEF __attribute__((visibility("default")))
#else
# define CSARCX_DEF
#endif
