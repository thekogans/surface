#include <algorithm>
#include <map>
#include "thekogans/surface/win32/Win32Surface.h"

namespace thekogans {
    namespace surface {

        namespace {
            util::ui32 GetBPP () {
                HDC hdc = GetDC (0);
                DWORD bpp = GetDeviceCaps (hdc, BITSPIXEL);
                ReleaseDC (0, hdc);
                return bpp;
            }
        }

        Win32Surface::Win32Surface (HWND hwnd_) :
                Surface (GetBPP ()),
                hwnd (hwnd_),
                hdc (0),
                bitmapInfo (bpp),
                hbitmap (0),
                hpalette (0) {
            // Create system font.
            class SystemFont : public Font {
            public:
                class Glyph : public Font::Glyph {
                protected:
                    util::ui32 width;
                    std::vector<util::ui32> bitmap;
                    std::vector<std::pair<util::ui32, util::i32> > kernInfo;

                public:
                    Glyph (util::ui32 width_, const std::vector<util::ui32> &bitmap_,
                        const std::vector<std::pair<util::ui32, util::i32> > &kernInfo_) :
                        width (width_), bitmap (bitmap_), kernInfo (kernInfo_) {}
                    virtual util::ui32 GetWidth () const {return width;}
                    virtual const util::ui32 *GetBitmap () const {return &bitmap[0];}
                    virtual util::ui32 GetKernInfoCount () const {return kernInfo.size ();}
                    virtual const std::pair<util::ui32, util::i32> *GetKernInfo () const {return &kernInfo[0];}
                };

            public:
                SystemFont () {
                    HDC hdcScreen = GetDC (0);
                    HFONT hfont = (HFONT)SelectObject (hdcScreen, GetStockObject (SYSTEM_FONT));
                    LOGFONT lf;
                    GetObject (GetStockObject (SYSTEM_FONT), sizeof (LOGFONT), &lf);
                    name = lf.lfFaceName;
                    std::vector<KERNINGPAIR> kerningPairs;
                    kerningPairs.resize (GetKerningPairs (hdcScreen, 0, 0));
                    if (!kerningPairs.empty ()) {
                        GetKerningPairs (hdcScreen, kerningPairs.size (), &kerningPairs[0]);
                    }
                    std::map<WORD, std::map<WORD, util::i32> > kerningMap;
                    for (size_t i = 0, count = kerningPairs.size (); i < count; ++i) {
                        kerningMap[kerningPairs[i].wFirst][kerningPairs[i].wSecond] = kerningPairs[i].iKernAmount;
                    }
                    TEXTMETRIC tm;
                    GetTextMetrics (hdcScreen, &tm);
                    aveCharWidth = tm.tmAveCharWidth;
                    height = tm.tmHeight;
                    HDC hdc = CreateCompatibleDC (hdcScreen);
                    HBITMAP hbitmap = (HBITMAP)SelectObject (hdc,
                        CreateCompatibleBitmap (hdcScreen, tm.tmHeight * 2, tm.tmMaxCharWidth * 2));
                    for (util::ui32 ch = 0; ch < 256; ++ch) {
                        char text[2] = {ch, 0};
                        TextOut (hdc, 0, 0, text, 1);
                        SIZE size;
                        GetTextExtentPoint (hdc, text, 1, &size);
                        std::vector<util::ui32> bitmap;
                        DWORD value = 0;
                        DWORD mask = 0x80000000;
                        for (util::i32 y = 0; y < size.cy; ++y) {
                            for (util::i32 x = 0; x < size.cx; ++x) {
                                COLORREF color = GetPixel (hdc, x, y);
                                if (!GetRValue (color)) {
                                    value |= mask;
                                }
                                mask >>= 1;
                                if (!mask) {
                                    bitmap.push_back (value);
                                    value = 0;
                                    mask = 0x80000000;
                                }
                            }
                        }
                        bitmap.push_back (value);
                        const std::map<WORD, util::i32> &charKerningMap = kerningMap[ch];
                        std::vector<std::pair<util::ui32, util::i32> > kernInfo;
                        typedef std::map<WORD, util::i32>::const_iterator const_iterator;
                        for (const_iterator it = charKerningMap.begin (), end = charKerningMap.end (); it != end; ++it) {
                            kernInfo.push_back (std::pair<util::ui32, util::i32> (it->first, it->second));
                        }
                        glyphs[ch] = new Glyph (size.cx, bitmap, kernInfo);
                    }
                    glyphsOwner = true;
                    DeleteObject (SelectObject (hdc, hbitmap));
                    DeleteDC (hdc);
                    SelectObject (hdcScreen, hfont);
                    ReleaseDC (0, hdcScreen);
                }
                SystemFont (const SystemFont &font) :
                    name (font.name), aveCharWidth (font.aveCharWidth), height (font.height), glyphsOwner (false) {
                    memcpy (glyphs, font.glyphs, 256 * sizeof (Glyph *));
                }
                virtual ~SystemFont () {
                    if (glyphsOwner) {
                        for (util::ui32 i = 0; i < 256; ++i) {
                            delete glyphs[i];
                        }
                    }
                }
                SystemFont &operator = (const SystemFont &font) {
                    if (&font != this) {
                        if (glyphsOwner) {
                            for (util::ui32 i = 0; i < 256; ++i) {
                                delete glyphs[i];
                            }
                        }
                        name = font.name;
                        aveCharWidth = font.aveCharWidth;
                        height = font.height;
                        memcpy (glyphs, font.glyphs, 256 * sizeof (Glyph *));
                        glyphsOwner = false;
                    }
                    return *this;
                }
                virtual std::auto_ptr<Surface::Font> Clone () const {return std::auto_ptr<Surface::Font> (new SystemFont (*this));}
                virtual const char *GetName () const {return name.c_str ();}
                virtual util::ui32 GetAveCharWidth () const {return aveCharWidth;}
                virtual util::ui32 GetHeight () const {return height;}
                virtual const Font::Glyph *GetGlyph (util::ui32 ch) const {return glyphs[ch];}

            protected:
                std::string name;
                util::ui32 aveCharWidth;
                util::ui32 height;
                Glyph *glyphs[256];
                bool glyphsOwner;
            };
            systemFont.reset (new SystemFont);
            assert (systemFont.get ());
            fonts.insert (util::OwnerHashMap<std::string, Font>::value_type (font->GetName (), font->Clone ().release ()));

            // Create off screen hdc.
            HDC wndHDC = GetDC (hwnd);
            if (!wndHDC) {
                throw Win32Error (GetLastError ());
            }
            hdc = CreateCompatibleDC (wndHDC);
            if (!hdc) {
                throw Win32Error (GetLastError ());
            }
            ReleaseDC (hwnd, wndHDC);

            SetColorMap (colorMap);
        }

        Win32Surface::~Win32Surface () {
            if (hbitmap) {
                DeleteObject (SelectObject (hdc, hbitmap));
            }
            if (hpalette) {
                DeleteObject (hpalette);
            }
            if (hdc) {
                DeleteDC (hdc);
            }
        }

        void Win32Surface::Resize (
                util::ui32 width_,
                util::ui32 height_) {
            assert (width_ > 0);
            assert (height_ > 0);
            if (width_ > 0 && height_ > 0) {
                if (hbitmap) {
                    DeleteObject (SelectObject (hdc, hbitmap));
                    hbitmap = 0;
                    surface = 0;
                }
                bitmapInfo.bmiHeader.biWidth = width_;
                bitmapInfo.bmiHeader.biHeight = -(util::i32)height_;
                hbitmap = CreateDIBSection (
                    hdc,
                    &bitmapInfo,
                    bpp == BPP_8 ? DIB_PAL_COLORS : DIB_RGB_COLORS,
                    (void **)&surface,
                    0,
                    0);
                if (!hbitmap) {
                    throw Win32Error (GetLastError ());
                }
                hbitmap = (HBITMAP)SelectObject (hdc, hbitmap);
                stride = (width_ * GetBytesPerPixel () + 3) & ~3;
                width = width_;
                height = height_;
            }
        }

        void Win32Surface::Flip (const blas::Rect *flipRect) {
            HDC wndHDC = GetDC (hwnd);
            if (!wndHDC) {
                throw Win32Error (GetLastError ());
            }
            blas::Rect clipRect;
            GetClipBox (wndHDC, (RECT *)&clipRect);
            if (!flipRect) {
                flipRect = &clipRect;
            }
            // Find a rectangle that describes the intersection of the
            // flip rect, clip rect, and DIB rect.
            blas::Rect bltRect = *flipRect & clipRect;
            if (!BitBlt (
                    wndHDC,
                    bltRect.left,
                    bltRect.top,
                    bltRect.right - bltRect.left + 1,
                    bltRect.bottom - bltRect.top + 1,
                    hdc,
                    bltRect.left,
                    bltRect.top,
                    SRCCOPY) || !ReleaseDC (hwnd, wndHDC)) {
                throw Win32Error (GetLastError ());
            }
        }

        // Set a new color map to be used during color translation.
        // ***NOTE***: While the color map can theoretically be any size,
        // in color index mode only the first 256 colors will be used to
        // create and populate the hardware palette.
        void Win32Surface::SetColorMap (const std::vector<COLOR> &colorMap_) {
            colorMap = colorMap_;
            // FIXME: We are assuming that hardware palettes only exist in
            // indexed color mode.
            if (bpp == BPP_8) {
                if (hpalette) {
                    DeleteObject (hpalette);
                    hpalette = 0;
                }
                if (!colorMap.empty ()) {
                    struct Palette : public LOGPALETTE {
                        PALETTEENTRY entries[256];
                        explicit Palette (const std::vector<COLOR> &colorMap) {
                            palVersion = 0x300;
                            palNumEntries = colorMap.size ();
                            for (util::ui32 i = 0, count = std::min (colorMap.size (), 256U); i < count; ++i) {
                                palPalEntry[i].peRed = GetRed (colorMap[i]);
                                palPalEntry[i].peGreen = GetGreen (colorMap[i]);
                                palPalEntry[i].peBlue = GetBlue (colorMap[i]);
                                palPalEntry[i].peFlags = PC_NOCOLLAPSE;
                            }
                        }
                    } palette (colorMap);
                    hpalette = CreatePalette (&palette);
                    if (!hpalette) {
                        throw Win32Error (GetLastError ());
                    }
                }
                SelectPalette (hdc, hpalette, FALSE);
            }
        }

    } // namespace surface
} // namespace thekogans
