#include "SceneSelector.h"
#include "008-Geom2d-Geom2d_Geometry.h"

namespace {
    struct Registrar {
        Registrar() {
            SceneSelector::RegisterScene("Basic/Geom2d", "008-Geom2d-Geom2d_Geometry", []() {
                return std::make_shared<Geom2d008>();
                });
        }
    };
    const Registrar registrar;

}