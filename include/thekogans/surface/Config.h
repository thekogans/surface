#if !defined (__thekogans_surface_Config_h)
#define __thekogans_surface_Config_h

#if !defined (__cplusplus)
    #error libthekogans_surface requires C++ compilation (use a .cpp suffix)
#endif // !defined (__cplusplus)

#if defined (TOOLCHAIN_OS_Windows)
    #define _LIB_THEKOGANS_SURFACE_API __stdcall
    #if defined (THEKOGANS_SURFACE_TYPE_Shared)
        #if defined (_LIB_THEKOGANS_SURFACE_BUILD)
            #define _LIB_THEKOGANS_SURFACE_DECL __declspec (dllexport)
        #else // defined (_LIB_THEKOGANS_SURFACE_BUILD)
            #define _LIB_THEKOGANS_SURFACE_DECL __declspec (dllimport)
        #endif // defined (_LIB_THEKOGANS_SURFACE_BUILD)
    #else // defined (THEKOGANS_SURFACE_TYPE_Shared)
        #define _LIB_THEKOGANS_SURFACE_DECL
    #endif // defined (THEKOGANS_SURFACE_TYPE_Shared)
    #if defined (_MSC_VER)
        #pragma warning (disable: 4251)  // using non-exported as public in exported
        #pragma warning (disable: 4786)
    #endif // defined (_MSC_VER)
#else // defined (TOOLCHAIN_OS_Windows)
    #define _LIB_THEKOGANS_SURFACE_API
    #define _LIB_THEKOGANS_SURFACE_DECL
#endif // defined (TOOLCHAIN_OS_Windows)

/// \def THEKOGANS_SURFACE_DISALLOW_COPY_AND_ASSIGN(type)
/// A convenient macro to suppress copy construction and assignment.
#define THEKOGANS_SURFACE_DISALLOW_COPY_AND_ASSIGN(type)\
private:\
    type (const type &);\
    type &operator = (const type &);

/// \def THEKOGANS_SURFACE
/// Logging subsystem name.
#define THEKOGANS_SURFACE "thekogans_surface"

#endif // !defined (__thekogans_surface_Config_h)
