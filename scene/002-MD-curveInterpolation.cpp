#include "SceneSelector.h"
#include "002-MD-curveInterpolation.h"

namespace {
    struct Registrar {
        Registrar() {
            SceneSelector::RegisterScene("Basic/GP", "002-GP-CurveInterpolation", []() {
                return std::make_shared<MDCurveInterpolation002>();
                });
        }
    };
    const Registrar registrar;

}