#pragma once

#include "pch.h"
#include <GCE2d_MakeLine.hxx> // 确保包含此头
#include <gce_MakePln.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx> 
#include <gp_Pln.hxx>
#include <Geom_Plane.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <Vector>
#include "BaseScene.h"
#include "VisSceneComponents.h"
#include "TutorialWindow.h"

class MDPlane003 : public BaseScene, public VisSceneComponents, public TutorialWindow {
public:
    MDPlane003() { openTutorialWindow(); }

    void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        if (context.IsNull()) return;

        if (!bIsSceneInit) {
            sceneInit(view, context);
            bIsSceneInit = true;
        }
        renderTutorialWindow(context);
    }

    void sceneInit(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        CreatePlaneFromThreePoints(context); // 使用三点构造的平面
    }

    void customInitTutorialWindow(const Handle(AIS_InteractiveContext)& context) override {
        CreatePlaneFromThreePoints(context); // 默认使用三点构造平面
    }

    void renderTutorialContent(const Handle(AIS_InteractiveContext)& context) override {
        if (ImGui::CollapsingHeader("Construct from 3 Points", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat3("Point A", pts.data(), -100.0f, 100.0f);
            ImGui::SliderFloat3("Point B", pts.data() + 3, -100.0f, 100.0f);
            ImGui::SliderFloat3("Point C", pts.data() + 6, -100.0f, 100.0f);

            if (ImGui::Button("Create Plane from 3 Points")) {
                CreatePlaneFromThreePoints(context);
            }

            if (ImGui::Checkbox("Flip Plane", &bFlipPlane)) {
                CreatePlaneFromThreePoints(context);
            }

            ImGui::SliderFloat3("Point D", pointD.data(), -100.0f, 100.0f); // 用于计算点到平面的距离

            if (ImGui::Button("Compute Point to Plane Distance")) {
                ComputePointToPlaneDistance(context);
            }
            ImGui::Text("Distance from point to plane: %.2f", m_distance);
        }
    }

private:
    void ClearAuxShapes(const Handle(AIS_InteractiveContext)& context) {
        for (auto& shape : auxShapes) {
            context->Remove(shape, Standard_False);
        }
        auxShapes.clear();
    }

    Handle(AIS_Shape) DisplayAuxLine(
        const Handle(AIS_InteractiveContext)& context,
        const gp_Pnt& start,
        const gp_Pnt& end,
        Quantity_NameOfColor color = Quantity_NOC_RED,
        Standard_Real width = 2.0) {
        TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(start, end);
        Handle(AIS_Shape) lineShape = new AIS_Shape(edge);
        lineShape->SetColor(color);
        lineShape->SetWidth(width);
        context->Display(lineShape, Standard_False);
        auxShapes.push_back(lineShape);
        return lineShape;
    }

    void CreatePlaneFromThreePoints(const Handle(AIS_InteractiveContext)& context) {
        try {
            gp_Pnt A(pts[0], pts[1], pts[2]);
            gp_Pnt B(pts[3], pts[4], pts[5]);
            gp_Pnt C(pts[6], pts[7], pts[8]);

            // 计算平面的法向量
            gp_Vec AB(A, B);
            gp_Vec AC(A, C);
            gp_Vec normal = AB.Crossed(AC);  // 使用叉积计算法向量

            if (bFlipPlane) {
                normal = normal.Reversed(); // 翻转法向量
            }

            // 创建平面
            gp_Pln plane(A, normal);
            CreateAndDisplayPlane(context, plane.Location(), plane.Axis().Direction(), Quantity_NOC_YELLOW);

            ClearAuxShapes(context);
            DisplayAuxLine(context, A, B, Quantity_NOC_GREEN);
            DisplayAuxLine(context, B, C, Quantity_NOC_GREEN);
            DisplayAuxLine(context, C, A, Quantity_NOC_GREEN);
            gp_Pnt normalTip = plane.Location().Translated(plane.Axis().Direction().XYZ() * 10);
            DisplayAuxLine(context, plane.Location(), normalTip, Quantity_NOC_RED);
            context->Redisplay(tutorialAisShape, Standard_True);
        }
        catch (Standard_Failure const&) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Invalid points for constructing plane.");
        }
    }

    void ComputePointToPlaneDistance(const Handle(AIS_InteractiveContext)& context) {
        try {
            gp_Pnt D(pointD[0], pointD[1], pointD[2]);

            if (tutorialAisShape.IsNull()) {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Plane not initialized.");
                return;
            }

            TopoDS_Shape shape = tutorialAisShape->Shape();
            if (shape.ShapeType() != TopAbs_FACE) {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Shape is not a face.");
                return;
            }

            TopoDS_Face face = TopoDS::Face(shape);
            Handle(Geom_Surface) surface = BRep_Tool::Surface(face);
            Handle(Geom_Plane) geomPlane = Handle(Geom_Plane)::DownCast(surface);

            if (geomPlane.IsNull()) {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Surface is not a plane.");
                return;
            }

            gp_Pln plane = geomPlane->Pln();
            m_distance = plane.Distance(D);

            // 可视化点与平面连线
            ClearAuxShapes(context);
            DisplayAuxLine(context, D, plane.Location(), Quantity_NOC_BLUE, 2.0);

        }
        catch (Standard_Failure const&) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error in computing distance.");
        }
    }

    void CreateAndDisplayPlane(const Handle(AIS_InteractiveContext)& context,
        const gp_Pnt& origin,
        const gp_Dir& normal,
        Quantity_NameOfColor color) {
        gp_Pln plane(origin, normal);
        Standard_Real size = 20;
        Handle(Geom_Plane) geomPlane = new Geom_Plane(plane);
        BRepBuilderAPI_MakeFace faceMaker(
            geomPlane->Pln(),
            -size / 2, size / 2,
            -size / 2, size / 2);

        if (!tutorialAisShape.IsNull()) {
            context->Remove(tutorialAisShape, Standard_False);
        }
        tutorialAisShape = new AIS_Shape(faceMaker.Shape());
        tutorialAisShape->SetColor(color);
        tutorialAisShape->SetTransparency(0.5);
        context->Display(tutorialAisShape, Standard_True);
    }

private:
    std::array<float, 3> planeOrigin{ 0.0f, 0.0f, 0.0f };
    std::array<float, 3> planeNormal{ 0.0f, 0.0f, 1.0f };
    std::array<float, 9> pts{ 0.0f, 0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 10.0f, 0.0f };
    std::array<float, 3> pointD{ 0.0f, 0.0f, 0.0f }; // 用于输入计算点到平面距离的点
    Handle(AIS_Shape) tutorialAisShape;
    std::vector<Handle(AIS_Shape)> auxShapes;
    bool            bFlipPlane  = false;    // 是否翻转平面
    Standard_Real   m_distance  = 0;        // 点到平面距离
};