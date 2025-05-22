#pragma once

#include "pch.h"
#include <gp_Sphere.hxx>
#include <Geom_SphericalSurface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <AIS_Shape.hxx>
#include "BaseScene.h"
#include "VisSceneComponents.h"
#include "TutorialWindow.h"

class MDSphere006 : public BaseScene, public VisSceneComponents, public TutorialWindow {
public:
    MDSphere006() { openTutorialWindow(); }

    void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        if (context.IsNull()) return;
        if (!bIsSceneInit) {
            sceneInit(view, context);
            bIsSceneInit = true;
        }
        renderTutorialWindow(context);
    }

    void sceneInit(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        // 初始不加载任何球体，等待用户点击按钮生成
    }

    void customInitTutorialWindow(const Handle(AIS_InteractiveContext)& context) override {}

    void renderTutorialContent(const Handle(AIS_InteractiveContext)& context) override {
        ImGui::SliderFloat3("Sphere Center", centerInput.data(), -100.f, 100.f);
        ImGui::SliderFloat("Radius", &radius, 1.f, 100.f);

        if (ImGui::CollapsingHeader("gp_Sphere Construction", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Button("Create Sphere from gp_Pnt + radius")) {
                CreateSphereFromPoint(context);
            }
            if (ImGui::Button("Create Sphere from gp_Ax3 + radius")) {
                CreateSphereFromAx3(context);
            }
        }

        if (ImGui::Button("Clear Scene")) {
            ClearShapes(context);
        }
    }

private:
    std::array<float, 3> centerInput{ 0.f, 0.f, 0.f };
    float radius = 20.f;
    std::vector<Handle(AIS_Shape)> shapes;

    void ClearShapes(const Handle(AIS_InteractiveContext)& context) {
        for (auto& s : shapes)
            context->Remove(s, Standard_False);
        shapes.clear();
        context->UpdateCurrentViewer();
    }

    void DisplaySphere(const Handle(AIS_InteractiveContext)& context, const gp_Sphere& sphere)
    {
        try {
            // 1. 提取信息
            gp_Pnt center = sphere.Location();
            gp_Ax2 axis(center, sphere.Position().Direction());
            Standard_Real radius = sphere.Radius();

            // 2. 使用 BRepPrimAPI_MakeSphere 创建球体
            TopoDS_Shape solidSphere = BRepPrimAPI_MakeSphere(axis, radius);

            Handle(AIS_Shape) aisSphere = new AIS_Shape(solidSphere);
            aisSphere->SetColor(Quantity_NOC_SKYBLUE1);
            context->Display(aisSphere, Standard_False);
            shapes.push_back(aisSphere);

            // 3. 可视化球心到北极点的法向量线
            gp_Pnt top = center.Translated(axis.Direction().XYZ() * radius);
            TopoDS_Edge axisLine = BRepBuilderAPI_MakeEdge(center, top);

            Handle(AIS_Shape) aisLine = new AIS_Shape(axisLine);
            aisLine->SetColor(Quantity_NOC_RED);
            aisLine->SetWidth(2.0);
            context->Display(aisLine, Standard_False);
            shapes.push_back(aisLine);

            context->UpdateCurrentViewer();
        }
        catch (Standard_Failure const& e) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Sphere Error: %s", e.GetMessageString());
        }
    }

    void CreateSphereFromPoint(const Handle(AIS_InteractiveContext)& context) {
        ClearShapes(context);
        gp_Pnt center(centerInput[0], centerInput[1], centerInput[2]);
        gp_Dir zDir(0, 0, 1);  
        gp_Ax3 ax3(center, zDir);
        gp_Sphere sphere(ax3, radius);
        DisplaySphere(context, sphere);
    }

    void CreateSphereFromAx3(const Handle(AIS_InteractiveContext)& context) {
        ClearShapes(context);
        gp_Pnt center(centerInput[0], centerInput[1], centerInput[2]);
        gp_Ax3 ax3(center, gp_Dir(0, 0, 1), gp_Dir(1, 0, 0));  
        gp_Sphere sphere(ax3, radius);
        DisplaySphere(context, sphere);
    }
};
