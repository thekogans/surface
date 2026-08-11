#include "thekogans/util/Heap.h"
#include "thekogans/surface/Surface.h"

namespace thekogans {
    namespace surface {

        THEKOGANS_UTIL_IMPLEMENT_HEAP_FUNCTIONS (Surface)

        Surface::Surface (
                const util::Rectangle::Extents &extents/*,
                const PixelFormat *pixelFormat_*/) :
                rectangle (util::Point (), extents),
                buffer (new PIXELBuffer (extents))/*,
                pixelFormat (pixelFormat_)*/ {
            // buffer will check extents and throw if they're degenerate.
            // if (pixelFormat == nullptr) {
            //     THEKOGANS_UTIL_THROW_ERROR_CODE_EXCEPTION (
            //         THEKOGANS_UTIL_OS_ERROR_CODE_EINVAL);
            // }
        }

        Surface::Surface (
                const util::Rectangle &rectangle_,
                PIXELBuffer::SharedPtr buffer_/*,
                const PixelFormat *pixelFormat_*/) :
                rectangle (rectangle_),
                buffer (buffer_)/*,
                pixelFormat (pixelFormat_)*/ {
            if (buffer == nullptr ||
                    !rectangle.IsInside (util::Rectangle (util::Point (), buffer->extents))/* ||
                    pixelFormat == nullptr*/) {
                THEKOGANS_UTIL_THROW_ERROR_CODE_EXCEPTION (
                    THEKOGANS_UTIL_OS_ERROR_CODE_EINVAL);
            }
        }

    } // namespace surface
} // namespace thekogans
