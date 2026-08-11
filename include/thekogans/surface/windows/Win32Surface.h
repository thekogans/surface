#if !defined (__thekogans_surface_win32_Win32Surface_h)
#define __thekogans_surface_win32_Win32Surface_h

#include "thekogans/util/Environment.h"
#include "thekogans/util/os/windows/WindowsHeader.h"
#include <vector>
#include "thekogans/util/Types.h"
#include "thekogans/blas/Rect.h"
#include "thekogans/surface/Config.h"
#include "thekogans/surface/Color.h"
#include "thekogans/surface/Surface.h"

namespace thekogans {
    namespace surface {

        // Provides windows gdi specific glue to have Surface act as a window
        // back buffer.

        class _LIB_THEKOGANS_SURFACE_DECL Win32Surface : public Surface {
        protected:
            HWND hwnd;
            HDC hdc;
            struct BitmapInfo : public BITMAPINFO {
                WORD palette[256];
                explicit BitmapInfo (util::ui32 bpp) {
                    bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
                    bmiHeader.biWidth = 0;
                    bmiHeader.biHeight = 0;
                    bmiHeader.biPlanes = 1;
                    bmiHeader.biBitCount = bpp;
                    bmiHeader.biCompression = BI_RGB;
                    bmiHeader.biSizeImage = 0;
                    bmiHeader.biXPelsPerMeter = 0;
                    bmiHeader.biYPelsPerMeter = 0;
                    bmiHeader.biClrUsed = 0;
                    bmiHeader.biClrImportant = 0;
                    // In color index mode, this will create an off screen bitmap
                    // with an identity palette. This in turn will result in much
                    // faster blts. It is understood that the user of the surface
                    // is responsible for creating and supplying an appropriate
                    // color map which will be used as the surface palette.
                    for (WORD i = 0; i < 256; ++i) {
                        palette[i] = i;
                    }
                }
            } bitmapInfo;
            HBITMAP hbitmap;
            HPALETTE hpalette;

        public:
            explicit Win32Surface (HWND hwnd_);
            virtual ~Win32Surface ();

            virtual void Resize (
                util::ui32 width_,
                util::ui32 height_);
            virtual void Flip (const blas::Rect *flipRect = 0);
            virtual void SetColorMap (const std::vector<COLOR> &colorMap_);
        };

    } // namespace surface
} // namespace thekogans

#endif // __thekogans_surface_win32_Win32Surface_h
