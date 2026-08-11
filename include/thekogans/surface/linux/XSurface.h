#if !defined (__surface_unix_XSurface_h)
#define __surface_unix_XSurface_h

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <vector>
#include "util/Types.h"
#include "blas/Rect.h"
#include "surface/Config.h"
#include "surface/Color.h"
#include "surface/Surface.h"

namespace surface {

    // Provides unix x windows specific glue to have Surface act as a
    // window back buffer.

    class _LIBSURFACE_DECL XSurface : public Surface {
    protected:
        Display *display;
        Window window;
        GC gc;
        XImage *image;
        XShmSegmentInfo segmentInfo;

    public:
        XSurface (Display *display_, Window window_);
        virtual ~XSurface ();

        virtual void Resize (util::ui32 width_, util::ui32 height_);
        virtual void Flip (const blas::Rect *flipRect = 0);
        virtual void SetColorMap (const std::vector<COLOR> &colorMap_);
    };

} // namespace surface

#endif // __surface_unix_XSurface_h
