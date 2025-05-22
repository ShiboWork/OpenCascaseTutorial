#include "BoxScene.h"
#include "SceneSelector.h"

namespace {
    struct Registrar {
        Registrar() {
            SceneSelector::RegisterScene("Test", "000-Demo-Box Scene", []() {
                return std::make_shared<BoxScene>();
                });
        }   
    };
    const Registrar registrar;

}