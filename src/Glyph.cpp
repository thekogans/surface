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
                int bitmap_left_,
                int bitmap_top_,
                const FT_Vector &advance_) :
                width (bitmap.width),
                rows (bitmap.rows),
                bitmap_left (bitmap_left_),
                bitmap_top (bitmap_top_),
                advance {advance_.x >> 6, advance_.y >> 6} {
            if (width > 0 && rows > 0) {
                alpha_mask.resize (width * rows);
                for (int row = 0; row < rows; ++row) {
                    std::memcpy (
                        &alpha_mask[row * width],
                        &bitmap.buffer[row * bitmap.pitch], width);
                }
                // for (unsigned int y = 0; y < rows; ++y) {
                //     for (unsigned int x = 0; x < width; ++x) {
                //         unsigned char pixel = alpha_mask[y * width + x];
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

        // Blends an 8-bit FreeType mask onto a 32-bit RGBA software image
        void Glyph::Draw (
                int targetX,
                int targetY,
                COLOR color,
                Surface::SharedPtr surface) const {
            for (unsigned int y = 0; y < rows; ++y) {
                int pixelY = targetY + y;
                // Skip rows outside image bounds
                if (pixelY >= 0 && pixelY < surface->rectangle.extents.height) {
                    for (unsigned int x = 0; x < width; ++x) {
                        int pixelX = targetX + x;
                        // Skip columns outside image bounds
                        if (pixelX >= 0 && pixelX < surface->rectangle.extents.width) {
                            util::ui8 glyphA = alpha_mask[y * width + x];
                            // Fully transparent, skip blending
                            if (glyphA != 0) {
                                util::ui32 alpha = (glyphA * color.a) / 255;
                                util::ui32 inv_alpha = 255 - alpha;
                                PIXEL *bg = surface->GetBufferAtPoint (util::Point (pixelX, pixelY));
                                bg->r = (color.r * alpha + bg->r * inv_alpha) / 255;
                                bg->g = (color.g * alpha + bg->g * inv_alpha) / 255;
                                bg->b = (color.b * alpha + bg->b * inv_alpha) / 255;
                                bg->a = alpha + (bg->a * inv_alpha) / 255;
                            }
                        }
                    }
                }
            }
        }

        GlyphCache::GlyphCache (
                const std::string &fontPath,
                int fontSize) {
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
                                glyph->bitmap_left,
                                glyph->bitmap_top,
                                glyph->advance);
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

        void GlyphCache::DrawText (
                const std::string &text,
                int startX,
                int startY,
                COLOR color,
                Surface::SharedPtr surface) {
            int penX = startX;
            int penY = startY;
            char prev_char = 0;
            for (char c : text) {
                Glyph::SharedPtr glyph = GetGlyph (c);
                if (glyph != nullptr) {
                    penX += GetKerning (prev_char, c).x;
                    int glyphX = penX + glyph->bitmap_left;
                    int glyphY = penY - glyph->bitmap_top;
                    glyph->Draw (glyphX, glyphY, color, surface);
                    penX += glyph->advance.x;
                    prev_char = c;
                }
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
                    int glyphY = startY - glyph->bitmap_top;
                    min_y = std::min (min_y, glyphY);
                    max_y = std::max (max_y, glyphY + glyph->rows);
                    penX += glyph->advance.x;
                    prev_char = c;
                }
            }
            return util::Rectangle (startX, min_y, penX - startX, max_y - min_y);
        }

    } // namespace surface
} // namespace thekogans
