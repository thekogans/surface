#include "thekogans/util/Rectangle.h"
#include "thekogans/surface/osx/SurfaceView.h"

@implementation SurfaceView

- (instancetype)initWithFrame: (NSRect)frame {
    if (self = [super initWithFrame: frame]) {
        [self updateBuffersForSize: frame.size];
    }
    return self;
}

- (void)dealloc {
    // Prevent memory leaks when the UI view breaks down
    surface.Reset ();
    [super dealloc];
}

// Safely resizes framebuffers dynamically when user stretches the OS window
- (void)updateBuffersForSize: (NSSize)size {
    CGFloat scaleFactor = [self backingScaleFactor];
    surface.Reset (
        new thekogans::surface::SurfaceRGBA (
            thekogans::util::Rectangle::Extents (
                static_cast<size_t> (size.width * scaleFactor),
                static_cast<size_t> (size.height * scaleFactor))));
    [self redraw];
    // Notify macOS that the screen area requires a redraw pass
    [self setNeedsDisplay: YES];
}

// Track resizing events natively
- (void)setFrameSize: (NSSize)newSize {
    [super setFrameSize: newSize];
    [self updateBuffersForSize: newSize];
}

- (void)viewDidChangeBackingProperties {
    [super viewDidChangeBackingProperties];
    [self updateBuffersForSize: self.bounds.size];
}

- (void)drawRect: (NSRect)dirtyRect {
    if (surface != nullptr) {
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB ();
        CGContextRef bitmapContext = CGBitmapContextCreate (
            surface->GetBufferAtPoint (surface->rectangle.origin),
            surface->rectangle.extents.width,
            surface->rectangle.extents.height,
            8,
            surface->buffer->extents.width * 4,
            colorSpace,
            kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
        CGImageRef image = CGBitmapContextCreateImage (bitmapContext);
        CGContextRef currentContext = (CGContextRef)[[NSGraphicsContext currentContext] CGContext];
        CGRect destRect = CGRectMake (0, 0, self.bounds.size.width, self.bounds.size.height);
        CGContextDrawImage (currentContext, destRect, image);
        CGImageRelease (image);
        CGContextRelease (bitmapContext);
        CGColorSpaceRelease (colorSpace);
    }
}

- (CGFloat)backingScaleFactor {
    return self.window ? self.window.backingScaleFactor : [[NSScreen mainScreen] backingScaleFactor];
}

- (void)redraw {
}
@end
