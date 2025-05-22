#include "SceneSelector.h"
#include "005-MD-gp_Ax.h"

namespace {
    struct Registrar {
        Registrar() {
            SceneSelector::RegisterScene("Basic/GP", "005-GP-gp_Ax", []() {
                return std::make_shared<MDAx005>();
                });
        }
    };
    const Registrar registrar;

}