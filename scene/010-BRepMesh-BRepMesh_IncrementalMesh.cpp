#include "SceneSelector.h"
#include "010-BRepMesh-BRepMesh_IncrementalMesh.h"

namespace {
    struct Registrar {
        Registrar() {
            SceneSelector::RegisterScene("Basic/BRepMesh", "010-BRepMesh-BRepMesh_IncrementalMesh", []() {
                return std::make_shared<BRepMesh010>();
                });
        }
    };
    const Registrar registrar;

}