#include "thekogans/surface/PIXEL.h"

namespace thekogans {
    namespace surface {

#if 0
        const PixelFormat *GetPixelFormat (PixelFormatType pixelFormatType) {
            static const PixelFormat pixelFormats[PIXEL_FORMAT_COUNT] = {
                // ARGB
                PixelFormat (0xff000000, 24, 0x00ff0000, 16, 0x0000ff00, 8, 0x000000ff, 0),
                // RGBA
                PixelFormat (0x000000ff, 0, 0xff000000, 24, 0x00ff0000, 16, 0x0000ff00, 8),
                // ABGR
                PixelFormat (0xff000000, 24, 0x000000ff, 0, 0x0000ff00, 8, 0x00ff0000, 16),
                // BGRA
                PixelFormat (0x000000ff, 0, 0x0000ff00, 8, 0x00ff0000, 16, 0xff000000, 24)
            };
            return &pixelFormats[pixelFormatType];
        }
#endif

    } // namespace surface
} // namespace thekogans
