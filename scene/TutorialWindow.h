#pragma once

#include <string>
#include "../imgui/imgui.h"

class TutorialWindow {
public:
    virtual ~TutorialWindow() = default;

    virtual void customInitTutorialWindow(const Handle(AIS_InteractiveContext)& context) = 0 ;

    void renderTutorialWindow(const Handle(AIS_InteractiveContext)& context) {
        if (context.IsNull())
            return;

        ImGui::Begin("Tutorial Window", &showTutorialWindow);  // ��һ���µ�ImGui����

        // ��ʼ��
        if (!bInitialized) {
            // ��ʼ������
            initTutorialWin();
            // �Զ����ʼ��
            customInitTutorialWindow(context);
            bInitialized = true;
        }
        // ��Ⱦ
        renderTutorialContent(context);
        ImGui::End();
    }

    void openTutorialWindow() { showTutorialWindow = true;  }

    void closeTutorialWindow() { showTutorialWindow = false; }

protected:
    virtual void renderTutorialContent(const Handle(AIS_InteractiveContext)& context) = 0;  // ���麯����Ҫ������ʵ�ִ��ڵ�����

    void initTutorialWin() const {
        static bool initialized = false;  // ��ֻ֤��ʼ��һ��
        if (!initialized) {
            ImVec2 windowPos(1200, 500);   // ���ô������Ͻ�λ��
            ImVec2 windowSize(600, 400);  // ���ô��ڴ�С

            ImGui::SetNextWindowPos(windowPos);
            ImGui::SetNextWindowSize(windowSize);

            initialized = true;
        }
    }

private:
    bool showTutorialWindow = false;  // ���ƴ�����ʾ�ı�־
    bool bInitialized = false;  // ��ʼ����־λ
};