#include "SceneSelector.h"
#include "003-MD-plane.h"

namespace {
    struct Registrar {
        Registrar() {
            SceneSelector::RegisterScene("Basic/GP", "003-GP-plane", []() {
                return std::make_shared<MDPlane003>();
                });
        }
    };
    const Registrar registrar;

}