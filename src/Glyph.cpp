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

#include <immintrin.h>
#include "thekogans/util/Heap.h"
#include "thekogans/util/ByteSwap.h"
#include "thekogans/surface/Glyph.h"

namespace thekogans {
    namespace surface {

        THEKOGANS_UTIL_IMPLEMENT_HEAP_FUNCTIONS (Glyph)

        Glyph::Glyph (
                const FT_Bitmap &bitmap,
                const util::Point &origin_,
                const util::Point &advance_) :
                extents (bitmap.width, bitmap.rows),
                origin (origin_),
                advance (advance_) {
            if (!extents.IsDegenerate ()) {
                alphaBuffer.resize (extents.GetArea ());
                for (int y = 0; y < extents.height; ++y) {
                    std::memcpy (
                        &alphaBuffer[y * extents.width],
                        &bitmap.buffer[y * bitmap.pitch], extents.width);
                }
                // for (unsigned int y = 0; y < rows; ++y) {
                //     for (unsigned int x = 0; x < width; ++x) {
                //         unsigned char pixel = alphaBuffer[y * width + x];
                //         // Map 0-255 opacity to character density
                //         if (pixel > 200) std::cout << "#";
                //         else if (pixel > 120) std::cout << "%";
                //         else if (pixel > 40)  std::cout << "*";
                //         else if (pixel > 0)   std::cout << ".";
                //         else                  std::cout << " ";
                //     }
                //     std::cout << std::endl;
                // }
            }
        }

        GlyphCache::GlyphCache (
                const std::string &fontPath,
                int fontSize,
                bool bold) {
            // Zero out the kerning table initially
            std::memset (kerningTable, 0, sizeof (kerningTable));
            FT_Library library;
            if (FT_Init_FreeType (&library) == 0) {
                FT_Face face;
                if (FT_New_Face (library, fontPath.c_str (), 0, &face) == 0) {
                    FT_Set_Pixel_Sizes (face, 0, fontSize);
                    hasKerning = FT_HAS_KERNING (face);
                    // 1. Rasterize and cache individual glyph masks
                    for (char c = ASCII_START; c <= ASCII_END; ++c) {
                        if (FT_Load_Char (face, c, FT_LOAD_RENDER) == 0) {
                            FT_GlyphSlot glyph = face->glyph;
                            glyphs[c - ASCII_START] = new Glyph (
                                glyph->bitmap,
                                util::Point (glyph->bitmap_left, glyph->bitmap_top),
                                util::Point (glyph->advance_.x >> 6, glyph->advance_.y >> 6));
                        }
                    }
                    // 2. Pre-compile the 2D Kerning Table matrix
                    if (hasKerning) {
                        for (char left = ASCII_START; left <= ASCII_END; ++left) {
                            FT_UInt left_idx = FT_Get_Char_Index (face, left);
                            if (left_idx != 0) {
                                for (char right = ASCII_START; right <= ASCII_END; ++right) {
                                    FT_UInt right_idx = FT_Get_Char_Index (face, right);
                                    if (right_idx != 0) {
                                        FT_Vector delta {0, 0};
                                        FT_Get_Kerning (face, left_idx, right_idx, FT_KERNING_DEFAULT, &delta);
                                        delta.x >>= 6;
                                        delta.y >>= 6;
                                        kerningTable[left - ASCII_START][right - ASCII_START] = delta;
                                    }
                                }
                            }
                        }
                    }
                    FT_Done_Face (face);
                }
                FT_Done_FreeType (library);
            }
        }

        // Returns the bounding region of where this string will be drawn
        util::Rectangle GlyphCache::GetTextBounds (
                const std::string &text,
                int startX,
                int startY) {
            int penX = startX;
            int min_y = startY;
            int max_y = startY;
            char prev_char = 0;
            for (char c : text) {
                Glyph::SharedPtr glyph = GetGlyph (c);
                if (glyph != nullptr) {
                    penX += GetKerning (prev_char, c).x;
                    int glyphY = startY - glyph->origin.y;
                    min_y = util::MIN (min_y, glyphY);
                    max_y = util::MAX (max_y, glyphY + glyph->extents.height);
                    penX += glyph->advance.x;
                    prev_char = c;
                }
            }
            return util::Rectangle (startX, min_y, penX - startX, max_y - min_y);
        }

    } // namespace surface
} // namespace thekogans
