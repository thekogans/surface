#include "surface/unix/XSurface.h"

namespace surface {

    namespace {
        util::ui32 GetBPP (Display *display) {
            return DefaultDepth (display, DefaultScreen (display));
        }
    }

    XSurface::XSurface (Display *display_, Window window_) : Surface (GetBPP (display_)), display (display_), window (window_), gc (0), image (0) {
        // Create system font.
        // FIXME: implement

        // Create off screen dc.
        gc = XCreateGC (display, window, 0, NULL);
        memset (&segmentInfo, 0, sizeof (XShmSegmentInfo));
        segmentInfo.shmid = -1;

        SetColorMap (colorMap);
    }

    XSurface::~XSurface () {
        // FIXME: error handling
        if (segmentInfo.shmaddr)
            XShmDetach (display, &segmentInfo);
        if (image)
            XDestroyImage (image);
        if (segmentInfo.shmaddr)
            shmdt (segmentInfo.shmaddr);
        if (segmentInfo.shmid >= 0)
            shmctl (segmentInfo.shmid, IPC_RMID, NULL);
        XFreeGC (display, gc);
    }

    void XSurface::Resize (util::ui32 width_, util::ui32 height_) {
        assert (width_ > 0);
        assert (height_ > 0);
        if (width != width_ || height != height_) {
            if (segmentInfo.shmid != -1) {
                XShmDetach (display, &segmentInfo);
                XDestroyImage (image);
                shmdt (segmentInfo.shmaddr);
                shmctl (segmentInfo.shmid, IPC_RMID, NULL);
                memset (&segmentInfo, 0, sizeof (XShmSegmentInfo));
                segmentInfo.shmid = -1;
            }

            image = XShmCreateImage (display, DefaultVisualOfScreen (DefaultScreenOfDisplay (display)),
                bpp, ZPixmap, NULL, &segmentInfo, width_, height_);
            if (!image)
                throw Error ("");

            segmentInfo.shmid = shmget (IPC_PRIVATE, image->bytes_per_line * image->height, IPC_CREAT | 0777);

            if (segmentInfo.shmid < 0)
                throw Error ("");

            surface = (util::ui8 *)(image->data = segmentInfo.shmaddr = (char *)shmat (segmentInfo.shmid, NULL, 0));
            if (!surface)
                throw Error ("");

            segmentInfo.readOnly = False;

            if (!XShmAttach (display, &segmentInfo))
                throw Error ("");

            stride = image->bytes_per_line;
            width = width_;
            height = height_;
        }
    }

    void XSurface::Flip (const blas::Rect *flipRect) {
        // FIXME: calculate intersection of clip, flip and dib rects.
        // FIXME: error handling
        XShmPutImage (display, window, gc, image, 0, 0, 0, 0, width, height, False);
        XFlush (display);
    }

    // Set a new color map to be used during color translation.
    // ***NOTE***: While the color map can theoretically be any size,
    // in color index mode only the first 256 colors will be used to
    // create and populate the hardware palette.
    void XSurface::SetColorMap (const std::vector<COLOR> &colorMap_) {
        colorMap = colorMap_;
        // FIXME: We are assuming that hardware palettes only exist in
        // indexed color mode.
        if (bpp == BPP_8) {
            // FIXME: create palette
        }
    }

} // namespace surface
