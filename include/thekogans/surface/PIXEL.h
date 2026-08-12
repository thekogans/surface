#if !defined (__thekogans_surface_PIXEL_h)
#define __thekogans_surface_PIXEL_h

#include <vector>
#include "thekogans/util/Types.h"
#include "thekogans/surface/Config.h"
#include "thekogans/surface/COLOR.h"

namespace thekogans {
    namespace surface {

        // kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big
        struct PIXEL_RGBA {
            util::ui8 r;
            util::ui8 g;
            util::ui8 b;
            util::ui8 a;

            PIXEL (
                util::ui8 r_ = 0,
                util::ui8 g_ = 0,
                util::ui8 b_ = 0,
                util::ui8 a_ = 255) :
                r (r_),
                g (g_),
                b (b_),
                a (a_) {}
        };

        // kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Big
        struct PIXEL_ARGB {
            util::ui8 a;
            util::ui8 r;
            util::ui8 g;
            util::ui8 b;

            PIXEL (
                util::ui8 a_ = 255,
                util::ui8 r_ = 0,
                util::ui8 g_ = 0,
                util::ui8 b_ = 0) :
                a (a_),
                r (r_),
                g (g_),
                b (b_) {}
        };

        // kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Little
        struct PIXEL_BGRA {
            util::ui8 b;
            util::ui8 g;
            util::ui8 r;
            util::ui8 a;

            PIXEL (
                util::ui8 b_ = 0,
                util::ui8 g_ = 0,
                util::ui8 r_ = 0,
                util::ui8 a_ = 255) :
                b (b_),
                g (g_),
                r (r_),
                a (a_) {}
        };

        // kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little
        struct PIXEL_ABGR {
            util::ui8 a;
            util::ui8 b;
            util::ui8 g;
            util::ui8 r;

            PIXEL (
                util::ui8 a_ = 255,
                util::ui8 b_ = 0,
                util::ui8 g_ = 0,
                util::ui8 r_ = 0) :
                a (a_)
                b (b_),
                g (g_),
                r (r_) {}
        };

    } // namespace surface
} // namespace thekogans

#endif // __thekogans_surface_PIXEL_h
