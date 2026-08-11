#include "thekogans/surface/Version.h"

using namespace thekogans;

namespace thekogans {
    namespace surface {

        _LIB_THEKOGANS_SURFACE_DECL const util::Version &
        _LIB_THEKOGANS_SURFACE_API GetVersion () {
            static const util::Version *version = new util::Version (
                THEKOGANS_SURFACE_MAJOR_VERSION,
                THEKOGANS_SURFACE_MINOR_VERSION,
                THEKOGANS_SURFACE_PATCH_VERSION);
            return *version;
        }

    } // namespace surface
} // namespace thekogans
