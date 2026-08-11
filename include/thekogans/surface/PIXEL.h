#if !defined (__thekogans_surface_PIXEL_h)
#define __thekogans_surface_PIXEL_h

#include <vector>
#include "thekogans/util/Types.h"
#include "thekogans/surface/Config.h"
#include "thekogans/surface/COLOR.h"

namespace thekogans {
    namespace surface {

        struct PIXEL {
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

#if 0
        using PIXEL = util::ui32;

        struct PixelFormat {
            util::ui32 alphaMask;
            util::ui32 alphaShift;
            util::ui32 redMask;
            util::ui32 redShift;
            util::ui32 greenMask;
            util::ui32 greenShift;
            util::ui32 blueMask;
            util::ui32 blueShift;
            bool identity;

            PixelFormat (
                util::ui32 alphaMask_,
                util::ui32 alphaShift_,
                util::ui32 redMask_,
                util::ui32 redShift_,
                util::ui32 greenMask_,
                util::ui32 greenShift_,
                util::ui32 blueMask_,
                util::ui32 blueShift_) :
                alphaMask (alphaMask_),
                alphaShift (alphaShift_),
                redMask (redMask_),
                redShift (redShift_),
                greenMask (greenMask_),
                greenShift (greenShift_),
                blueMask (blueMask_),
                blueShift (blueShift_),
                identity (
                    alphaMask == colorAlphaMask &&
                    alphaShift == colorAlphaShift &&
                    redMask == colorRedMask &&
                    redShift == colorRedShift &&
                    greenMask == colorGreenMask &&
                    greenShift == colorGreenShift &&
                    blueMask == colorBlueMask &&
                    blueShift == colorBlueShift) {}

            inline PIXEL MakePIXEL (
                    util::ui32 alpha,
                    util::ui32 red,
                    util::ui32 green,
                    util::ui32 blue) const {
                return
                    ((alpha << alphaShift) & alphaMask) |
                    ((red << redShift) & redMask) |
                    ((green << greenShift) & greenMask) |
                    ((blue << blueShift) & blueMask);
            }
            inline PIXEL MakePIXEL (COLOR color) const {
                return identity ? color :
                    MakePIXEL (
                        GetCOLORAlpha (color),
                        GetCOLORRed (color),
                        GetCOLORGreen (color),
                        GetCOLORBlue (color));
            }

            inline util::ui8 GetPIXELAlpha (PIXEL pixel) const {
                return (util::ui8)((pixel & alphaMask) >> alphaShift);
            }
            inline util::ui8 GetPIXELRed (PIXEL pixel) const {
                return (util::ui8)((pixel & redMask) >> redShift);
            }
            inline util::ui8 GetPIXELGreen (PIXEL pixel) const {
                return (util::ui8)((pixel & greenMask) >> greenShift);
            }
            inline util::ui8 GetPIXELBlue (PIXEL pixel) const {
                return (util::ui8)((pixel & blueMask) >> blueShift);
            }

            inline COLOR MakeCOLOR (PIXEL pixel) const {
                return identity ? pixel :
                    surface::MakeCOLOR (
                        GetPIXELAlpha (pixel),
                        GetPIXELRed (pixel),
                        GetPIXELGreen (pixel),
                        GetPIXELBlue (pixel));
            }
        };

        enum PixelFormatType {
            PIXEL_FORMAT_ARGB,
            PIXEL_FORMAT_RGBA,
            PIXEL_FORMAT_ABGR,
            PIXEL_FORMAT_BGRA,
            PIXEL_FORMAT_COUNT
        };
        const PixelFormat *GetPixelFormat (PixelFormatType pixelFormatType);
#endif

    } // namespace surface
} // namespace thekogans

#endif // __thekogans_surface_PIXEL_h
