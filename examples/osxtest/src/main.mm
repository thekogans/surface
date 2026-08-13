#import <Cocoa/Cocoa.h>
#include "thekogans/surface/Glyph.h"
#include "thekogans/surface/osx/SurfaceView.h"

using namespace thekogans;

// Custom macOS view wrapper handling framebuffers and Core Graphics pixel pushing
@interface SoftwareRenderView : SurfaceView {
    surface::GlyphCache::SharedPtr glyphCache;
}
@end

@implementation SoftwareRenderView

- (instancetype)initWithFrame: (NSRect)frame {
    if (self = [super initWithFrame: frame]) {
        glyphCache.Reset (new surface::GlyphCache (
            "/Users/boris/development/thekogans/mex/tests/FantasticPete3.03.ttf", 64 * [self backingScaleFactor]));
    }
    return self;
}

- (void)redraw {
    if (glyphCache != nullptr) {
        glyphCache->DrawText<surface::SurfaceRGBA::PixelType> (
            "E   F   G",
            50, 300,
            surface::COLOR (0xff, 0xff, 0xff, 0xff),
            surface);
    }
}
@end

// Boilerplate initialization code to construct window interfaces without Storyboards
@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (strong) NSWindow *window;
@end

@implementation AppDelegate
- (void)applicationDidFinishLaunching: (NSNotification *)aNotification {
    NSRect frame = NSMakeRect (0, 0, 800, 600);
    self.window = [[NSWindow alloc] initWithContentRect: frame
                                              styleMask: (NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable)
                                                backing: NSBackingStoreBuffered
                                                  defer: NO];
    [self.window setTitle: @"AVX2 Custom Software Renderer"];
    SoftwareRenderView *view = [[SoftwareRenderView alloc] initWithFrame:frame];
    [self.window setContentView: view];
    [self.window makeKeyAndOrderFront: nil];
}

// Ensure the application quits completely when the window is closed
- (BOOL)applicationShouldTerminateAfterLastWindowClosed: (NSApplication *)sender {
    return YES;
}
@end

int main (
        int argc,
        const char *argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        [app setDelegate: [[AppDelegate alloc] init]];
        [app run];
    }
    return 0;
}
