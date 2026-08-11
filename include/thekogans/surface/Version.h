#if !defined (__thekogans_surface_Version_h)
#define __thekogans_surface_Version_h

#include <thekogans/util/Version.h>
#include "thekogans/surface/Config.h"

namespace thekogans {
    namespace surface {

        /// \brief
        /// Return the compiled thekogans_surface version.
        /// \return Compiled thekogans_surface version.
        _LIB_THEKOGANS_SURFACE_DECL const thekogans::util::Version &
            _LIB_THEKOGANS_SURFACE_API GetVersion ();

    } // namespace surface
} // namespace thekogans

#endif // !defined (__thekogans_surface_Version_h)
