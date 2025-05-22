#include "SceneSelector.h"
#include "Poly-Poly_Triangle.h"

namespace {
    struct Registrar {
        Registrar() {
            SceneSelector::RegisterScene("Basic/Poly", "Poly_Triangle", []() {
                return std::make_shared<PolyTriangle>();
                });
        }
    };
    const Registrar registrar;

}