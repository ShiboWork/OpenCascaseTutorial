#pragma once

#include <memory>
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <any>

#include "BaseScene.h"

class SceneSelector {
public:
    struct SceneEntry {
        std::string name;
        std::function<std::shared_ptr<BaseScene>()> createFunc;
    };

    SceneSelector();
    void renderGui();
    std::shared_ptr<BaseScene> getCurrentScene() const;

    static void RegisterScene(const std::string& path, const std::string& name,
        const std::function<std::shared_ptr<BaseScene>()>& createFunc);

private:
    std::string selectedPath;
    std::string selectedName;
    std::shared_ptr<BaseScene> currentScene;

    struct SceneNode {
        std::map<std::string, SceneNode> children;
        std::vector<SceneEntry> scenes;
    };

    static SceneNode& GetRoot();
    void renderTreeNode(SceneNode& node, const std::string& path = "");
};
