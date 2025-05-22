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

        ImGui::Begin("Tutorial Window", &showTutorialWindow);  // 打开一个新的ImGui窗口

        // 初始化
        if (!bInitialized) {
            // 初始化窗口
            initTutorialWin();
            // 自定义初始化
            customInitTutorialWindow(context);
            bInitialized = true;
        }
        // 渲染
        renderTutorialContent(context);
        ImGui::End();
    }

    void openTutorialWindow() { showTutorialWindow = true;  }

    void closeTutorialWindow() { showTutorialWindow = false; }

protected:
    virtual void renderTutorialContent(const Handle(AIS_InteractiveContext)& context) = 0;  // 纯虚函数，要求子类实现窗口的内容

    void initTutorialWin() const {
        static bool initialized = false;  // 保证只初始化一次
        if (!initialized) {
            ImVec2 windowPos(1200, 500);   // 设置窗口左上角位置
            ImVec2 windowSize(600, 400);  // 设置窗口大小

            ImGui::SetNextWindowPos(windowPos);
            ImGui::SetNextWindowSize(windowSize);

            initialized = true;
        }
    }

private:
    bool showTutorialWindow = false;  // 控制窗口显示的标志
    bool bInitialized = false;  // 初始化标志位
};