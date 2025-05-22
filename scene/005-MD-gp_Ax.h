#pragma once

#include "pch.h"
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <AIS_Shape.hxx>
#include "BaseScene.h"
#include "VisSceneComponents.h"
#include "TutorialWindow.h"

class MDAx005 : public BaseScene, public VisSceneComponents, public TutorialWindow {
public:
    MDAx005() { openTutorialWindow(); }

    void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        if (context.IsNull()) return;
        if (!bIsSceneInit) {
            sceneInit(view, context);
            bIsSceneInit = true;
        }
        renderTutorialWindow(context);
    }

    void sceneInit(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        // 初始不显示任何轴，等用户点击按钮后再渲染
    }

    void customInitTutorialWindow(const Handle(AIS_InteractiveContext)& context) override {}

    void renderTutorialContent(const Handle(AIS_InteractiveContext)& context) override {
        ImGui::SliderFloat3("Origin", originInput.data(), -100.f, 100.f);
        ImGui::SliderFloat3("Z Axis (Main Dir)", zDirInput.data(), -1.f, 1.f);
        ImGui::SliderFloat3("X Axis (Ref Dir)", xDirInput.data(), -1.f, 1.f);

        if (ImGui::CollapsingHeader("gp_Ax1: Line (origin + direction)", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Button("Visualize Ax1")) {
                VisualizeAx1(context);
            }
        }

        if (ImGui::CollapsingHeader("gp_Ax2: Local 2D coordinate system", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Button("Visualize Ax2")) {
                VisualizeAx2(context);
            }
        }

        if (ImGui::CollapsingHeader("gp_Ax3: Full 3D orthonormal basis", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Button("Visualize Ax3")) {
                VisualizeAx3(context);
            }
        }
    }

private:
    std::array<float, 3> originInput{ 0.f, 0.f, 0.f };
    std::array<float, 3> zDirInput{ 0.f, 0.f, 1.f };
    std::array<float, 3> xDirInput{ 1.f, 0.f, 0.f };
    std::vector<Handle(AIS_Shape)> auxShapes;

    void ClearAuxShapes(const Handle(AIS_InteractiveContext)& context) {
        for (auto& shape : auxShapes)
            context->Remove(shape, Standard_False);
        auxShapes.clear();
    }

    Handle(AIS_Shape) DisplayAuxLine(
        const Handle(AIS_InteractiveContext)& context,
        const gp_Pnt& start,
        const gp_Pnt& end,
        Quantity_NameOfColor color,
        Standard_Real width = 2.0) {

        TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(start, end);
        Handle(AIS_Shape) shape = new AIS_Shape(edge);
        shape->SetColor(color);
        shape->SetWidth(width);
        context->Display(shape, Standard_False);
        auxShapes.push_back(shape);
        return shape;
    }

    void VisualizeAx1(const Handle(AIS_InteractiveContext)& context) {
        ClearAuxShapes(context);
        try {
            gp_Pnt origin(originInput[0], originInput[1], originInput[2]);
            gp_Dir dirZ(zDirInput[0], zDirInput[1], zDirInput[2]);
            gp_Ax1 ax1(origin, dirZ);
            DisplayAuxLine(context, origin, origin.Translated(dirZ.XYZ() * 20), Quantity_NOC_RED);
            context->UpdateCurrentViewer();
        }
        catch (Standard_Failure const& e) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Ax1 Error: %s", e.GetMessageString());
        }
    }

    void VisualizeAx2(const Handle(AIS_InteractiveContext)& context) {
        ClearAuxShapes(context);
        try {
            gp_Pnt origin(originInput[0], originInput[1], originInput[2]);
            gp_Dir dirZ(zDirInput[0], zDirInput[1], zDirInput[2]);
            gp_Dir dirX(xDirInput[0], xDirInput[1], xDirInput[2]);
            gp_Ax2 ax2(origin, dirZ, dirX);
            DisplayAuxLine(context, origin, origin.Translated(dirZ.XYZ() * 20), Quantity_NOC_RED);   // Z axis
            DisplayAuxLine(context, origin, origin.Translated(dirX.XYZ() * 20), Quantity_NOC_GREEN); // X axis
            context->UpdateCurrentViewer();
        }
        catch (Standard_Failure const& e) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Ax2 Error: %s", e.GetMessageString());
        }
    }

    void VisualizeAx3(const Handle(AIS_InteractiveContext)& context) {
        ClearAuxShapes(context);
        try {
            gp_Pnt origin(originInput[0], originInput[1], originInput[2]);
            gp_Dir dirZ(zDirInput[0], zDirInput[1], zDirInput[2]);
            gp_Dir dirX(xDirInput[0], xDirInput[1], xDirInput[2]);
            gp_Dir dirY = dirZ.Crossed(dirX); // 正交 Y 方向
            gp_Ax3 ax3(origin, dirZ, dirX);
            DisplayAuxLine(context, origin, origin.Translated(dirZ.XYZ() * 20), Quantity_NOC_RED);   // Z
            DisplayAuxLine(context, origin, origin.Translated(dirX.XYZ() * 20), Quantity_NOC_GREEN); // X
            DisplayAuxLine(context, origin, origin.Translated(dirY.XYZ() * 20), Quantity_NOC_BLUE);  // Y
            context->UpdateCurrentViewer();
        }
        catch (Standard_Failure const& e) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Ax3 Error: %s", e.GetMessageString());
        }
    }
};
