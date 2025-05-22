#include "SceneSelector.h"
#include "001-MD-pointAndVector.h"

namespace {
    struct Registrar {
        Registrar() {
            SceneSelector::RegisterScene("Basic/GP","001-GP-PointAndVector", []() {
                return std::make_shared<MDPointAndVector001>();
                });
        }
    };
    const Registrar registrar;

}