#include <string>
#include <vector>

// CodeHintManager �����ڹ��������ʾ���������ӻ���ʾ
class CodeHintManager {
public:
    // ���һ����ʾ�label ��ǩ + code �������ݣ�
    void AddHint(const std::string& label, const std::string& code) {
        hints.emplace_back(label, code);  // ��ӵ���ʾ�б�
        scrollToBottom = true;            // ��Ӻ�������ײ���־����Ϊ true
    }

    // ��Ⱦ��ʾ����
    void RenderWindow() {
        ImGui::Begin("Code Hint Window");  // �������ڣ�����Ϊ "Code Hint Window"

        ImGui::Separator();  // ��ӷָ���

        // ����һ��������������������ʾ������ʾ��
        ImGui::BeginChild("ScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        // ����������ʾ������ʾ
        for (const auto& [label, code] : hints) {
            ImGui::TextWrapped("[%s]", label.c_str());  // ��ʾ��ǩ���Զ�����
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.3f, 1.0f));  // ���ô����ı���ɫΪ����ɫ
            ImGui::TextUnformatted(code.c_str());  // ��ʾ�����ı�������ԭʼ��ʽ������
            ImGui::PopStyleColor();  // �ָ���ɫ����
            ImGui::Separator();      // ÿ����ʾ֮����ӷָ���
        }

        // �����Ҫ�������ײ�
        if (scrollToBottom) {
            ImGui::SetScrollHereY(1.0f);  // ���������ƶ����ײ�
            scrollToBottom = false;       // ���ñ�־
        }

        ImGui::EndChild();  // ����������
        ImGui::End();       // ����������
    }

private:
    std::vector<std::pair<std::string, std::string>> hints;  // �洢������ʾ���ǩ+���룩
    bool scrollToBottom = false;  // �Ƿ�������ײ��ı�־
};

