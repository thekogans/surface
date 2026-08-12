#if !defined (__thekogans_surface_COLOR_h)
#define __thekogans_surface_COLOR_h

#include <vector>
#include "thekogans/util/Types.h"
#include "thekogans/surface/Config.h"

namespace thekogans {
    namespace surface {

        struct COLOR {
            util::ui8 r;
            util::ui8 g;
            util::ui8 b;
            util::ui8 a;

            COLOR (
                util::ui8 r_,
                util::ui8 g_,
                util::ui8 b_,
                util::ui8 a_) :
                r (r_),
                g (g_),
                b (b_),
                a (a_) {}
        };

    } // namespace surface
} // namespace thekogans

#endif // __thekogans_surface_COLOR_h
