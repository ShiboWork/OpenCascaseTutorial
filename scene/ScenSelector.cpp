#include "SceneSelector.h"
#include "../imgui/imgui.h"

SceneSelector::SceneSelector() : selectedPath(""), selectedName(""), currentScene(nullptr) {}

SceneSelector::SceneNode& SceneSelector::GetRoot() {
    static SceneNode root;
    return root;
}

void SceneSelector::RegisterScene(const std::string& path, const std::string& name,
    const std::function<std::shared_ptr<BaseScene>()>& createFunc) {
    SceneNode* node = &GetRoot();
    size_t start = 0, end;
    while ((end = path.find('/', start)) != std::string::npos) {
        std::string part = path.substr(start, end - start);
        node = &node->children[part];
        start = end + 1;
    }
    std::string lastPart = path.substr(start);
    node = &node->children[lastPart];
    node->scenes.emplace_back(SceneEntry{ name, createFunc });
}

void SceneSelector::renderGui() {
    ImGui::Begin("Scene Selector");

    renderTreeNode(GetRoot());

    ImGui::End();
}

void SceneSelector::renderTreeNode(SceneNode& node, const std::string& path) {
    // 显示子文件夹
    for (auto& [key, child] : node.children) {
        if (ImGui::TreeNode(key.c_str())) {
            std::string newPath = path.empty() ? key : path + "/" + key;
            renderTreeNode(child, newPath);
            ImGui::TreePop();
        }
    }

    // 显示本节点下的场景项
    for (size_t i = 0; i < node.scenes.size(); ++i) {
        const auto& entry = node.scenes[i];
        bool isSelected = (selectedPath == path && selectedName == entry.name);
        if (ImGui::Selectable(entry.name.c_str(), isSelected)) {
            selectedPath = path;
            selectedName = entry.name;
            currentScene = entry.createFunc();
        }
    }
}

std::shared_ptr<BaseScene> SceneSelector::getCurrentScene() const {
    return currentScene;
}
