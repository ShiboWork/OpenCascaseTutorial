#pragma once

#include "pch.h"
#include <Geom2d_AxisPlacement.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Line.hxx>
#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <V3d_View.hxx>
#include "BaseScene.h"
#include "VisSceneComponents.h"
#include "TutorialWindow.h"
#include "GC_MakeSegment.hxx"
#include "CodeHintAction.h"

class Geom2d009 : public BaseScene, public VisSceneComponents, public TutorialWindow {
public:
    Geom2d009() { 
		openTutorialWindow();   // 打开教学窗口
    }

    void displayScene(const Handle(V3d_View)& view, const Handle(AIS_InteractiveContext)& context) override {
        if (!bIsSceneInit) {
            sceneInit(view, context);
            bIsSceneInit = true;
        }
        renderTutorialWindow(context);
    }

    void customInitTutorialWindow(const Handle(AIS_InteractiveContext)&) override {}

    void sceneInit(const Handle(V3d_View)&, const Handle(AIS_InteractiveContext)& context) override {
        // 创建一个初始 axisPlacement
        axis = new Geom2d_AxisPlacement(gp_Pnt2d(0, 0), gp_Dir2d(1, 0));
        drawAxis(context, axis, Quantity_NOC_BLUE1);
    }

    void renderTutorialContent(const Handle(AIS_InteractiveContext)& context) override {
        ImGui::Text("Geom2d_AxisPlacement Tutorial");
        ImGui::Separator();

        const gp_Pnt2d loc = axis->Location();
        const gp_Dir2d dir = axis->Direction();
        ImGui::Text("Current Origin: (%.2f, %.2f)", loc.X(), loc.Y());
        ImGui::Text("Current Direction: (%.2f, %.2f)", dir.X(), dir.Y());

        static float newLoc[2] = { 0.0f, 0.0f };
        static float newDir[2] = { 1.0f, 0.0f };
        static float angleBetween = 0.0f;

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Set Axis Parameters");

        ImGui::InputFloat2("Origin", newLoc);
        if (ImGui::Button("Apply Origin")) {
            axis->SetLocation(gp_Pnt2d(newLoc[0], newLoc[1]));
            redraw(context);
            hintMgr.AddHint("Apply Origin", "axis->SetLocation(gp_Pnt2d(newLoc[0], newLoc[1]));");
        }

        ImGui::InputFloat2("Direction", newDir);
        if (ImGui::Button("Apply Direction")) {
            axis->SetDirection(gp_Dir2d(newDir[0], newDir[1]));
            redraw(context);
            hintMgr.AddHint("Apply Direction", "axis->SetDirection(gp_Dir2d(newDir[0], newDir[1]));");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Axis Operations");

        if (ImGui::Button("Reverse")) {
            axis->Reverse();
            redraw(context);
            hintMgr.AddHint("Reverse", "axis->Reverse();");
        }

        if (ImGui::Button("Draw Reversed (Red)")) {
            Handle(Geom2d_AxisPlacement) reversed = axis->Reversed();
            drawAxis(context, reversed, Quantity_NOC_RED);
            hintMgr.AddHint("Draw Reversed", R"(Handle(Geom2d_AxisPlacement) reversed = axis->Reversed();
drawAxis(context, reversed, Quantity_NOC_RED);)");
        }

        if (ImGui::Button("Apply from gp_Ax2d")) {
            gp_Ax2d ax2d(gp_Pnt2d(newLoc[0], newLoc[1]), gp_Dir2d(newDir[0], newDir[1]));
            axis->SetAxis(ax2d);
            redraw(context);
            hintMgr.AddHint("SetAxis", R"(gp_Ax2d ax2d(gp_Pnt2d(newLoc[0], newLoc[1]), gp_Dir2d(newDir[0], newDir[1]));
axis->SetAxis(ax2d);)");
        }

        if (ImGui::Button("Get Angle with Reversed")) {
            Handle(Geom2d_AxisPlacement) reversed = axis->Reversed();
            angleBetween = axis->Angle(reversed);
            hintMgr.AddHint("Angle", R"(Handle(Geom2d_AxisPlacement) reversed = axis->Reversed();
angle = axis->Angle(reversed);)");
        }
        ImGui::Text("Angle with reversed axis: %.3f radians", angleBetween);

        if (ImGui::Button("Mirror Axis (Cyan)")) {
            gp_Trsf2d trsf;
            trsf.SetMirror(axis->Location());
            Handle(Geom2d_AxisPlacement) mirrored = Handle(Geom2d_AxisPlacement)::DownCast(axis->Copy());
            mirrored->Transform(trsf);
            drawAxis(context, mirrored, Quantity_NOC_CYAN1);
            hintMgr.AddHint("Mirror Axis", R"(gp_Trsf2d trsf;
trsf.SetMirror(axis->Location());
Handle(Geom2d_AxisPlacement) mirrored = Handle(Geom2d_AxisPlacement)::DownCast(axis->Copy());
mirrored->Transform(trsf);
drawAxis(context, mirrored, Quantity_NOC_CYAN1);)");
        }

        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button("Clear Scene")) {
            context->RemoveAll(Standard_False);
            sceneInit(Handle(V3d_View)(), context);
            hintMgr.AddHint("Clear Scene", R"(context->RemoveAll(Standard_False);
sceneInit(Handle(V3d_View)(), context);)");
        }

        // 最后：显示集中提示窗口
        hintMgr.RenderWindow();
    }


private:
    Handle(Geom2d_AxisPlacement)    axis;       // Geom2d_AxisPlacement
    CodeHintManager hintMgr;  // 用于集中管理教学代码提示

    void redraw(const Handle(AIS_InteractiveContext)& context) {
        context->RemoveAll(Standard_False);
        drawAxis(context, axis, Quantity_NOC_BLUE1);
    }

    void drawAxis(const Handle(AIS_InteractiveContext)& context, const Handle(Geom2d_AxisPlacement)& ap, Quantity_NameOfColor color) {
        gp_Pnt2d p1 = ap->Location();
        gp_Vec2d vec(ap->Direction());
        gp_Pnt2d p2 = p1.Translated(vec.Multiplied(50.0));
        Handle(Geom_TrimmedCurve) curve = GC_MakeSegment(gp_Pnt(p1.X(), p1.Y(), 0), gp_Pnt(p2.X(), p2.Y(), 0));
        Handle(AIS_Shape) shape = new AIS_Shape(BRepBuilderAPI_MakeEdge(curve));
        shape->SetColor(color);
        context->Display(shape, Standard_True);
    }

};
