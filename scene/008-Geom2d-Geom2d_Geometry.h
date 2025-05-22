#pragma once

#include "pch.h"
#include <Geom2d_Line.hxx>
#include <Geom2d_Geometry.hxx>
#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS_Edge.hxx>
#include "BaseScene.h"
#include "VisSceneComponents.h"
#include "TutorialWindow.h"
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom2d_Transformation.hxx>

class Geom2d008 : public BaseScene, public VisSceneComponents, public TutorialWindow {
public:
    Geom2d008() { openTutorialWindow(); }

    void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        if (!bIsSceneInit) {
            sceneInit(view, context);
            bIsSceneInit = true;
        }
        renderTutorialWindow(context);
    }

    void customInitTutorialWindow(const Handle(AIS_InteractiveContext)& context) override {}

    void sceneInit(const Handle(V3d_View)&, const Handle(AIS_InteractiveContext)& context) override {
        // 原始二维线
        gp_Pnt2d p1(0.0, 0.0);
        gp_Dir2d dir(1.0, 1.0);
        originalLine = new Geom2d_Line(p1, dir);

        // 显示原始线（蓝色）
        originalAIS = CreateLineShape(originalLine, Quantity_NOC_BLUE);
        context->Display(originalAIS, Standard_False);

        // 显示初始镜像点
        VisualizeMirrorPoint(context);
    }

    void renderTutorialContent(const Handle(AIS_InteractiveContext)& context) override {
        ImGui::Text("Transformation Parameters");

        ImGui::SliderFloat2("Mirror Point", mirrorPt, -50.0f, 50.0f);
        ImGui::SliderFloat("Rotate Angle", &rotateAngle, 0.0f, 360.0f);

        UpdateMirrorPoint(context);

        ImGui::Text("Transformations:");
        if (ImGui::Button("Mirror about Point")) ApplyMirror(context, MirrorType::Point);
        if (ImGui::Button("Mirror about Axis")) ApplyMirror(context, MirrorType::Axis);
        if (ImGui::Button("Rotate Line")) ApplyRotation(context);
        if (ImGui::Button("Reset")) ResetLine(context);
    }

private:
    enum class MirrorType { Point, Axis };

    Handle(Geom2d_Line) originalLine;
    Handle(Geom2d_Line) transformedLine;
    Handle(AIS_Shape) originalAIS;
    Handle(AIS_Shape) transformedAIS;
    Handle(AIS_Shape) mirrorPointAIS;

    float mirrorPt[2] = { 10.0f, 10.0f };
    float rotateCenter[2] = { 0.0f, 0.0f };
    float rotateAngle = 45.0f;
    float scaleFactor = 1.5f;

    // 创建线段AIS对象（从2D线转为3D可视化）
    Handle(AIS_Shape) CreateLineShape(const Handle(Geom2d_Line)& line, Quantity_NameOfColor color) {
        gp_Pnt2d p2d = line->Location();
        gp_Dir2d d2d = line->Direction();
        gp_Pnt origin3D(p2d.X(), p2d.Y(), 0);
        gp_Dir dir3D(d2d.X(), d2d.Y(), 0);
        Handle(Geom_Curve) curve3d = new Geom_Line(origin3D, dir3D);

        TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(curve3d, -50, 50);
        Handle(AIS_Shape) ais = new AIS_Shape(edge);
        ais->SetColor(color);
        return ais;
    }

    // 显示镜像点
    void VisualizeMirrorPoint(const Handle(AIS_InteractiveContext)& context) {
        gp_Pnt mirrorPnt(mirrorPt[0], mirrorPt[1], 0);
        mirrorPointAIS = new AIS_Shape(BRepBuilderAPI_MakeVertex(mirrorPnt));
        mirrorPointAIS->SetColor(Quantity_NOC_RED);
        context->Display(mirrorPointAIS, Standard_False);
    }

    // 更新镜像点位置
    void UpdateMirrorPoint(const Handle(AIS_InteractiveContext)& context) {
        gp_Pnt mirrorPnt(mirrorPt[0], mirrorPt[1], 0);
        BRepBuilderAPI_MakeVertex vertex(mirrorPnt);
        mirrorPointAIS->SetShape(vertex);
        context->Redisplay(mirrorPointAIS, Standard_False);
    }

    // 镜像变换
    void ApplyMirror(const Handle(AIS_InteractiveContext)& context, MirrorType type) {
        Handle(Geom2d_Geometry) geo = originalLine->Copy();
        if (type == MirrorType::Point) {
            geo->Mirror(gp_Pnt2d(mirrorPt[0], mirrorPt[1]));
        }
        else {
            gp_Ax2d axis(gp_Pnt2d(mirrorPt[0], mirrorPt[1]), gp_Dir2d(1.0, 0.0));
            geo->Mirror(axis);
        }
        transformedLine = Handle(Geom2d_Line)::DownCast(geo);
        ShowTransformedLine(context, Quantity_NOC_RED);
    }

    // 旋转变换
    void ApplyRotation(const Handle(AIS_InteractiveContext)& context) {
        Handle(Geom2d_Geometry) geo = originalLine->Copy();
        geo->Rotate(gp_Pnt2d(rotateCenter[0], rotateCenter[1]), rotateAngle * M_PI / 180.0);
        transformedLine = Handle(Geom2d_Line)::DownCast(geo);
        ShowTransformedLine(context, Quantity_NOC_GREEN);
    }

    // 显示变换后线段（覆盖旧变换）
    void ShowTransformedLine(const Handle(AIS_InteractiveContext)& context, Quantity_NameOfColor color) {
        if (!transformedAIS.IsNull()) {
            context->Remove(transformedAIS, Standard_False);
        }
        transformedAIS = CreateLineShape(transformedLine, color);
        context->Display(transformedAIS, Standard_False);
        context->UpdateCurrentViewer();
    }

    // 重置，仅清除变换线
    void ResetLine(const Handle(AIS_InteractiveContext)& context) {
        if (!transformedAIS.IsNull()) {
            context->Remove(transformedAIS, Standard_False);
            transformedAIS.Nullify();
        }
        context->UpdateCurrentViewer();
    }
};
