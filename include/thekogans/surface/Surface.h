#if !defined (__thekogans_surface_Surface_h)
#define __thekogans_surface_Surface_h

#include "thekogans/util/Types.h"
#include "thekogans/util/Rectangle.h"
#include "thekogans/util/Exception.h"
#include "thekogans/surface/Config.h"
#include "thekogans/surface/COLOR.h"
#include "thekogans/surface/PIXEL.h"

namespace thekogans {
    namespace surface {

        // Implements an off screen (bitmap) drawing surface. The two overriding
        // design goals are portability and speed. As such, it lacks all the bells
        // and whistles usually found in more complete implementations. Specifically,
        // there is no rendering pipeline. There is no user space coordinate system,
        // or transformations of any kind. There is also no bounds checking of parameters.
        // The last point is very important. For it says that even though primitive
        // drawing operations (line, circle, ellipse) are provided, its up to the caller
        // to make sure they don't protrude outside the bitmap boundary. It is understood
        // that a rendering pipeline will be layered on top of this class, and that
        // drawing to a bitmap is but the last step of that pipeline.

        // The surface has a very simple coordinate system:
        // units: pixels
        // x = 0, y = 0: upper left corner
        // x = width - 1, y = height - 1: lower right corner

        template<typename T>
        struct Surface : public util::RefCounted {
            using PixelType = T;

            THEKOGANS_UTIL_DECLARE_REF_COUNTED_POINTERS (Surface)
            THEKOGANS_UTIL_DECLARE_STD_ALLOCATOR_FUNCTIONS

            /// \brief
            /// Surface width. Might not be the same as buffer true width.
            /// That's what stride is for. It holds buffer's true width for
            /// scanline navigation. This value is used for coordinate
            /// bounds checking. In pixels.
            util::Rectangle rectangle;
            /// \brief
            /// PIXELBuffer manages the lifetime of... the PIXEL buffer.
            /// Wrapping PIXEL * with a RefCounted object alows us to
            /// share the buffer accross multiple Surface instances and
            /// have them all manage heir own reference. When the last
            /// Surface goes out of scope, the buffer will be destroyed.
            /// This design alows for very fast sub-image Surface creation.
            struct PIXELBuffer : public util::RefCounted {
                THEKOGANS_UTIL_DECLARE_REF_COUNTED_POINTERS (PIXELBuffer)

                util::Rectangle::Extents extents;
                PixelType *buffer;

                PIXELBuffer (const util::Rectangle::Extents &extents_) :
                        extents (extents_),
                        buffer (new PixelType[extents.width * extents.height]) {
                    if (extents.IsDegenerate ()) {
                        THEKOGANS_UTIL_THROW_ERROR_CODE_EXCEPTION (
                            THEKOGANS_UTIL_OS_ERROR_CODE_EINVAL);
                    }
                }
                ~PIXELBuffer () {
                    delete [] buffer;
                }

                inline PixelType *get () {
                    return buffer;
                }

                /// \brief
                /// const (rvalue) element accessor.
                /// \param[in] index Element index to return.
                /// \return reference to element at index.
                inline const PixelType &operator [] (std::size_t index) const {
                    return buffer[index];
                }
                /// \brief
                /// lvalue element accessor.
                /// \param[in] index Element index to return.
                /// \return Reference to element at index.
                inline PixelType &operator [] (std::size_t index) {
                    return buffer[index];
                }

                /// \brief
                /// Implicit conversion to const T *.
                /// \return const T *.
                inline operator const PixelType * () const {
                    return buffer;
                }
                /// \brief
                /// Implicit conversion to T *.
                /// \return T *.
                inline operator PixelType * () {
                    return buffer;
                }

                THEKOGANS_UTIL_DISALLOW_COPY_AND_ASSIGN (PIXELBuffer)
            };
            /// \brief
            /// The surface frame buffer. The only muttable member.
            typename PIXELBuffer::SharedPtr buffer;

            Surface (const util::Rectangle::Extents &extents) :
                    rectangle (util::Point (), extents),
                    buffer (new PIXELBuffer (extents)) {
                if (extents.IsDegenerate ()) {
                    THEKOGANS_UTIL_THROW_ERROR_CODE_EXCEPTION (
                        THEKOGANS_UTIL_OS_ERROR_CODE_EINVAL);
                }
            }
            Surface (
                    const util::Rectangle &rectangle_,
                    typename PIXELBuffer::SharedPtr buffer_) :
                    rectangle (rectangle_),
                    buffer (buffer_) {
                if (buffer == nullptr ||
                        !rectangle.IsInside (util::Rectangle (util::Point (), buffer->extents))) {
                    THEKOGANS_UTIL_THROW_ERROR_CODE_EXCEPTION (
                        THEKOGANS_UTIL_OS_ERROR_CODE_EINVAL);
                }
            }

            inline PixelType *GetBufferAtPoint (const util::Point &point) {
                return buffer->get () + (rectangle.origin.y + point.y) * buffer->extents.width +
                    rectangle.origin.x + point.x;
            }

            inline PixelType *GetBufferAtPointSafe (const util::Point &point) {
                return point.InRectangle (rectangle) ? GetBufferAtPoint (point) : nullptr;
            }

            THEKOGANS_UTIL_DISALLOW_COPY_AND_ASSIGN (Surface)
        };

        // kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big
        using SurfaceRGBA = Surface<PIXEL_RGBA>;
        // kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Big
        using SurfaceARGB = Surface<PIXEL_ARGB>;
        // kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Little
        using SurfaceBGRA = Surface<PIXEL_BGRA>;
        // kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little
        using SurfaceABGR = Surface<PIXEL_ABGR>;

    } // namespace surface
} // namespace thekogans

#endif // __thekogans_surface_Surface_h
