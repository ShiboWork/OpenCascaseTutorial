#include "SceneSelector.h"
#include "004-MD-gp_Circ.h"

namespace {
    struct Registrar {
        Registrar() {
            SceneSelector::RegisterScene("Basic/GP", "004-GP-gp_Circ", []() {
                return std::make_shared<MDCircle004>();
                });
        }
    };
    const Registrar registrar;

}