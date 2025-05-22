#pragma once

#include "pch.h"
#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pln.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <AIS_Shape.hxx>
#include "BaseScene.h"
#include "VisSceneComponents.h"
#include "TutorialWindow.h"

class MDTrsf007 : public BaseScene, public VisSceneComponents, public TutorialWindow {
public:
    MDTrsf007() { openTutorialWindow(); }

    void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        if (context.IsNull()) return;
        if (!bIsSceneInit) {
            sceneInit(view, context);
            bIsSceneInit = true;
        }
        renderTutorialWindow(context);
    }

    void sceneInit(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        // 创建初始立方体
        CreateOriginalBox(context);
    }

    void customInitTutorialWindow(const Handle(AIS_InteractiveContext)& context) override {}

    void renderTutorialContent(const Handle(AIS_InteractiveContext)& context) override {
        // 变换参数控制
        ImGui::SliderFloat3("Translation", translation.data(), -50.f, 50.f);
        ImGui::SliderFloat3("Rotation Axis", rotationAxis.data(), -1.f, 1.f);
        ImGui::SliderFloat("Rotation Angle", &rotationAngle, 0.f, 360.f);
        ImGui::SliderFloat("Scale Factor", &scaleFactor, 0.1f, 5.f);

        // 变换操作按钮
        if (ImGui::CollapsingHeader("Transformation Operations", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Button("Apply Translation")) ApplyTransform(context, OperationType::Translation);
            if (ImGui::Button("Apply Rotation")) ApplyTransform(context, OperationType::Rotation);
            if (ImGui::Button("Apply Scale")) ApplyTransform(context, OperationType::Scale);
            if (ImGui::Button("Reset to Original")) ResetShape(context);
        }
    }

private:
    enum class OperationType { Translation, Rotation, Mirror, Scale };

    // 变换参数
    std::array<float, 3> translation{ 0.f, 0.f, 0.f };
    std::array<float, 3> rotationAxis{ 0.f, 0.f, 1.f };
    float rotationAngle = 45.f;
    float scaleFactor = 1.5f;

    // 几何对象
    Handle(AIS_Shape) originalShape;  // 原始立方体
    Handle(AIS_Shape) transformedShape;  // 变换后的物体
    std::vector<Handle(AIS_Shape)> auxShapes; // 辅助线

    // 创建原始立方体
    void CreateOriginalBox(const Handle(AIS_InteractiveContext)& context) {
        TopoDS_Shape box = BRepPrimAPI_MakeBox(50.0, 50.0, 50.0).Shape();
        originalShape = new AIS_Shape(box);
        originalShape->SetColor(Quantity_NOC_SKYBLUE);
        context->Display(originalShape, Standard_True);
    }

    // 清除变换后的物体
    void ClearTransformed(const Handle(AIS_InteractiveContext)& context) {
        if (!transformedShape.IsNull()) {
            context->Remove(transformedShape, Standard_False);
            transformedShape.Nullify();
        }
        for (auto& shape : auxShapes) context->Remove(shape, Standard_False);
        auxShapes.clear();
        context->UpdateCurrentViewer();
    }

    // 重置到原始状态
    void ResetShape(const Handle(AIS_InteractiveContext)& context) {
        ClearTransformed(context);
        originalShape->SetTransparency(0.0);
        context->UpdateCurrentViewer();
    }

    void ApplyTransform(const Handle(AIS_InteractiveContext)& context, OperationType op) {
        ClearTransformed(context);

        try {
            gp_Trsf trsf;
            switch (op) {
            case OperationType::Translation: {
                trsf.SetTranslation(gp_Vec(translation[0], translation[1], translation[2]));
                break;
            }
            case OperationType::Rotation: {
                gp_Ax1 axis(gp_Pnt(0, 0, 0),
                    gp_Dir(rotationAxis[0], rotationAxis[1], rotationAxis[2]));
                trsf.SetRotation(axis, rotationAngle * M_PI / 180.0);
                break;
            }
            case OperationType::Scale: {
                trsf.SetScale(gp_Pnt(0, 0, 0), scaleFactor);
                break;
            }
            }

            BRepBuilderAPI_Transform transformer(originalShape->Shape(), trsf);
            transformedShape = new AIS_Shape(transformer.Shape());
            transformedShape->SetColor(Quantity_NOC_ORANGE);
            transformedShape->SetTransparency(0.5);
            context->Display(transformedShape, Standard_True);

        }
        catch (Standard_Failure const& e) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Transform Error: %s", e.GetMessageString());
        }
    }
};