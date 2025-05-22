#include "SceneSelector.h"
#include "BRepMesh-BRepMesh_IncrementalMesh.h"

namespace {
    struct Registrar {
        Registrar() {
            SceneSelector::RegisterScene("Basic/BRepMesh", "BRepMesh-BRepMesh_IncrementalMesh", []() {
                return std::make_shared<BRepMesh001>();
                });
        }
    };
    const Registrar registrar;

}