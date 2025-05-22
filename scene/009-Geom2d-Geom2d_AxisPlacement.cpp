#include "SceneSelector.h"
#include "009-Geom2d-Geom2d_AxisPlacement.h"

namespace {
    struct Registrar {
        Registrar() {
            SceneSelector::RegisterScene("Basic/Geom2d", "009-Geom2d-Geom2d_AxisPlacement", []() {
                return std::make_shared<Geom2d009>();
                });
        }
    };
    const Registrar registrar;

}