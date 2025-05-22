#pragma once

#include "Imgui/imgui.h"

namespace OCCImguiTutorial {
    struct Log
    {
        ImGuiTextBuffer Buf;
        ImGuiTextFilter Filter;
        ImVector<int> LineOffsets;
        bool AutoScroll;

        Log();
        void Clear();
        void AddLog(const char* fmt, ...);
        void Draw(const char* title, bool* p_open = nullptr);
    };
}

