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

#include "thekogans/util/Environment.h"

#if defined (TOOLCHAIN_ARCH_i386) || defined (TOOLCHAIN_ARCH_x86_64)
    #include <immintrin.h>
#endif // defined (TOOLCHAIN_ARCH_i386) || defined (TOOLCHAIN_ARCH_x86_64)
#include "thekogans/util/Heap.h"
#include "thekogans/util/Constants.h"
#include "thekogans/util/ByteSwap.h"
#include "thekogans/surface/Glyph.h"

namespace thekogans {
    namespace surface {

#if 0
        void blend_glyph_avx2_complete (
                COLOR *buffer,
                const util::ui8 *glyphAlpha,
                COLOR color,
                int width) {
            // Broadcast foreground color channels to 16-bit vector lanes
            __m256i v_r_fg  = _mm256_set1_epi16 (color.r);
            __m256i v_g_fg  = _mm256_set1_epi16 (color.g);
            __m256i v_b_fg  = _mm256_set1_epi16 (color.b);

            // Constant vectors for division optimization: (val + 128) / 255 => ((val * 257) + 128) >> 16
            __m256i v_128   = _mm256_set1_epi16 (128);
            __m256i v_257   = _mm256_set1_epi16 (257);

            // Byte-shuffling masks to isolate individual R, G, B channels from packed RGBA into 16-bit lanes
            // Works across two separate 128-bit lanes inside the 256-bit AVX register
            __m256i mask_r = _mm256_setr_epi8 (
                0, -1, 4, -1, 8, -1, 12, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                0, -1, 4, -1, 8, -1, 12, -1, -1, -1, -1, -1, -1, -1, -1, -1
            );
            __m256i mask_g = _mm256_setr_epi8 (
                1, -1, 5, -1, 9, -1, 13, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                1, -1, 5, -1, 9, -1, 13, -1, -1, -1, -1, -1, -1, -1, -1, -1
            );
            __m256i mask_b = _mm256_setr_epi8 (
                2, -1, 6, -1, 10, -1, 14, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                2, -1, 6, -1, 10, -1, 14, -1, -1, -1, -1, -1, -1, -1, -1, -1
            );

            int i = 0;
            // Process 8 pixels (32 bytes of destination, 8 bytes of alpha glyph) per iteration
            for (; i <= width - 8; i += 8) {
                // 1. Load FreeType coverage alpha (8 bytes total) and expand to 8 lanes of 16-bit integers
                __m128i alpha_8b   = _mm_loadl_epi64 ((const __m128i*)(glyphAlpha + i));
                __m256i alpha_16   = _mm256_cvtepu8_epi16 (alpha_8b);

                // 2. Load 8 destination RGBA pixels (256 bits)
                __m256i dst_256    = _mm256_loadu_si256 ((const __m256i*)(buffer + i));

                // 3. Isolate destination channels into independent 16-bit lanes using the masks
                __m256i dst_r      = _mm256_shuffle_epi8 (dst_256, mask_r);
                __m256i dst_g      = _mm256_shuffle_epi8 (dst_256, mask_g);
                __m256i dst_b      = _mm256_shuffle_epi8 (dst_256, mask_b);

                // 4. Calculate: (Color_FG - Color_DST) * Alpha_Glyph
                __m256i diff_r     = _mm256_sub_epi16 (v_r_fg, dst_r);
                __m256i diff_g     = _mm256_sub_epi16 (v_g_fg, dst_g);
                __m256i diff_b     = _mm256_sub_epi16 (v_b_fg, dst_b);

                __m256i prod_r     = _mm256_mullo_epi16 (diff_r, alpha_16);
                __m256i prod_g     = _mm256_mullo_epi16 (diff_g, alpha_16);
                __m256i prod_b     = _mm256_mullo_epi16 (diff_b, alpha_16);

                // 5. Fast division by 255: ((Product + 128) * 257) >> 16
                __m256i blend_r    = _mm256_mulhi_epu16 (_mm256_add_epi16 (prod_r, v_128), v_257);
                __m256i blend_g    = _mm256_mulhi_epu16 (_mm256_add_epi16 (prod_g, v_128), v_257);
                __m256i blend_b    = _mm256_mulhi_epu16 (_mm256_add_epi16 (prod_b, v_128), v_257);

                // 6. Add results back to original destination components
                __m256i final_r    = _mm256_add_epi16 (dst_r, blend_r);
                __m256i final_g    = _mm256_add_epi16 (dst_g, blend_g);
                __m256i final_b    = _mm256_add_epi16 (dst_b, blend_b);

                // 7. Re-interleave and pack the components back into standard RGBA structure
                // Shift bits to their correct byte locations inside 16-bit blocks
                __m256i packed_r   = final_r;                         // R is in lower byte
                __m256i packed_g   = _mm256_slli_epi16 (final_g, 8);   // Move G to higher byte
                __m256i packed_b   = final_b;                         // B is in lower byte
                __m256i packed_a   = _mm256_set1_epi16 (0xFF00);       // Maintain solid alpha channel (255 << 8)

                // Or channels into adjacent pairs (RG and BA)
                __m256i rg_pairs   = _mm256_or_si256 (packed_r, packed_g);
                __m256i ba_pairs   = _mm256_or_si256 (packed_b, packed_a);

                // Interleave the 16-bit pairs into final 32-bit elements
                __m256i out_pixels = _mm256_unpacklo_epi16 (rg_pairs, ba_pairs);

                // 8. Store results back to memory
                _mm256_storeu_si256((__m256i*)(buffer + i), out_pixels);
            }
            // Scalar cleanup loop for remaining pixels
            for (; i < width; ++i) {
                uint32_t alpha = glyphAlpha[i];
                if (alpha != 0) {
                    COLOR &pixel = buffer[i];
                    pixel.r += ((color.r - pixel.r) * alpha + 128) / 255;
                    pixel.g += ((color.g - pixel.g) * alpha + 128) / 255;
                    pixel.b += ((color.b - pixel.b) * alpha + 128) / 255;
                }
            }
        }
#endif

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
                                util::Point (glyph->advance.x >> 6, glyph->advance.y >> 6));
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
                    min_y = MIN (min_y, glyphY);
                    max_y = MAX (max_y, glyphY + glyph->extents.height);
                    penX += glyph->advance.x;
                    prev_char = c;
                }
            }
            return util::Rectangle (startX, min_y, penX - startX, max_y - min_y);
        }

    } // namespace surface
} // namespace thekogans
