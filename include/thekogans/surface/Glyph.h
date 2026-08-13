// Copyright 2011 Boris Kogan (boris@thekogans.net)
//
// This file is part of libthekogans_surface.
//
// libthekogans_surface is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libthekogans_surface is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libthekogans_surface. If not, see <http://www.gnu.org/licenses/>.

#if !defined (__thekogans_surface_Glyph_h)
#define __thekogans_surface_Glyph_h

#include <ft2build.h>
#include FT_FREETYPE_H
#include <vector>
#include <string>
#include "thekogans/util/Types.h"
#include "thekogans/util/Rectangle.h"
#include "thekogans/util/RefCounted.h"
#include "thekogans/surface/Config.h"
#include "thekogans/surface/COLOR.h"
#include "thekogans/surface/PIXEL.h"
#include "thekogans/surface/Surface.h"

namespace thekogans {
    namespace surface {

        struct _LIB_THEKOGANS_SURFACE_DECL Glyph : public util::RefCounted {
            THEKOGANS_UTIL_DECLARE_REF_COUNTED_POINTERS (Glyph)
            THEKOGANS_UTIL_DECLARE_STD_ALLOCATOR_FUNCTIONS

            util::Rectangle::Extents extents;
            util::Point origin;
            util::Point advance;
            std::vector<util::ui8> alphaBuffer;

            Glyph (
                const FT_Bitmap &bitmap,
                const util::Point &origin_,
                const util::Point &advance_);

            template<typename T>
            void Draw (
                    int startX,
                    int startY,
                    COLOR color,
                    typename Surface<T>::SharedPtr surface) const {
                for (unsigned int y = 0; y < extents.height; ++y) {
                    int pixelY = startY + y;
                    // Skip rows outside image bounds
                    if (pixelY >= 0 && pixelY < surface->rectangle.extents.height) {
                        for (unsigned int x = 0; x < extents.width; ++x) {
                            int pixelX = startX + x;
                            // Skip columns outside image bounds
                            if (pixelX >= 0 && pixelX < surface->rectangle.extents.width) {
                                util::ui8 glyphA = alphaBuffer[y * extents.width + x];
                                // Fully transparent, skip blending
                                if (glyphA != 0) {
                                    util::ui32 alpha = (glyphA * color.a + 127) / 255;
                                    util::ui32 inverseAlpha = 255 - alpha;
                                    typename Surface<T>::PixelType *bg =
                                        surface->GetBufferAtPoint (util::Point (pixelX, pixelY));
                                    bg->r = (color.r * alpha + bg->r * inverseAlpha + 127) / 255;
                                    bg->g = (color.g * alpha + bg->g * inverseAlpha + 127) / 255;
                                    bg->b = (color.b * alpha + bg->b * inverseAlpha + 127) / 255;
                                    bg->a = alpha + (bg->a * inverseAlpha + 127) / 255;
                                }
                            }
                        }
                    }
                }
            }

            THEKOGANS_UTIL_DISALLOW_COPY_AND_ASSIGN (Glyph)
        };

        class _LIB_THEKOGANS_SURFACE_DECL GlyphCache : public util::RefCounted {
            THEKOGANS_UTIL_DECLARE_REF_COUNTED_POINTERS (GlyphCache)

        private:
            // 95 printable ASCII chars (32 to 126).
            static constexpr int ASCII_START = 32;
            static constexpr int ASCII_END = 126;
            static constexpr int NUM_CHARS = ASCII_END - ASCII_START + 1;

            Glyph::SharedPtr glyphs[NUM_CHARS];
            FT_Vector kerningTable[NUM_CHARS][NUM_CHARS];
            bool hasKerning = false;

            // Helper to check if a character falls within our cached ASCII range
            inline bool InRange (char c) const {
                return c >= ASCII_START && c <= ASCII_END;
            }

        public:
            GlyphCache (
                const std::string &fontPath,
                int fontSize,
                bool bold = false);

            template<typename T>
            void DrawText (
                    const std::string &text,
                    int startX,
                    int startY,
                    COLOR color,
                    typename Surface<T>::SharedPtr surface) {
                int penX = startX;
                int penY = startY;
                char prev_char = 0;
                for (char c : text) {
                    Glyph::SharedPtr glyph = GetGlyph (c);
                    if (glyph != nullptr) {
                        penX += GetKerning (prev_char, c).x;
                        int glyphX = penX + glyph->origin.x;
                        int glyphY = penY - glyph->origin.y;
                        glyph->Draw<T> (glyphX, glyphY, color, surface);
                        penX += glyph->advance.x;
                        prev_char = c;
                    }
                }
            }
            util::Rectangle GetTextBounds (
                const std::string &text,
                int startX = 0,
                int startY = 0);

            // Fast O(1) lookup inline function for real-time loops
            inline const FT_Vector &GetKerning (
                    char left,
                    char right) const {
                static const FT_Vector zero {0, 0};
                return hasKerning && InRange (left) && InRange (right) ?
                    kerningTable[left - ASCII_START][right - ASCII_START] : zero;
            }

            inline Glyph::SharedPtr GetGlyph (char c) const {
                return InRange (c) ? glyphs[c - ASCII_START] : nullptr;
            }

            THEKOGANS_UTIL_DISALLOW_COPY_AND_ASSIGN (GlyphCache)
        };

    } // namespace surface
} // namespace thekogans

#endif // !defined (__thekogans_surface_Glyph_h)
