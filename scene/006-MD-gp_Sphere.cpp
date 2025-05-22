#include "SceneSelector.h"
#include "006-MD-gp_Sphere.h"

namespace {
    struct Registrar {
        Registrar() {
            SceneSelector::RegisterScene("Basic/GP", "006-GP-gp_Sphere", []() {
                return std::make_shared<MDSphere006>();
                });
        }
    };
    const Registrar registrar;

}