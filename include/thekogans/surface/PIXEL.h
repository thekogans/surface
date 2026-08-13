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

            PIXEL_RGBA (
                util::ui8 r_ = 0,
                util::ui8 g_ = 0,
                util::ui8 b_ = 0,
                util::ui8 a_ = 255) :
                r (r_),
                g (g_),
                b (b_),
                a (a_) {}
            PIXEL_RGBA (COLOR color) :
                r (color.r),
                g (color.g),
                b (color.b),
                a (color.a) {}
        };

        // kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Big
        struct PIXEL_ARGB {
            util::ui8 a;
            util::ui8 r;
            util::ui8 g;
            util::ui8 b;

            PIXEL_ARGB (
                util::ui8 r_ = 0,
                util::ui8 g_ = 0,
                util::ui8 b_ = 0,
                util::ui8 a_ = 255) :
                a (a_),
                r (r_),
                g (g_),
                b (b_) {}
            PIXEL_ARGB (COLOR color) :
                a (color.a),
                r (color.r),
                g (color.g),
                b (color.b) {}
        };

        // kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Little
        struct PIXEL_BGRA {
            util::ui8 b;
            util::ui8 g;
            util::ui8 r;
            util::ui8 a;

            PIXEL_BGRA (
                util::ui8 r_ = 0,
                util::ui8 g_ = 0,
                util::ui8 b_ = 0,
                util::ui8 a_ = 255) :
                b (b_),
                g (g_),
                r (r_),
                a (a_) {}
            PIXEL_BGRA (COLOR color) :
                b (color.b),
                g (color.g),
                r (color.r),
                a (color.a) {}
        };

        // kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little
        struct PIXEL_ABGR {
            util::ui8 a;
            util::ui8 b;
            util::ui8 g;
            util::ui8 r;

            PIXEL_ABGR (
                util::ui8 r_ = 0,
                util::ui8 g_ = 0,
                util::ui8 b_ = 0,
                util::ui8 a_ = 255) :
                a (a_),
                b (b_),
                g (g_),
                r (r_) {}
            PIXEL_ABGR (COLOR color) :
                a (color.a),
                b (color.b),
                g (color.g),
                r (color.r) {}
        };

    } // namespace surface
} // namespace thekogans

#endif // __thekogans_surface_PIXEL_h
