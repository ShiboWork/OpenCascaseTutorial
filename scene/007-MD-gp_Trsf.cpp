#include "SceneSelector.h"
#include "007-MD-gp_Trsf.h"

namespace {
    struct Registrar {
        Registrar() {
            SceneSelector::RegisterScene("Basic/GP", "007-GP-gp_Trsf", []() {
                return std::make_shared<MDTrsf007>();
                });
        }
    };
    const Registrar registrar;

}