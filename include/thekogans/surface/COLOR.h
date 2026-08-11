#if !defined (__thekogans_surface_COLOR_h)
#define __thekogans_surface_COLOR_h

#include <vector>
#include "thekogans/util/Types.h"
#include "thekogans/surface/Config.h"

namespace thekogans {
    namespace surface {

        struct COLOR {
            util::ui8 r;
            util::ui8 g;
            util::ui8 b;
            util::ui8 a;

            COLOR (
                util::ui8 r_,
                util::ui8 g_,
                util::ui8 b_,
                util::ui8 a_) :
                r (r_),
                g (g_),
                b (b_),
                a (a_) {}
        };

#if 0
        // The only difference between COLOR and PIXEL is COLOR has a fixed format (ARGB).
        using COLOR = util::ui32;

        constexpr util::ui32 colorAlphaMask = 0xff000000;
        constexpr util::ui32 colorAlphaShift = 24;
        constexpr util::ui32 colorRedMask = 0x00ff0000;
        constexpr util::ui32 colorRedShift = 16;
        constexpr util::ui32 colorGreenMask = 0x0000ff00;
        constexpr util::ui32 colorGreenShift = 8;
        constexpr util::ui32 colorBlueMask = 0x000000ff;
        constexpr util::ui32 colorBlueShift = 0;

        inline COLOR MakeCOLOR (
                util::ui32 alpha,
                util::ui32 red,
                util::ui32 green,
                util::ui32 blue) {
            return
                ((alpha << colorAlphaShift) & colorAlphaMask) |
                ((red << colorRedShift) & colorRedMask) |
                ((green << colorGreenShift) & colorGreenMask) |
                ((blue << colorBlueShift ) & colorBlueMask);
        }

        inline util::ui8 GetCOLORAlpha (COLOR color) {
            return (util::ui8)((color & colorAlphaMask) >> colorAlphaShift);
        }
        inline util::ui8 GetCOLORRed (COLOR color) {
            return (util::ui8)((color & colorRedMask) >> colorRedShift);
        }
        inline util::ui8 GetCOLORGreen (COLOR color) {
            return (util::ui8)((color & colorGreenMask) >> colorGreenShift);
        }
        inline util::ui8 GetCOLORBlue (COLOR color) {
            return (util::ui8)((color & colorBlueMask) >> colorBlueShift);
        }
#endif

    } // namespace surface
} // namespace thekogans

#endif // __thekogans_surface_COLOR_h
