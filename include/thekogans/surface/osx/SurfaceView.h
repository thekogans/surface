#if !defined (__thekogans_surface_osx_SurfaceView_h)
#define __thekogans_surface_osx_SurfaceView_h

#import <Cocoa/Cocoa.h>
#import <CoreGraphics/CoreGraphics.h>
#include "thekogans/surface/Surface.h"

@interface SurfaceView : NSView {
    thekogans::surface::SurfaceRGBA::SharedPtr surface;
}

- (CGFloat)backingScaleFactor;
- (void)redraw;
@end

#endif // __thekogans_surface_osx_SurfaceView_h
