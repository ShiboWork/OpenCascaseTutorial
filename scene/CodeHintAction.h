#include <string>
#include <vector>

// CodeHintManager 类用于管理代码提示的添加与可视化显示
class CodeHintManager {
public:
    // 添加一个提示项（label 标签 + code 代码内容）
    void AddHint(const std::string& label, const std::string& code) {
        hints.emplace_back(label, code);  // 添加到提示列表
        scrollToBottom = true;            // 添加后滚动到底部标志设置为 true
    }

    // 渲染提示窗口
    void RenderWindow() {
        ImGui::Begin("Code Hint Window");  // 创建窗口，标题为 "Code Hint Window"

        ImGui::Separator();  // 添加分隔线

        // 创建一个带滚动条的子区域，显示所有提示项
        ImGui::BeginChild("ScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        // 遍历所有提示项，逐个显示
        for (const auto& [label, code] : hints) {
            ImGui::TextWrapped("[%s]", label.c_str());  // 显示标签，自动换行
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.3f, 1.0f));  // 设置代码文本颜色为黄绿色
            ImGui::TextUnformatted(code.c_str());  // 显示代码文本，保持原始格式不换行
            ImGui::PopStyleColor();  // 恢复颜色设置
            ImGui::Separator();      // 每条提示之间添加分隔线
        }

        // 如果需要滚动到底部
        if (scrollToBottom) {
            ImGui::SetScrollHereY(1.0f);  // 将滚动条移动到底部
            scrollToBottom = false;       // 重置标志
        }

        ImGui::EndChild();  // 结束子区域
        ImGui::End();       // 结束主窗口
    }

private:
    std::vector<std::pair<std::string, std::string>> hints;  // 存储所有提示项（标签+代码）
    bool scrollToBottom = false;  // 是否滚动到底部的标志
};

